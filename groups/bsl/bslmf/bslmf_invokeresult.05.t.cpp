// bslmf_invokeresult.05.t.cpp                                        -*-C++-*-



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
// [5] FUNCTOR CLASS INVOCABLES
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



struct ToIntPtr
{
    // Objects of this class are convertible to pointer-to-int.

    operator int*() const;
        // Convertion to 'int*' (declared but not defined).
};


namespace bsl {

template <class TP>
class invoke_result<ManyFunc, char*, MetaType<TP> > {
    // Specialization of 'invoke_result' for a situation where, in C++03 mode,
    // return-type deduction would otherwise fail.

  public:
    typedef TP type;
        // Correct return type for 'ManyFunc::operator()(char*, MetaType<TP>)'
};

}  // close namespace bsl



template <bool RET_USER_TYPE = false>
struct FunctorTest
{
    // A test kernel, intended as the 'TEST_KERNEL' template argument to
    // 'applyPtrAndRef', which tests 'invoke_result<Fn>', where 'Fn' is a class
    // (functor) type, or reference to cv-qualified functor type.  If the
    // specified 'RET_USER_TYPE' template parameter is true, it indicates that
    // the caller intends to call 'apply<RT>' with user-defined (class or
    // enumeration) type for 'RT'.  Otherwise, the caller must supply a basic
    // (numeric, pointer, or void) type for 'RT'.

    template <class RT>
    static void apply(int LINE)
        // Instantiate 'invoke_result<Fn, Args...>' where 'Fn' is a variety of
        // functor types, and verify that it yields a 'type' of the specified
        // template parameter type 'RT'.  This test checks the return type for
        // the following functor types:
        //
        //: o 'FuncRt1', which uses the 'result_type' idiom
        //: o 'FuncRt2', which uses the 'ResultType' idiom
        //: o 'FuncRt3', which uses both 'result_type' and 'ResultType'
        //: o 'ManyFunc', which uses neither 'result_type' and 'ResultType'
        //
        // If 'RET_USER_TYPE' is true and the compiler does not support
        // 'decltype', then the expected result type for 'ManyFunc' cannot be
        // deduced and 'invoke_result' is expected to yield
        // 'bslmf::InvokeResultDeductionFailed'.  As a special case, for
        // 'FuncRt1', 'FuncRt2', and 'FuncRt3', test that using 'invoke_result'
        // arguments that choose an overload that returns 'int' will correctly
        // yield 'int' regardless of the 'result_type' and 'ResultType'
        // idioms.
        //
        // The specified parmeter 'LINE' is passed to 'ASSERTV()' calls.
    {
        using bsl::invoke_result;
        using bslmf::InvokeResultDeductionFailed;

        // The 'char*' overload of 'FuncRt1<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt1<RT> , char*>::type Rt1ResultR;
        typedef typename invoke_result<FuncRt1<RT>&, char*>::type Rt1rResultR;
        typedef typename invoke_result<FuncRt1<RT> , int  >::type Rt1ResultInt;
        typedef typename invoke_result<FuncRt1<RT>&, int  >::type
            Rt1rResultInt;

        ASSERTV(LINE, (bsl::is_same<RT , Rt1ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT , Rt1rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt1ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt1rResultInt>::value));

        // The 'char*' overload of 'FuncRt2<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt2<RT> , char*>::type Rt2ResultR;
        typedef typename invoke_result<FuncRt2<RT>&, char*>::type Rt2rResultR;
        typedef typename invoke_result<FuncRt2<RT> , int  >::type Rt2ResultInt;
        typedef typename invoke_result<FuncRt2<RT>&, int  >::type
            Rt2rResultInt;
        ASSERTV(LINE, (bsl::is_same<RT,   Rt2ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT,   Rt2rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt2ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt2rResultInt>::value));

        // The 'char*' overload of 'FuncRt3<RT>::operator()' will return 'RT',
        // whereas the 'int' overload will return 'int'.
        typedef typename invoke_result<FuncRt3<RT> , char*>::type Rt3ResultR;
        typedef typename invoke_result<FuncRt3<RT>&, char*>::type Rt3rResultR;
        typedef typename invoke_result<FuncRt3<RT> , int  >::type Rt3ResultInt;
        typedef typename invoke_result<FuncRt3<RT>&, int  >::type
            Rt3rResultInt;
        ASSERTV(LINE, (bsl::is_same<RT , Rt3ResultR>::value));
        ASSERTV(LINE, (bsl::is_same<RT , Rt3rResultR>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt3ResultInt>::value));
        ASSERTV(LINE, (bsl::is_same<int, Rt3rResultInt>::value));

        typedef typename invoke_result<ManyFunc,  MetaType<RT>, int>::type
            MFResult;
        typedef typename invoke_result<ManyFunc&, MetaType<RT>, int>::type
            MFrResult;
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013
        // 'decltype' is supported, the result type will always be correct.
        ASSERTV(LINE, (bsl::is_same<RT, MFResult>::value));
        ASSERTV(LINE, (bsl::is_same<RT, MFrResult>::value));
#else
        // 'decltype' is not supported, the result type will always be correct
        // for basic types like 'int', 'int*' and 'void', but will be a
        // place-holder for user-defined types that don't support the
        // 'result_type' or 'ResultType' idioms.
        if (RET_USER_TYPE) {
            // User-defined type, expect 'InvokeResultDeductionFailed' place
            // holder.
            ASSERTV(LINE, (bsl::is_same<InvokeResultDeductionFailed,
                                 MFResult>::value));
            ASSERTV(LINE, (bsl::is_same<InvokeResultDeductionFailed,
                                 MFrResult>::value));
        }
        else {
            // Basic type, expect correct result.
            ASSERTV(LINE, (bsl::is_same<RT, MFResult>::value));
            ASSERTV(LINE, (bsl::is_same<RT, MFrResult>::value));
        }
#endif // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
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

// Steps 4 & 5, concerns 5, 6, and 7
PREINSTANTIATE_AR(FunctorTest<true>, MyEnum);
PREINSTANTIATE_AR(FunctorTest<true>, MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyEnum*);
PREINSTANTIATE_AR(FunctorTest<true>, MyClass);
PREINSTANTIATE_AR(FunctorTest<true>, MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyClass*);
PREINSTANTIATE_AR(FunctorTest<true>, MyDerivedClass);
PREINSTANTIATE_AR(FunctorTest<true>, MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, volatile MyDerivedClass*);
PREINSTANTIATE_AR(FunctorTest<true>, const volatile MyDerivedClass*);

void testCase5APRforEnum()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
    // became overwhelmed with the number of nested template instantiations, so
    // the call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyEnum            >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyEnum>::apply(L_);
    ApplyRef<FunctorTest<true>, MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyEnum*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyEnum*>::apply(L_);
}

void testCase5APRforClass()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
    // became overwhelmed with the number of nested template instantiations, so
    // the call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyClass           >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyClass>::apply(L_);
    ApplyRef<FunctorTest<true>, MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyClass*>::apply(L_);
}

