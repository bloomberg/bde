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
// bslmf_MemberFunctionPointerTraits: meta-function for detecting member
//   function pointer traits
//     bslmf_IsMemberFunctionPointer: meta-function to determine if a type is
//  a member function pointer
//
//@AUTHOR: Ilougino Rocha (irocha)
//
//@SEE_ALSO: bslmf_functionpointertraits
//
//@DESCRIPTION: This component provides meta-functions for determining the
// traits of a member function pointer.  Two meta-functions are provided:
// 'bslmf_IsMemberFunctionPointer', and 'bslmf_MemberFunctionPointerTraits'.
// 'bslmf_IsMemberFunctionPointer' tests if a given type is member function
// pointer.  'bslmf_MemberFunctionPointerTraits' determines the traits of a
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
// In order to deduce the types of 'voidFunc0' and 'func1', we will use the
// C++ template system to get two auxiliary functions:
//..
//  template <class TYPE>
//  void checkNotMemberFunctionPointer(TYPE object)
//  {
//      assert(0 == bslmf_IsMemberFunctionPointer<TYPE>::VALUE);
//  }
//
//  template <class BSLMF_RETURN, class ARGS, class TYPE>
//  void checkMemberFunctionPointer(TYPE object)
//  {
//      assert(1 == bslmf_IsMemberFunctionPointer<TYPE>::VALUE);
//      typedef typename bslmf_MemberFunctionPointerTraits<TYPE>::ResultType
//          ResultType;
//      typedef typename bslmf_MemberFunctionPointerTraits<TYPE>::ArgumentList
//          ArgumentList;
//      assert(1 == (bslmf_IsSame<ResultType, BSLMF_RETURN>::VALUE));
//      assert(1 == (bslmf_IsSame<ArgumentList, ARGS>::VALUE));
//  }
//..
// The following program should compile and run without errors:
//..
//  void usageExample()
//  {
//      assert(0 == bslmf_IsMemberFunctionPointer<int>::VALUE);
//      assert(0 == bslmf_IsMemberFunctionPointer<int>::VALUE);
//
//      checkNotMemberFunctionPointer(&MyTestClass::voidFunc0);
//      checkMemberFunctionPointer<int, bslmf_TypeList1<int> >(
//                                                        &MyTestClass::func1);
//      checkMemberFunctionPointer<int, bslmf_TypeList2<int, int> >(
//                                                        &MyTestClass::func2);
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

namespace BloombergLP {

template <class PROTOTYPE, class TEST_PROTOTYPE>
struct bslmf_MemberFunctionPointerTraitsImp;

                  // =======================================
                  // class bslmf_MemberFunctionPointerTraits
                  // =======================================

template <class PROTOTYPE>
struct bslmf_MemberFunctionPointerTraits
    : public bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,PROTOTYPE> {
};

                    // ===================================
                    // class bslmf_IsMemberFunctionPointer
                    // ===================================

template <class PROTOTYPE>
struct bslmf_IsMemberFunctionPointer {
    // This template determines if the specified 'PROTOTYPE' is a member
    // function pointer.  VALUE is defined as 1 if the specified 'PROTOTYPE'
    // is a member function, and a zero value otherwise.

    enum {
        VALUE =
         (bslmf_MemberFunctionPointerTraits<PROTOTYPE>::IS_MEMBER_FUNCTION_PTR)
    };
};

// ---- Anything below this line is implementation specific.  Do not use. ----

             // --------------------------------------------------
             // class bslmf_MemberFunctionPointerTraits__ClassType
             // --------------------------------------------------

template <class PROTOTYPE,
          class BSLMF_RETURN,
          class TYPE,
          class ARG1  = int, class ARG2  = int, class ARG3  = int,
          class ARG4  = int, class ARG5  = int, class ARG6  = int,
          class ARG7  = int, class ARG8  = int, class ARG9  = int,
          class ARG10 = int, class ARG11 = int, class ARG12 = int,
          class ARG13 = int, class ARG14 = int>
class bslmf_MemberFunctionPointerTraits__ClassType {
    // This 'struct' determines whether the specified 'PROTOTYPE' is a const or
    // non-const member function of the specified 'TYPE'.

    struct bslmf_True { char dummy[2]; };
    struct bslmf_False { char dummy[1]; };

