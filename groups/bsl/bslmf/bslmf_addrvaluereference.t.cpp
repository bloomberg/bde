// bslmf_addrvaluereference.t.cpp                                     -*-C++-*-
#include <bslmf_addrvaluereference.h>

#include <bslmf_issame.h>

#include <bsls_bsltestutil.h>

#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The object under test is a meta-function, 'bsl::add_rvalue_reference', which
// transform a template parameter 'TYPE' to its rvalue reference type.  Thus,
// we need to ensure that the value returned by this meta-functions is correct
// for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::add_rvalue_reference::type
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
    // 'bsl::add_rvalue_reference'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.
};

union  UnionTestType {
    // This user-defined 'union' type is intended to be used during testing as
    // an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.
};

class  BaseClassTestType {
    // This user-defined base class type is intended to be used during testing
    // as an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.
};

class  DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This non-static function member type is intended to be used during
    // testing as an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used during testing as an
    // argument as an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.

typedef int StructTestType::* PMD;
    // This class public data member pointer type is intended to be used during
    // testing as an argument as an argument for the template parameter 'TYPE'
    // of 'bsl::add_rvalue_reference'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used during testing as
    // an argument as an argument for the template parameter 'TYPE' of
    // 'bsl::add_rvalue_reference'.

}  // close unnamed namespace

#define ASSERT_RVALUE_REF_TRUE(TYPE)                                          \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<TYPE>::type, TYPE&&                   \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<const TYPE>::type, const TYPE&&       \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<volatile TYPE>::type, volatile TYPE&& \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<const volatile TYPE>::type,           \
              const volatile TYPE&&                                           \
              >::value));

#define ASSERT_RVALUE_REF_FALSE(TYPE)                                         \
    ASSERT(false == (bsl::is_same<                                            \
              bsl::add_rvalue_reference<TYPE>::type, TYPE                     \
              >::value));                                                     \
    ASSERT(false == (bsl::is_same<                                            \
              bsl::add_rvalue_reference<const TYPE>::type, const TYPE         \
              >::value));                                                     \
    ASSERT(false == (bsl::is_same<                                            \
              bsl::add_rvalue_reference<volatile TYPE>::type, volatile TYPE   \
              >::value));                                                     \
    ASSERT(false == (bsl::is_same<                                            \
              bsl::add_rvalue_reference<const volatile TYPE>::type,           \
              const volatile TYPE                                             \
              >::value));

#define ASSERT_RVALUE_REF_SAME(TYPE)                                          \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<TYPE>::type, TYPE                     \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<const TYPE>::type, const TYPE         \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<volatile TYPE>::type, volatile TYPE   \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_rvalue_reference<const volatile TYPE>::type,           \
              const volatile TYPE                                             \
              >::value));

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

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
///Example 1: Transform to Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a couple of types to rvalue reference
// types.
//
// Now, we instantiate the 'bsl::add_rvalue_reference' template for these
// types, and use the 'bsl::is_same' meta-function to assert the 'type' static
// data member of each instantiation:
//..
  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ASSERT(true  ==
         (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int&&>::value));
    ASSERT(false ==
         (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int  >::value));
    ASSERT(true  ==
         (bsl::is_same<bsl::add_rvalue_reference<int&>::type,  int& >::value));
    ASSERT(true  ==
         (bsl::is_same<bsl::add_rvalue_reference<int&&>::type, int&&>::value));
  #endif
