// bslmf_invokeresult.07.t.cpp                                        -*-C++-*-



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
// [7] C++17 SEMANTICS
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
      case 8: {
        referUserToElsewhere(test, verbose);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING C++17 SEMANTICS
        //
        // Concerns:
        //: 1 In C++11, 'bsl::invoke_result<F, ARGS...>' has a nested typedef
        //:   'type' if the expression 'INVOKE(f, args...)' is well-defined
        //:   according to the C++17 standard, given 'f' is an object of type
        //:   'F', and 'args...' are objects of type 'ARGS...', and has no such
        //:   typedef otherwise, for all types 'F' and 'ARGS...'.
        //
        // Plan:
        //: 1 For a large array of various 'const'-, 'volatile'-, and/or
        //:   reference-qualified invocable types, instantiate
        //:   'bsl::invoke_result' with that invocable type, and several
        //:   argument lists for which that type is and is not invocable.
        //:   Verify that 'bsl::invoke_result' has a nested typedef 'type' if
        //:   the invocable type is invocable, for each particular argument
        //:   list, and has no such typedef otherwise.
        //
        // Testing:
        //   C++17 SEMANTICS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING C++17 SEMANTICS"
                            "\n======================="
                            "\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        // Remove the "defined but not used" warning:
        ASSERT(true);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013

        typedef void                V;
        typedef int                 I;
        typedef const int           cI;
        typedef volatile int        vI;
        typedef const volatile int  cvI;
        typedef int&                rI;
        typedef const int&          crI;
        typedef volatile int&       vrI;
        typedef const volatile int& cvrI;

        typedef void(FV)(V);
        typedef void(FI)(I);
        typedef void(FrI)(rI);
        typedef void(FcrI)(crI);
        typedef void(FvrI)(vrI);
        typedef void(FcvrI)(cvrI);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        typedef int&& rrI;
        typedef void(FrrI)(rrI);
#endif

        typedef MyClass                 M;
        typedef const MyClass           cM;
        typedef volatile MyClass        vM;
        typedef const volatile MyClass  cvM;
        typedef MyClass&                rM;
        typedef const MyClass&          rcM;
        typedef volatile MyClass&       rvM;
        typedef const volatile MyClass& rcvM;

        typedef MyClass                   *pM;
        typedef const MyClass             *pcM;
        typedef volatile MyClass          *pvM;
        typedef const volatile MyClass    *pcvM;
        typedef MyClass *&                 rpM;
        typedef const MyClass *&           rpcM;
        typedef volatile MyClass *&        rpvM;
        typedef const volatile MyClass *&  rpcvM;

        typedef SmartPtr<MyClass>                sM;
        typedef SmartPtr<const MyClass>          scM;
        typedef SmartPtr<volatile MyClass>       svM;
        typedef SmartPtr<const volatile MyClass> scvM;

        typedef stub_reference_wrapper<MyClass>                wM;
        typedef stub_reference_wrapper<const MyClass>          wcM;
        typedef stub_reference_wrapper<volatile MyClass>       wvM;
        typedef stub_reference_wrapper<const volatile MyClass> wcvM;

        typedef void (M::*mFV)(V);
        typedef void (M::*cmFV)(V) const;
        typedef void (M::*vmFV)(V) volatile;
        typedef void (M::*cvmFV)(V) const volatile;

        typedef    I M::*mI;
        typedef   cI M::*cmI;
        typedef   vI M::*vmI;
        typedef  cvI M::*cvmI;

        typedef MyDerivedClass                 D;
        typedef const MyDerivedClass           cD;
        typedef volatile MyDerivedClass        vD;
        typedef const volatile MyDerivedClass  cvD;
        typedef MyDerivedClass&                rD;
        typedef const MyDerivedClass&          rcD;
        typedef volatile MyDerivedClass&       rvD;
        typedef const volatile MyDerivedClass& rcvD;

        static const bool YES = true;
        static const bool NO = false;

        const SfinaeFriendlinessTest TEST;

        //          'type' IS DEFINED                         LINE NUMBER
        //         .=================                         ===========.
        //        /    INVOCABLE TYPE          ARGUMENT TYPES             \.
        //       === ================== ================================  ==

        // Void functions
        TEST.run<YES,                FV                                 >(L_);
        TEST.run< NO,                FV,          I                     >(L_);

        // Functions taking one optionally const- and/or volatile-qualified
        // integer.
        TEST.run< NO,                FI                                 >(L_);
        TEST.run<YES,                FI,          I                     >(L_);
        TEST.run<YES,                FI,         cI                     >(L_);
        TEST.run<YES,                FI,         vI                     >(L_);
        TEST.run<YES,                FI,        cvI                     >(L_);
        TEST.run<YES,                FI,         rI                     >(L_);
        TEST.run<YES,                FI,        crI                     >(L_);
        TEST.run<YES,                FI,        vrI                     >(L_);
        TEST.run<YES,                FI,       cvrI                     >(L_);
        TEST.run< NO,                FI,          I,     I              >(L_);
        TEST.run< NO,               FrI                                 >(L_);
        TEST.run< NO,               FrI,          I                     >(L_);
        TEST.run< NO,               FrI,         cI                     >(L_);
        TEST.run< NO,               FrI,         vI                     >(L_);
        TEST.run< NO,               FrI,        cvI                     >(L_);
        TEST.run<YES,               FrI,         rI                     >(L_);
        TEST.run< NO,               FrI,        crI                     >(L_);
        TEST.run< NO,               FrI,        vrI                     >(L_);
        TEST.run< NO,               FrI,       cvrI                     >(L_);
        TEST.run< NO,               FrI,          I,     I              >(L_);
        TEST.run< NO,              FcrI                                 >(L_);
        TEST.run<YES,              FcrI,          I                     >(L_);
        TEST.run<YES,              FcrI,         cI                     >(L_);
        TEST.run< NO,              FcrI,         vI                     >(L_);
        TEST.run< NO,              FcrI,        cvI                     >(L_);
        TEST.run<YES,              FcrI,         rI                     >(L_);
        TEST.run<YES,              FcrI,        crI                     >(L_);
        TEST.run< NO,              FcrI,        vrI                     >(L_);
        TEST.run< NO,              FcrI,       cvrI                     >(L_);
        TEST.run< NO,              FcrI,          I,     I              >(L_);
        TEST.run< NO,              FvrI                                 >(L_);
        TEST.run< NO,              FvrI,          I                     >(L_);
        TEST.run< NO,              FvrI,         cI                     >(L_);
        TEST.run< NO,              FvrI,         vI                     >(L_);
        TEST.run< NO,              FvrI,        cvI                     >(L_);
        TEST.run<YES,              FvrI,         rI                     >(L_);
        TEST.run< NO,              FvrI,        crI                     >(L_);
        TEST.run<YES,              FvrI,        vrI                     >(L_);
        TEST.run< NO,              FvrI,       cvrI                     >(L_);
        TEST.run< NO,              FvrI,          I,     I              >(L_);
        TEST.run< NO,             FcvrI                                 >(L_);
        TEST.run<MSVC,            FcvrI,          I                     >(L_);
        TEST.run<MSVC,            FcvrI,         cI                     >(L_);
        TEST.run<MSVC,            FcvrI,         vI                     >(L_);
        TEST.run<MSVC,            FcvrI,        cvI                     >(L_);
        TEST.run<YES,             FcvrI,         rI                     >(L_);
        TEST.run<YES,             FcvrI,        crI                     >(L_);
        TEST.run<YES,             FcvrI,        vrI                     >(L_);
        TEST.run<YES,             FcvrI,       cvrI                     >(L_);

        // Functions taking one rvalue-reference-qualified integer.  Note that
        // this is the only section in this test case that considers rvalue
        // references, because this case is already as big enough as it is.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        TEST.run< NO,              FrrI                                 >(L_);
        TEST.run<YES,              FrrI,          I                     >(L_);
        TEST.run< NO,              FrrI,         cI                     >(L_);
        TEST.run< NO,              FrrI,         vI                     >(L_);
        TEST.run< NO,              FrrI,        cvI                     >(L_);
        TEST.run< NO,              FrrI,         rI                     >(L_);
        TEST.run< NO,              FrrI,        crI                     >(L_);
        TEST.run< NO,              FrrI,        vrI                     >(L_);
        TEST.run< NO,              FrrI,       cvrI                     >(L_);
        TEST.run<YES,              FrrI,        rrI                     >(L_);
#endif

        // Pointers to functions taking one optionally const- and/or
        // volatile-qualified integer.
        TEST.run< NO,              FI *                                 >(L_);
        TEST.run<YES,              FI *,          I                     >(L_);
        TEST.run<YES,              FI *,         cI                     >(L_);
        TEST.run<YES,              FI *,         vI                     >(L_);
        TEST.run<YES,              FI *,        cvI                     >(L_);
        TEST.run<YES,              FI *,         rI                     >(L_);
        TEST.run<YES,              FI *,        crI                     >(L_);
        TEST.run<YES,              FI *,        vrI                     >(L_);
        TEST.run<YES,              FI *,       cvrI                     >(L_);
        TEST.run< NO,              FI *,          I,     I              >(L_);
        TEST.run< NO,             FrI *                                 >(L_);
        TEST.run< NO,             FrI *,          I                     >(L_);
        TEST.run< NO,             FrI *,         cI                     >(L_);
        TEST.run< NO,             FrI *,         vI                     >(L_);
        TEST.run< NO,             FrI *,        cvI                     >(L_);
        TEST.run<YES,             FrI *,         rI                     >(L_);
        TEST.run< NO,             FrI *,        crI                     >(L_);
        TEST.run< NO,             FrI *,        vrI                     >(L_);
        TEST.run< NO,             FrI *,       cvrI                     >(L_);
        TEST.run< NO,             FrI *,          I,     I              >(L_);
        TEST.run< NO,            FcrI *                                 >(L_);
        TEST.run<YES,            FcrI *,          I                     >(L_);
        TEST.run<YES,            FcrI *,         cI                     >(L_);
        TEST.run< NO,            FcrI *,         vI                     >(L_);
        TEST.run< NO,            FcrI *,        cvI                     >(L_);
        TEST.run<YES,            FcrI *,         rI                     >(L_);
        TEST.run<YES,            FcrI *,        crI                     >(L_);
        TEST.run< NO,            FcrI *,        vrI                     >(L_);
        TEST.run< NO,            FcrI *,       cvrI                     >(L_);
        TEST.run< NO,            FcrI *,          I,     I              >(L_);
        TEST.run< NO,           FcvrI *                                 >(L_);
        TEST.run<MSVC,          FcvrI *,          I                     >(L_);
        TEST.run<MSVC,          FcvrI *,         cI                     >(L_);
        TEST.run<MSVC,          FcvrI *,         vI                     >(L_);
        TEST.run<MSVC,          FcvrI *,        cvI                     >(L_);
        TEST.run<YES,           FcvrI *,         rI                     >(L_);
        TEST.run<YES,           FcvrI *,        crI                     >(L_);
        TEST.run<YES,           FcvrI *,        vrI                     >(L_);
        TEST.run<YES,           FcvrI *,       cvrI                     >(L_);

        // References to functions taking one optionally const- and/or
        // volatile-qualified integer.
        TEST.run< NO,               FI&                                 >(L_);
        TEST.run<YES,               FI&,          I                     >(L_);
        TEST.run<YES,               FI&,         cI                     >(L_);
        TEST.run<YES,               FI&,         vI                     >(L_);
        TEST.run<YES,               FI&,        cvI                     >(L_);
        TEST.run<YES,               FI&,         rI                     >(L_);
        TEST.run<YES,               FI&,        crI                     >(L_);
        TEST.run<YES,               FI&,        vrI                     >(L_);
        TEST.run<YES,               FI&,       cvrI                     >(L_);
        TEST.run< NO,               FI&,          I,     I              >(L_);
        TEST.run< NO,              FrI&                                 >(L_);
        TEST.run< NO,              FrI&,          I                     >(L_);
        TEST.run< NO,              FrI&,         cI                     >(L_);
        TEST.run< NO,              FrI&,         vI                     >(L_);
        TEST.run< NO,              FrI&,        cvI                     >(L_);
        TEST.run<YES,              FrI&,         rI                     >(L_);
        TEST.run< NO,              FrI&,        crI                     >(L_);
        TEST.run< NO,              FrI&,        vrI                     >(L_);
        TEST.run< NO,              FrI&,       cvrI                     >(L_);
        TEST.run< NO,              FrI&,          I,     I              >(L_);
        TEST.run< NO,             FcrI&                                 >(L_);
        TEST.run<YES,             FcrI&,          I                     >(L_);
        TEST.run<YES,             FcrI&,         cI                     >(L_);
        TEST.run< NO,             FcrI&,         vI                     >(L_);
        TEST.run< NO,             FcrI&,        cvI                     >(L_);
        TEST.run<YES,             FcrI&,         rI                     >(L_);
        TEST.run<YES,             FcrI&,        crI                     >(L_);
        TEST.run< NO,             FcrI&,        vrI                     >(L_);
        TEST.run< NO,             FcrI&,       cvrI                     >(L_);
        TEST.run< NO,             FcrI&,          I,     I              >(L_);
        TEST.run< NO,            FcvrI&                                 >(L_);
        TEST.run<MSVC,           FcvrI&,          I                     >(L_);
        TEST.run<MSVC,           FcvrI&,         cI                     >(L_);
        TEST.run<MSVC,           FcvrI&,         vI                     >(L_);
        TEST.run<MSVC,           FcvrI&,        cvI                     >(L_);
        TEST.run<YES,            FcvrI&,         rI                     >(L_);
        TEST.run<YES,            FcvrI&,        crI                     >(L_);
        TEST.run<YES,            FcvrI&,        vrI                     >(L_);
        TEST.run<YES,            FcvrI&,       cvrI                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified, int-typed
        // member objects
        TEST.run< NO,                mI                                 >(L_);
        TEST.run<YES,                mI,          M                     >(L_);
        TEST.run< NO,                mI,          M,     I              >(L_);
        TEST.run<YES,                mI,         cM                     >(L_);
        TEST.run<YES,                mI,         vM                     >(L_);
        TEST.run<YES,                mI,        cvM                     >(L_);
        TEST.run<YES,                mI,         rM                     >(L_);
        TEST.run< NO,                mI,         rM,     I              >(L_);
        TEST.run<YES,                mI,        rcM                     >(L_);
        TEST.run<YES,                mI,        rvM                     >(L_);
        TEST.run<YES,                mI,       rcvM                     >(L_);
        TEST.run<YES,                mI,         pM                     >(L_);
        TEST.run< NO,                mI,         pM,     I              >(L_);
        TEST.run<YES,                mI,        pcM                     >(L_);
        TEST.run<YES,                mI,        pvM                     >(L_);
        TEST.run<YES,                mI,       pcvM                     >(L_);
        TEST.run<YES,                mI,        rpM                     >(L_);
        TEST.run< NO,                mI,        rpM,     I              >(L_);
        TEST.run<YES,                mI,       rpcM                     >(L_);
        TEST.run<YES,                mI,       rpvM                     >(L_);
        TEST.run<YES,                mI,      rpcvM                     >(L_);
        TEST.run<YES,                mI,         sM                     >(L_);
        TEST.run< NO,                mI,         sM,     I              >(L_);
        TEST.run<YES,                mI,        scM                     >(L_);
        TEST.run<YES,                mI,        svM                     >(L_);
        TEST.run<YES,                mI,       scvM                     >(L_);
        TEST.run<YES,                mI,         wM                     >(L_);
        TEST.run< NO,                mI,         wM,     I              >(L_);
        TEST.run<YES,                mI,        wcM                     >(L_);
        TEST.run<YES,                mI,        wvM                     >(L_);
        TEST.run<YES,                mI,       wcvM                     >(L_);
        TEST.run<YES,               cmI,          M                     >(L_);
        TEST.run< NO,               cmI,          M,     I              >(L_);
        TEST.run<YES,               cmI,         cM                     >(L_);
        TEST.run<YES,               cmI,         vM                     >(L_);
        TEST.run<YES,               cmI,        cvM                     >(L_);
        TEST.run<YES,               cmI,         rM                     >(L_);
        TEST.run< NO,               cmI,         rM,     I              >(L_);
        TEST.run<YES,               cmI,        rcM                     >(L_);
        TEST.run<YES,               cmI,        rvM                     >(L_);
        TEST.run<YES,               cmI,       rcvM                     >(L_);
        TEST.run<YES,               cmI,         pM                     >(L_);
        TEST.run< NO,               cmI,         pM,     I              >(L_);
        TEST.run<YES,               cmI,        pcM                     >(L_);
        TEST.run<YES,               cmI,        pvM                     >(L_);
        TEST.run<YES,               cmI,       pcvM                     >(L_);
        TEST.run<YES,               cmI,        rpM                     >(L_);
        TEST.run< NO,               cmI,        rpM,     I              >(L_);
        TEST.run<YES,               cmI,       rpcM                     >(L_);
        TEST.run<YES,               cmI,       rpvM                     >(L_);
        TEST.run<YES,               cmI,      rpcvM                     >(L_);
        TEST.run<YES,               cmI,         sM                     >(L_);
        TEST.run< NO,               cmI,         sM,     I              >(L_);
        TEST.run<YES,               cmI,        scM                     >(L_);
        TEST.run<YES,               cmI,        svM                     >(L_);
        TEST.run<YES,               cmI,       scvM                     >(L_);
        TEST.run<YES,               cmI,         wM                     >(L_);
        TEST.run< NO,               cmI,         wM,     I              >(L_);
        TEST.run<YES,               cmI,        wcM                     >(L_);
        TEST.run<YES,               cmI,        wvM                     >(L_);
        TEST.run<YES,               cmI,       wcvM                     >(L_);
        TEST.run<YES,               vmI,          M                     >(L_);
        TEST.run< NO,               vmI,          M,     I              >(L_);
        TEST.run<YES,               vmI,         cM                     >(L_);
        TEST.run<YES,               vmI,         vM                     >(L_);
        TEST.run<YES,               vmI,        cvM                     >(L_);
        TEST.run<YES,               vmI,         rM                     >(L_);
        TEST.run< NO,               vmI,         rM,     I              >(L_);
        TEST.run<YES,               vmI,        rcM                     >(L_);
        TEST.run<YES,               vmI,        rvM                     >(L_);
        TEST.run<YES,               vmI,       rcvM                     >(L_);
        TEST.run<YES,               vmI,         pM                     >(L_);
        TEST.run< NO,               vmI,         pM,     I              >(L_);
        TEST.run<YES,               vmI,        pcM                     >(L_);
        TEST.run<YES,               vmI,        pvM                     >(L_);
        TEST.run<YES,               vmI,       pcvM                     >(L_);
        TEST.run<YES,               vmI,        rpM                     >(L_);
        TEST.run< NO,               vmI,        rpM,     I              >(L_);
        TEST.run<YES,               vmI,       rpcM                     >(L_);
        TEST.run<YES,               vmI,       rpvM                     >(L_);
        TEST.run<YES,               vmI,      rpcvM                     >(L_);
        TEST.run<YES,               vmI,         sM                     >(L_);
        TEST.run< NO,               vmI,         sM,     I              >(L_);
        TEST.run<YES,               vmI,        scM                     >(L_);
        TEST.run<YES,               vmI,        svM                     >(L_);
        TEST.run<YES,               vmI,       scvM                     >(L_);
        TEST.run<YES,               vmI,         wM                     >(L_);
        TEST.run< NO,               vmI,         wM,     I              >(L_);
        TEST.run<YES,               vmI,        wcM                     >(L_);
        TEST.run<YES,               vmI,        wvM                     >(L_);
        TEST.run<YES,               vmI,       wcvM                     >(L_);
        TEST.run<YES,              cvmI,          M                     >(L_);
        TEST.run< NO,              cvmI,          M,     I              >(L_);
        TEST.run<YES,              cvmI,         cM                     >(L_);
        TEST.run<YES,              cvmI,         vM                     >(L_);
        TEST.run<YES,              cvmI,        cvM                     >(L_);
        TEST.run<YES,              cvmI,         rM                     >(L_);
        TEST.run< NO,              cvmI,         rM,     I              >(L_);
        TEST.run<YES,              cvmI,        rcM                     >(L_);
        TEST.run<YES,              cvmI,        rvM                     >(L_);
        TEST.run<YES,              cvmI,       rcvM                     >(L_);
        TEST.run<YES,              cvmI,         pM                     >(L_);
        TEST.run< NO,              cvmI,         pM,     I              >(L_);
        TEST.run<YES,              cvmI,        pcM                     >(L_);
        TEST.run<YES,              cvmI,        pvM                     >(L_);
        TEST.run<YES,              cvmI,       pcvM                     >(L_);
        TEST.run<YES,              cvmI,        rpM                     >(L_);
        TEST.run< NO,              cvmI,        rpM,     I              >(L_);
        TEST.run<YES,              cvmI,       rpcM                     >(L_);
        TEST.run<YES,              cvmI,       rpvM                     >(L_);
        TEST.run<YES,              cvmI,      rpcvM                     >(L_);
        TEST.run<YES,              cvmI,         sM                     >(L_);
        TEST.run< NO,              cvmI,         sM,     I              >(L_);
        TEST.run<YES,              cvmI,        scM                     >(L_);
        TEST.run<YES,              cvmI,        svM                     >(L_);
        TEST.run<YES,              cvmI,       scvM                     >(L_);
        TEST.run<YES,              cvmI,         wM                     >(L_);
        TEST.run< NO,              cvmI,         wM,     I              >(L_);
        TEST.run<YES,              cvmI,        wcM                     >(L_);
        TEST.run<YES,              cvmI,        wvM                     >(L_);
        TEST.run<YES,              cvmI,       wcvM                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified, int-typed
        // member objects, applied to a derived class type
        TEST.run<YES,                mI,          D                     >(L_);
        TEST.run< NO,                mI,          D,     I              >(L_);
        TEST.run<YES,                mI,         cD                     >(L_);
        TEST.run<YES,                mI,         vD                     >(L_);
        TEST.run<YES,                mI,        cvD                     >(L_);
        TEST.run<YES,                mI,         rD                     >(L_);
        TEST.run< NO,                mI,         rD,     I              >(L_);
        TEST.run<YES,                mI,        rcD                     >(L_);
        TEST.run<YES,                mI,        rvD                     >(L_);
        TEST.run<YES,                mI,       rcvD                     >(L_);
        TEST.run<YES,               cmI,          D                     >(L_);
        TEST.run< NO,               cmI,          D,     I              >(L_);
        TEST.run<YES,               cmI,         cD                     >(L_);
        TEST.run<YES,               cmI,         vD                     >(L_);
        TEST.run<YES,               cmI,        cvD                     >(L_);
        TEST.run<YES,               cmI,         rD                     >(L_);
        TEST.run< NO,               cmI,         rD,     I              >(L_);
        TEST.run<YES,               cmI,        rcD                     >(L_);
        TEST.run<YES,               cmI,        rvD                     >(L_);
        TEST.run<YES,               cmI,       rcvD                     >(L_);
        TEST.run<YES,               vmI,          D                     >(L_);
        TEST.run< NO,               vmI,          D,     I              >(L_);
        TEST.run<YES,               vmI,         cD                     >(L_);
        TEST.run<YES,               vmI,         vD                     >(L_);
        TEST.run<YES,               vmI,        cvD                     >(L_);
        TEST.run<YES,               vmI,         rD                     >(L_);
        TEST.run< NO,               vmI,         rD,     I              >(L_);
        TEST.run<YES,               vmI,        rcD                     >(L_);
        TEST.run<YES,               vmI,        rvD                     >(L_);
        TEST.run<YES,               vmI,       rcvD                     >(L_);
        TEST.run<YES,              cvmI,          D                     >(L_);
        TEST.run< NO,              cvmI,          D,     I              >(L_);
        TEST.run<YES,              cvmI,         cD                     >(L_);
        TEST.run<YES,              cvmI,         vD                     >(L_);
        TEST.run<YES,              cvmI,        cvD                     >(L_);
        TEST.run<YES,              cvmI,         rD                     >(L_);
        TEST.run< NO,              cvmI,         rD,     I              >(L_);
        TEST.run<YES,              cvmI,        rcD                     >(L_);
        TEST.run<YES,              cvmI,        rvD                     >(L_);
        TEST.run<YES,              cvmI,       rcvD                     >(L_);

        // Pointers to optionally const- and/or volatile-qualified member
        // functions.
        TEST.run< NO,               mFV                                 >(L_);
        TEST.run<YES,               mFV,          M                     >(L_);
        TEST.run< NO,               mFV,          M,     I              >(L_);
        TEST.run< NO,               mFV,         cM                     >(L_);
        TEST.run< NO,               mFV,         vM                     >(L_);
        TEST.run< NO,               mFV,        cvM                     >(L_);
        TEST.run<YES,               mFV,         rM                     >(L_);
        TEST.run< NO,               mFV,         rM,     I              >(L_);
        TEST.run< NO,               mFV,        rcM                     >(L_);
        TEST.run< NO,               mFV,        rvM                     >(L_);
        TEST.run< NO,               mFV,       rcvM                     >(L_);
        TEST.run<YES,               mFV,         pM                     >(L_);
        TEST.run< NO,               mFV,         pM,     I              >(L_);
        TEST.run< NO,               mFV,        pcM                     >(L_);
        TEST.run< NO,               mFV,        pvM                     >(L_);
        TEST.run< NO,               mFV,       pcvM                     >(L_);
        TEST.run<YES,               mFV,        rpM                     >(L_);
        TEST.run< NO,               mFV,        rpM,     I              >(L_);
        TEST.run< NO,               mFV,       rpcM                     >(L_);
        TEST.run< NO,               mFV,       rpvM                     >(L_);
        TEST.run< NO,               mFV,      rpcvM                     >(L_);
        TEST.run<YES,               mFV,         sM                     >(L_);
        TEST.run< NO,               mFV,         sM,     I              >(L_);
        TEST.run< NO,               mFV,        scM                     >(L_);
        TEST.run< NO,               mFV,        svM                     >(L_);
        TEST.run< NO,               mFV,       scvM                     >(L_);
        TEST.run<YES,               mFV,         wM                     >(L_);
        TEST.run< NO,               mFV,         wM,     I              >(L_);
        TEST.run< NO,               mFV,        wcM                     >(L_);
        TEST.run< NO,               mFV,        wvM                     >(L_);
        TEST.run< NO,               mFV,       wcvM                     >(L_);
        TEST.run<YES,              cmFV,          M                     >(L_);
        TEST.run< NO,              cmFV,          M,     I              >(L_);
        TEST.run<YES,              cmFV,         cM                     >(L_);
        TEST.run< NO,              cmFV,         vM                     >(L_);
        TEST.run< NO,              cmFV,        cvM                     >(L_);
        TEST.run<YES,              cmFV,         rM                     >(L_);
        TEST.run< NO,              cmFV,         rM,     I              >(L_);
        TEST.run<YES,              cmFV,        rcM                     >(L_);
        TEST.run< NO,              cmFV,        rvM                     >(L_);
        TEST.run< NO,              cmFV,       rcvM                     >(L_);
        TEST.run<YES,              cmFV,         pM                     >(L_);
        TEST.run< NO,              cmFV,         pM,     I              >(L_);
        TEST.run<YES,              cmFV,        pcM                     >(L_);
        TEST.run< NO,              cmFV,        pvM                     >(L_);
        TEST.run< NO,              cmFV,       pcvM                     >(L_);
        TEST.run<YES,              cmFV,        rpM                     >(L_);
        TEST.run< NO,              cmFV,        rpM,     I              >(L_);
        TEST.run<YES,              cmFV,       rpcM                     >(L_);
        TEST.run< NO,              cmFV,       rpvM                     >(L_);
        TEST.run< NO,              cmFV,      rpcvM                     >(L_);
        TEST.run<YES,              cmFV,         sM                     >(L_);
        TEST.run< NO,              cmFV,         sM,     I              >(L_);
        TEST.run<YES,              cmFV,        scM                     >(L_);
        TEST.run< NO,              cmFV,        svM                     >(L_);
        TEST.run< NO,              cmFV,       scvM                     >(L_);
        TEST.run<YES,              cmFV,         wM                     >(L_);
        TEST.run< NO,              cmFV,         wM,     I              >(L_);
        TEST.run<YES,              cmFV,        wcM                     >(L_);
        TEST.run< NO,              cmFV,        wvM                     >(L_);
        TEST.run< NO,              cmFV,       wcvM                     >(L_);
        TEST.run<YES,              vmFV,          M                     >(L_);
        TEST.run< NO,              vmFV,          M,     I              >(L_);
        TEST.run< NO,              vmFV,         cM                     >(L_);
        TEST.run<YES,              vmFV,         vM                     >(L_);
        TEST.run< NO,              vmFV,        cvM                     >(L_);
        TEST.run<YES,              vmFV,         rM                     >(L_);
        TEST.run< NO,              vmFV,         rM,     I              >(L_);
        TEST.run< NO,              vmFV,        rcM                     >(L_);
        TEST.run<YES,              vmFV,        rvM                     >(L_);
        TEST.run< NO,              vmFV,       rcvM                     >(L_);
        TEST.run<YES,              vmFV,         pM                     >(L_);
        TEST.run< NO,              vmFV,         pM,     I              >(L_);
        TEST.run< NO,              vmFV,        pcM                     >(L_);
        TEST.run<YES,              vmFV,        pvM                     >(L_);
        TEST.run< NO,              vmFV,       pcvM                     >(L_);
        TEST.run<YES,              vmFV,        rpM                     >(L_);
        TEST.run< NO,              vmFV,        rpM,     I              >(L_);
        TEST.run< NO,              vmFV,       rpcM                     >(L_);
        TEST.run<YES,              vmFV,       rpvM                     >(L_);
        TEST.run< NO,              vmFV,      rpcvM                     >(L_);
        TEST.run<YES,              vmFV,         sM                     >(L_);
        TEST.run< NO,              vmFV,         sM,     I              >(L_);
        TEST.run< NO,              vmFV,        scM                     >(L_);
        TEST.run<YES,              vmFV,        svM                     >(L_);
        TEST.run< NO,              vmFV,       scvM                     >(L_);
        TEST.run<YES,              vmFV,         wM                     >(L_);
        TEST.run< NO,              vmFV,         wM,     I              >(L_);
        TEST.run< NO,              vmFV,        wcM                     >(L_);
        TEST.run<YES,              vmFV,        wvM                     >(L_);
        TEST.run< NO,              vmFV,       wcvM                     >(L_);
        TEST.run<YES,             cvmFV,          M                     >(L_);
        TEST.run< NO,             cvmFV,          M,     I              >(L_);
        TEST.run<YES,             cvmFV,         cM                     >(L_);
        TEST.run<YES,             cvmFV,         vM                     >(L_);
        TEST.run<YES,             cvmFV,        cvM                     >(L_);
        TEST.run<YES,             cvmFV,         rM                     >(L_);
        TEST.run< NO,             cvmFV,         rM,     I              >(L_);
        TEST.run<YES,             cvmFV,        rcM                     >(L_);
        TEST.run<YES,             cvmFV,        rvM                     >(L_);
        TEST.run<YES,             cvmFV,       rcvM                     >(L_);
        TEST.run<YES,             cvmFV,         pM                     >(L_);
        TEST.run< NO,             cvmFV,         pM,     I              >(L_);
        TEST.run<YES,             cvmFV,        pcM                     >(L_);
        TEST.run<YES,             cvmFV,        pvM                     >(L_);
        TEST.run<YES,             cvmFV,       pcvM                     >(L_);
        TEST.run<YES,             cvmFV,        rpM                     >(L_);
        TEST.run< NO,             cvmFV,        rpM,     I              >(L_);
        TEST.run<YES,             cvmFV,       rpcM                     >(L_);
        TEST.run<YES,             cvmFV,       rpvM                     >(L_);
        TEST.run<YES,             cvmFV,      rpcvM                     >(L_);
        TEST.run<YES,             cvmFV,         sM                     >(L_);
        TEST.run< NO,             cvmFV,         sM,     I              >(L_);
        TEST.run<YES,             cvmFV,        scM                     >(L_);
        TEST.run<YES,             cvmFV,        svM                     >(L_);
        TEST.run<YES,             cvmFV,       scvM                     >(L_);
        TEST.run<YES,             cvmFV,         wM                     >(L_);
        TEST.run< NO,             cvmFV,         wM,     I              >(L_);
        TEST.run<YES,             cvmFV,        wcM                     >(L_);
        TEST.run<YES,             cvmFV,        wvM                     >(L_);
        TEST.run<YES,             cvmFV,       wcvM                     >(L_);

        // Function objects invoked with various kinds and sequences of
        // implicit conversions.
        TEST.run<YES, ConvertibleToFunc                                 >(L_);
        TEST.run<YES, ConvertibleToFunc, int                            >(L_);
        TEST.run<YES, ConvertibleToFunc, int       , float              >(L_);
        TEST.run<YES, ConvertibleToFunc, int       , float, short       >(L_);
        TEST.run< NO, ConvertibleToFunc, int       , float, short, short>(L_);
        TEST.run< NO, ConvertibleToFunc, int *                          >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float              >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float, short       >(L_);
        TEST.run< NO, ConvertibleToFunc, int *     , float, short, short>(L_);
        TEST.run<YES, ConvertibleToFunc, char                           >(L_);
        TEST.run<YES, ConvertibleToFunc, char      , char               >(L_);
        TEST.run<YES, ConvertibleToFunc, char      , char , char        >(L_);
        TEST.run< NO, ConvertibleToFunc, char      , char , char , char >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong                     >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong, char               >(L_);
        TEST.run<YES, ConvertibleToFunc, ToLongLong, char , char        >(L_);
        TEST.run< NO, ConvertibleToFunc, ToLongLong, char , char , char >(L_);
#endif

      } break;
      case 6: BSLA_FALLTHROUGH;
      case 5: BSLA_FALLTHROUGH;
      case 4: BSLA_FALLTHROUGH;
      case 3: BSLA_FALLTHROUGH;
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
