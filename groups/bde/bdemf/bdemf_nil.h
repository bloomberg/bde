// bdemf_nil.h                -*-C++-*-
#ifndef INCLUDED_BDEMF_NIL
#define INCLUDED_BDEMF_NIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a nil type
//
//@DEPRECATED: Use 'bslmf_nil' instead.
//
//@CLASSES:
// bdemf_Nil  : class representing a nil (non-existent) type.
// bdemf_IsNil: meta-function to test for nil.
//
//@AUTHOR: Oleg Grunin (ogrunin)
//
//@SEE_ALSO:  bdemf_nil.h
//
//@DESCRIPTION: bdemf_Nil is not really a type but rather a marker
// representing an absence of type. This property is emphasized by
// the fact that bdemf_Nil is only a declaration. No definition of
// it exits anywhere and any attempt to instantiate it directly or
// indirectly will result in a compile error.
// It is useful for such things as terminating template recursions,
// providing defaults for template parameters, and other strictly
// compile time operations not directly resulting in generation
// of runtime code.
// The following example illustrates both mentioned usages of the
// nil type. bdemf_Inherit is a metafunction that 'returns' a type
// derived from all its template arguments. The variant presented
// bellow accepts at least 1 argument and up to the maximum of 3. Parameters
// T2, T3 default to bdemf_Nil. The function is recursive. It works by
// building on each successive iteration a new type derived from the function
// 1st parameter - T1 and the result of the function's next invocation
// for which the T1 parameter is dropped and the rested shifted to the left.
// The metafunction is specialized for the special case when all but the
// first argument become bdemf_Nil which stops the recurtion. bdemf_Nil
// itself never makes it into the inheritance tree. If it inadvertently did,
// the compiler would notice it first and loudly complain.
//..
//     template <class T1 = bdemf_Nil, class T2 = bdemf_Nil,
//                                                        class T2 = bdemf_Nil>
//     struct bdemf_Inherit {
//         struct TYPE : T1, bdemf_Inherit<T2, T3>::TYPE {};
//     };
//
//     template<class T1>
//     struct bdemf_Inherit<T1, bdemf_Nil, bdemf_Nil>
//     {
//         struct TYPE : T1 {};
//     };
//
//     struct A{}; struct B{}; struct C{};
//     typedef bdemf_Inherit<A, B, C>::TYPE ABCType; //ABCType is derived
//                                                   //from A, B, and C.
//..
// bdemf_IsNil meta-function 'returns' true (its sole static const
// boolean VALUE is set to true) if it's instantiated with bdemf_Nil,
// false for any other type.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef bdemf_Nil
#define bdemf_Nil   bslmf_Nil
#endif

#ifndef bdemf_IsNil
#define bdemf_IsNil bslmf_IsNil
#endif

namespace BloombergLP {

}  // close namespace BloombergLP

#endif //INCLUDED_BDEMF_NIL

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