void testCase5APRforDerivedClass()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<true>, TYPE >(L_).  However xlC and Sun CC
    // became overwhelmed with the number of nested template instantiations, so
    // the call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<true>, MyDerivedClass    >(L_);
#endif
    ApplyRef<FunctorTest<true>, MyDerivedClass>::apply(L_);
    ApplyRef<FunctorTest<true>, MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, volatile MyDerivedClass*>::apply(L_);
    ApplyRef<FunctorTest<true>, const volatile MyDerivedClass*>::apply(L_);
}

void testCase5APRforVoid()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
    // overwhelmed with the number of nested template instantiations, so the
    // call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
    // Step 3 & 5, concerns 4 and 7
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, void              >(L_);
#endif
    ApplyRef<FunctorTest<>,                void>::apply(L_);
    ApplyRef<FunctorTest<>,                void*>::apply(L_);
    ApplyRef<FunctorTest<>,          const void*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile void*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile void*>::apply(L_);
}

void testCase5APRforChar()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
    // overwhelmed with the number of nested template instantiations, so the
    // call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
    // Step 3 & 5, concerns 4 and 7
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, char              >(L_);
#endif
    ApplyRef<FunctorTest<>,                char >::apply(L_);
    ApplyRef<FunctorTest<>,                char*>::apply(L_);
    ApplyRef<FunctorTest<>,          const char*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile char*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile char*>::apply(L_);
}

PREINSTANTIATE_AR(FunctorTest<>,                long double);
PREINSTANTIATE_AR(FunctorTest<>,                long double*);
PREINSTANTIATE_AR(FunctorTest<>,          const long double*);
PREINSTANTIATE_AR(FunctorTest<>,       volatile long double*);
PREINSTANTIATE_AR(FunctorTest<>, const volatile long double*);