    static bslmf_False test(BSLMF_RETURN(TYPE::*)());
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8,ARG9));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                  ARG6,ARG7,ARG8,ARG9,ARG10));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                             ARG6,ARG7,ARG8,ARG9,ARG10,ARG11));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                       ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                 ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13));
    static bslmf_False test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                           ARG6,ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14));

    static bslmf_True test(BSLMF_RETURN(TYPE::*)() const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,
                                                 ARG5) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,
                                                 ARG6) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8,ARG9) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                                 ARG7,ARG8,ARG9,ARG10) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                            ARG7,ARG8,ARG9,ARG10,ARG11) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                      ARG7,ARG8,ARG9,ARG10,ARG11,ARG12) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                                ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13) const);
    static bslmf_True test(BSLMF_RETURN(TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,
                          ARG7,ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14) const);

  public:
    // TYPES
    enum {
        IS_CONST = sizeof(test((PROTOTYPE)0)) - sizeof(bslmf_False)
    };

    typedef typename bslmf_If<IS_CONST, const TYPE, TYPE>::Type Type;
        // The underlying class type of the specified 'PROTOTYPE', either
        // 'TYPE' or 'const TYPE' as determined by .
};

             // -------------------------------------------------
             // class bslmf_MemberFunctionPointerTraits__NonConst
             // -------------------------------------------------

template <class PROTOTYPE>
struct bslmf_MemberFunctionPointerTraits__NonConst {
    // This template is specialized to determine the traits of the specified
    // 'PROTOTYPE'.  'PROTOTYPE' is expected to be of the form
    // 'BSLMF_RETURN (TYPE::*)(ARG1,ARG2,...) const'.

    enum {
        IS_MEMBER_FUNCTION_PTR = 0
    };
};

template <class BSLMF_RETURN, class TYPE>
struct bslmf_MemberFunctionPointerTraits__NonConst<BSLMF_RETURN (TYPE::*)()> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 0 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE            ClassType;
    typedef BSLMF_RETURN    ResultType;
    typedef bslmf_TypeList0 ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                                                BSLMF_RETURN (TYPE::*)(ARG1)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 1 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                  ClassType;
    typedef BSLMF_RETURN          ResultType;
    typedef bslmf_TypeList1<ARG1> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                                           BSLMF_RETURN (TYPE::*)(ARG1,ARG2)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 2 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                       ClassType;
    typedef BSLMF_RETURN               ResultType;
    typedef bslmf_TypeList2<ARG1,ARG2> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                                      BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 3 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                            ClassType;
    typedef BSLMF_RETURN                    ResultType;
    typedef bslmf_TypeList3<ARG1,ARG2,ARG3> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                                 BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 4 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                 ClassType;
    typedef BSLMF_RETURN                         ResultType;
    typedef bslmf_TypeList4<ARG1,ARG2,ARG3,ARG4> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                            BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 5 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                      ClassType;
    typedef BSLMF_RETURN                              ResultType;
    typedef bslmf_TypeList5<ARG1,ARG2,ARG3,ARG4,ARG5> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                       BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 6 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                           ClassType;
    typedef BSLMF_RETURN                                   ResultType;
    typedef bslmf_TypeList6<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6, class ARG7>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                  BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 7 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList7<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6, class ARG7, class ARG8>
struct bslmf_MemberFunctionPointerTraits__NonConst<
             BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 8 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList8<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                          ARG8> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6, class ARG7, class ARG8,
          class ARG9>
struct bslmf_MemberFunctionPointerTraits__NonConst<
        BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,ARG9)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 9 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList9<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                     ARG8,ARG9> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6, class ARG7, class ARG8,
          class ARG9, class ARG10>
struct bslmf_MemberFunctionPointerTraits__NonConst<
  BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,ARG9,ARG10)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 10 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                              ClassType;
    typedef BSLMF_RETURN                      ResultType;
    typedef bslmf_TypeList10<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                             ARG8,ARG9,ARG10> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5,  class ARG6, class ARG7, class ARG8,
          class ARG9, class ARG10, class ARG11>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                     BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                            ARG8,ARG9,ARG10,ARG11)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 11 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                 ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList11<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                          ARG8,ARG9,ARG10,ARG11> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE,  class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5,  class ARG6, class ARG7, class ARG8,
          class ARG9, class ARG10, class ARG11, class ARG12>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                     BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                            ARG8,ARG9,ARG10,ARG11,ARG12)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 12 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                 ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList12<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                    ARG8,ARG9,ARG10,ARG11,ARG12> ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5,  class ARG6,  class ARG7,  class ARG8,
          class ARG9, class ARG10, class ARG11, class ARG12, class ARG13>
