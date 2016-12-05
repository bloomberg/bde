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
struct MemberFunctionPointerTraits_Imp;
    // Forward declaration.

                  // =================================
                  // class MemberFunctionPointerTraits
                  // =================================

template <class PROTOTYPE>
struct MemberFunctionPointerTraits
    : public MemberFunctionPointerTraits_Imp<PROTOTYPE,PROTOTYPE> {
    // This metafunction determines the traits of a member function type,
    // including the type of the object that it is a member of, its result
    // type, and the type of its list of arguments.
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

             // -------------------------------------------
             // class MemberFunctionPointerTraits_ClassType
             // -------------------------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class...ARGS>
class MemberFunctionPointerTraits_ClassType {
    // This 'class' determines whether the specified 'PROTOTYPE' is a const or
    // volatile member function of the specified 'TYPE'.  The 'Type' member
    // will be a correctly const and/or volatile qualified version of 'TYPE'.
    // This metafunction is necessary because some old compilers do not
    // correctly dispatch to the correct partial specialization of
    // 'MemberFunctionPointerTraits_Imp' based on cv-qualification of the
    // member-function pointer.

    typedef bslmf::Tag<0> NonCVTag;    // non-const, non-volatile member func
    typedef bslmf::Tag<1> ConstTag;    // const member func
    typedef bslmf::Tag<2> VolTag;      // volatile member func
    typedef bslmf::Tag<3> ConstVolTag; // const volatile member func

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS...));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS...) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS...) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS...) const volatile);

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
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslmf_memberfunctionpointertraits.h
#ifndef BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#define BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT 14
#endif
#ifndef BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A
#define BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#endif
template <class PROTOTYPE,
          class BSLMF_RETURN,
          class TYPE
#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0
,
          class ARGS_0 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1
,
          class ARGS_1 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2
,
          class ARGS_2 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3
,
          class ARGS_3 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4
,
          class ARGS_4 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5
,
          class ARGS_5 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6
,
          class ARGS_6 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7
,
          class ARGS_7 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8
,
          class ARGS_8 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9
,
          class ARGS_9 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10
,
          class ARGS_10 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11
,
          class ARGS_11 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12
,
          class ARGS_12 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13
,
          class ARGS_13 = BSLS_COMPILERFEATURES_NILT
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13
,
          class = BSLS_COMPILERFEATURES_NILT>
class MemberFunctionPointerTraits_ClassType;

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)());
    static ConstTag test(BSLMF_RETURN(TYPE::*)() const);
    static VolTag test(BSLMF_RETURN(TYPE::*)() volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)() const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06,
                                                                     ARGS_07> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06,
                                             ARGS_07) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06,
                                                                     ARGS_07,
                                                                     ARGS_08> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06,
                                             ARGS_07,
                                             ARGS_08) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06,
                                                                     ARGS_07,
                                                                     ARGS_08,
                                                                     ARGS_09> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06,
                                             ARGS_07,
                                             ARGS_08,
                                             ARGS_09) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10
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
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06,
                                                                     ARGS_07,
                                                                     ARGS_08,
                                                                     ARGS_09,
                                                                     ARGS_10> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09,
                                               ARGS_10));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09,
                                               ARGS_10) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06,
                                             ARGS_07,
                                             ARGS_08,
                                             ARGS_09,
                                             ARGS_10) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11
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
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
                                                                     ARGS_02,
                                                                     ARGS_03,
                                                                     ARGS_04,
                                                                     ARGS_05,
                                                                     ARGS_06,
                                                                     ARGS_07,
                                                                     ARGS_08,
                                                                     ARGS_09,
                                                                     ARGS_10,
                                                                     ARGS_11> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09,
                                               ARGS_10,
                                               ARGS_11));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                               ARGS_02,
                                               ARGS_03,
                                               ARGS_04,
                                               ARGS_05,
                                               ARGS_06,
                                               ARGS_07,
                                               ARGS_08,
                                               ARGS_09,
                                               ARGS_10,
                                               ARGS_11) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                             ARGS_02,
                                             ARGS_03,
                                             ARGS_04,
                                             ARGS_05,
                                             ARGS_06,
                                             ARGS_07,
                                             ARGS_08,
                                             ARGS_09,
                                             ARGS_10,
                                             ARGS_11) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10,
                                                  ARGS_11) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12
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
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
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
                                                                     ARGS_12> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_12));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_12) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                             ARGS_12) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                                  ARGS_12) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13
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
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
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
                                                                     ARGS_13> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_13));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_13) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                             ARGS_13) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                                  ARGS_13) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 13

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14
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
class MemberFunctionPointerTraits_ClassType<PROTOTYPE, BSLMF_RETURN, TYPE,
                                                                     ARGS_01,
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
                                                                     ARGS_14> {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_14));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                               ARGS_14) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                             ARGS_14) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS_01,
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
                                                  ARGS_14) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_A >= 14

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class...ARGS>
class MemberFunctionPointerTraits_ClassType {

