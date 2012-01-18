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
//  bslmf_IsEnum: meta-function for detecting enumerated types
//
//@SEE_ALSO: bslmf_isfundamental
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's single type parameter is of enumeration type.
// 'bslmf_IsEnum' defines a 'VALUE' enumerator that is initialized
// (at compile-time) to 1 if the parameter is of enumeration type, and to 0
// otherwise.
//
///Usage
///-----
// For example:
//..
//  enum Enum { MY_ENUMERATOR = 5 };
//  class Class { Class(Enum); };
//
//  assert(1 == bslmf_IsEnum<Enum>::VALUE);
//  assert(0 == bslmf_IsEnum<Class>::VALUE);
//  assert(0 == bslmf_IsEnum<int>::VALUE);
//  assert(0 == bslmf_IsEnum<int *>::VALUE);
//..
// Note that the 'bslmf_IsEnum' meta-function also evaluates to true (i.e., 1)
// when applied to references to enumeration types:
//..
//   assert(1 == bslmf_IsEnum<const Enum&>::VALUE);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

                      // ====================================
                      // class bslmf_IsEnum_AnyArithmeticType
                      // ====================================

struct bslmf_IsEnum_AnyArithmeticType {
    // This struct provides a type that is convertible from any arithmetic type
    // (integral, floating-point, or enumeration).  Converting any type to a
    // 'bslmf_IsEnum_AnyArithmeticType' is a user-defined conversion and cannot
    // be combined with any other implicit user-defined conversions.  Thus,
    // even class types that have conversion operators to arithmetic types will
    // not be implicitly convertible to 'bslmf_IsEnum_AnyArithmeticType'.
    //
    // This is a component-private 'struct'.  Do *not* use.

    // NOT IMPLEMENTED
    bslmf_IsEnum_AnyArithmeticType(wchar_t);
    bslmf_IsEnum_AnyArithmeticType(int);
    bslmf_IsEnum_AnyArithmeticType(unsigned int);
    bslmf_IsEnum_AnyArithmeticType(long);
    bslmf_IsEnum_AnyArithmeticType(unsigned long);
    bslmf_IsEnum_AnyArithmeticType(long long);
    bslmf_IsEnum_AnyArithmeticType(unsigned long long);
    bslmf_IsEnum_AnyArithmeticType(double);
    bslmf_IsEnum_AnyArithmeticType(long double);
        // Conversion constructor from any arithmetic type.  Note that it is
        // not necessary to provide overloads taking 'bool', 'char', or 'short'
        // because they are automatically promoted to 'int'; nor is a 'float'
        // overload needed because it is automatically promoted to 'double'.
        // Also note that the other variants are necessary because a conversion
        // from, e.g., a 'long double' to a 'double' does not take precedence
        // over a conversion from 'long double' to 'int' and, therefore, would
        // be ambiguous.
};

                        // ==================
                        // class bslmf_IsEnum
                        // ==================

template <class TYPE>
struct bslmf_IsEnum 
: bslmf_MetaInt<!bslmf_IsFundamental<TYPE>::VALUE
         && bslmf_IsConvertible<TYPE, bslmf_IsEnum_AnyArithmeticType>::VALUE> {
    // This struct provides a meta-function that computes, at compile time,
    // whether 'TYPE' is of enumeration type.  It derives from
    // 'bslmf_MetaInt<1>' if 'TYPE' is an enumeration type, or from
    // 'bslmf_MetaInt<0>' otherwise.
    //
    // Enumeration types are the only user-defined types that have the
    // characteristics of a native arithmetic type (i.e., they can be promoted
    // to 'int' without invoking user-defined conversions).  This class takes
    // advantage if this property to distinguish 'enum' types from class types
    // that are convertible to 'int'.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
