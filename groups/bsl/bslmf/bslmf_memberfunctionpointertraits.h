// bslmf_memberfunctionpointertraits.h                                -*-C++-*-
#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#define INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide meta-functions to detect member function pointer traits.
//
//@CLASSES:
//  bslmf::MemberFunctionPointerTraits: meta-function for detecting member
//  function pointer traits
//  bslmf::IsMemberFunctionPointer: meta-function to determine if a type is
//  a member function pointer
//
//@SEE_ALSO: bslmf_functionpointertraits
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@DESCRIPTION: This component provides meta-functions for determining the
// traits of a member function pointer.  Two meta-functions are provided:
// 'bslmf::IsMemberFunctionPointer', and 'bslmf::MemberFunctionPointerTraits'.
// 'bslmf::IsMemberFunctionPointer' tests if a given type is member function
// pointer.  'bslmf::MemberFunctionPointerTraits' determines the traits of a
// member function type, including the type of the object that it is a member
// of, its result type, and the type of its list of arguments.
//
///Usage
///-----
// Define the following function types:
//..
//  typedef void (*VoidFunc0)();
//..
// and the following 'struct' with the following members:
//..
//  struct MyTestClass {
//      static void voidFunc0() {}
//      int func1(int) { return 0; }
//      int func2(int, int) { return 1; }
//  };
//..
// In order to deduce the types of 'voidFunc0' and 'func1', we will use the C++
// template system to get two auxiliary functions:
//..
//  template <class TYPE>
//  void checkNotMemberFunctionPointer(TYPE object)
//  {
//      assert(0 == bslmf::IsMemberFunctionPointer<TYPE>::value);
//  }
//
//  template <class BSLMF_RETURN, class ARGS, class TYPE>
//  void checkMemberFunctionPointer(TYPE object)
//  {
//      assert(1 == bslmf::IsMemberFunctionPointer<TYPE>::value);
//      typedef typename bslmf::MemberFunctionPointerTraits<TYPE>::ResultType
//          ResultType;
//      typedef typename bslmf::MemberFunctionPointerTraits<TYPE>::ArgumentList
//          ArgumentList;
//      assert(1 == (bsl::is_same<ResultType, BSLMF_RETURN>::value));
//      assert(1 == (bsl::is_same<ArgumentList, ARGS>::value));
//  }
//..
// The following program should compile and run without errors:
//..
//  void usageExample()
//  {
//      assert(0 == bslmf::IsMemberFunctionPointer<int>::value);
//      assert(0 == bslmf::IsMemberFunctionPointer<int>::value);
//
//      checkNotMemberFunctionPointer(&MyTestClass::voidFunc0);
//      checkMemberFunctionPointer<int, bslmf::TypeList1<int> >(
//                                                        &MyTestClass::func1);
//      checkMemberFunctionPointer<int, bslmf::TypeList2<int, int> >(
//                                                        &MyTestClass::func2);
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_TAG
#include <bslmf_tag.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

namespace BloombergLP {


namespace bslmf {

template <class PROTOTYPE, class TEST_PROTOTYPE>
struct MemberFunctionPointerTraitsImp;

                  // =================================
                  // class MemberFunctionPointerTraits
                  // =================================

template <class PROTOTYPE>
struct MemberFunctionPointerTraits
    : public MemberFunctionPointerTraitsImp<PROTOTYPE,PROTOTYPE> {
};

                    // =============================
                    // class IsMemberFunctionPointer
                    // =============================

template <class PROTOTYPE>
struct IsMemberFunctionPointer
    : bsl::integral_constant<
            bool,
            MemberFunctionPointerTraits<PROTOTYPE>::IS_MEMBER_FUNCTION_PTR> {
    // This template determines if the specified 'PROTOTYPE' is a member
    // function pointer.  'value' is defined as 1 if the specified 'PROTOTYPE'
    // is a member function, and a zero value otherwise.
};

// ---- Anything below this line is implementation specific.  Do not use. ----

