// bslmf_invokeresult.04.t.cpp                                        -*-C++-*-



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
//-----------------------------------------------------------------------------
// [4] POINTER-TO-MEMBER-OBJECT INVOCABLES
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 90000
// Prior to version 9, GCC does not retroactively apply Core Working Group
// resolution 616, which applies to C++11 and later.  This resolution, among
// other things not relevant to this test driver, changes the algorithm used to
// determine the value category of pointer-to-member operator expressions,
// which affects the reference qualification (or lack thereof) of the type
// of 'bsl::invoke_result' for pointers to data members.  Prior to the
// resolution of CWG616, the value category of a pointer-to-member operator
// expression was the same as the value category of its left-hand side -- that
// of the object.  After the resolution of CWG616, such an expression has
// lvalue category if its left-hand side is an lvalue, and has xvalue category
// otherwise.
#   define GCC_PRE_CWG616_RESOLUTION_SEMANTICS
#endif //defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 90000

template <class T1, class T2>
struct AssertSame;
    // For debugging purposes, instantiating this template with two types that
    // are not the same will result in a compile-time error that, on most
    // compilers, will print 'T1' and 'T2' as well as a trace of the
    // instantiation stack.

template <class T1>
struct AssertSame<T1, T1> : bsl::true_type
{
    // Specialization of 'AssertSame' when instantiated with two parameters of
    // the same type yields a successful compilation.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TP>
struct MovePtr {
    // This template has the interface of a smart pointer except that, the
    // dereference operator returns an RVALUE reference to 'TP' instead of an
    // LVALUE reference.

    TP&& operator*() const volatile;
        // Dereference operator (declared but not defined).
};
#endif

template <class TP>
struct ProxyPtr {
    // This template has the interface of a "proxy" pointer, where the
    // dereference operator returns an rvalue of type 'TP'.

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    typedef typename bsl::remove_const<TP>::type value_type;
        // Type being proxied. Detection of const rvalues is problematic on
        // older Sun/Oracle compiler (they get confused with lvalues in our
        // metaprograms).
#else
    typedef TP value_type;
        // Type being proxied.
#endif

    value_type operator*() const volatile;
        // Dereference operator (declared but not defined)
};

struct ValueCategory {
    enum Enum {
        e_LVALUE,
        e_PRVALUE,
        e_XVALUE
    };
};

template <class T1, class T2>
struct PropagateCVQ {
    // Apply the cv-qualifications from the specified template parameter type
    // 'T1' to the specified template parameter 'T2', producing a typedef
    // 'type'.  More specifically: 'type' is 'cvq T2&', where 'cvq' is the
    // cv-qualifiers (if any) on 'T1'.  Note that any cv-qualifiers already on
    // 'T2' are unioned with 'cvq' to produce the result.

