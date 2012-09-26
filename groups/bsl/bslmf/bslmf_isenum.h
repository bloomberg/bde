// bslmf_isenum.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_ISENUM
#define INCLUDED_BSLMF_ISENUM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide compile-time detection of enumerated types.
//
//@CLASSES:
//  bsl::is_class: standard meta-function for determining enumerated types
//  bslmf::IsEnum: meta-function for detecting enumerated types
//
//@SEE_ALSO: bslmf_isfundamental
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines two meta-functions, 'bsl::is_enum' and
// 'BloombergLP::bslmf::IsEnum', both of which may be used to query whether a
// type is a enumerated, optionally qualified with 'const' or volatile'.
//
// 'bsl::is_enum' meets the requirements of the 'is_enum' template defined in
// the C++11 standard [meta.unary.cat], while 'bslmf::IsEnum' was devised
// before 'is_class' was standardized.
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
// Suppose that we want to assert whether a particular type is a class type.
//
// First, we create an enumerated type, 'MyEnum', and a non-enumerated class
// type, 'MyClass':
//..
//  enum MyEnum { MY_ENUMERATOR = 5 };
//  class MyClass { MyClass(MyEnum); };
//..
// Now, we instantiate the 'bsl::is_enum' template for both types we defined
// previously, asserting the 'value' static data member of each instantiation:
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

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

namespace bslmf {

                      // ==============================
                      // class IsEnum_AnyArithmeticType
                      // ==============================

struct IsEnum_AnyArithmeticType {
    // This struct provides a type that is convertible from any arithmetic type
    // (integral, floating-point, or enumeration).  Converting any type to a
    // 'IsEnum_AnyArithmeticType' is a user-defined conversion and cannot be
    // combined with any other implicit user-defined conversions.  Thus, even
    // class types that have conversion operators to arithmetic types will not
    // be implicitly convertible to 'IsEnum_AnyArithmeticType'.
    //
    // This is a component-private 'struct'.  Do *not* use.

    // NOT IMPLEMENTED
    IsEnum_AnyArithmeticType(wchar_t);
    IsEnum_AnyArithmeticType(int);
    IsEnum_AnyArithmeticType(unsigned int);
    IsEnum_AnyArithmeticType(long);
    IsEnum_AnyArithmeticType(unsigned long);
    IsEnum_AnyArithmeticType(long long);
    IsEnum_AnyArithmeticType(unsigned long long);
    IsEnum_AnyArithmeticType(double);
    IsEnum_AnyArithmeticType(long double);
        // Conversion constructor from any arithmetic type.  Note that it is
        // not necessary to provide overloads taking 'bool', 'char', or 'short'
        // because they are automatically promoted to 'int'; nor is a 'float'
        // overload needed because it is automatically promoted to 'double'.
        // Also note that the other variants are necessary because a conversion
        // from, e.g., a 'long double' to a 'double' does not take precedence
        // over a conversion from 'long double' to 'int' and, therefore, would
        // be ambiguous.
};

                        // ============
                        // class IsEnum
                        // ============

template <class TYPE>
struct IsEnum
    : bsl::integral_constant<
        bool,
        !bsl::is_fundamental<typename bsl::remove_reference<
                             typename bsl::remove_cv<TYPE>::type>::type>::value
        && bsl::is_convertible<TYPE, IsEnum_AnyArithmeticType>::value>
    // This struct provides a meta-function that computes, at compile time,
    // whether 'TYPE' is of enumeration type.  It derives from 'bsl::true_type'
    // if 'TYPE' is an enumeration type, or from 'bsl::false_type' otherwise.
    //
    // Enumeration types are the only user-defined types that have the
    // characteristics of a native arithmetic type (i.e., they can be promoted
    // to 'int' without invoking user-defined conversions).  This class takes
    // advantage if this property to distinguish 'enum' types from class types
    // that are convertible to 'int'.
{
};

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_enum
    : integral_constant<
        bool,
        !is_fundamental<typename remove_cv<TYPE>::type>::value
        && !is_reference<TYPE>::value
        && is_convertible<TYPE,
                          BloombergLP::bslmf::IsEnum_AnyArithmeticType>::value>
{};

}  // close namespace bsl

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsEnum
#undef bslmf_IsEnum
#endif
#define bslmf_IsEnum bslmf::IsEnum
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

#endif

// ---------------------------------------------------------------------------
// NOTICE: //      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