struct bslmf_MemberFunctionPointerTraits__NonConst<
                  BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                         ARG8,ARG9, ARG10,ARG11,ARG12,ARG13)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 13 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                 ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList13<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                             ARG8,ARG9,ARG10,ARG11,ARG12,ARG13>  ArgumentList;
};

template <class BSLMF_RETURN, class TYPE, class ARG1, class ARG2, class ARG3,
          class ARG4, class ARG5, class ARG6, class ARG7, class ARG8,
          class ARG9, class ARG10, class ARG11, class ARG12, class ARG13,
          class ARG14>
struct bslmf_MemberFunctionPointerTraits__NonConst<
             BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                    ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14)> {
    // Specialization to determine the traits of member functions that
    // return 'BSLMF_RETURN' and accept 14 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef TYPE                                                  ClassType;
    typedef BSLMF_RETURN                                          ResultType;
    typedef bslmf_TypeList14<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                         ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14> ArgumentList;
};

                    // ------------------------------------------
                    // class bslmf_MemberFunctionPointerTraitsImp
                    // ------------------------------------------

template <class PROTOTYPE, class TEST_PROTOTYPE>
struct bslmf_MemberFunctionPointerTraitsImp :
    public bslmf_MemberFunctionPointerTraits__NonConst<PROTOTYPE> {
    // This template is specialized to determine the traits of the specified
    // 'PROTOTYPE'.  'PROTOTYPE' is expected to be of the form
    // 'BSLMF_RETURN (TYPE::*)(ARG1,ARG2,...)' or
    // 'BSLMF_RETURN (TYPE::*)(ARG1,ARG2,...) const'.  If a match of the type
    // 'BSLMF_RETURN (TYPE::*)(ARG1,ARG2,...)' is not found,
    // 'bslmf_MemberFunctionPointerTraitsImp' will be used to attempt to find
    // a const match.  Note that the const and non-const specialization are
    // separated since some compilers can't distinguish the two.
};

// SPECIALIZATIONS
template <class PROTOTYPE, class BSLMF_RETURN, class TYPE>
struct bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,
                                            BSLMF_RETURN (TYPE::*)() const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 0 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename bslmf_MemberFunctionPointerTraits__ClassType<
                               PROTOTYPE,BSLMF_RETURN,TYPE>::Type ClassType;
    typedef BSLMF_RETURN                                          ResultType;
    typedef bslmf_TypeList0                                       ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARG1>
struct bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,
                                          BSLMF_RETURN (TYPE::*)(ARG1) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 1 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                TYPE, ARG1>::Type ClassType;
    typedef BSLMF_RETURN          ResultType;
    typedef bslmf_TypeList1<ARG1> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2>
struct bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,
                                     BSLMF_RETURN (TYPE::*)(ARG1,ARG2) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 2 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
            TYPE, ARG1, ARG2>::Type    ClassType;
    typedef BSLMF_RETURN               ResultType;
    typedef bslmf_TypeList2<ARG1,ARG2> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE, class ARG1,
                                               class ARG2, class ARG3>
struct bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,
                                BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 3 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
            TYPE, ARG1, ARG2, ARG3>::Type   ClassType;
    typedef BSLMF_RETURN                    ResultType;
    typedef bslmf_TypeList3<ARG1,ARG2,ARG3> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4>
struct bslmf_MemberFunctionPointerTraitsImp<PROTOTYPE,
                           BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 4 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
            TYPE, ARG1, ARG2, ARG3, ARG4>::Type  ClassType;
    typedef BSLMF_RETURN                         ResultType;
    typedef bslmf_TypeList4<ARG1,ARG2,ARG3,ARG4> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5>
struct bslmf_MemberFunctionPointerTraitsImp<
                      PROTOTYPE,
                      BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 5 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
            TYPE, ARG1, ARG2, ARG3, ARG4, ARG5>::Type ClassType;
    typedef BSLMF_RETURN                              ResultType;
    typedef bslmf_TypeList5<ARG1,ARG2,ARG3,ARG4,ARG5> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6>
