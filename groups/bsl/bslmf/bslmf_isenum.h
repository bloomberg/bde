// bslmf_isenum.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISENUM
#define INCLUDED_BSLMF_ISENUM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time check for determining enumerated types.
//
//@CLASSES:
//  bsl::is_enum: standard meta-function for determining enumerated types
//  bslmf::IsEnum: meta-function for determining enumerated types
//
//@SEE_ALSO: bslmf_isfundamental
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_enum' and
// 'BloombergLP::bslmf::IsEnum', both of which may be used to query whether a
// type is an enumerated type, optionally qualified with 'const' or 'volatile'.
//
// 'bsl::is_enum' meets the requirements of the 'is_enum' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsEnum' was devised
// before 'is_enum' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_enum' is indicated by the class
// member 'value', while the result for 'bslmf::IsEnum' is indicated by the
// class member 'VALUE'.
//
// Note that 'bsl::is_enum' should be preferred over 'bslmf::IsEnum', and in
// general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Enumerated Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are 'enum' types.
//
// First, we create an enumerated type, 'MyEnum', and a class type, 'MyClass':
//..
//  enum MyEnum { MY_ENUMERATOR = 5 };
//  class MyClass { explicit MyClass(MyEnum); };
//..
// Now, we instantiate the 'bsl::is_enum' template for both types we defined
// previously, and assert the 'value' static data member of each instantiation:
//..
//  assert(true  == bsl::is_enum<MyEnum>::value);
//  assert(false == bsl::is_enum<MyClass>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLETOANY
#include <bslmf_isconvertibletoany.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

                      // ==============================
                      // class IsEnum_AnyArithmeticType
                      // ==============================

struct IsEnum_AnyArithmeticType {
    // This 'struct' provides a type that is convertible from any arithmetic
    // (i.e., integral or floating-point) type, or any enumerated type.
    // Converting any type to an 'IsEnum_AnyArithmeticType' is a user-defined
    // conversion and cannot be combined with any other implicit user-defined
    // conversions.  Thus, even class types that have conversion operators to
    // arithmetic types or enumerated types will not be implicitly convertible
    // to 'IsEnum_AnyArithmeticType'.

    // NOT IMPLEMENTED
    IsEnum_AnyArithmeticType(wchar_t);                              // IMPLICIT
    IsEnum_AnyArithmeticType(int);                                  // IMPLICIT
    IsEnum_AnyArithmeticType(unsigned int);                         // IMPLICIT
    IsEnum_AnyArithmeticType(long);                                 // IMPLICIT
    IsEnum_AnyArithmeticType(unsigned long);                        // IMPLICIT
    IsEnum_AnyArithmeticType(long long);                            // IMPLICIT
    IsEnum_AnyArithmeticType(unsigned long long);                   // IMPLICIT
    IsEnum_AnyArithmeticType(double);                               // IMPLICIT
    IsEnum_AnyArithmeticType(long double);                          // IMPLICIT
        // Create an 'IsEnum_AnyArithmeticType' object from a value of one of
        // the indicated arithmetic types.  Note that it is not necessary to
        // provide overloads taking 'bool', 'char', or 'short' because they are
        // automatically promoted to 'int'; nor is a 'float' overload needed
        // because it is automatically promoted to 'double'.  Also note that
        // the other variants are necessary because a conversion from, e.g., a
        // 'long double' to a 'double' does not take precedence over a
        // conversion from 'long double' to 'int' and, therefore, would be
        // ambiguous.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                               // ==============
                               // struct is_enum
                               // ==============

template <class TYPE>
struct is_enum
    : integral_constant<
        bool,
        !is_fundamental<TYPE>::value
        && !is_reference<TYPE>::value
        && is_convertible<TYPE,
                        BloombergLP::bslmf::IsEnum_AnyArithmeticType>::value
        && !BloombergLP::bslmf::IsConvertibleToAny<TYPE>::value> {
    // This 'struct' template implements the 'is_enum' meta-function defined in
    // the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is an enumerated type.  This 'struct' derives from
    // 'bsl::true_type' if the 'TYPE' is an enumerated type, and from
    // 'bsl::false_type' otherwise.
};

// Additional partial specializations for cv-qualified types ensure that the
// correct result is obtained for cv-qualified enums.  Note that there is a
// peculiar bug wit the IBM xlC compiler that requires an additional use of the
// 'remove_cv' trait to obtain the correct result (without infinite recursion)
// for arrays of more than one dimension.

template <class TYPE>
struct is_enum<const TYPE>
    : is_enum<typename bsl::remove_cv<TYPE>::type>::type {
};

template <class TYPE>
struct is_enum<volatile TYPE>
    : is_enum<typename bsl::remove_cv<TYPE>::type>::type {
};

template <class TYPE>
struct is_enum<const volatile TYPE>
    : is_enum<typename bsl::remove_cv<TYPE>::type>::type {
};

template <>
struct is_enum<void>
    : bsl::false_type {
};

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                                // ============
                                // class IsEnum
                                // ============

template <class TYPE>
struct IsEnum : bsl::is_enum<TYPE>::type {
    // This 'struct' provides a meta-function that computes, at compile time,
    // whether the (template parameter) 'TYPE' is an enumerated type.  It
    // derives from 'bsl::true_type' if 'TYPE' is an enumerated type, and from
    // 'bsl::false_type' otherwise.
    //
    // Enumerated types are the only user-defined types that have the
    // characteristics of a native arithmetic type (i.e., they can be converted
    // to an integral type without invoking user-defined conversions).  This
    // class takes advantage of this property to distinguish 'enum' types from
    // class types that are convertible to an integral or enumerated type.
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
