// bslmf_invokeresult.02.t.cpp                                        -*-C++-*-



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
// [2] FUNCTION INVOCABLES
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------



struct FuncTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, Args...>', where 'Fn'
    // is a function type, pointer-to-function type, reference to function
    // type, or reference to cv-qualified pointer to function type.

    template <class RT>
    static void apply(int LINE)
    {
        apply<RT, RT>(LINE);
    }

    template <class RT, class EXPECTED_RT>
    static void apply(int LINE)
        // Instantiate 'invoke_result' for function type 'RT(int)' and pointers
        // and references to that function type.  Verify that it yields 'RT' as
        // the result type.  Uses 'ASSERTV' to show the specified 'LINE'.
    {
        typedef RT Fn(int);

        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *const&, char>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn&&, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXPECTED_RT, Fn *&&, char>::value));
#endif
    }
};


// ============================================================================
//                      TEMPLATE PREINSTANTIATIONS
// ----------------------------------------------------------------------------

// On some compilers, the number of template instantiations within a single
// function are limited, so we pre-instantiate a number of templates to ensure
// the tests will compile.
//
// These macro calls enable templates to be forcibly instantiated in xlc before
// they are required, preventing them from getting re-instantiated when used
// and helping to prevent xlc getting overwhelmed by the number of templates.
// Note: use of explicit template instantiation syntax instead has been tried
// and does not prevent the overwhelming implicit instantiations.