void testCase5APRforLongDouble()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
    // overwhelmed with the number of nested template instantiations, so the
    // call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, long double       >(L_);
#endif
    ApplyRef<FunctorTest<>,                long double>::apply(L_);
    ApplyRef<FunctorTest<>,                long double*>::apply(L_);
    ApplyRef<FunctorTest<>,          const long double*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile long double*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile long double*>::apply(L_);
}

PREINSTANTIATE_AR(FunctorTest<>,                bsl::nullptr_t);
PREINSTANTIATE_AR(FunctorTest<>,                bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>,          const bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>,       volatile bsl::nullptr_t*);
PREINSTANTIATE_AR(FunctorTest<>, const volatile bsl::nullptr_t*);

void testCase5APRforNullptr()
    // This function was originally a call to the function:
    // applyPtrAndRef<FunctorTest<>, TYPE >(L_).  However xlC and Sun CC became
    // overwhelmed with the number of nested template instantiations, so the
    // call was replaced with a function replicating the behaviour of the
    // original applyPtrAndRef call.  This unfortunately does add duplication.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    applyPtrAndRef<FunctorTest<>, bsl::nullptr_t    >(L_);
#endif
    ApplyRef<FunctorTest<>,                bsl::nullptr_t>::apply(L_);
    ApplyRef<FunctorTest<>,                bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>,          const bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>,       volatile bsl::nullptr_t*>::apply(L_);
    ApplyRef<FunctorTest<>, const volatile bsl::nullptr_t*>::apply(L_);
}

