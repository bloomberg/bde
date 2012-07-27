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
//  bslmf::IsFundamental: meta-function for fundamental type checking
//
//@AUTHOR: Shawn Edwards (sedwards)
//
//@SEE_ALSO: bslmf_isenum, bslmf_ispointer
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a fundamental type.
// 'bslmf::IsFundamental' defines a member, 'VALUE', whose value is initialized
// (at compile-time) to 1 if the parameter is a fundamental type (ignoring any
// 'const' or 'volatile' qualification), and 0 if it is not.  Instantiating
// 'bslmf::IsFundamental' on a reference type is the same as instantiating it
// under underlying (non-reference) type.  Instantiating 'bslmf::IsFundamental'
// on a pointer type yields a 'VALUE' of zero.  (Pointers are not fundamental
// types, but see 'bslmf_ispointer'.)
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
// The following example shows the result of instantiating
// 'bslmf::IsFundamental' on a number of different types.
//..
//  struct MyType {};
//
//  static const int a1 = bslmf::IsFundamental<int>::VALUE;          // a1 == 1
//  static const int a1 = bslmf::IsFundamental<int&>::VALUE;         // a1 == 1
//  static const int a2 = bslmf::IsFundamental<const int>::VALUE;    // a2 == 1
//  static const int a3 = bslmf::IsFundamental<volatile int>::VALUE; // a3 == 1
//  static const int a4 = bslmf::IsFundamental<int *>::VALUE;        // a4 == 0
//  static const int a5 = bslmf::IsFundamental<MyType>::VALUE;       // a5 == 0
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
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

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_fundamental
    : BloombergLP::bslmf::IsFundamental_Imp<typename remove_cv<TYPE>::type>
{
    // This class implements a meta-function for checking if a type is
    // fundamental.
};

template <typename TYPE>
struct is_fundamental<TYPE&>
    : BloombergLP::bslmf::IsFundamental_Imp<typename remove_cv<TYPE>::type>
{
    // This specialization of 'is_fundamental' causes references to be treated
    // as their underlying (non-reference) types.
};

}

namespace BloombergLP {

namespace bslmf {

                         // ====================
                         // struct IsFundamental
                         // ====================

template <typename TYPE>
struct IsFundamental : MetaInt<bsl::is_fundamental<TYPE>::value>
{
    // This class implements a meta-function for checking if a type is
    // fundamental.  The static constant 'VALUE' member will be 1 if 'TYPE' is
    // fundamental and 0 otherwise.
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsFundamental
#undef bslmf_IsFundamental
#endif
#define bslmf_IsFundamental bslmf::IsFundamental
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
