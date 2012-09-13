// bslmf_ispointer.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTER
#define INCLUDED_BSLMF_ISPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer types.
//
//@CLASSES:
//  bsl::is_pointer: meta-function for determining pointer types
//
//@SEE_ALSO: bslmf_integerconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a 'struct' templates, 'bsl::is_pointer',
// that may be used to query whether a type is a pointer type.
//
// 'bsl::is_pointer' meets the requirements of the 'is_pointer' template
// defined in the C++11 standard [meta.unary.cat].  'bsl::is_pointer' provides
// a single static data member, 'value', which has a integral value 1 if the
// (template parameter) 'TYPE' is a pointer type (but not a pointer to
// non-static members), and 0 otherwise.
//
// This component also provides another struct template, 'bslmf::IsPointer',
// that is functionality identical to 'bsl::is_pointer', except that the static
// data member of 'bslmf::IsPointer' is named 'VALUE' instead of 'value'.  Note
// that 'bslmf::IsPointer' is provided to preserve backward-compatibility and
// should not be used in new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Determining Pointer Types
/// - - - - - - - - - - - - - - - - - -
// Suppose that we want to determine a particular type is a pointer type.
//
// First, we create two 'typedef's -- a pointer type and a non-pointer type:
//..
//  typedef int MyType;
//  typedef int * MyPtrType;
//..
// Now, we instantiate the 'bsl::is_pointer' template for each of the
// 'typedef's and print the 'value' static data member of each instantiation:
//..
//  printf("MyType is_pointer: %d\n", bsl::is_pointer<MyType>::value);
//  printf("MyPtrType is_pointer: %d\n", bsl::is_pointer<MyPtrType>::value);
//..
// Finally, we observe the following output:
//..
//  MyType is_pointer: 0
//  MyPtrType is_pointer: 1
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {
                         // ====================
                         // struct IsPointer_Imp
                         // ====================

template <typename TYPE>
struct IsPointer_Imp : bsl::false_type {
// This 'struct' template provides a meta-function to determine whether the
// (template parameter) 'TYPE' is a non-const and non-volatile qualified
// pointer type.
};

                         // ====================
                         // struct IsPointer_Imp
                         // ====================

template <typename TYPE>
struct IsPointer_Imp<TYPE *> : bsl::true_type {
// This is a partial specialization of 'IsPointer_Imp' for pointer types.
};

}
}

namespace bsl {
                         // =================
                         // struct is_pointer
                         // =================

template <typename TYPE>
struct is_pointer
    : BloombergLP::bslmf::IsPointer_Imp<typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_pointer' template defined in
    // the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is a pointer.  This 'struct' contains a single static
    // data member, 'value', which has a integral value 1 if the (template
    // parameter) 'TYPE' is a pointer type (but not a pointer to non-static
    // members), and 0 otherwise.

};

}

namespace BloombergLP {
namespace bslmf {

                         // ================
                         // struct IsPointer
                         // ================

template <typename TYPE>
struct IsPointer : MetaInt<bsl::is_pointer<TYPE>::value> {
    // This 'struct' template implements a meta-function to determine if the
    // (template parameter) 'TYPE' is a pointer type.  This 'struct' contains a
    // single static data member, 'VALUE', which has a integral value 1 if the
    // (template parameter) 'TYPE' is a pointer type (but not a pointer to
    // non-static members), and 0 otherwise.
    //
    // Note that this 'struct' is functionality identical to 'bsl::is_pointer',
    // except that the static data member of this 'struct' is named 'VALUE'
    // instead of 'value'.  Also note that this 'struct' is mainly provided for
    // backward-compatibility and new components should use 'bsl::is_pointer'..
};

}  // close package namespace
}  // close enterprise namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslmf_IsPointer
#undef bslmf_IsPointer
#endif
#define bslmf_IsPointer bslmf::IsPointer
    // This alias is defined for backward compatibility.

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