PREINSTANTIATE_APR(FuncTest, void);
PREINSTANTIATE_APR(FuncTest, int);
PREINSTANTIATE_APR(FuncTest, float);
PREINSTANTIATE_APR(FuncTest, MyClass);
PREINSTANTIATE_APR(FuncTest, MyEnum);


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
      case 3: {
        referUserToElsewhere(test, verbose);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of function type,
        //:   'bsl::invoke_result' returns the return type of the function.
        //: 2 Concern 1 applies to functions returning
        //:    o 'void'
        //:    o built-in numeric types
        //:    o user-defined class and enumeration types
        //:    o pointer (possibly cv-qualified) to any of the above
        //:    o Pointer-to-function, reference-to-function, and
        //:      pointer-to-member function.  No recursion is expected.
        //:    o Reference to any of the (possibly cv-qualified) types above
        //:      except 'void', including reference-to-pointer types.
        //:    o Reference-to-array or (in C++11) rvalue-reference-to-array.
        //: 3 Concern 1 applies for pointers to functions taking 0 to 10
        //:   arguments, where some arguments are convertible to the function
        //:   argument rather than an exact match.
        //: 4 Concerns 1 to 3 apply equally for invocables of
        //:   pointer-to-function type.
        //: 5 Concerns 1 to 3 apply equally for invocables of (lvalue & rvalue)
        //:   reference-to-function type.
        //: 6 Results are not affected by arguments that decay (arrays,
        //:   functions), reference binding, or valid volatile arguments.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor template 'FuncTest' which
        //:   declares a function-returning-R type and verifies that applying
        //:   'bsl::invoke_result' to that type yields 'R'. Invoke this functor
        //:   through a function 'applyPtrAndRef' that adds every combination
        //:   of pointer and reference qualifiers to 'R'.
        //: 2 For concerns 1 and 3, instantiate 'bsl::invoke_result' on a
        //:   function type taking 0 to 10 arguments.  Concerns 2 and
        //:   3 do not interract, so it is not necessary to test every
        //:   combination of 0 to 10 arguments with every possible return
        //:   type.
        //: 3 For concern 4, extend 'FuncTest' to also test
        //:   pointer-to-function, reference to pointer-to-function, and
        //:   reference to 'const' pointer-to-function.  Also repeat step 2
        //:   with pointer-to-function types.
        //: 4 For concern 5, extend 'FuncTest' to also test (lvalue & rvalue)
        //:   reference-to-function.  Also repeat step 2 with
        //:   reference-to-function types.
        //: 5 For concern 6, invoke 'bsl::invoke_result' in a one-off fashion
        //:   with the argument categories listed in the concern and verify
        //:   correct operation.
        //
        // Testing:
        //     FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION INVOCABLES"
                            "\n===========================\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        BSLA_MAYBE_UNUSED typedef Ic01 (*Fp)(MyClass, int);
        BSLA_MAYBE_UNUSED typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Elem;
        typedef Elem Arry[10];
        typedef Ic04 F(MyClass, int);

        // Step 1, Concerns 1 & 2
        applyPtrAndRef<FuncTest, void>(L_);
        applyPtrAndRef<FuncTest, int>(L_);
        applyPtrAndRef<FuncTest, float>(L_);
        applyPtrAndRef<FuncTest, MyClass>(L_);
        applyPtrAndRef<FuncTest, MyEnum>(L_);

        // Arrays and functions cannot be returned by value, so we test only
        // references.  We can't apply a pointer or reference to something that
        // is already a reference, so simply call the 'apply' method directly.
        // MSVC 2019 and earlier fails to propagate the reference qualifier.
        // However, MSVC 2013 uses the C++03 implementation of
        // 'bsl::invoke_result', which correctly calculates the return type.

#if MSVC_2019 || MSVC_2017 || MSVC_2015
        FuncTest::apply<Arry&      , Arry>(L_);
        FuncTest::apply<Arry const&, Arry const>(L_);
#else
        FuncTest::apply<Arry&>(L_);
        FuncTest::apply<Arry const&>(L_);
#endif
        FuncTest::apply<F&>(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#if MSVC_2019 || MSVC_2017 || MSVC_2015
        FuncTest::apply<Arry&&, Arry>(L_);
#else
        FuncTest::apply<Arry&&>(L_);
#endif

        // Rvalue references to functions are special in that they are lvalues,
        // unlike rvalue references to other types, which are conditionally
        // either lvalues or xvalues.  MSVC 2019 and earlier appears to get
        // this wrong.
#if MSVC
        FuncTest::apply<F&&, F&&>(L_);
#else
        FuncTest::apply<F&&, F&>(L_);
#endif
#endif

        // Step 2, Concern 3
        typedef int        i;
        typedef ToLongLong q;
        typedef short      s;

        typedef Ic00 F00();
        typedef Ic01 F01(i);
        typedef Ic02 F02(i,i);
        typedef Ic03 F03(i,i,i);
        typedef Ic04 F04(i,i,i,i);
        typedef Ic05 F05(i,i,i,i,i);
        typedef Ic06 F06(i,i,i,i,i,i);
        typedef Ic07 F07(i,i,i,i,i,i,i);
        typedef Ic08 F08(i,i,i,i,i,i,i,i);
        typedef Ic09 F09(i,i,i,i,i,i,i,i,i);
        typedef Ic10 F10(i,i,i,i,i,i,i,i,i,i);


#define TEST(exp, ...) \
      ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        TEST(Ic00, F00                             );
        TEST(Ic01, F01,i                           );
        TEST(Ic02, F02,q,i                         );
        TEST(Ic03, F03,i,s,i                       );
        TEST(Ic04, F04,q,s,q,s                     );
        TEST(Ic05, F05,i,i,i,i,i                   );
        TEST(Ic06, F06,i,i,i,i,i,i                 );
        TEST(Ic07, F07,i,i,i,q,i,i,i               );
        TEST(Ic08, F08,i,q,i,i,i,i,i,s             );
        TEST(Ic09, F09,i,i,i,i,i,i,i,i,i           );
        TEST(Ic10, F10,s,i,i,i,i,i,i,i,i,i         );

        TEST(Ic00, F00*                             );
        TEST(Ic01, F01*,s                           );
        TEST(Ic02, F02*,i,q                         );
        TEST(Ic03, F03*,i,i,i                       );
        TEST(Ic04, F04*,i,i,i,i                     );
        TEST(Ic05, F05*,i,i,i,i,i                   );
        TEST(Ic06, F06*,s,q,s,q,s,q                 );
        TEST(Ic07, F07*,i,i,i,i,i,i,i               );
        TEST(Ic08, F08*,i,i,i,i,i,i,i,i             );
        TEST(Ic09, F09*,i,i,i,i,s,i,i,i,i           );
        TEST(Ic10, F10*,i,i,i,i,i,q,i,i,i,i         );


        TEST(Ic00, F00&                             );
        TEST(Ic01, F01&,i                           );
        TEST(Ic02, F02&,i,i                         );
        TEST(Ic03, F03&,q,s,q                       );
        TEST(Ic04, F04&,i,i,i,i                     );
        TEST(Ic05, F05&,i,i,i,i,i                   );
        TEST(Ic06, F06&,i,i,s,i,i,i                 );
        TEST(Ic07, F07&,i,i,i,q,i,s,i               );
        TEST(Ic08, F08&,i,i,i,i,i,i,i,i             );
        TEST(Ic09, F09&,i,q,i,s,i,q,i,s,i           );
        TEST(Ic10, F10&,i,i,i,i,i,i,i,i,i,i         );

        // Steps 3 & 4 incorporated into 'FuncTest' (step 1) and into step 2

        // Step 5, concern 6
        typedef Ic05 FArry(Ic03[6]);
        typedef Ic06 FArryUB(Ic03[]);
        typedef Ic07 FPtr(Ic03*);
        TEST(Ic04, F       , MyClass,        volatile int );
        TEST(Ic04, F       , MyDerivedClass, volatile int&);
        TEST(Ic04, F&      , MyClass,        volatile int );
        TEST(Ic04, F*      , MyClass,        volatile int );
        TEST(Ic05, FArry   , Arry                         );
        TEST(Ic05, FArry   , Arry&                        );
        TEST(Ic05, FArry&  , Arry                         );
        TEST(Ic05, FArry*  , Arry&                        );
        TEST(Ic06, FArryUB , Arry                         );
        TEST(Ic06, FArryUB , Arry&                        );
        TEST(Ic06, FArryUB&, Arry&                        );
        TEST(Ic06, FArryUB*, Arry                         );
        TEST(Ic07, FPtr    , Arry                         );
        TEST(Ic07, FPtr    , Arry&                        );
        TEST(Ic07, FPtr&   , Arry                         );
        TEST(Ic07, FPtr*   , Arry                         );
#undef TEST

      } break;
      case 1: {
        referUserToElsewhere(test, verbose);
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