    typedef T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<const T1, T2> {
    // Specialization of 'PropagateCVQ' for 'const T1'.

    typedef const T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<volatile T1, T2> {
    // Specialization of 'PropagateCVQ' for 'volatile T1'.

    typedef volatile T2 type;
};

template <class T1, class T2>
struct PropagateCVQ<const volatile T1, T2> {
    // Specialization of 'PropagateCVQ' for 'const volatile T1'.

    typedef const volatile T2 type;
};

template <class T1, class T2>
struct PropagateCVQRef {
    // Apply the reference and cv-qualifications from the specified template
    // parameter type 'T1' to the specified template parameter 'T2', producing
    // a typedef 'type'.  More specifically: if 'T1' is an rvalue, 'type' is
    // simply 'T2' (unmodified); otherwise, 'type' is 'cvq T2&', where 'cvq'
    // is the cv-qualifiers (if any) on 'T1'.  Note that any cv-qualifiers
    // already on 'T2' are unioned with 'cvq' to produce the result.  This
    // primary template is instantiated when 'T1' is not a reference.

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    typedef T2 type;
        // Result type.  In C++03, if 'T1' is an rvalue, then 'T2' is returned
        // unchanged.  The cv-qualifications on 'T1' are ignored.
#else
    typedef typename PropagateCVQ<T1, T2>::type&& type;
        // Result type.  In C++11 and later, if 'T1' is a prvalue, then 'T2'
        // is returned as an rvalue reference, with the cv-qualifications
        // copied from 'T1'.
#endif
};

template <class T1, class T2>
struct PropagateCVQRef<T1&, T2> {
    // Specialization of 'PropagateCVQRef' for lvalue reference 'T1'.

    typedef typename PropagateCVQ<T1, T2>::type& type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class T1, class T2>
struct PropagateCVQRef<T1&&, T2> {
    // Specialization of 'PropagateCVQRef' for rvalue reference 'T1'.

    typedef typename PropagateCVQ<T1, T2>::type&& type;
};
#endif

template <class               T1             = MyClass,
          class               EFFECTIVE_ARG  = T1,
          ValueCategory::Enum VALUE_CATEGORY =
              bsl::is_lvalue_reference<EFFECTIVE_ARG>::value
                  ? ValueCategory::e_LVALUE
                  : ValueCategory::e_XVALUE>
struct PtrToMemObjTest {
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn, T1>', where 'Fn' is a
    // pointer-to-member-object type, or reference to cv-qualified
    // pointer-to-member-object type.
    //
    // If the specified 'T1' is 'MyClass' or a class derived from 'MyClass',
    // let the expression I be 'std::decltype<T1>()'.  If the specified 'T1' is
    // a specialization of 'bsl::reference_wrapper', let the expression I be
    // 'std::decltype<T1>().get()'.  Otherwise, let the expression I be
    // '*std::decltype<T1>()'.  The behavior is undefined unless the specified
    // 'EFFECTIVE_ARG' is the type of the expression I, and the specified
    // 'VALUE_CATEGORY' identifies the value category of the expression I.
    // Note that in most cases (e.g. unless 'T1' is 'bsl::reference_wrapper', a
    // pointer type, or a smart pointer type,) the default arguments for
    // 'EFFECTIVE_ARG' and 'VALUE_CATEGORY' are equal to the type and value
    // category of the expression I, respectively.

    template <class RT>
    static void apply(int LINE)
        // For 'Fn' of type 'RT MyClass::*Mp', instantiate 'invoke_result<Fn>'
        // and 'invoke_result<Fn cvq&>'.  Verify that it yields 'RT' as the
        // result type.
        //
        // The specified 'LINE' is passed to 'ASSERTV' for diagnostics.
    {
        // If 'RT' is 'void' or a reference type, 'doApply' is a no-op.  There
        // is no such thing as a member pointer to void or reference.
        enum { kIS_VOID_OR_REF = (bsl::is_void<RT>::value ||
                                  bsl::is_reference<RT>::value) };

        doApply<RT>(bsl::integral_constant<bool, kIS_VOID_OR_REF>(),
                    LINE);
    }

    template <class RT>
    static void doApply(bsl::true_type /* void or reference */,
                        BSLA_MAYBE_UNUSED int                  LINE)
        // No-op implementation of 'apply' for when 'RT' is 'void' or a
        // reference type.  There is no such thing as a member pointer to void
        // or reference.
        //
        // The specified 'LINE' is passed to 'ASSERTV' for diagnostics.
    {
    }

    template <class RT>
    static void doApply(bsl::false_type /* void or reference */,
                        BSLA_MAYBE_UNUSED int                   LINE)
        // For 'Fn' of type 'RT MyClass::*Mp', instantiate
        // 'invoke_result<Fn, T1>' and 'invoke_result<Fn cvq&, T1>'.  Verify
        // that it yields 'RT' as the result type.  This overload is called
        // when 'RT' is not 'void' and is not a reference.
        //
        // The specified 'LINE' is passed to 'ASSERTV' for diagnostics.
    {
        typedef RT MyClass::*Mp;

#if defined(GCC_PRE_CWG616_RESOLUTION_SEMANTICS)
        typedef typename
            bsl::conditional<
                VALUE_CATEGORY == ValueCategory::e_LVALUE,
                typename bsl::add_lvalue_reference<
                    typename PropagateCVQ<
                        typename bsl::remove_reference<EFFECTIVE_ARG>::type,
                        RT
                    >::type
                >::type,
                typename bsl::conditional<
                    VALUE_CATEGORY == ValueCategory::e_PRVALUE,
                    typename bsl::remove_cv<RT>::type,
                    typename bsl::add_rvalue_reference<
                        typename PropagateCVQ<
                            typename bsl::remove_reference<
                                EFFECTIVE_ARG
                                >::type,
                            RT
                        >::type
                    >::type
                >::type
            >::type CvqR;
#elif defined(MSVC_2015_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG)
        typedef typename
            bsl::conditional<
                bsl::is_array<RT>::value,
                RT,
                typename PropagateCVQRef<EFFECTIVE_ARG, RT>::type
            >::type CvqR;
#else
        typedef typename PropagateCVQRef<EFFECTIVE_ARG, RT>::type CvqR;
#endif
        AssertSame<CvqR, typename bsl::invoke_result<Mp , T1>::type>();

        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp const&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp volatile&, T1>::value));
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp const volatile&, T1>::value));
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ASSERTV(LINE, (IsInvokeResult<CvqR, Mp&&, T1>::value));
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


PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyClass>,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyDerivedClass>,
                        MyDerivedClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass *,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass *,
                        MyDerivedClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass>,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass>,
                        MyDerivedClass&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const volatile ProxyPtr<MyClass>&,
                        MyClass,
                        ValueCategory::e_PRVALUE>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass>&,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const SmartPtr<MyClass>&,
                        MyClass&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<const volatile SmartPtr<MyClass>&,
                        MyClass&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const*,
                        MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const *,
                        MyDerivedClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass const>,
                        MyClass const&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass const>,
                        MyDerivedClass const&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass volatile*,
                        MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass volatile*,
                        MyDerivedClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass volatile>,
                        MyClass volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass volatile>,
                        MyDerivedClass volatile&>);

PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyClass const volatile*,
                        MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<MyDerivedClass const volatile*,
                        MyDerivedClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyClass const volatile>,
                        MyClass const volatile&>);
