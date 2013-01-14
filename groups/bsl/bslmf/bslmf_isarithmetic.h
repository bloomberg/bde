// bslmf_isarithmetic.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARITHMETIC
#define INCLUDED_BSLMF_ISARITHMETIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining arithmetic types.
//
//@CLASSES:
//  bsl::is_arithmetic: standard meta-function for determining arithmetic types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_arithmetic',
// that may be used to query whether a template parameter type is an arithmetic
// type.
//
// 'bsl::is_arithmetic' meets the requirements of the 'is_arithmetic' template
// defined in the C++11 standard [meta.unary.comp].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Arithmetic Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are arithmetic types.
//
// Now, we instantiate the 'bsl::is_arithmetic' template for these types, and
// assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_arithmetic<int&>::value);
//  assert(false == bsl::is_arithmetic<int*>::value);
//  assert(true  == bsl::is_arithmetic<int >::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#include <bslmf_isfloatingpoint.h>
#endif

#ifndef INCLUDED_BSLMF_ISINTEGRAL
#include <bslmf_isintegral.h>
#endif

namespace bsl {
                         // ====================
                         // struct is_arithmetic
                         // ====================

template <class TYPE>
struct is_arithmetic
    : integral_constant<bool,
                        is_integral<TYPE>::value
                        || is_floating_point<TYPE>::value> {
    // This 'struct' template implements the 'is_arithmetic' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 'TYPE' is an arithmetic type.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is an arithmetic type,
    // and from 'bsl::false_type' otherwise.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