struct bslmf_MemberFunctionPointerTraitsImp<
                 PROTOTYPE,
                 BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 6 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
           TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6>::Type ClassType;
    typedef BSLMF_RETURN                                   ResultType;
    typedef bslmf_TypeList6<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7>
struct bslmf_MemberFunctionPointerTraitsImp<
            PROTOTYPE,
            BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 7 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                                      TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6,
                                            ARG7>::Type         ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList7<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8>
struct bslmf_MemberFunctionPointerTraitsImp<
                    PROTOTYPE,
                    BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                                ARG8) const > {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 8 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7, ARG8>::Type
                                                                ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList8<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                          ARG8> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8, class ARG9>
struct bslmf_MemberFunctionPointerTraitsImp<
                    PROTOTYPE,
                    BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                           ARG8,ARG9) const > {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 9 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                                TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
                                ARG8, ARG9>::Type               ClassType;
    typedef BSLMF_RETURN                                        ResultType;
    typedef bslmf_TypeList9<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                     ARG8,ARG9> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8, class ARG9, class ARG10>
struct bslmf_MemberFunctionPointerTraitsImp<
                PROTOTYPE,
                BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                                           ARG9,ARG10) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 10 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                                TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
                                      ARG8, ARG9, ARG10>::Type   ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList10<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                                ARG8,ARG9,ARG10> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8, class ARG9, class ARG10,
          class ARG11>
struct bslmf_MemberFunctionPointerTraitsImp<
                PROTOTYPE,
                BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                                     ARG9,ARG10,ARG11) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 11 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                              TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
                              ARG8, ARG9, ARG10, ARG11> ::Type   ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList11<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                          ARG8,ARG9,ARG10,ARG11> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8, class ARG9, class ARG10,
          class ARG11, class ARG12>
struct bslmf_MemberFunctionPointerTraitsImp<
                   PROTOTYPE,
                   BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                          ARG8,ARG9,ARG10,ARG11,ARG12) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 12 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                       TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
                       ARG8, ARG9, ARG10, ARG11, ARG12>::Type    ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList12<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                                    ARG8,ARG9,ARG10,ARG11,ARG12> ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1, class ARG2, class ARG3, class ARG4, class ARG5,
          class ARG6, class ARG7, class ARG8, class ARG9, class ARG10,
          class ARG11, class ARG12, class ARG13>
struct bslmf_MemberFunctionPointerTraitsImp<
                PROTOTYPE,
                BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                       ARG9,ARG10,ARG11,ARG12,ARG13) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 13 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
                  TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
                  ARG8, ARG9, ARG10, ARG11, ARG12, ARG13>::Type  ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList13<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                             ARG8,ARG9,ARG10,ARG11,ARG12,ARG13>  ArgumentList;
};

template <class PROTOTYPE, class BSLMF_RETURN, class TYPE,
          class ARG1,  class ARG2,  class ARG3,  class ARG4, class ARG5,
          class ARG6,  class ARG7,  class ARG8,  class ARG9, class ARG10,
          class ARG11, class ARG12, class ARG13, class ARG14>
struct bslmf_MemberFunctionPointerTraitsImp<
          PROTOTYPE,
          BSLMF_RETURN (TYPE::*)(ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,ARG8,
                                 ARG9,ARG10,ARG11,ARG12,ARG13,ARG14) const> {
    // Specialization to determine the traits of const member functions that
    // return 'BSLMF_RETURN' and accept 14 arguments.

    enum {
        IS_MEMBER_FUNCTION_PTR = 1
    };
    typedef typename
        bslmf_MemberFunctionPointerTraits__ClassType<PROTOTYPE, BSLMF_RETURN,
            TYPE, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6, ARG7,
            ARG8, ARG9, ARG10, ARG11, ARG12, ARG13, ARG14>::Type ClassType;
    typedef BSLMF_RETURN                                         ResultType;
    typedef bslmf_TypeList14<ARG1,ARG2,ARG3,ARG4,ARG5,ARG6,ARG7,
                        ARG8,ARG9,ARG10,ARG11,ARG12,ARG13,ARG14> ArgumentList;
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