PREINSTANTIATE_APR_FOR_BASIC_TYPES(
        PtrToMemObjTest<SmartPtr<MyDerivedClass const volatile>,
                        MyDerivedClass const volatile&>);

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
      case 5: {
        referUserToElsewhere(test, verbose);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING POINTER-TO-MEMBER-OBJECT INVOCABLES
        //
        // Concerns:
        //: 1 Given class 'B', class 'D' derived from 'B' and
        //:   pointer-to-member type 'typedef R B::*MP',
        //:   'bsl::invoke_result<MP, T1>', yields an rvalue type 'R' if 'T1'
        //:   is (rvalue type) 'B' or 'D'. In C++03 mode, the rvalue type is
        //:   'R'; in C++11 and subsequent modes, the rvalue type is 'R&&'.
        //: 2 Similar to concern 1, except for 'T1' being a smart pointer type
        //:   which, when dereferenced, yields an rvalue of 'B' or 'D'. CV and
        //:   reference qualifiers on the smart pointer have no effect.
        //: 3 Given the same 'MP' as in concern 1,
        //:   'bsl::invoke_result<MP, T1>', yields type 'R&' if 'T1' is 'B&'
        //:   or 'D&'.
        //: 4 Similar to concern 3, except with 'T1' being a pointer or smart
        //:   pointer type that, when dereferenced, yields an lvalue of type
        //:   'B&' or 'D&'.
        //: 5 Similar to concerns 3 and 4, in C++11 and later,
        //:   'bsl::invoke_result<MP, T1>', yields type 'R&&' if 'T1' is 'B&&'
        //:   or is a smart pointer type that, when dereferenced, yields an
        //:   rvalue reference of type 'B&&' or 'D&&'.
        //: 6 If 'B' or 'D' in concerns 1-5 are cv-qualified, the result has
        //:   the union of cv qualifications of 'R' and the class type 'B' or
        //:   'D'. Reference and CV qualifiers on a pointer or smart pointer
        //:   itself have no effect.
        //: 7 Concerns 1-6 apply for 'R' of the following types:
        //:    o built-in numeric types
        //:    o user-defined class and enumeration types
        //:    o pointer (possibly cv-qualified) to any of the above
        //:    o pointer to (possibly cv-qualified) void
        //:    o cv-qualified types
        //:    o arrays (which do not decay)
        //
        // Plan:
        //: 1 For concern 1, define a functor template 'PtrToMemObjTest<T1>'
        //:   with an 'apply<R>' member function template that declares a
        //:   pointer to 'R' member of 'MyClass' and verifies that
        //:   applying 'bsl::invoke_result' to 'R (MyClass::*)(T1)' yields
        //:   'R'. Invoke this functor through a function 'applyPtrAndRef'
        //:   that adds every combination of pointer, reference, and const
        //:   qualifiers to 'R'. Use 'MyClass' and 'MyDerivedClass' for 'T1',
        //:   along with a sampling of CV qualifications for those.
        //: 2 For concern 2, add a parameter to 'PtrToMemObjTest' to indicate
        //:   "effective argument" type, i.e., the variation of 'MyClass'
        //:   yielded by the second template parameter to 'invoke_result'.
        //:   Create a smart pointer class template 'ProxyPtr<TP>' that defines
        //:   'TP operator*()'.  Repeat step 1, using 'ProxyPtr<MyClass>' and
        //:   'ProxyPtr<MyDerivedClass>' for 'T1', and 'MyClass' and
        //:   'MyDerivedClass', respectively, for the effective argument
        //:   type. Also try a sampling of CV qualifications on both the
        //:   'ProxyPtr' and on its template parameter. Show that a reference
        //:   qualifier on the 'ProxyPtr' has no effect, as well.
        //: 3 Repeat step 1 using 'MyClass&' and 'MyDerivedClass&' for 'T1'
        //:   and indicating an expected lvalue result.
        //: 4 For concern 4, repeat step 3, using 'MyClass*' and
        //:   'MyDerivedClass*' insted of 'MyClass&' and
        //:   'MyDerivedClass&'.  Create a class template 'SmartPtr<TP>' that
        //:   defines 'TP& operator*()' and repeat step 3, using
        //:   'SmartPtr<MyClass>' and 'SmartPtr<MyDerivedClass>' for 'T1' and
        //:   using 'MyClass&' and 'MyDerivedClass&', respectively, for the
        //:   effective argument type.  Use a sampling of reference and CV
        //:   qualifiers on the 'SmartPtr' (but not it's template parameter)
        //:   to show that it has no effect.
        //: 5 For concern 5, repeat step 3 using 'MyClass&' and
        //:   'MyDerivedClass&' for 'T1' and indicating an expected rvalue
        //:   reference result. Create a class template 'MovePtr<TP>' that
        //:   defines 'TP&& operator*()' and repeat step 3 using
        //:   'MovePtr<MyClass>' and 'MovePtr<MyDerivedClass>' for 'T1' and
        //:   using 'MyClass&&' and 'MyDerivedClass&&', respectively, for the
        //:   effective argument type. Use a sampling of reference and CV
        //:   qualifiers on the 'MovePtr' (but not it's template parameter) to
        //:   show that it has no effect.
        //: 6 For concern 6, modify 'PtrToMemObjTest' so that it expects the
        //:   union of the cv-qualifications specified in the effective
        //:   argument type parameter and those already on 'R'. Repeat steps
        //:   1-5, supplying different cv qualifications for the 'MyClass'
        //:   and 'MyDerivedClass' parameters.
        //: 7 For concern 7, modify steps 1-6 with 'R' of type 'void',
        //:   'int', 'float', user-defined type 'Ic01', 'const int',
        //:   'volatile short', and 'const volatile MyEnum'. Note that 'void'
        //:   will be ignored, but *pointer to (cv-qualified)* 'void' will not.
        //
        // Testing:
        //     POINTER-TO-MEMBER-OBJECT INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING POINTER-TO-MEMBER-OBJECT INVOCABLES"
                            "\n===========================================\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        typedef const int                               Cint;
        typedef volatile short                          Vshort;
        typedef const volatile MyEnum                   CVenum;
        typedef int                                     Arry[10][5];

        typedef MyClass                                 Mc;
        typedef MyClass const                           CMc;
        typedef MyClass volatile                        VMc;
        typedef MyClass const volatile                  CvMc;
        typedef MyDerivedClass                          Mdc;
        typedef MyDerivedClass const                    CMdc;
        typedef MyDerivedClass volatile                 VMdc;
        typedef MyDerivedClass const volatile           CvMdc;

        static const ValueCategory::Enum RV = ValueCategory::e_PRVALUE;

        // Step 1: rvalue of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc>                       , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc>                      , Arry  >(L_);

        // Step 2: rvalue proxy of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>, Mc, RV>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mdc>, Mc, RV>    , Arry  >(L_);

        // Reference qualifications on 'ProxyPtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<Mc>&, Mc, RV>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const ProxyPtr<Mdc>&, Mc, RV>
                                                                , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<const volatile ProxyPtr<Mc>&, Mc, RV>
                                                                 , int>(L_);

        // Step 3: lvalue of 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&>                      , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&>                     , Arry  >(L_);

        // Step 4a: pointer to 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc*, Mc&>                 , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc*, Mdc&>               , Arry  >(L_);

        // Step 4b: smart pointer to (lvalue of) 'MyClass' or 'MyDerivedClass'
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mc>, Mc&>        , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<Mdc>, Mc&>       , Arry  >(L_);

        // Reference and CV qualifications on 'SmartPtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>&, CMc&>     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const SmartPtr<VMc>&, VMc&>
                                                                 , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<const volatile SmartPtr<Mc>&, Mc&>
                                                                 , int>(L_);

        // Step 5a: rvalue reference to 'MyClass' or 'MyDerivedClass'
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mc&&>                     , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<Mdc&&>                    , Arry  >(L_);

        // Step 5b: smart pointer returning 'MyClass&&' or 'MyDerivedClass&&'
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , void  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , int   >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mdc>, Mc&&>       , Ic01  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<Mc>, Mc&&>        , Arry  >(L_);

        // Reference and CV qualifications on 'MovePtr' have no affect
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>&&, CMc&&>    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<const volatile MovePtr<Mc>&, Mc&&>
                                                                 , int>(L_);
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<Mc>&, Mc&&> , CVenum>(L_);

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // 'MovePtr<volatile MyClass>' conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<const MovePtr<VMc>&, VMc&&>
                                                                 , CVenum>(L_);
#endif // ! MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

        // Step 6: Expect CV-qualification on reference should be union of
        // cv-qualification on the effective argument and the cv-qualification
        // of the member.
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&>                     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&>                     , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc&>                   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , CVenum>(L_);

    // Older sun compiler gets confused returning a const-qualified rvalue.
#if ! (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMdc>, CMc, RV>  , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , CVenum>(L_);
#endif // Not Sun CC
#if 0 // Returning volatile prvalues is problematic in most compilers
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>       , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMc>, VMc>       , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<VMdc>, VMc>      , Arry>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , float>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , Cint>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMdc>, CvMc>    , Vshort>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , CVenum>();
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CvMc>, CvMc>     , Arry>();
#endif // 0

        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc*, CMc&>              , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>               , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc*, VMc&>               , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc*, CvMc&>            , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMdc>, CMc&>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMc>, VMc&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMdc>, CvMc&>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , CVenum>(L_);

#ifndef MSVC_PTR_TO_ARRAY_MEMBER_QUALIFIER_PROPAGATION_BUG
        applyPtrAndRef<PtrToMemObjTest<CMc&>                     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<ProxyPtr<CMc>, CMc, RV>   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc*, CMc&>               , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc*, VMc&>              , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc*, CvMc&>             , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CMc>, CMc&>      , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<VMdc>, VMc&>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<SmartPtr<CvMc>, CvMc&>    , Arry  >(L_);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , Arry  >(L_);
#endif
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMdc&&>                   , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CMc&&>                    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<VMc&&>                    , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<VMdc&&>                   , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMdc&&>                  , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<CvMc&&>                   , CVenum>(L_);

        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMdc>, CMc&&>     , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CMc>, CMc&&>      , CVenum>(L_);

#ifndef MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
        // 'MovePtr<volatile MyClass>' conditionally compiled for non-MSVC
        // compiler to avoid bug.
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>      , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMc>, VMc&&>      , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<VMdc>, VMc&&>     , Arry  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , float >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , Cint  >(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMdc>, CvMc&&>   , Vshort>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , CVenum>(L_);
        applyPtrAndRef<PtrToMemObjTest<MovePtr<CvMc>, CvMc&&>    , Arry  >(L_);
#endif // MSVC_PTR_TO_MEMBER_OF_RVALUE_REF_BUG
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

      } break;
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
