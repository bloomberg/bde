// bdlb_nullopt.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLB_NULLOPT
#define INCLUDED_BDLB_NULLOPT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tag type and constant indicating an empty nullable value.
//
//@DEPRECATED: use `bsl::nullopt_t` from `bsl_optional.h` instead.
//
//@CLASSES:
//  NullOptType: tag type representing an empty nullable value
//  nullOpt:     literal value of type `NullOptType`
//
//@SEE_ALSO: bsl::optional
//
//@DESCRIPTION: This component provides a class, `bdlb::NullOptType`, that
// is a synonym for `bsl::nullopt_t`.
//
// In addition to the `NullOptType` class type, this component defines a
// constant, `bdlb::nullOpt`, of type `NullOptType`, which is a synonym for
// `bsl::nullopt`.
//
// Please use `bsl::nullopt_t` and `bsl::nullopt` instead of
// `bdlb::NullOptType` and `bdlb::nullOpt`, respectively.
//

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>

#include <bsl_optional.h>

namespace BloombergLP {
namespace bdlb {

                                // =================
                                // class NullOptType
                                // =================

BSLS_DEPRECATE_FEATURE("bdl", "NullOptType", "Use 'bsl::nullopt_t' instead")
/// This `class` provides an empty tag type so that `bdlb::NullableValue`
/// can be explicitly constructed in or assigned to an empty state.  There
/// are no publicly accessible constructors for this type other than the
/// copy constructor.  In particular, it is not default constructible, nor
/// list-initializable in C++11.
typedef bsl::nullopt_t NullOptType;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr bsl::nullopt_t nullOpt = bsl::nullopt;
#else
extern const bsl::nullopt_t nullOpt;
#endif
    // Value of type 'NullOptType' that serves as a literal value for the empty
    // state of any nullable value.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BDLB_NULLOPT)

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
