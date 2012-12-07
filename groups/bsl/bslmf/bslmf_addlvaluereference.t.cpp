// bslmf_addlvaluereference.t.cpp                                     -*-C++-*-
#include <bslmf_addlvaluereference.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bsl::add_lvalue_reference', which transform a template parameter 'TYPE' to
// its lvalue reference type.  Thus, we need to ensure that the value returned
// by this meta-functions is correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::add_lvalue_reference::type
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum   EnumTestType {
    // This user-defined 'enum' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.
};

union  UnionTestType {
    // This user-defined 'union' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.
};

class  BaseClassTestType {
    // This user-defined base class type is intended to be used during testing
    // as an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.
};

class  DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::add_lvalue_reference'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::add_lvalue_reference'.

}  // close unnamed namespace

#define ASSERT_ADD_LVALUE_REF_CVQ(TYPE1, TYPE2)                               \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<TYPE1>::type, TYPE2                   \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<const TYPE1>::type, const TYPE2       \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<volatile TYPE1>::type, volatile TYPE2 \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<const volatile TYPE1>::type,          \
              const volatile TYPE2                                            \
              >::value));

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transforming Types to Lvalue Reference Types
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a set of types to their lvalue reference
// types.
//
// Now, we instantiate the 'bsl::add_lvalue_reference' template for these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
//..
    ASSERT(true ==
          (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int&>::value));
    ASSERT(false ==
          (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int >::value));
    ASSERT(true ==
          (bsl::is_same<bsl::add_lvalue_reference<int&>::type,  int&>::value));
  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ASSERT(true ==
          (bsl::is_same<bsl::add_lvalue_reference<int&&>::type, int&>::value));
  #endif
//..
// Notice that the rvalue reference used above is a feature introduced in the
// C++11 standard and may not be supported by all compilers.

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_lvalue_reference::type'
        //   Ensure that the static data member 'type' of
        //   'bsl::add_lvalue_reference' instantiations having various
        //   (template parameter) 'TYPE' has the correct value.
        //
        // Concerns:
        //: 1 'add_lvalue_reference::type' correctly transforms 'TYPE' to its
        //:   lvalue reference type when 'TYPE' is an object or a function.
        //:
        //: 2 'add_lvalue_reference::type' does not transform 'TYPE' when
        //:   'TYPE' is already an lvalue reference type.
        //:
        //: 3 'add_lvalue_reference::type' transforms 'TYPE' to the lvalue
        //:   reference type of its underlying type when 'TYPE' is an rvalue
        //:   reference type.
        //
        // Plan:
        //   Instantiate 'bsl::add_lvalue_reference' with various types and
        //   verify that the 'type' member is initialized properly.
        //
        // Testing:
        //   bsl::add_lvalue_reference::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::add_lvalue_reference::type\n"
                            "===============================\n");

        // C-1

        ASSERT_ADD_LVALUE_REF_CVQ(int , int &);
        ASSERT_ADD_LVALUE_REF_CVQ(int*, int*&);

        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType , EnumTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType*, EnumTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType , StructTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType*, StructTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType , UnionTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType*, UnionTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType , BaseClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType*, BaseClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType ,
                                  DerivedClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType*,
                                  DerivedClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType , MethodPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType*, MethodPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType , FunctionPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType*, FunctionPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD , PMD &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD*, PMD*&);

        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::*,
                                  int StructTestType::*&);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* *,
                                  int StructTestType::* *&);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* * *,
                                  int StructTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::*,
                                  PMD BaseClassTestType::*&);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* *,
                                  PMD BaseClassTestType::* *&);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* * *,
                                  PMD BaseClassTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete , Incomplete &);
        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete*, Incomplete*&);

#ifndef BSLS_PLATFORM__CMP_IBM
        // Some cv-qualified function types are not compilable on AIX.

        typedef int F(int);
        ASSERT_ADD_LVALUE_REF_CVQ(F , F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*, F*&);
#endif

        // C-2

        ASSERT_ADD_LVALUE_REF_CVQ(int &, int &);
        ASSERT_ADD_LVALUE_REF_CVQ(int*&, int*&);

        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType &, EnumTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType*&, EnumTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType &, StructTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType*&, StructTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType& , UnionTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType*&, UnionTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType &, BaseClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType*&, BaseClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType &,
                                  DerivedClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType*&,
                                  DerivedClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType &, MethodPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType*&, MethodPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType &,
                                  FunctionPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType*&,
                                  FunctionPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD &, PMD &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD*&, PMD*&);

        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::*    &,
                                  int StructTestType::*    &);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* *  &,
                                  int StructTestType::* *  &);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* * *&,
                                  int StructTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::*    &,
                                  PMD BaseClassTestType::*    &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* *  &,
                                  PMD BaseClassTestType::* *  &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* * *&,
                                  PMD BaseClassTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete &, Incomplete &);
        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete*&, Incomplete*&);

#ifndef BSLS_PLATFORM__CMP_IBM
        // Some cv-qualified function types are not compilable on AIX.

        ASSERT_ADD_LVALUE_REF_CVQ(F &, F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*&, F*&);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        // C-3

        ASSERT_ADD_LVALUE_REF_CVQ(int &&, int &);
        ASSERT_ADD_LVALUE_REF_CVQ(int*&&, int*&);

        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType &&, EnumTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(EnumTestType*&&, EnumTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType &&, StructTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(StructTestType*&&, StructTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType &&, UnionTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(UnionTestType*&&, UnionTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType &&, BaseClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(BaseClassTestType*&&, BaseClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType &&,
                                  DerivedClassTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(DerivedClassTestType*&&,
                                  DerivedClassTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType &&, MethodPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(MethodPtrTestType*&&, MethodPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType &&,
                                  FunctionPtrTestType &);
        ASSERT_ADD_LVALUE_REF_CVQ(FunctionPtrTestType*&&,
                                  FunctionPtrTestType*&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD &&, PMD &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD*&&, PMD*&);

        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::*    &&,
                                  int StructTestType::*    &);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* *  &&,
                                  int StructTestType::* *  &);
        ASSERT_ADD_LVALUE_REF_CVQ(int StructTestType::* * *&&,
                                  int StructTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::*    &&,
                                  PMD BaseClassTestType::*    &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* *  &&,
                                  PMD BaseClassTestType::* *  &);
        ASSERT_ADD_LVALUE_REF_CVQ(PMD BaseClassTestType::* * *&&,
                                  PMD BaseClassTestType::* * *&);

        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete &&, Incomplete &);
        ASSERT_ADD_LVALUE_REF_CVQ(Incomplete*&&, Incomplete*&);

#ifndef BSLS_PLATFORM__CMP_IBM
        // Some cv-qualified function types are not compilable on AIX.

        ASSERT_ADD_LVALUE_REF_CVQ(F &&, F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*&&, F*&);
#endif

#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
