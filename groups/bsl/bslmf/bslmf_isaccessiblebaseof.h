// bslmf_isaccessiblebaseof.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ISACCESSIBLEBASEOF
#define INCLUDED_BSLMF_ISACCESSIBLEBASEOF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for derived classes.
//
//@CLASSES:
//  bslmf::IsAccessibleBaseOf: meta-function for detecting derived classes
//
//@DESCRIPTION: This component provides a meta-function,
// 'bslmf::IsAccessibleBaseOf', that determines whether one class is an
// accessible base class of another class.  The static constant
// 'bslmf::IsAccessibleBaseOf::value' is 'true' if the template parameter
// 't_BASE' class is an accessible base class of, or the same class as, the
// template parameter 't_DERIVED'.  Otherwise,
// 'bslmf::IsAccessibleBaseOf::value' is 'false'.  The specific cases of
// private, protected, and ambiguous inheritance are not supported for versions
// of C++ prior to 11.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Base And Derived Classes
///- - - - - - - - - - - - - - - - - -
// Define two classes, one inheriting from the other.
//..
//  struct Base
//  {};
//
//  struct Derived : Base
//  {};
//..
// Evaluate 'bslmf::IsAccessibleBaseOf::value'.
//..
//  void example1()
//  {
//      assert((true  == bslmf::IsAccessibleBaseOf<Base,    Derived>::value));
//      assert((true  == bslmf::IsAccessibleBaseOf<Base,    Base>   ::value));
//      assert((true  == bslmf::IsAccessibleBaseOf<Derived, Derived>::value));
//      assert((false == bslmf::IsAccessibleBaseOf<Derived, Base>   ::value));
//  }
//..
///Example 2: Unrelated Classes
/// - - - - - - - - - - - - - -
// Define two classes, one inheriting privately from the other.
//..
//  class Unrelated
//  {};
//
//  class Unrelated2
//  {};
//..
// Evaluate 'bslmf::IsAccessibleBaseOf::value'.  Note that 'Derived' is not
// *observably* derived from 'Base', so std::is_base_of would evaluate 'true',
// but 'bslmf::IsAccessibleBaseOf' evaluates 'false'.
//..
//  void example2()
//  {
//      assert((false ==
//              bslmf::IsAccessibleBaseOf<Unrelated, Unrelated2>::value));
//  }
//..

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_isclass.h>
#include <bslmf_isconvertible.h>
#include <bslmf_removecv.h>

namespace BloombergLP {
namespace bslmf {

                          // ========================
                          // class IsAccessibleBaseOf
                          // ========================

template <class t_BASE, class t_DERIVED>
class IsAccessibleBaseOf
: public bsl::integral_constant<
      bool,
      bsl::is_class<typename bsl::remove_cv<t_BASE>::type>::value &&
          bsl::is_class<typename bsl::remove_cv<t_DERIVED>::type>::value &&
          bsl::is_convertible<
              typename bsl::remove_cv<t_DERIVED>::type *,
              typename bsl::remove_cv<t_BASE>::type *>::value> {
    // This 'struct' template provides a type trait to determine if one class
    // is an accessible base class of another class.  Note that, while similar
    // to 'std::is_base_of', when the derived relationship is via private,
    // protected, or ambiguous inheritance 'IsAccessibleBaseOf' evaluates to
    // false.
};

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLMF_ISACCESSIBLEBASEOF

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
