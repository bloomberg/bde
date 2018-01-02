// bslmf_isarray.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARRAY
#define INCLUDED_BSLMF_ISARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for array types.
//
//@CLASSES:
//  bsl::is_array: standard meta-function for detecting array types
//  bslmf::IsArray: meta-function for detecting array types
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_array' and
// 'BloombergLP::bslmf::IsArray', both of which may be used to query whether a
// type is an array type.
//
// 'bsl::is_array' meets the requirements of the 'is_array' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsArray' was devised
// before 'is_array' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_array' is indicated by the
// class member 'value', while the result for 'bslmf::IsArray' is indicated by
// the class member 'VALUE'.
//
// Note that 'bsl::is_array' should be preferred over 'bslmf::IsArray', and in
// general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Array Types
///- - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an array type.
//
// First, we create two 'typedef's -- an array type and a non-array type:
//..
//  typedef int MyType;
//  typedef int MyArrayType[]
//..
// Now, we instantiate the 'bsl::is_array' template for each of the 'typedef's
// and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_array<MyType>::value);
//  assert(true  == bsl::is_array<MyArrayType>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>  // Robo transitively needs this for 'bsl::atoi', etc.
#define INCLUDED_CSTDLIB
#endif

#endif

namespace bsl {

                         // ===============
                         // struct is_array
                         // ===============

template <class TYPE>
struct is_array : false_type {
    // This 'struct' template implements the 'is_array' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is an array type.  This 'struct' derives from
    // 'bsl::true_type' if the 'TYPE' is an array type, and 'bsl::false_type'
    // otherwise.
};

                         // ====================================
                         // struct is_array<TYPE [NUM_ELEMENTS]>
                         // ====================================

template <class TYPE, std::size_t NUM_ELEMENTS>
struct is_array<TYPE [NUM_ELEMENTS]> : true_type {
     // This specialization of 'is_array', for when the (template parameter)
     // 'TYPE' is an array type of known bound, derives from 'bsl::true_type'.
};

                         // ========================
                         // struct is_array<TYPE []>
                         // ========================

template <class TYPE>
struct is_array<TYPE []> : true_type {
     // This specialization of 'is_array', for when the (template parameter)
     // 'TYPE' is an array type of unknown bound, derives from
     // 'bsl::true_type'.
};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ==============
                         // struct IsArray
                         // ==============

template <class TYPE>
struct IsArray  : bsl::is_array<TYPE>::type {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'TYPE' is an array type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is an array type, and
    // 'bsl::false_type' otherwise.
    //
    // Note that although this 'struct' is functionally equivalent to
    // 'bsl::is_array', the use of 'bsl::is_array' should be preferred.
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
