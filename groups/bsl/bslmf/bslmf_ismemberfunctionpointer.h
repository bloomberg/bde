// bslmf_ismemberfunctionpointer.h                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER
#define INCLUDED_BSLMF_ISMEMBERFUNCTIONPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for member function pointer types.
//
//@CLASSES:
//  bsl::is_member_function_pointer: standard meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_ismemberobjectpointer
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_member_function_pointer', that may be used to query whether a type
// is a pointer to non-static member function type.
//
// 'bsl::is_member_function_pointer' meets the requirements of the
// 'is_member_function_pointer' template defined in the C++11 standard
// [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Member Function Pointer Types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are pointers to
// non-static member function types.
//
// First, we create a user-defined type 'MyStruct':
//..
//  struct MyStruct
//  {
//  };
//..
// Now, we create two 'typedef's -- a function pointer type and a member
// function pointer type:
//..
//  typedef int (MyStruct::*MyStructMethodPtr) ();
//  typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_function_pointer' template for
// each of the 'typedef's and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_member_function_pointer<MyFunctionPtr    >::value);
//  assert(true  == bsl::is_member_function_pointer<MyStructMethodPtr>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

                     // ==================================
                     // struct IsMemberFunctionPointer_Imp
                     // ==================================

template <class PROTOTYPE>
struct IsMemberFunctionPointer_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a (non-cv-qualified) pointer to
    // non-static data member type.  This generic default template derives from
    // 'bsl::false_type'.  Template specializations are provided (below) that
    // derive from 'bsl::true_type'.
};

template <class RETURN, class CLASS>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)()> : bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes no arguments.
};

template <class RETURN, class CLASS, class ARG1>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1)> : bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes one argument.
};

template <class RETURN, class CLASS, class ARG1, class ARG2>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1, ARG2)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes two arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2, class ARG3>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1, ARG2, ARG3)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes three arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes four arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4, class ARG5>
struct IsMemberFunctionPointer_Imp<
                               RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes five arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6>
struct IsMemberFunctionPointer_Imp<
                         RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes six arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7>
struct IsMemberFunctionPointer_Imp<
                   RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes seven arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8>
struct IsMemberFunctionPointer_Imp<
             RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes eight arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8, ARG9)>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes nine arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9, class ARG10>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10)> : bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static member function type that takes ten arguments.
};

// Match pointer to 'const' member function:
template <class RETURN, class CLASS>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)() const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes no
    // arguments.
};

template <class RETURN, class CLASS, class ARG1>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes one
    // argument.
};

template <class RETURN, class CLASS, class ARG1, class ARG2>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes two
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2, class ARG3>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes three
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes four
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes five
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes six
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes seven
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes eight
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes nine
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9, class ARG10>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const' member function type that takes ten
    // arguments.
};

// Match pointer to 'volatile' member function:
template <class RETURN, class CLASS>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)() volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes no
    // arguments.
};

template <class RETURN, class CLASS, class ARG1>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)(ARG1) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes one
    // argument.
};

template <class RETURN, class CLASS, class ARG1, class ARG2>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) volatile> : bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes two
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2, class ARG3>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) volatile> : bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes three
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes four
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes five
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes six
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes seven
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes eight
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes nine
    // arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9, class ARG10>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'volatile' member function type that takes ten
    // arguments.
};

// Match pointer to 'const volatile' member function:
template <class RETURN, class CLASS>
struct IsMemberFunctionPointer_Imp<RETURN (CLASS::*)() const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member type function that takes
    // no arguments.
};

template <class RETURN, class CLASS, class ARG1>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // one argument.
};

template <class RETURN, class CLASS, class ARG1, class ARG2>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // two arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2, class ARG3>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // three arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // four arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // five arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5, ARG6) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // six arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // seven arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // eight arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // nine arguments.
};

template <class RETURN, class CLASS, class ARG1, class ARG2,
                                     class ARG3, class ARG4,
                                     class ARG5, class ARG6,
                                     class ARG7, class ARG8,
                                     class ARG9, class ARG10>
struct IsMemberFunctionPointer_Imp<
       RETURN (CLASS::*)(ARG1, ARG2, ARG3, ARG4, ARG5,
                         ARG6, ARG7, ARG8, ARG9, ARG10) const volatile>
: bsl::true_type {
    // This partial specialization of 'IsMemberFunctionPointer_Imp' derives
    // from 'bsl::true_type' for when the (template parameter) 'TYPE' is a
    // pointer to non-static 'const volatile' member function type that takes
    // ten arguments.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                       // =================================
                       // struct is_member_function_pointer
                       // =================================

template <class TYPE>
struct is_member_function_pointer
    : integral_constant<bool,
                        BloombergLP::bslmf::IsMemberFunctionPointer_Imp<
                            typename remove_cv<TYPE>::type>::value
                        && !is_reference<TYPE>::value> {
    // This 'struct' template implements the 'is_member_function_pointer'
    // meta-function defined in the C++11 standard [meta.unary.cat] to
    // determine if the (template parameter) 'TYPE' is a pointer to non-static
    // member function type.  This 'struct' derives from 'bsl::true_type' if
    // the 'TYPE' is a pointer to non-static member function type, and from
    // 'bsl::false_type' otherwise.
};

}  // close namespace bsl

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