void testCase5APRforOther()
    // Only used for the non-abridged (full) test, this contains all of the
    // calls to applyPtrAndRef previously contained within testCase5() but not
    // split into the individual functions required to support abridged tests.
{
#if !BSLMF_INVOKERESULT_ABRIDGED_TEST
    // Step 3 & 5, concerns 4 and 7
    applyPtrAndRef<FunctorTest<>, signed char       >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned char     >(L_);
#   ifdef BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE
    applyPtrAndRef<FunctorTest<>, char8_t           >(L_);
#   endif
    applyPtrAndRef<FunctorTest<>, wchar_t           >(L_);
#   ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
    applyPtrAndRef<FunctorTest<>, char16_t          >(L_);
    applyPtrAndRef<FunctorTest<>, char32_t          >(L_);
#   endif
    applyPtrAndRef<FunctorTest<>, short             >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned short    >(L_);
    applyPtrAndRef<FunctorTest<>, int               >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned int      >(L_);
    applyPtrAndRef<FunctorTest<>, long int          >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned long     >(L_);
    applyPtrAndRef<FunctorTest<>, long long         >(L_);
    applyPtrAndRef<FunctorTest<>, unsigned long long>(L_);
    applyPtrAndRef<FunctorTest<>, float             >(L_);
    applyPtrAndRef<FunctorTest<>, double            >(L_);
#endif
}


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
      case 6: {
        referUserToElsewhere(test, verbose);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTOR CLASS INVOCABLES
        //
        // Concerns:
        //: 1 For invocables of functor class type, 'bsl::invoke_result' can
        //:   deduce the return type of 'operator()' if that return type is a
        //:   simple built-in integral type.
        //: 2 The correct overload of 'operator()' is selected, depending on
        //:   the number and types of arguments.
        //: 3 Appropriate overloads of 'operator()' are selected depending on
        //:   the const and/or volatile qualifiers on the functor type.
        //: 4 The return-type deduction described in concerns 1-3 additionally
        //:   extends to return types of:
        //:    o 'void'
        //:    o 'bsl::nullptr_t'
        //:    o built-in numeric types
        //:    o pointer to (possibly cv-qualified) numeric type or 'void'
        //:    o Reference to any of the (possibly cv-qualified) types above
        //:      except 'void', including reference-to-pointer types. Rvalue
        //:      references should be tested on C++11-compliant compilers.
        //: 5 In C++03, if the invocable returns a user-defined type, pointer
        //:   to a user-defined type, or reference to one of these, and the
        //:   invocable has a nested type 'result_type' or 'ResultType',
        //:   'invoke_result' yields that nested type; otherwise it yields
        //:   'bslmf::InvokeResultDeductionFailed'.  If the invocable has both
        //:   'result_type' and 'ResultType' nested types, then 'invoke_result'
        //:   yields 'result_type'.  However, if the invocation would return a
        //:   type listed in concern 4, 'result_type' and 'ResultType' are
        //:   ignored.
        //: 6 In C++11 or later, if the invocable returns a user-defined type,
        //:   pointer to a user-defined type, or reference to one of these,
        //:   'invoke_result' always yields the correct return type, regardless
        //:    of whether or not there exists a nested 'result_type' or
        //:   'ResultType' type.
        //: 7 The previous concerns apply when the invocable is a *reference*
        //:   to functor class type.
        //: 8 If 'invoke_result<FN, args...>' is specialized or partially
        //:   specialized for a specific 'FN' and/or 'args...', that
        //:   specialization is used instead of the primary template.  Such
        //:   specializations can produce usable results even if deduction
        //:   of the return value would have failed for the unspecialized
        //:   template.
        //: 9 The above concerns apply to functors taking 0 to 13 arguments.
        //: 10 Though not technically a functor, a class that is convertible
        //:   to a pointer-to-function or reference-to-function behaves like a
        //:   functor within this component.  If the class is convertible to
        //:   more than one function type, then the correct one is chosen
        //:   based on overloading rules.  If the chosen (pointer or reference
        //:   to) function returns a user-defined type, then return
        //:   'bslmf::InvokeResultDeductionFailed' in C++03 mode.
        //
        // Plan:
        //: 1 For concerns 1 and 2, define a functor class, 'ManyFunc', with
        //:   'operator()' overloaded to return different integral types for a
        //:   variety of argument combinations.  Verify that
        //:   'bsl::invoke_result' yields the return type corresponding to the
        //:   best overload for a set of specific arguments types.
        //: 2 For concern 3, add 'const' and 'volatile' overloads. Using
        //:   typedefs for 'const' and 'volitile' references to 'ManyFunc',
        //:   verify that the best overload is selected.
        //: 3 For concern 4 add overloads of 'operator()' to 'ManyFunc' that
        //:   return 'TP' when called with a first argument of type
        //:   'MetaType<TP>'. Define a struct 'FunctorTest' whose 'apply<R>'
        //:   method verifies that 'invoke_result' yields the expected result
        //:   of invoking a 'ManyFunc' object with an argument of type
        //:   'MetaType<R>'.  Invoke 'FunctorTest' through the function
        //:   'applyPtrAndRef' that adds every combination of pointer,
        //:   reference, and cv qualifiers to 'R'. Repeat this test with every
        //:   numeric type and with 'void' for 'R'.
        //: 4 For concerns 5 and 6, define three invocables: 'FuncRt1<R>' that
        //:   defines 'result_type' as 'R', 'FuncRt2<R>' that defines
        //:   'ResultType' as 'R', and 'FuncRt3<R>' that defines 'result_type'
        //:   as 'R' and 'ResultType' as 'void'.  Each of them would define 'R
        //:   operator()(const char*)' and 'int operator()(int)'. Modify
        //:   'FunctorTest' such that the method 'FunctorTest<true>::apply<R>'
        //:   verifies that, if 'F' is 'FuncRt?<R>' and 'R' is one of the
        //:   types listed in concern 5, 'invoke_result<F, char*>::type' is
        //:   'R'; 'invoke_result<F, int>::type' is 'int', regardless of 'R';
        //:   and 'invoke_result<ManyFunc, MetaType<R>>::type' is
        //:   'bslmf::InvokeResultDeductionFailed' in C++03 mode and 'R' in
        //:   C++11 mode.  Invoke 'FunctorTest<true>' through the function
        //:   'applyPtrAndRef' as in step 3, to generate every 'R' with every
        //:   combination of pointer and reference qualifiers to user-defined
        //:   class 'MyClass' and user-defined enumeration 'MyEnum'.
        //: 5 For concern 7, repeat each step using a reference as the first
        //:   argument. For many of the tests, the modification would be in
        //:   one or more 'apply<R>' functions.
        //: 6 For concern 8, add an easily-distinguished set of overloads to
        //:   'ManyFunc::operator()' that return 'MyClass', 'MyClass *', etc..
        //:   Create explicit specializations of 'bsl::invoke_result' for those
        //:   overloads.  Verify that the explicit specializations yield the
        //:   expected type.
        //: 7 For concern 9, instantiate 'bsl::invoke_result' on overloads of
        //:   'ManyFunc::operator()' taking 0 to 10 arguments. Concern 8 does
        //:   not interact with the others, so it is not necessary to
        //:   test every combination of 0 to 10 arguments with every possible
        //:   return type.
        //: 8 For concern 10, define a type, 'ConvertibleToFunc' which is
        //:   convertible to pointer-to-function and reference-to-function,
        //:   where the result of each conversion has a different prototype.
        //:   Instantiate 'bsl::invoke_result' with arguments that select
        //:   each of the overloads and verify the correct result.
        //
        // Testing:
        //     FUNCTOR CLASS INVOCABLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTOR CLASS INVOCABLES"
                            "\n================================\n");
        if (verbose)
            printf(
                "This test only exists in file bslm_invokeresult.%02d.t.cpp\n"
                "(versions in other files are no-ops)\n",
                test);

        using BloombergLP::bslmf::InvokeResultDeductionFailed;

        typedef ManyFunc const           ManyFuncC;
        typedef ManyFunc volatile        ManyFuncV;
        typedef ManyFunc const volatile  ManyFuncCV;

