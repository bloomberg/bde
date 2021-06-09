// bslmf_invokeresult.01.t.cpp                                        -*-C++-*-



// ============================================================================
//                 INCLUDE STANDARD TEST MACHINERY FROM CASE 0
// ----------------------------------------------------------------------------

#define BSLMF_INVOKERESULT_00T_AS_INCLUDE
#include <bslmf_invokeresult.00.t.cpp>


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// See the test plan in 'bslmf_invokeresult.00.t.cpp'.
//
// This file contains the following test:
// [1] BREATHING TEST
//-----------------------------------------------------------------------------


// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

// These are defined as part of the standard machinery included from file
// bslmf_invokeresult.00.t.cpp
//
// We need to suppress the bde_verify error due to them not being in this file:
// BDE_VERIFY pragma: -TP19


// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

// These are defined as part of the standard machinery included from file
// bslmf_invokeresult.00.t.cpp
//
// We need to suppress the bde_verify error due to them not being in this file:
// BDE_VERIFY pragma: -TP19



using namespace BloombergLP;



// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int test = argc > 1 ? atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool verbose             = argc > 2;
    BSLA_MAYBE_UNUSED const bool veryVerbose         = argc > 3;
    BSLA_MAYBE_UNUSED const bool veryVeryVerbose     = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    using BloombergLP::bslmf::InvokeResultDeductionFailed;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // As test cases are defined elsewhere, we need to suppress bde_verify
    // warnings about missing test case comments/banners.
    // BDE_VERIFY pragma: push
    // BDE_VERIFY pragma: -TP05
    // BDE_VERIFY pragma: -TP17

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: BSLA_FALLTHROUGH;
      case 7: BSLA_FALLTHROUGH;
      case 6: BSLA_FALLTHROUGH;
      case 5: BSLA_FALLTHROUGH;
      case 4: BSLA_FALLTHROUGH;
      case 3: BSLA_FALLTHROUGH;
      case 2: {
        referUserToElsewhere(test, verbose);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 'bsl::invoke_result<FT, ARGS...>' compiles for a sampling of
        //:   parameter types
        //
        // Plan:
        //: 1 Instantiate 'bsl::resulf_of' on a sampling of invocable types
        //:   and verify that the computed return type matches the expected
        //:   return type.
        //
        // Testing:
        //      BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

#define TEST(exp, ...) \
        ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, \
                exp>::value))

        typedef void (*f1)();
        typedef int (*f2)(int);
        typedef const int& (*f3)(bool, double);
        typedef volatile int& (*f4)(const char*, const int&, int*);
        typedef const volatile int& (*f5)(int, int, int, int);
        typedef ManyFunc (&f6)(bool);
        typedef double (MyClass::*g1)(int);
        typedef double& (MyClass::*g2)(volatile int);
        typedef char *MyClass::*dp;
        typedef const ManyFunc& ConstManyFunc;

        //   Expected             Arguments
        //   ===================  ==============================
        TEST(void               , f1                             );
        TEST(int                , f2, char                       );
        TEST(const int&         , f3, bool, float                );
        TEST(volatile int&      , f4, char*, int, int*           );
        TEST(const volatile int&, f5, short, int&, int, char     );
        TEST(ManyFunc           , f6, bool                       );
        TEST(double             , g1, MyClass, int               );
        TEST(double&            , g2, MyClass, int               );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST(char*&&            , dp, MyClass                    );
#else
        TEST(char*              , dp, MyClass                    );
#endif
        TEST(bool               , ManyFunc, float, void*         );
        TEST(float              , ConstManyFunc, float, void*    );

        TEST(void               , ManyFunc, MetaType<void>, int          );
        TEST(int *              , ManyFunc, MetaType<int *>, int         );
#ifndef BSLS_PLATFORM_CMP_SUN
        TEST(int *              , ManyFunc, MetaType<int *volatile>, int );
#endif
        TEST(int *volatile&     , ManyFunc, MetaType<int *volatile&>, int);
        TEST(const void *       , ManyFunc, MetaType<const void *>, int  );
        TEST(volatile void *const&, ManyFunc
                                , MetaType<volatile void *const&>, int   );
        TEST(const char*        , ManyFunc, MetaType<const char*>, int   );

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // BDE_VERIFY pragma: pop

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
