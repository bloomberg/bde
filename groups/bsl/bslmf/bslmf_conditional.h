// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type transformation
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type transforming
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_enableif
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// that may be used to transform into one of the two given types based on its
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
// 'bool' value.  If the 'bool' value is 'true', the 'int' type is returned;
// otherwise the 'char' type is returned.
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
                           // ==================
                           // struct conditional
                           // ==================

template <bool COND, typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional
{
    // This 'struct' template implements a meta-function, 'bsl::conditional',
    // that transforms into one of the two given (template parameter) types,
    // 'TRUE_TYPE' and 'FALSE_TYPE', depending on value of the (template
    // parameter) 'COND'.  If 'COND' is 'true', this meta-function returns the
    // 'TRUE_TYPE', and returns the 'FALSE_TYPE' otherwise.  This generic
    // default template is instantiated when 'COND' is true.  There is another
    // partial specialization when 'COND' is false.

    typedef TRUE_TYPE type;
        // This 'typedef' defines the return type of this meta function.
};

template <typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE>
{
    // This partial specialization of 'bsl::conditional' defines a return type
    // to 'FALSE_TYPE' when it is instantiated to have the 'false' template
    // parameter value.

    typedef FALSE_TYPE type;
        // This 'typedef' defines the return type of this meta function.
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
