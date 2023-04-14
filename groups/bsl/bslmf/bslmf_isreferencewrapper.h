// bslmf_isreferencewrapper.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ISREFERENCEWRAPPER
#define INCLUDED_BSLMF_ISREFERENCEWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to detect reference-wrapper specializations.
//
//@CLASSES:
//  bslmf::IsReferenceWrapper: metafunction for detecting reference wrappers
//
//@SEE_ALSO: bslstl_referencewrapper
//
//@DESCRIPTION: This component provides a 'struct' template,
// 'bslmf::IsReferenceWrapper', that is a boolean metafunction with a single
// class template parameter 't_TYPE'.  'bslmf::IsReferenceWrapper' derives from
// 'bsl::false_type' by default.  The only intended specialization of
// 'bslmf::IsReferenceWrapper' is for 'bsl::reference_wrapper'.  Clients are
// not allowed to specialized 'bslmf::IsReferenceWrapper' for any other type.
//
// This component exists primarily because there are other type traits provided
// by components in the 'bslmf' package that require the ability to detect
// whether or not a type is a specialization of 'bsl::reference_wrapper',
// notably 'bsl::invoke_result'.  'bsl::reference_wrapper' is defined in the
// 'bslstl_referencewrapper' component in the 'bslstl' package, and is
// available to clients through the 'bsl_functional' component in the
// 'bsl+bslhdrs' package.  These packages are levelized above 'bslmf', and so
// this trait permits components in the 'bslmf' package (and above) to detect
// whether or not a type is a reference wrapper even if they cannot refer to
// the 'bsl::reference_wrapper' type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example: Reference Access
///- - - - - - - - - - - - -
// In this example, we suppose that we would like to provide a software
// component that treats specializations of 'bsl::reference_wrapper' and
// true reference qualified types in the same way.
//
// Suppose that we would like to do this by having a utility function that
// returns an lvalue reference to an object given either an lvalue-reference
// qualified type or a 'bsl::reference_wrapper'.
//
// First, we define a utility 'struct' that contains the overload of this
// utility function for 'const' and non-'const' lvalue-reference qualified
// types:
//..
//  struct MyLvalueReferenceUtil {
//      // CLASS METHODS
//      template <class t_TYPE>
//      static t_TYPE& access(t_TYPE& reference)
//      {
//          return reference;
//      }
//
//      template <class t_TYPE>
//      static const t_TYPE& access(const t_TYPE& reference)
//      {
//          return reference;
//      }
//..
// Then, we define the overload of this utility function for reference
// wrappers, taking care to define it such that it does not participate in
// overload resolution unless it is passed a reference wrapper:
//..
//      template <class t_TYPE>
//      static typename bsl::enable_if<
//          bslmf::IsReferenceWrapper<t_TYPE>::value,
//          typename bsl::add_lvalue_reference<typename t_TYPE::type>::type
//      >::type
//      access(t_TYPE referenceWrapper)
//      {
//          return referenceWrapper.get();
//      }
//  };
//..
// Finally, we can verify that this utility allows us to transparently access
// lvalue references:
//..
//  void example()
//  {
//      int x = 1;
//      assert(1 == MyLvalueReferenceUtil::access(x));
//
//      const int y = 2;
//      assert(2 == MyLvalueReferenceUtil::access(y));
//
//      // Note that even though the following invokes 'access' with the
//      // literal 3, which is a prvalue expression, the call expression is
//      // an lvalue with type 'const int&'.
//      assert(3 == MyLvalueReferenceUtil::access(3));
//
//      // Further, note that the levelization of the
//      // 'bslmf_isreferencewrapper' component prohibits showing the
//      // application of 'MyLvalueReferenceUtil' to a reference wrapper.
//      // The following commented-out code would be valid given a suitable
//      // 'bsl::reference_wrapper' type that acts like a reference wrapper
//      // and specializes the 'bslmf::IsReferenceWrapper' trait accordingly.
//      //..
//      //  assert(x == MyLvalueReferenceUtil::access(
//      //                                    bsl::reference_wrapper<int>(x)));
//      //..
//  }
//..

#include <bslmf_integralconstant.h>

namespace BloombergLP {
namespace bslmf {

                         // =========================
                         // struct IsReferenceWrapper
                         // =========================

template <class t_TYPE>
struct IsReferenceWrapper : bsl::false_type {
    // This 'struct' template implements a boolean metafunction used to detect
    // if the specified 't_TYPE' is a reference wrapper.  Clients may
    // specialize this 'struct' template to inherit from 'bsl::true_type' for
    // 't_TYPE' types that are specializations of 'bsl::reference_wrapper'.
    // The behavior is undefined if any other specialization of this 'struct'
    // template is defined.  Note that this 'struct' template
};

}  // close enterprise namespace
}  // close package namespace

#endif // INCLUDED_BSLMF_ISREFERENCEWRAPPER

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