             // --------------------------------------------
             // class MemberFunctionPointerTraits_ClassType
             // --------------------------------------------

template <class PROTOTYPE,
          class BSLMF_RETURN,
          class TYPE,
          class ARG1  = int, class ARG2  = int, class ARG3  = int,
          class ARG4  = int, class ARG5  = int, class ARG6  = int,
          class ARG7  = int, class ARG8  = int, class ARG9  = int,
          class ARG10 = int, class ARG11 = int, class ARG12 = int,
          class ARG13 = int, class ARG14 = int>
class MemberFunctionPointerTraits_ClassType {
    // This 'struct' determines whether the specified 'PROTOTYPE' is a const or
    // volatile member function of the specified 'TYPE'.  The 'Type' member
    // will be correctly const and/or volatile qualified version of 'TYPE'.
    // This metafunction is necessary because some old compilers do not
    // correctly dispatch to the correct partial specialization of
    // 'MemberFunctionPointerTraitsImp' based on cv-qualification of the
    // member-function pointer.

    typedef bslmf_Tag<0> NonCVTag;    // non-const, non-volatile member func
    typedef bslmf_Tag<1> ConstTag;    // const member func
    typedef bslmf_Tag<2> VolTag;      // volatile member func
    typedef bslmf_Tag<3> ConstVolTag; // const volatile member func

    static NonCVTag test(BSLMF_RETURN(TYPE::*)());
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8,ARG9));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8,ARG9,ARG10));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(
                                             ARG1,ARG2,ARG3,ARG4,ARG5,
                                             ARG6,ARG7,ARG8,ARG9,ARG10,ARG11));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(
                                       ARG1,ARG2,ARG3,ARG4,ARG5,
                                       ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(
                                 ARG1,ARG2,ARG3,ARG4,ARG5,
                                 ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13));
    static NonCVTag test(BSLMF_RETURN(TYPE::*)(
                           ARG1,ARG2,ARG3,ARG4,ARG5,
                           ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14));

    static ConstTag test(BSLMF_RETURN(TYPE::*)() const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,
                                                 ARG5) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                 ARG6) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8,ARG9) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8,ARG9,ARG10) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(
                                            ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                            ARG7,ARG8,ARG9,ARG10,ARG11) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(
                                      ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                      ARG7,ARG8,ARG9,ARG10,ARG11,ARG12) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(
                                ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13) const);
    static ConstTag test(BSLMF_RETURN(TYPE::*)(
                          ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                          ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14) const);

    static VolTag test(BSLMF_RETURN(TYPE::*)() volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,
                                                   ARG4) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,
                                                   ARG5) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                   ARG6) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                   ARG6,ARG7) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                   ARG6,ARG7,ARG8) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                ARG6,ARG7,ARG8,ARG9) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(
                                            ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                            ARG7,ARG8,ARG9,ARG10) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(
                                            ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                            ARG8,ARG9,ARG10,ARG11) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(
                                        ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                        ARG8,ARG9,ARG10,ARG11,ARG12) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(
                                       ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                       ARG9,ARG10,ARG11,ARG12,ARG13) volatile);
    static VolTag test(BSLMF_RETURN(TYPE::*)(
                                 ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                 ARG9,ARG10,ARG11,ARG12,ARG13,ARG14) volatile);

    static ConstVolTag test(BSLMF_RETURN(TYPE::*)() const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,
                                                   ARG3) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,
                                                   ARG4) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,
                                                   ARG5) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                   ARG6) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                   ARG6,ARG7) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                               ARG1,ARG2,ARG3,ARG4,ARG5,
                                               ARG6,ARG7,ARG8) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                               ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                               ARG7,ARG8,ARG9) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                         ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                         ARG7,ARG8,ARG9,ARG10) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                        ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                        ARG8,ARG9,ARG10,ARG11) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                       ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                       ARG9,ARG10,ARG11,ARG12) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                 ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                 ARG9,ARG10,ARG11,ARG12,ARG13) const volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(
                                ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,ARG9,
                                ARG10,ARG11,ARG12,ARG13,ARG14) const volatile);

  public:
    // TYPES
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;
        // Depending on 'IS_CONST', add or do not add a const qualifier to
        // 'TYPE'.

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
        // Depending on 'IS_VOLATILE', add or do not add a volatile qualifier
        // to 'TYPE'.
};


                    // ------------------------------------
                    // class MemberFunctionPointerTraitsImp
                    // ------------------------------------

template <class PROTOTYPE, class TEST_PROTOTYPE>
struct MemberFunctionPointerTraitsImp {

    enum {
        IS_MEMBER_FUNCTION_PTR = 0
    };
};