#define TEST(exp, ...)                                                        \
    ASSERT((bsl::is_same<bsl::invoke_result<__VA_ARGS__>::type, exp>::value))

        // Step 1, concerns 1 and 2
        TEST(bool          , ManyFunc                                  );
        TEST(short         , ManyFunc,short                            );
        TEST(int           , ManyFunc,int,float                        );
        TEST(long          , ManyFunc,int,float,bool,short,unsigned,
                             long,int,int,int,float                    );
        TEST(unsigned int  , ManyFunc,int,short                        );
        TEST(unsigned short, ManyFunc,short,int                        );

        // Step 2 & 5, concern 3 and 7
        TEST(bool&         , ManyFunc,int, short, bool                 );
        TEST(bool&         , ManyFunc&,int, short, bool                );
        TEST(short&        , ManyFuncC,int, short, bool                );
        TEST(short&        , ManyFuncC&,int, short, bool               );
        TEST(int&          , ManyFuncV,int, short, bool                );
        TEST(int&          , ManyFuncV&,int, short, bool               );
        TEST(long&         , ManyFuncCV,int, short, bool               );
        TEST(long&         , ManyFuncCV&,int, short, bool              );

        // Step 3 & 5, concerns 4 and 7
        testCase5APRforVoid();
        testCase5APRforChar();
        testCase5APRforLongDouble();
        testCase5APRforNullptr();
        testCase5APRforOther();

        // Steps 4 & 5, concerns 5, 6, and 7
        testCase5APRforEnum();
        testCase5APRforClass();
        testCase5APRforDerivedClass();

        // Step 6, concern 8
        TEST(MyClass       , ManyFunc,char*,MetaType<MyClass>          );
        TEST(MyEnum *      , ManyFunc,char*,MetaType<MyEnum *>         );
        TEST(MyEnum *const&, ManyFunc,char*,MetaType<MyEnum *const&>   );

        // Step 7, concern 9
        typedef void     *P;
        typedef ToIntPtr  Q;
        typedef char     *S;
        TEST(bool        , ManyFunc                           );
        TEST(char        , ManyFunc,Q                         );
        TEST(short       , ManyFunc,P,S                       );
        TEST(int         , ManyFunc,S,Q,P                     );
        TEST(long        , ManyFunc,P,P,P,P                   );
        TEST(long long   , ManyFunc,P,P,P,P,P                 );
        TEST(float       , ManyFunc,P,P,P,P,P,P               );
        TEST(double      , ManyFunc,P,P,P,P,P,P,P             );
        TEST(long double , ManyFunc,P,P,P,P,P,P,P,P           );
        TEST(const char *, ManyFunc,P,P,P,P,P,P,P,P,P         );
        TEST(void       *, ManyFunc,P,P,P,P,P,P,P,P,P,Q       );

        // step 8, concern 10
        TEST(bool                       , ConvertibleToFunc               );
        TEST(short                      , ConvertibleToFunc,short         );
        TEST(int*                       , ConvertibleToFunc,int,float     );
#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && !MSVC_2013
        TEST(MyClass                    , ConvertibleToFunc,int,float,bool);
#else
        TEST(InvokeResultDeductionFailed, ConvertibleToFunc,int,float,bool);
#endif

#undef TEST

      } break;
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