//..
// Note that rvalue is introduced in C++11 and may not be supported by all
// compilers.  Note also that according to 'reference collapsing' semantics
// [8.3.2], 'add_rvalue_reference' does not transform 'TYPE' to rvalue
// reference type if 'TYPE' is a lvalue reference type.


      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::add_rvalue_reference::type'
        //   Ensure that the static data member 'type' of
        //   'bsl::add_rvalue_reference' instantiations having various
        //   (template parameter) 'TYPE' has the correct value.
        //
        // Concerns:
        //: 1 'add_rvalue_reference::type' correctly transforms 'TYPE' to its
        //:   rvalue reference type when 'TYPE' is an object or a function.
        //:
        //: 2 'add_rvalue_reference::type' does not transform 'TYPE' when
        //:   'TYPE' is neither an object nor function.
        //
        // Plan:
        //   Instantiate 'bsl::add_rvalue_reference' with various types and
        //   verify that the 'type' member is initialized properly.
        //
        // Testing:
        //   bsl::add_rvalue_reference::type
        // --------------------------------------------------------------------

        if (verbose) printf("bsl::add_rvalue_reference\n"
                            "=========================\n");

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

        ASSERT_RVALUE_REF_TRUE (int );
        ASSERT_RVALUE_REF_FALSE(int );
        ASSERT_RVALUE_REF_SAME (int&);
        ASSERT_RVALUE_REF_SAME (int&&);
        ASSERT_RVALUE_REF_TRUE (int* );
        ASSERT_RVALUE_REF_FALSE(int* );
        ASSERT_RVALUE_REF_SAME (int*&);
        ASSERT_RVALUE_REF_SAME (int*&&);

        ASSERT_RVALUE_REF_TRUE (EnumTestType );
        ASSERT_RVALUE_REF_FALSE(EnumTestType );
        ASSERT_RVALUE_REF_SAME (EnumTestType&);
        ASSERT_RVALUE_REF_SAME (EnumTestType&&);
        ASSERT_RVALUE_REF_TRUE (EnumTestType* );
        ASSERT_RVALUE_REF_FALSE(EnumTestType* );
        ASSERT_RVALUE_REF_SAME (EnumTestType*&);
        ASSERT_RVALUE_REF_SAME (EnumTestType*&&);

        ASSERT_RVALUE_REF_TRUE (StructTestType );
        ASSERT_RVALUE_REF_FALSE(StructTestType );
        ASSERT_RVALUE_REF_SAME (StructTestType&);
        ASSERT_RVALUE_REF_SAME (StructTestType&&);
        ASSERT_RVALUE_REF_TRUE (StructTestType* );
        ASSERT_RVALUE_REF_FALSE(StructTestType* );
        ASSERT_RVALUE_REF_SAME (StructTestType*&);
        ASSERT_RVALUE_REF_SAME (StructTestType*&&);

        ASSERT_RVALUE_REF_TRUE (UnionTestType );
        ASSERT_RVALUE_REF_FALSE(UnionTestType );
        ASSERT_RVALUE_REF_SAME (UnionTestType&);
        ASSERT_RVALUE_REF_SAME (UnionTestType&&);
        ASSERT_RVALUE_REF_TRUE (UnionTestType* );
        ASSERT_RVALUE_REF_FALSE(UnionTestType* );
        ASSERT_RVALUE_REF_SAME (UnionTestType*&);
        ASSERT_RVALUE_REF_SAME (UnionTestType*&&);

        ASSERT_RVALUE_REF_TRUE (BaseClassTestType );
        ASSERT_RVALUE_REF_FALSE(BaseClassTestType );
        ASSERT_RVALUE_REF_SAME (BaseClassTestType&);
        ASSERT_RVALUE_REF_SAME (BaseClassTestType&&);
        ASSERT_RVALUE_REF_TRUE (BaseClassTestType* );
        ASSERT_RVALUE_REF_FALSE(BaseClassTestType* );
        ASSERT_RVALUE_REF_SAME (BaseClassTestType*&);
        ASSERT_RVALUE_REF_SAME (BaseClassTestType*&&);

        ASSERT_RVALUE_REF_TRUE (DerivedClassTestType );
        ASSERT_RVALUE_REF_FALSE(DerivedClassTestType );
        ASSERT_RVALUE_REF_SAME (DerivedClassTestType&);
        ASSERT_RVALUE_REF_SAME (DerivedClassTestType&&);
        ASSERT_RVALUE_REF_TRUE (DerivedClassTestType* );
        ASSERT_RVALUE_REF_FALSE(DerivedClassTestType* );
        ASSERT_RVALUE_REF_SAME (DerivedClassTestType*&);
        ASSERT_RVALUE_REF_SAME (DerivedClassTestType*&&);

        ASSERT_RVALUE_REF_TRUE (MethodPtrTestType );
        ASSERT_RVALUE_REF_FALSE(MethodPtrTestType );
        ASSERT_RVALUE_REF_SAME (MethodPtrTestType&);
        ASSERT_RVALUE_REF_SAME (MethodPtrTestType&&);
        ASSERT_RVALUE_REF_TRUE (MethodPtrTestType* );
        ASSERT_RVALUE_REF_FALSE(MethodPtrTestType* );
        ASSERT_RVALUE_REF_SAME (MethodPtrTestType*&);
        ASSERT_RVALUE_REF_SAME (MethodPtrTestType*&&);

        ASSERT_RVALUE_REF_TRUE (FunctionPtrTestType );
        ASSERT_RVALUE_REF_FALSE(FunctionPtrTestType );
        ASSERT_RVALUE_REF_SAME (FunctionPtrTestType&);
        ASSERT_RVALUE_REF_SAME (FunctionPtrTestType&&);
        ASSERT_RVALUE_REF_TRUE (FunctionPtrTestType* );
        ASSERT_RVALUE_REF_FALSE(FunctionPtrTestType* );
        ASSERT_RVALUE_REF_SAME (FunctionPtrTestType*&);
        ASSERT_RVALUE_REF_SAME (FunctionPtrTestType*&&);

        ASSERT_RVALUE_REF_TRUE (PMD );
        ASSERT_RVALUE_REF_FALSE(PMD );
        ASSERT_RVALUE_REF_SAME (PMD&);
        ASSERT_RVALUE_REF_SAME (PMD&&);
        ASSERT_RVALUE_REF_TRUE (PMD* );
        ASSERT_RVALUE_REF_FALSE(PMD* );
        ASSERT_RVALUE_REF_SAME (PMD*&);
        ASSERT_RVALUE_REF_SAME (PMD*&&);

        ASSERT_RVALUE_REF_TRUE (int StructTestType::* );
        ASSERT_RVALUE_REF_FALSE(int StructTestType::* );
        ASSERT_RVALUE_REF_SAME (int StructTestType::*&);
        ASSERT_RVALUE_REF_SAME (int StructTestType::*&&);
        ASSERT_RVALUE_REF_TRUE (int StructTestType::* * );
        ASSERT_RVALUE_REF_FALSE(int StructTestType::* * );
        ASSERT_RVALUE_REF_SAME (int StructTestType::* *&);
        ASSERT_RVALUE_REF_SAME (int StructTestType::* *&&);

        ASSERT_RVALUE_REF_TRUE (int StructTestType::* * );
        ASSERT_RVALUE_REF_FALSE(int StructTestType::* * );
        ASSERT_RVALUE_REF_SAME (int StructTestType::* *&);
        ASSERT_RVALUE_REF_SAME (int StructTestType::* *&&);
        ASSERT_RVALUE_REF_TRUE (int StructTestType::* * * );
        ASSERT_RVALUE_REF_FALSE(int StructTestType::* * * );
        ASSERT_RVALUE_REF_SAME (int StructTestType::* * *&);
        ASSERT_RVALUE_REF_SAME (int StructTestType::* * *&&);

        ASSERT_RVALUE_REF_TRUE (PMD BaseClassTestType::* );
        ASSERT_RVALUE_REF_FALSE(PMD BaseClassTestType::* );
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::*&);
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::*&&);
        ASSERT_RVALUE_REF_TRUE (PMD BaseClassTestType::* * );
        ASSERT_RVALUE_REF_FALSE(PMD BaseClassTestType::* * );
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* *&);
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* *&&);

        ASSERT_RVALUE_REF_TRUE (PMD BaseClassTestType::* * );
        ASSERT_RVALUE_REF_FALSE(PMD BaseClassTestType::* * );
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* *&);
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* *&&);
        ASSERT_RVALUE_REF_TRUE (PMD BaseClassTestType::* * * );
        ASSERT_RVALUE_REF_FALSE(PMD BaseClassTestType::* * * );
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* * *&);
        ASSERT_RVALUE_REF_SAME (PMD BaseClassTestType::* * *&&);

        ASSERT_RVALUE_REF_TRUE (Incomplete );
        ASSERT_RVALUE_REF_FALSE(Incomplete );
        ASSERT_RVALUE_REF_SAME (Incomplete&);
        ASSERT_RVALUE_REF_SAME (Incomplete&&);
        ASSERT_RVALUE_REF_TRUE (Incomplete* );
        ASSERT_RVALUE_REF_FALSE(Incomplete* );
        ASSERT_RVALUE_REF_SAME (Incomplete*&);
        ASSERT_RVALUE_REF_SAME (Incomplete*&&);

#ifndef BSLS_PLATFORM__CMP_IBM
        // Some function types are not compilable on AIX.

        typedef int F(int);
        ASSERT_RVALUE_REF_TRUE (F);
        ASSERT_RVALUE_REF_FALSE(F);
        ASSERT_RVALUE_REF_SAME (F&);
        ASSERT_RVALUE_REF_SAME (F&&);
        ASSERT_RVALUE_REF_TRUE (F*);
        ASSERT_RVALUE_REF_FALSE(F*);
        ASSERT_RVALUE_REF_SAME (F*&);
        ASSERT_RVALUE_REF_SAME (F*&&);
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
