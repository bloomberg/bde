// bslmf_invokeresult.03.t.cpp                                        -*-C++-*-



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
// [3] POINTER-TO-MEMBER-FUNCTION INVOCABLES
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




template <class T1 = MyClass>
struct PtrToMemFuncTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, Args...>', where 'Fn'
    // is a pointer-to-member-function type, or reference to cv-qualified
    // pointer-to-member-function type.

    template <class RT>
    static void apply(int LINE)
    {
        apply<RT, RT>(LINE);
    }

    template <class RT, class EXP>
    static void apply(int LINE)
        // Instantiate 'invoke_result' for function type
        // 'RT (MyClass::*Fp)(int);' and references to that function type.
        //
        // Verify that it yields 'RT' as the result type.  The specified 'LINE'
        // is passed into the 'ASSERTV' calls.
    {
        typedef RT (MyClass::*Fp)(int);

        ASSERTV(LINE, (IsInvokeResult<EXP, Fp, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp&, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp const&, T1, char>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp volatile&, T1, char>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, Fp const volatile&, T1, char>::value));

        typedef RT (MyClass::*CFp)(int) const;
        typedef RT (MyClass::*VFp)(int) volatile;
        typedef RT (MyClass::*CVFp)(int) const volatile;

        ASSERTV(LINE, (IsInvokeResult<EXP, CFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CFp, const T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VFp, volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, volatile T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CVFp, const T1, short>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, CVFp, const volatile T1, short>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<EXP, Fp&&, T1, char>::value));
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
        // Test ref-qualified member-function-pointers
        typedef RT (MyClass::*LFp)(int) &;
        typedef RT (MyClass::*CLFp)(int) const &;

        ASSERTV(LINE, (IsInvokeResult<EXP, LFp, T1&, int>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CLFp, T1&, int>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, CLFp, const T1&, int>::value));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef RT (MyClass::*RFp)(int) &&;
        typedef RT (MyClass::*VRFp)(int) volatile &&;

        ASSERTV(LINE, (IsInvokeResult<EXP, RFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, RFp, T1&&, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VRFp, T1, short>::value));
        ASSERTV(LINE, (IsInvokeResult<EXP, VRFp, T1&&, short>::value));
        ASSERTV(LINE,
                (IsInvokeResult<EXP, VRFp, volatile T1&&, short>::value));
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#endif // BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS
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

PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, void);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, int);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, float);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, MyClass);
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, MyEnum);

void applyPtrAndRefMemberFunctionTestForFunctionPointer(int LINE)
    // The number of template instantiations from a single call to
    // 'applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3Fp>' overwhelmed AIX, so
    // this function performs the first half of that call (covering
    // non-volatile functions). The specified 'LINE' is passed into the
    // 'apply()' calls for display using 'ASSERTV'.
{
    typedef Ic01 (*C3Fp)(MyClass, int);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3Fp>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3Fp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const C3Fp *>::apply(LINE);
}

void applyPtrAndRefMemberFunctionTestForFunctionPointerVolatile(int LINE)
    // The number of template instantiations from a single call to
    // 'applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3Fp>' overwhelmed AIX, so
    // this function performs the second half of that call (covering volatile
    // functions). The specified 'LINE' is passed into the 'apply()' calls for
    // display using 'ASSERTV'.
{
    typedef Ic01 (*C3Fp)(MyClass, int);
    ApplyRef<PtrToMemFuncTest<MyClass>, volatile C3Fp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const volatile C3Fp *>::apply(LINE);
}

void applyPtrAndRefMemberFunctionTestForMemberFunctionPointer(int LINE)
    // The number of template instantiations from a single call to
    // 'applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3MFp>' overwhelmed AIX, so
    // this function performs the first half of that call (covering
    // non-volatile functions). The specified 'LINE' is passed into the
    // 'apply()' calls for display using 'ASSERTV'.
{
    typedef Ic02 (MyClass::*C3MFp)(int);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3MFp>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, C3MFp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const C3MFp *>::apply(LINE);
}

void applyPtrAndRefMemberFunctionTestForMemberFunctionPointerVolatile(int LINE)
    // The number of template instantiations from a single call to
    // 'applyPtrAndRef<PtrToMemFuncTest<MyClass>, C3MFp>' overwhelmed AIX, so
    // this function performs the second half of that call (covering volatile
    // functions). The specified 'LINE' is passed into the 'apply()' calls for
    // display using 'ASSERTV'.
{
    typedef Ic02 (MyClass::*C3MFp)(int);
    ApplyRef<PtrToMemFuncTest<MyClass>, volatile C3MFp *>::apply(LINE);
    ApplyRef<PtrToMemFuncTest<MyClass>, const volatile C3MFp *>::apply(LINE);
}

PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, Ic01 (*)(MyClass, int));
PREINSTANTIATE_APR(PtrToMemFuncTest<MyClass>, Ic02 (MyClass::*)(int));


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
      case 4: {
        referUserToElsewhere(test, verbose);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of type pointer-to-member-function,
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
        //: 3 Given, 'bsl::invoke_result<F, T1, ...>', where 'F' is a pointer
        //:   to member function of class 'B' and class 'D' is either 'B' or a
        //:   class derived from 'B', 'T1' can be 'D', cv-qualified 'D&',
        //:   'bsl::reference_wrapper<D>', 'D*', or 'smart_ptr<D>' for some
        //:   smart-pointer template.
        //: 4 Concern 1 applies to member functions that are cv-qualified
        //:   and/or ref-qualified.
        //: 5 Concern 1 applies for pointers to member functions taking 0 to 9
        //:   arguments, where some of the arguments are convertible to the
        //:    function parameters, rather than being an exact match.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor template
        //:   'PtrToMemFuncTest' which declares a pointer to
        //:   member-function-returning-R type and verifies that applying
        //:   'bsl::invoke_result' to that type yields 'R'. Invoke this functor
        //:   through a function 'applyPtrAndRef' that adds every combination
        //:   of pointer and reference qualifiers to 'R'.
        //: 2 For concern 3, parameterize 'PtrToMemFuncTest' on the type of
        //:   the first argument. Repeat step 1, supplying different
        //:   first-argument types that comprise the types in the concern.
        //: 3 For concern 4, include in 'PtrToMemFuncTest' pointers to member
        //:   functions that are lvalue-ref-qualified and
        //:   rvalue-ref-qualified, with every combination of cv-qualifiers.
        //: 4 For concern 5, instantiate 'bsl::invoke_result' on
        //:   member-function-pointer types taking 0 to 9 arguments.
        //:   Concerns 2 and 3 do not interact, so it is not necessary to
        //:   test every combination of 0 to 9 arguments with every possible
        //:   return type.
        //
        // Testing:
        //     POINTER-TO-MEMBER-FUNCTION INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POINTER-TO-MEMBER-FUNCTION INVOCABLES"
                            "\n============================================="
                            "\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        typedef Ic01 (*Fp)(MyClass, int);
        typedef Ic02 (MyClass::*MFp)(int);
        typedef Ic03 Arry[10];
        typedef Ic04 F(MyClass, int);

        applyPtrAndRef<PtrToMemFuncTest<MyClass>, void   >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, int    >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, float  >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyClass>(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MyEnum >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, Fp     >(L_);
        applyPtrAndRef<PtrToMemFuncTest<MyClass>, MFp    >(L_);

        // Arrays and functions cannot be returned by value, so we test only
        // references.  Can't apply a pointer or reference to something that is
        // already a reference, so simply call the 'apply' method directly.
        // MSVC 2019 and earlier fails to propagate the reference qualifier.
        // However, MSVC 2013 uses the C++03 implementation of
        // 'bsl::invoke_result', which calculates the return type manually,
        // without 'decltype' machinery, and is correct.
#if MSVC_2019 || MSVC_2017 || MSVC_2015
        PtrToMemFuncTest<MyClass>::apply<Arry&      , Arry>(L_);
        PtrToMemFuncTest<MyClass>::apply<Arry const&, Arry const>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<Arry&>(L_);
        PtrToMemFuncTest<MyClass>::apply<Arry const&>(L_);
#endif
        PtrToMemFuncTest<MyClass>::apply<F&>(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#if MSVC_2019 || MSVC_2017 || MSVC_2015
        PtrToMemFuncTest<MyClass>::apply<Arry&&, Arry>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<Arry&&>(L_);
#endif

        // Rvalue references to functions are special in that they are lvalues,
        // unlike rvalue references to other types, which are conditionally
        // either lvalues or xvalues.  MSVC 2019 and earlier appears to get
        // this wrong.
#if MSVC
        PtrToMemFuncTest<MyClass>::apply<F&&, F&&>(L_);
#else
        PtrToMemFuncTest<MyClass>::apply<F&&, F&>(L_);
#endif
#endif

        typedef int        i;
        typedef ToLongLong q;
        typedef short      s;

        typedef Ic01 (MyClass::*Fp01)();
        typedef Ic02 (MyClass::*Fp02)(i);
        typedef Ic03 (MyClass::*Fp03)(i,i);
        typedef Ic04 (MyClass::*Fp04)(i,i,i);
        typedef Ic05 (MyClass::*Fp05)(i,i,i,i);
        typedef Ic05 (MyClass::*Fp05c)(i,i,i,i) const;
        typedef Ic05 (MyClass::*Fp05v)(i,i,i,i) volatile;
        typedef Ic05 (MyClass::*Fp05cv)(i,i,i,i) const volatile;
        typedef Ic06 (MyClass::*Fp06)(i,i,i,i,i);
        typedef Ic06 (MyClass::*Fp06c)(i,i,i,i,i) const;
        typedef Ic06 (MyClass::*Fp06v)(i,i,i,i,i) volatile;
        // typedef Ic06 (MyClass::*Fp06cv)(i,i,i,i,i) const volatile;
        typedef Ic07 (MyClass::*Fp07)(i,i,i,i,i,i);
        typedef Ic08 (MyClass::*Fp08)(i,i,i,i,i,i,i);
        typedef Ic09 (MyClass::*Fp09)(i,i,i,i,i,i,i,i) const;
        typedef Ic10 (MyClass::*Fp10)(i,i,i,i,i,i,i,i,i) const;
        // typedef Ic11 (MyClass::*Fp11)(i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic12 (MyClass::*Fp12)(i,i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic13 (MyClass::*Fp13)(i,i,i,i,i,i,i,i,i,i,i,i);
        //
        // typedef Ic14 (MyClass::*Fp14)(i,i,i,i,i,i,i,i,i,i,i,i,i);

#define TEST(exp, ...) \
      ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        TEST(Ic01, Fp01,MyClass                           );
        TEST(Ic02, Fp02,MyClass,q                         );
        TEST(Ic03, Fp03,MyClass,i,s                       );
        TEST(Ic04, Fp04,MyClass,s,i,q                     );
        TEST(Ic05, Fp05,MyClass,q,s,q,s                   );
        TEST(Ic05, Fp05c,MyClass,i,i,i,i                  );
        TEST(Ic05, Fp05v,MyClass,i,i,i,i                  );
        TEST(Ic05, Fp05cv,MyClass,i,i,i,i                 );
        TEST(Ic06, Fp06,MyClass,i,i,i,i,i                 );
        TEST(Ic07, Fp07,MyClass,i,i,i,i,i,i               );
        TEST(Ic08, Fp08,MyClass,i,i,i,i,i,i,i             );
        TEST(Ic09, Fp09,MyClass,i,i,i,i,i,i,i,i           );
        TEST(Ic10, Fp10,MyClass,i,i,i,i,i,i,i,i,q         );
        // TEST(Ic11, Fp11,MyClass,i,i,i,i,i,i,i,i,i,i       );
        //
        // TEST(Ic12, Fp12,MyClass,i,i,i,i,i,i,i,i,i,i,i     );
        //
        // TEST(Ic13, Fp13,MyClass,i,i,i,i,i,i,i,i,i,i,i,i   );
        //
        // TEST(Ic14, Fp14,MyClass,i,i,i,i,i,i,i,i,i,i,i,i,i );

        typedef SmartPtr<MyClass>              SpMc;
        typedef SmartPtr<MyDerivedClass>       SpMdc;
        typedef SmartPtr<const MyClass>        SpCMc;
        typedef SmartPtr<const MyDerivedClass> SpCMdc;

        typedef stub_reference_wrapper<MyClass>    Rr;

        TEST(Ic01, Fp01  , MyClass&                                  );
        TEST(Ic02, Fp02  , MyClass*,i                                );
        TEST(Ic03, Fp03  , MyDerivedClass,q,s                        );
        TEST(Ic04, Fp04  , MyDerivedClass*,i,i,i                     );
        TEST(Ic05, Fp05  , MyDerivedClass&,i,i,i,i                   );
        TEST(Ic05, Fp05c , MyDerivedClass&,i,i,i,i                  );
        TEST(Ic05, Fp05v , MyDerivedClass&,i,i,i,i                  );
        TEST(Ic05, Fp05cv, MyDerivedClass&,i,i,i,i                  );
        TEST(Ic05, Fp05c , const MyDerivedClass&,i,i,i,i            );
        TEST(Ic05, Fp05cv, const MyDerivedClass&,i,i,i,i           );
        TEST(Ic05, Fp05v , volatile MyDerivedClass&,i,i,i,i        );
        TEST(Ic05, Fp05cv, volatile MyDerivedClass&,i,i,i,i        );
        TEST(Ic05, Fp05cv, const volatile MyDerivedClass&,i,i,i,i  );
        TEST(Ic06, Fp06c , const MyClass&,i,i,i,i,i                  );
        TEST(Ic06, Fp06v , volatile MyClass&,i,i,i,i,i               );
        TEST(Ic07, Fp07  , SpMc,i,i,i,i,i,i                          );
        TEST(Ic08, Fp08  , SpMdc,i,i,q,i,i,i,i                       );
        TEST(Ic09, Fp09  , SpCMc,i,s,i,i,i,i,i,i                     );
        TEST(Ic10, Fp10  , SpCMdc,q,i,i,i,i,i,i,i,i                  );

        TEST(Ic01, Fp01  , Rr                                        );
        TEST(Ic02, Fp02  , Rr,i                                      );
        TEST(Ic03, Fp03  , Rr&,i,i                                   );
#undef TEST

      } break;
      case 2: BSLA_FALLTHROUGH;
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
