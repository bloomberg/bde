// bslstl_iserrorcondition.h                                          -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISERRORCONDITIONENUM
#define INCLUDED_BSLSTL_ISERRORCONDITIONENUM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide C++11-defined error condition trait for C++03.
//
//@CLASSES:
//  bsl::is_error_condition_enum: C++03 version of std::is_error_condition_enum
//
//@MACROS:
//  BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE:  namespace in which to specialize
//
//@DESCRIPTION: This component defines a class template,
// 'bsl::is_error_condition_enum', intended to be specialized for enumeration
// types that are designated as error conditions for the '<system_error>'
// facility.  In C++11 mode, the vendor-supplied '<system_error>'
// implementation is used instead, and the corresponding names from 'std' are
// imported into 'bsl'.  This component also defines a macro,
// 'BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE', to be used as the namespace in
// which to write specializations of 'is_error_condition_enum'.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Dedicated Error Values
///- - - - - - - - - - - - - - - - -
// Suppose we have a dedicated system with a set of possible errors, and we
// want to be able to throw descriptive exceptions when an error occurs.  We
// need to work with the '<system_error>' facility to support this, starting by
// marking the enumeration type that defines the error literals as eligible to
// participate.  We can use 'bsl::is_error_condition' to do this.
//
// First, we define the set of error values for our system.
//..
//  struct car_errc {
//  enum Enum {
//      car_wheels_came_off = 1,
//      car_engine_fell_out = 2
//  };
//  };
//..
// Then, we enable the trait marking this as an error condition.
//..
//  namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE {
//  template <> struct is_error_condition_enum<car_errc::car_errc>
//  : public true_type { };
//  }  // close namespace BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE
//..
// Finally, we verify that the trait marks our type as eligible.
//..
//  assert(is_error_condition_enum<car_errc::car_errc>::value);
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_libraryfeatures.h>
#include <bsls_nativestd.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <system_error>

#define BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE std

namespace bsl {
    using native_std::is_error_condition_enum;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using native_std::is_error_condition_enum_v;
#elif defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    template <class TYPE>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
    inline
#endif
    BSLS_KEYWORD_CONSTEXPR bool is_error_condition_enum_v =
        is_error_condition_enum<TYPE>::value;
#endif
}  // close namespace bsl

#else

#define BSL_IS_ERROR_CONDITION_ENUM_NAMESPACE bsl

namespace bsl {
                       // ==============================
                       // struct is_error_condition_enum
                       // ==============================

template <class TYPE>
struct is_error_condition_enum : public bsl::false_type
    // This class template represents a trait defining whether the specified
    // enumeration type 'TYPE' is to be treated as an error condition by the
    // 'error_condition' template methods.
{
};

}  // close namespace bsl

#endif
#endif

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
