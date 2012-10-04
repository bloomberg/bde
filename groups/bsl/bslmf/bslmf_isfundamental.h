// bslmf_isfundamental.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#define INCLUDED_BSLMF_ISFUNDAMENTAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for fundamental types.
//
//@CLASSES:
//  bsl::is_fundamental: standard meta-function for fundamental type checking
//  bslmf::IsFundamental: meta-function for fundamental type checking
//
//@SEE_ALSO: bslmf_isenum, bslmf_ispointer
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@DESCRIPTION: This component defines two meta-functions,
// 'bsl::is_fundamental' and 'BloombergLP::bslmf::IsFundamental', both of which
// may be used to query whether a type is a pointer type.
//
// 'bsl::is_fundamental' meets the requirements of the 'is_fundamental'
// template defined in the C++11 standard [meta.unary.comp], while
// 'bslmf::Fundamental' was devised before 'is_fundamental' was standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for 'bsl::is_fundamental' is indicated by
// the class member 'value', while the result for 'bslmf::Fundamental' is
// indicated by the class member 'VALUE'.
//
// Note that 'bsl::is_fundamental' should be preferred over
// 'bslmf::Fundamental', and in general, should be used by new components.
//
// The C++ fundamental types are described in the C++ standard, section 3.9.1
// ("Fundamental types"), and consist of the following distinct types:
//..
//  bool
//  char
//  signed char
//  unsigned char
//  wchar_t
//  short int                                    (also referred to as "short")
//  unsigned short int                  (also referred to as "unsigned short")
//  int
//  unsigned int
//  long int                                      (also referred to as "long")
//  unsigned long int                    (also referred to as "unsigned long")
//  float
//  double
//  long double
//  void
//..
// To these types, this component adds the following non-standard types and
// identifies them as fundamental types:
//..
//  long long int                            (also referred to as "long long")
//  unsigned long long int          (also referred to as "unsigned long long")
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Fundamental Types
///- - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a fundamental
// type.
//
// Now, we instantiate the 'bsl::is_fundamental' template for a couple of
// non-fundamental and fundamental types, and assert the 'value' static data
// member of each instantiation:
//..
//  assert(true  == bsl::is_fundamental<int>::value);
//  assert(false == bsl::is_fundamental<int&>::value);
//  assert(true  == bsl::is_fundamental<long long>::value);
//  assert(false == bsl::is_fundamental<long long*>::value);
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

#ifndef INCLUDED_BSLMF_ISARITHMETIC
#include <bslmf_isarithmetic.h>
#endif

#ifndef INCLUDED_BSLMF_ISVOID
#include <bslmf_isvoid.h>
#endif

namespace BloombergLP {

namespace bslmf {

                         // ========================
                         // struct IsFundamental_Imp
                         // ========================

template <typename T>
struct IsFundamental_Imp : bsl::false_type
{
    // This class is an implementation detail.  Do not use directly.  The
    // general case for type 'T' is that it is assumed not to be fundamental.
    // This class is specialized for each fundamental type.  Note that the
    // specializations are always for 'const volatile' types.  The
    // 'IsFundamental' derived class (below) takes advantage of the fact that
    // cv-qualifications are idempotent to add 'const volatile' to every type
    // and thus avoid having 4 times as many specialization (for each of 4
    // combinations of 'const' and 'volatile' qualifiers).
};

template <> struct IsFundamental_Imp<bool>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<char>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<signed char>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<unsigned char>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<wchar_t>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<short>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<unsigned short>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<int>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<unsigned int>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<long>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<unsigned long>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<long long>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<unsigned long long>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<float>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<double>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<long double>
    : bsl::true_type { };
template <> struct IsFundamental_Imp<void>
    : bsl::true_type { };

                         // ====================
                         // struct IsFundamental
                         // ====================

template <typename TYPE>
struct IsFundamental
    : IsFundamental_Imp<typename bsl::remove_cv<TYPE>::type>::type
{
    // This class implements a meta-function for checking if a type is
    // fundamental.  The static constant 'value' member will be 1 if 'TYPE' is
    // fundamental and 0 otherwise.
};

template <typename TYPE>
struct IsFundamental<TYPE&>
    : IsFundamental_Imp<typename bsl::remove_cv<TYPE>::type>::type
{
    // This specialization of 'IsFundamental' causes references to be treated
    // as their underlying (non-reference) types.
};

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_fundamental
    : integral_constant<bool,
                        is_arithmetic<TYPE>::value
                        || is_void<TYPE>::value>
{
    // This class implements a meta-function for checking if a type is
    // fundamental as defined in C++11 section 3.9.1 [basic.fundamental] with
    // the exception of 'nullptr_t' which is not supported yet.
};

}  // close namespace bsl

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsFundamental
#undef bslmf_IsFundamental
#endif
#define bslmf_IsFundamental bslmf::IsFundamental
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