// SPECIALIZATIONS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS...)> {
    // Specialization to determine the traits of member functions.  A modern
    // compiler will match only non-cv member functions, but some older
    // compilers might match this to any member function.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS...) const> {
    // Specialization to determine the traits of member functions.  A modern
    // compiler will match only const member functions, but some older
    // compilers might match this to any member function.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS...) volatile> {
    // Specialization to determine the traits of member functions.  A modern
    // compiler will match only volatile member functions, but some older
    // compilers might match this to any member function.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS...) const volatile> {
    // Specialization to determine the traits of member functions.  A modern
    // compiler will match only const volatile member functions, but some older
    // compilers might match this to any member function.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslmf_memberfunctionpointertraits.h
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)()> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12,
                                                             ARGS_13)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13,
                                                           class ARGS_14>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12,
                                                             ARGS_13,
                                                             ARGS_14)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
};


template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)() const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12,
                                                             ARGS_13) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13,
                                                           class ARGS_14>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                             ARGS_02,
                                                             ARGS_03,
                                                             ARGS_04,
                                                             ARGS_05,
                                                             ARGS_06,
                                                             ARGS_07,
                                                             ARGS_08,
                                                             ARGS_09,
                                                             ARGS_10,
                                                             ARGS_11,
                                                             ARGS_12,
                                                             ARGS_13,
                                                             ARGS_14) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
};


template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)() volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09,
                                                           ARGS_10) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09,
                                                           ARGS_10,
                                                           ARGS_11) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09,
                                                           ARGS_10,
                                                           ARGS_11,
                                                           ARGS_12) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09,
                                                           ARGS_10,
                                                           ARGS_11,
                                                           ARGS_12,
                                                           ARGS_13) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13,
                                                           class ARGS_14>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                           ARGS_02,
                                                           ARGS_03,
                                                           ARGS_04,
                                                           ARGS_05,
                                                           ARGS_06,
                                                           ARGS_07,
                                                           ARGS_08,
                                                           ARGS_09,
                                                           ARGS_10,
                                                           ARGS_11,
                                                           ARGS_12,
                                                           ARGS_13,
                                                           ARGS_14) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
};


template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)() const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09,
                                                     ARGS_10) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09,
                                                     ARGS_10,
                                                     ARGS_11) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09,
                                                     ARGS_10,
                                                     ARGS_11,
                                                     ARGS_12) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09,
                                                     ARGS_10,
                                                     ARGS_11,
                                                     ARGS_12,
                                                     ARGS_13) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09,
                                                           class ARGS_10,
                                                           class ARGS_11,
                                                           class ARGS_12,
                                                           class ARGS_13,
                                                           class ARGS_14>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01,
                                                     ARGS_02,
                                                     ARGS_03,
                                                     ARGS_04,
                                                     ARGS_05,
                                                     ARGS_06,
                                                     ARGS_07,
                                                     ARGS_08,
                                                     ARGS_09,
                                                     ARGS_10,
                                                     ARGS_11,
                                                     ARGS_12,
                                                     ARGS_13,
                                                     ARGS_14) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01,
                              ARGS_02,
                              ARGS_03,
                              ARGS_04,
                              ARGS_05,
                              ARGS_06,
                              ARGS_07,
                              ARGS_08,
                              ARGS_09,
                              ARGS_10,
                              ARGS_11,
                              ARGS_12,
                              ARGS_13,
                              ARGS_14>::Type ArgumentList;
};

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS...)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS...) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS...) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraitsImp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS...) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS...>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS...>::Type ArgumentList;
};
// }}} END GENERATED CODE
#endif

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_MemberFunctionPointerTraits
#undef bslmf_MemberFunctionPointerTraits
#endif
#define bslmf_MemberFunctionPointerTraits bslmf::MemberFunctionPointerTraits
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsMemberFunctionPointer
#undef bslmf_IsMemberFunctionPointer
#endif
#define bslmf_IsMemberFunctionPointer bslmf::IsMemberFunctionPointer
    // This alias is defined for backward compatibility.

#ifdef bslmf_MemberFunctionPointerTraitsImp
#undef bslmf_MemberFunctionPointerTraitsImp
#endif
#define bslmf_MemberFunctionPointerTraitsImp  \
        bslmf::MemberFunctionPointerTraitsImp
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