    typedef bslmf_Tag<0> NonCVTag;
    typedef bslmf_Tag<1> ConstTag;
    typedef bslmf_Tag<2> VolTag;
    typedef bslmf_Tag<3> ConstVolTag;

    static NonCVTag test(BSLMF_RETURN(TYPE::*)(ARGS...));
    static ConstTag test(BSLMF_RETURN(TYPE::*)(ARGS...) const);
    static VolTag test(BSLMF_RETURN(TYPE::*)(ARGS...) volatile);
    static ConstVolTag test(BSLMF_RETURN(TYPE::*)(ARGS...) const volatile);

  public:
    enum {
        IS_CONST    = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 1) != 0,
        IS_VOLATILE = (BSLMF_TAG_TO_UINT(test((PROTOTYPE)0)) & 2) != 0
    };

    typedef typename If<IS_CONST, const TYPE, TYPE>::Type CType;

    typedef typename If<IS_VOLATILE, volatile CType, CType>::Type Type;
};
// }}} END GENERATED CODE
#endif


                    // -------------------------------------
                    // class MemberFunctionPointerTraits_Imp
                    // -------------------------------------

template <class PROTOTYPE, class TEST_PROTOTYPE>
struct MemberFunctionPointerTraits_Imp {
    // Implementation of 'MemberFunctionPointerTraits', containing the actual
    // traits types.  This primary template is instantiated when 'PROTOTYPE'
    // does not match a pointer-to-member-function type. In actual use,
    // 'PROTOTYPE' and 'TEST_PROTOTYPE' are the same, but specializations
    // treat 'PROTOTYPE' as an opaque type and 'TEST_PROTOTYPE' as a pattern
    // match. Thiis redundancy is needed to work around some old compiler
    // bugs.

    enum {
        IS_MEMBER_FUNCTION_PTR = 0
    };
};

// SPECIALIZATIONS
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#ifndef BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#define BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT 14
#endif
#ifndef BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B
#define BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT
#endif
#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)()> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01)> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14


#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)() const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                      BSLMF_RETURN (TYPE::*)(ARGS_01) const> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14


#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)() volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                                    BSLMF_RETURN (TYPE::*)(ARGS_01) volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };

    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14


#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)() const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 0

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
                              BSLMF_RETURN (TYPE::*)(ARGS_01) const volatile> {

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename MemberFunctionPointerTraits_ClassType<PROTOTYPE,
          BSLMF_RETURN, TYPE, ARGS_01>::Type ClassType;
    typedef BSLMF_RETURN                     ResultType;
    typedef typename TypeList<ARGS_01>::Type ArgumentList;
};
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 1

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 2

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 3

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 4

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 5

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 6

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 7

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 8

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARGS_01,
                                                           class ARGS_02,
                                                           class ARGS_03,
                                                           class ARGS_04,
                                                           class ARGS_05,
                                                           class ARGS_06,
                                                           class ARGS_07,
                                                           class ARGS_08,
                                                           class ARGS_09>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 9

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 10

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 11

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 12

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 13

#if BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
#endif  // BSLMF_MEMBERFUNCTIONPOINTERTRAITS_VARIADIC_LIMIT_B >= 14

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class... ARGS>
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
struct MemberFunctionPointerTraits_Imp<PROTOTYPE,
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
