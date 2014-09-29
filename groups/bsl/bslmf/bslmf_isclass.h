// bslmf_isclass.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCLASS
#define INCLUDED_BSLMF_ISCLASS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining class types.
//
//@CLASSES:
//  bsl::is_class: standard meta-function for determining class types
//  bslmf::IsClass: meta-function for determining class types
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_class' and
// 'BloombergLP::bslmf::IsClass', both of which may be used to query whether a
// type is a 'class', 'struct', or 'union', optionally qualified with 'const'
// or 'volatile'.
//
// 'bsl::is_class' meets the requirements of the 'is_class' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsClass' was devised
// before 'is_class' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_class' is indicated by the
// class member 'value', while the result for 'bslmf::IsClass' is indicated by
// the class member 'VALUE'.
//
// Note that 'bsl::is_class' should be preferred over 'bslmf::IsClass', and in
// general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Class Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are class types.
//
// First, we create a class type 'MyClass':
//..
//  class MyClass
//  {
//  };
//..
// Now, we instantiate the 'bsl::is_class' template for both a non-class type
// and the defined type 'MyClass', and assert the 'value' static data member of
// each instantiation:
//..
//  assert(false == bsl::is_class<int>::value);
//  assert(true  == bsl::is_class<MyClass>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // TBD Robo transitively needs this for 'bsl::atoi', etc.
#define INCLUDED_CSTDLIB
#endif

#endif

namespace BloombergLP {
namespace bslmf {

                             // ==================
                             // struct IsClass_Imp
                             // ==================

template <class TYPE>
struct IsClass_Imp {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a class type.

    typedef struct { char a; }    YesType;
    typedef struct { char a[2]; } NoType;

    template <class TEST_TYPE>
    static
    YesType test(int TEST_TYPE::*);

    template <class TEST_TYPE>
    static
    NoType test(...);

    enum { Value = (sizeof(test<TYPE>(0)) == sizeof(YesType)) };
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                             // ===============
                             // struct is_class
                             // ===============

template <class TYPE>
struct is_class : integral_constant<bool,
                                 BloombergLP::bslmf::IsClass_Imp<
                                      typename remove_cv<TYPE>::type>::Value> {
    // This 'struct' template implements the 'is_class' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is a class.
};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {
                            // ==============
                            // struct IsClass
                            // ==============

template <class TYPE>
struct IsClass : bsl::is_class<TYPE>::type {
    // This meta-function derives from 'bsl::true_type' if the (template
    // parameter) 'TYPE' is a class type, or a reference to a class type, and
    // from 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally identical to
    // 'bsl::is_class', the use of 'bsl::is_class' should be preferred.
};

}  // close package namespace
}  // close enterprise namespace


#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
