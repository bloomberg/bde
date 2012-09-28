// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type selection
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type selection
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// which may be used to tranform into one of the two given types based on its
// (template parameter) 'bool' type value.
//
// 'bsl::conditional' meets the requirements of the 'conditional' template
// defined in the C++11 standard [meta.trans.other].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditional Type Transformation Based on Boolean Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types, 'int' and 'char', based on
// 'bool' value.  If value is 'true', type 'int' is selected.  Otherwise type
// 'char' is selected.
//
// Now, we instantiate the 'bsl::conditional' template using 'int', 'char', and
// each of the two 'bool' values.  We use the 'bsl::is_same' meta-function to
// assert the 'type' static data member of each instantiation:
//..
//  assert(true ==
//      (bsl::is_same<bsl::conditional<true,  int, char>::type, int >::value));
//  assert(true ==
//      (bsl::is_same<bsl::conditional<false, int, char>::type, char>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

template <bool COND, typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional
{
    typedef TRUE_TYPE type;
};

template <typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE>
{
    typedef FALSE_TYPE type;
};

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
