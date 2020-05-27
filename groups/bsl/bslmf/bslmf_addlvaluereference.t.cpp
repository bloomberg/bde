// bslmf_addlvaluereference.t.cpp                                     -*-C++-*-
#include <bslmf_addlvaluereference.h>

#include <bslmf_issame.h>  // for testing only

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines meta-functions, 'bsl::add_lvalue_reference'
// and 'bsl::add_lvalue_reference_t', which transform a template parameter
// 'TYPE' to its lvalue reference type.  Thus, we need to ensure that the value
// returned by this meta-functions is correct for each possible category of
// types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::add_lvalue_reference::type
// [ 1] bsl::add_lvalue_reference_t
//
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//              STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

//=============================================================================
//              COMPILER DEFECT MACROS TO GUIDE TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_ADDLVALUEREFERENCE_DO_NOT_TEST_CV_FUNCTIONS 1
    // IBM xlC compiler does not correctly ignore applying cv-qualifiers to
    // function types.  Note that this is different to testing "abominable"
    // function types, which have a cv-(ref-)qualifier denoting qualifiers on
    // the dereferenced 'this' pointer for a member function.
#endif

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentional creates types with unusual use of cv-qualifiers
// in order to confirm that there are no strange corners of the type system
// that are not addressed by this traits component.  Consequently, we disable
// certain warnings from common compilers.

#if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
# pragma GCC diagnostic ignored "-Wignored-qualifiers"
#elif defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#elif defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

//=============================================================================
//              GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
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
              bsl::add_lvalue_reference<               TYPE1>::type,          \
                                                       TYPE2                  \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<const          TYPE1>::type,          \
                                        const          TYPE2                  \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<      volatile TYPE1>::type,          \
                                              volatile TYPE2                  \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference<const volatile TYPE1>::type,          \
                                        const volatile TYPE2                  \
              >::value));
    // Test all cv-qualified combination on the specified 'TYPE1' and confirm
    // that the result type of the 'bsl::add_lvalue_reference' meta-function
    // and same cv-qualified 'TYPE2' are the same.

#define ASSERT_ADD_LVALUE_REF_T_CVQ(TYPE)                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference  <               TYPE>::type,         \
              bsl::add_lvalue_reference_t<               TYPE>                \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference  <const          TYPE>::type,         \
              bsl::add_lvalue_reference_t<const          TYPE>                \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference  <      volatile TYPE>::type,         \
              bsl::add_lvalue_reference_t<      volatile TYPE>                \
              >::value));                                                     \
    ASSERT(true == (bsl::is_same<                                             \
              bsl::add_lvalue_reference  <const volatile TYPE>::type,         \
              bsl::add_lvalue_reference_t<const volatile TYPE>                \
              >::value));
    // Test that the result types of the 'bsl::add_lvalue_reference' and
    // 'bsl::add_lvalue_reference_t' meta-functions are the same for all
    // cv-qualified combination on the specified 'TYPE'.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

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
//
// Finally, if the current compiler supports alias templates C++11 feature, we
// instantiate the 'bsl::add_lvalue_reference_t' template for the same set of
// types, and use the 'bsl::is_same' meta-function to assert the resultant type
// of each instantiation:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
    ASSERT(true  ==
              (bsl::is_same<bsl::add_lvalue_reference_t<int>,   int&>::value));
    ASSERT(false ==
              (bsl::is_same<bsl::add_lvalue_reference_t<int>,   int >::value));
    ASSERT(true  ==
              (bsl::is_same<bsl::add_lvalue_reference_t<int&>,  int&>::value));
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ASSERT(true ==
              (bsl::is_same<bsl::add_lvalue_reference_t<int&&>, int&>::value));
#endif
#endif
//..

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
        //:
        //: 4 'bsl::add_lvalue_reference_t' represents the return type of
        //:   'bsl::add_lvalue_reference' meta-function for a variety of
        //:   template parameter types.
        //
        // Plan:
        //  1 Instantiate 'bsl::add_lvalue_reference' with various types and
        //    verify that the 'type' member is initialized properly.
        //
        //  2 Verify that 'bsl::add_lvalue_reference_t' has the same type as
        //    the return type of 'bsl::add_lvalue_reference' for a variety of
        //    template parameter types. (C-4)
        //
        // Testing:
        //   bsl::add_lvalue_reference::type
        //   bsl::add_lvalue_reference_t
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

        typedef int F(int);

#ifndef BSLMF_ADDLVALUEREFERENCE_DO_NOT_TEST_CV_FUNCTIONS
        // Some cv-qualified function types are not compilable on AIX.

        ASSERT_ADD_LVALUE_REF_CVQ(F , F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*, F*&);
#else
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F>::type, F&>::value));
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F *>::type,
                                                       F *&>::value));
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

#ifndef BSLMF_ADDLVALUEREFERENCE_DO_NOT_TEST_CV_FUNCTIONS
        // Some cv-qualified function types are not compilable on AIX.

        ASSERT_ADD_LVALUE_REF_CVQ(F &, F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*&, F*&);
#else
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F&>::type, F&>::value));
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F *&>::type,
                                                       F *&>::value));
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

#ifndef BSLMF_ADDLVALUEREFERENCE_DO_NOT_TEST_CV_FUNCTIONS
        // Some cv-qualified function types are not compilable on AIX.

        ASSERT_ADD_LVALUE_REF_CVQ(F &&, F &);
        ASSERT_ADD_LVALUE_REF_CVQ(F*&&, F*&);
#else
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F&&>::type
                                                     , F&>::value));
        ASSERT((bsl::is_same<bsl::add_lvalue_reference<F *&&>::type,
                                                       F *&>::value));
#endif

#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

        if (verbose) printf("bsl::add_lvalue_reference_t\n"
                            "===========================\n");

        // C-4
        ASSERT_ADD_LVALUE_REF_T_CVQ(int);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType );
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType );
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType );
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType );
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* *);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* * *);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::*);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* *);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* * *);
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete );
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete*);

        ASSERT_ADD_LVALUE_REF_T_CVQ(int &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD*&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::*       &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* *     &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* * *   &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::*    &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* *  &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* * *&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete &);
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete*&);

        ASSERT_ADD_LVALUE_REF_T_CVQ(int &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(EnumTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(StructTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(UnionTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(BaseClassTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(DerivedClassTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(MethodPtrTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(FunctionPtrTestType*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD*&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::*       &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* *     &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(int StructTestType::* * *   &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::*    &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* *  &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(PMD BaseClassTestType::* * *&&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete &&);
        ASSERT_ADD_LVALUE_REF_T_CVQ(Incomplete*&&);

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
