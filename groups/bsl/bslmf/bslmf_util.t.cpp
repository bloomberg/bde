// bslmf_util.t.cpp                                                   -*-C++-*-
#include <bslmf_util.h>

#include <bslmf_issame.h>
#include <bslmf_removeconst.h>
#include <bslmf_removereference.h>

#include <bsls_objectbuffer.h>
#include <bsls_bsltestutil.h>
#include <bsls_nameof.h>

#include <stdio.h>
#include <stdlib.h>

#include <new>   // required to support use of "placement new"

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// 'bslmf::Util' is a utility class, where each function will be tested in a
// separate test case.  Any significant test machinery will be tested before
// any function whose test case relies upon it.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ ] template <class T> forward(T&& value)
// [2] template <class T, class U> forward_like(U&& value)
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [ ] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) &&                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define DECLTYPE_AND_RVALUE_REFERENCES_ARE_SUPPORTED
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#   define BSLMF_UTIL_FORWARD_REF(T) const T&
#   define BSLMF_UTIL_FORWARD(T,V) \
        ::BloombergLP::bslmf::Util::forward(V)
#else
#   define BSLMF_UTIL_FORWARD_REF(T) T&&
#   define BSLMF_UTIL_FORWARD(T,V)       \
        ::BloombergLP::bsls::Util::forward<T>(V)
#endif

struct CUtil {
#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=2
    template <class TARGET, class ...Args>
    static void construct(TARGET *address, Args&&...args)
    {
      new (address) TARGET(bslmf::Util::forward<Args>(args)...);
    }
#else
    template <class TARGET, class Arg1>
    static void construct(TARGET *address, BSLMF_UTIL_FORWARD_REF(Arg1) a1)
    {
      new (address) TARGET(BSLMF_UTIL_FORWARD(Arg1,a1));
    }
    template <class TARGET, class Arg1, class Arg2>
    static void construct(TARGET *address,
                          BSLMF_UTIL_FORWARD_REF(Arg1) a1,
                          BSLMF_UTIL_FORWARD_REF(Arg2) a2)
    {
      new (address) TARGET(BSLMF_UTIL_FORWARD(Arg1,a1),
                           BSLMF_UTIL_FORWARD(Arg2,a2));
    }
#endif
};

namespace {

enum ParameterTypes {
    e_CONST_LVALUE_REF                     = 0,
    e_RVALUE_REF                           = 1,
    e_CONST_RVALUE_REF                     = 2,
    e_NONE                                 = 3
};

class Obj {
    // DATA
    ParameterTypes d_type;

  public:
    // CREATORS
    Obj()
    : d_type(e_NONE)
    {
    }

    Obj(int)
    : d_type(e_NONE)
    {
    }

    Obj(const Obj&)
    : d_type(e_CONST_LVALUE_REF)
    {
    }
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    Obj(Obj&&)
    : d_type(e_RVALUE_REF)
    {
    }
#else
    Obj(bslmf::MovableRef<Obj>)
    : d_type(e_RVALUE_REF)
    {
    }
#endif

    // ACCESSORS
    ParameterTypes type()  const
    {
        return d_type;
    }
};

//=============================================================================
//                            TEST DRIVER TEMPLATE
//-----------------------------------------------------------------------------

template <class t_TYPE>
struct TestDriver {
    // This struct provide a namespace for testing the 'bslmf::Util' functions
    // parameterized with the (template parameter) unqualified non-reference
    // 't_TYPE'.

  private:
    // PRIVATE CLASS METHODS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class t_MODEL, class t_ARG, class t_RESULT>
    static void testSingleCase(const volatile void *expectedAddress,
                               t_RESULT&&           result);
#else
    template <class t_MODEL, class t_ARG, class t_RESULT>
    static void testSingleCase(const volatile void *expectedAddress,
                               t_RESULT&            result);

    template <class t_MODEL, class t_ARG, class t_RESULT>
    static void testSingleCase(const volatile void         *expectedAddress,
                               bslmf::MovableRef<t_RESULT>  result);
#endif
        // Verify the correctness of the 'forward_like' for a single case when
        // it is parameterized by the (template parameter) 't_MODEL' type and
        // an object of the (template parameter) 't_ARG' type is passed.
        // Verify that the address of the specified 'result' (the value
        // returned by the 'forward_like') is equal to the specified
        // 'expectedAddress'.  Note that the 't_MODEL' is the first template
        // argument of the invoked 'forward_like', the 't_ARG' is the second
        // one that deduced from the type of the passed argument and the
        // (template parameter) 't_RESULT' is the type deduced from the
        // returned value.

    template <class t_MODEL>
    static void testSingleModelType();
        // Verify the correctness of the 'forward_like' for a single (template
        // parameter) 't_MODEL' type.  Note that the 't_MODEL' is the first
        // template argument of the 'forward_like'.

  public:
    // TEST CASES
    static void testCase2();
        // Test 'forward_like' function.
};

                               // -----------------
                               // struct TestDriver
                               // -----------------

// PRIVATE CLASS METHODS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class t_TYPE>
template <class t_MODEL, class t_ARG, class t_RESULT>
void TestDriver<t_TYPE>::testSingleCase(const volatile void *expectedAddress,
                                        t_RESULT&&           result)
{
    using bsls::NameOf;

    typedef typename bsl::remove_reference<t_MODEL>::type     UnRefModelType;
    typedef typename bsl::remove_reference<t_ARG>::type       UnRefArgType;
    typedef typename bsl::remove_reference<t_RESULT>::type    UnRefResultType;

    typedef typename bsl::remove_const<UnRefArgType>::type    UnCRefArgType;
    typedef typename bsl::remove_const<UnRefResultType>::type UnCRefResultType;

    // Check that the reference, returned by the 'forward_like' points to the
    // same object that has been passed as a parameter to this function.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedAddress == &result);

    // Check the identity of the stripped types.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnCRefArgType>(), NameOf<UnCRefResultType>(),
            (bsl::is_same<UnCRefArgType, UnCRefResultType>::value));

    bool expectedConst     = bsl::is_const<UnRefModelType>::value ||
                             bsl::is_const<UnRefArgType>::value;
    bool expectedLvalueRef =
                      bslmf::MovableRefUtil::IsLvalueReference<t_MODEL>::value;
    bool expectedVolatile  = bsl::is_volatile<UnRefArgType>::value;

    // Check if lvalue reference-ness has been added.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedLvalueRef == bsl::is_lvalue_reference<t_RESULT>::value);

    // Check if constancy has been added.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnRefResultType>(), expectedConst,
            expectedConst == bsl::is_const<UnRefResultType>::value);

    // Check if volatile-ness has been preserved.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnRefResultType>(), expectedVolatile,
            expectedVolatile == bsl::is_volatile<UnRefResultType>::value);
}
#else
template <class TYPE>
template <class t_MODEL, class t_ARG, class t_RESULT>
void TestDriver<TYPE>::testSingleCase(const volatile void *expectedAddress,
                                      t_RESULT&            result)
{
    using bsls::NameOf;

    typedef typename bslmf::MovableRefUtil::RemoveReference<t_MODEL>::type
                                                              UnRefModelType;
    typedef typename bslmf::MovableRefUtil::RemoveReference<t_ARG>::type
                                                              UnRefArgType;
    typedef typename bslmf::MovableRefUtil::RemoveReference<t_RESULT>::type
                                                              UnRefResultType;

    typedef typename bsl::remove_const<UnRefArgType>::type    UnCRefArgType;
    typedef typename bsl::remove_const<UnRefResultType>::type UnCRefResultType;

    // Check that the reference, returned by the 'forward_like' points to the
    // same object that has been passed as a parameter to this function.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedAddress == &result);

    // Check the identity of the stripped types.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnCRefArgType>(), NameOf<UnCRefResultType>(),
            (bsl::is_same<UnCRefArgType, UnCRefResultType>::value));

    bool expectedConst     = bsl::is_const<UnRefModelType>::value ||
                             bsl::is_const<UnRefArgType>::value;
    bool expectedLvalueRef =
                      bslmf::MovableRefUtil::IsLvalueReference<t_MODEL>::value;
    bool expectedVolatile  = bsl::is_volatile<UnRefArgType>::value;

    // Check if lvalue reference-ness has not been added by mistake.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            true == expectedLvalueRef);

    // Check if constancy has been added.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedConst == bsl::is_const<t_RESULT>::value);

    // Check if volatile-ness has been preserved.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnRefResultType>(), expectedVolatile,
            expectedVolatile == bsl::is_volatile<UnRefResultType>::value);
}

template <class TYPE>
template <class t_MODEL, class t_ARG, class t_RESULT>
void TestDriver<TYPE>::testSingleCase(
                                  const volatile void         *expectedAddress,
                                  bslmf::MovableRef<t_RESULT>  result)
{
    using bsls::NameOf;

    typedef typename bslmf::MovableRefUtil::RemoveReference<t_MODEL>::type
                                                              UnRefModelType;
    typedef typename bslmf::MovableRefUtil::RemoveReference<t_ARG>::type
                                                              UnRefArgType;
    typedef typename bslmf::MovableRefUtil::RemoveReference<t_RESULT>::type
                                                              UnRefResultType;

    typedef typename bsl::remove_const<UnRefArgType>::type    UnCRefArgType;
    typedef typename bsl::remove_const<UnRefResultType>::type UnCRefResultType;

    // Check that the reference, returned by the 'forward_like' points to the
    // same object that has been passed as a parameter to this function.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedAddress == &static_cast<t_RESULT&>(result));

    // Check the identity of the stripped types.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnCRefArgType>(), NameOf<UnCRefResultType>(),
            (bsl::is_same<UnCRefArgType, UnCRefResultType>::value));

    bool expectedConst     = bsl::is_const<UnRefModelType>::value ||
                             bsl::is_const<UnRefArgType>::value;
    bool expectedLvalueRef =
                      bslmf::MovableRefUtil::IsLvalueReference<t_MODEL>::value;
    bool expectedVolatile  = bsl::is_volatile<UnRefArgType>::value;

    // Check if lvalue reference-ness has not been expected.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            false == expectedLvalueRef);

    // Check if constancy has been added.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            expectedConst == bsl::is_const<t_RESULT>::value);

    // Check if volatile-ness has been preserved.

    ASSERTV(NameOf<t_MODEL>(), NameOf<t_ARG>(), NameOf<t_RESULT>(),
            NameOf<UnRefResultType>(), expectedVolatile,
            expectedVolatile == bsl::is_volatile<UnRefResultType>::value);
}
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class t_TYPE>
template <class t_MODEL>
void TestDriver<t_TYPE>::testSingleModelType()
{
    typedef bslmf::Util                     U;
    typedef bslmf::MovableRefUtil           MU;
#ifndef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    typedef bslmf::MovableRef<t_TYPE>       MovableRef;
    typedef bslmf::MovableRef<const t_TYPE> MovableRefConst;
#endif

    t_TYPE obj;

    testSingleCase<t_MODEL, t_TYPE>(&obj, U::forward_like<t_MODEL>(obj));

    testSingleCase<t_MODEL, t_TYPE&>(
                          &obj,
                          U::forward_like<t_MODEL>(static_cast<t_TYPE&>(obj)));

    testSingleCase<t_MODEL, const t_TYPE>(
                    &obj,
                    U::forward_like<t_MODEL>(static_cast<const t_TYPE&>(obj)));

    testSingleCase<t_MODEL, const t_TYPE&>(
                    &obj,
                    U::forward_like<t_MODEL>(static_cast<const t_TYPE&>(obj)));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    testSingleCase<t_MODEL, t_TYPE&&>(&obj,
                                      U::forward_like<t_MODEL>(MU::move(obj)));

    testSingleCase<t_MODEL, const t_TYPE&&>(
          &obj,
          U::forward_like<t_MODEL>(MU::move(static_cast<const t_TYPE&>(obj))));
#else
    testSingleCase<t_MODEL, MovableRef>(
                                      &obj,
                                      U::forward_like<t_MODEL>(MU::move(obj)));

    testSingleCase<t_MODEL, MovableRefConst>(
          &obj,
          U::forward_like<t_MODEL>(MU::move(static_cast<const t_TYPE&>(obj))));
#endif

    testSingleCase<t_MODEL, volatile t_TYPE>(
                 &obj,
                 U::forward_like<t_MODEL>(static_cast<volatile t_TYPE&>(obj)));

    testSingleCase<t_MODEL, const volatile t_TYPE>(
           &obj,
           U::forward_like<t_MODEL>(static_cast<const volatile t_TYPE&>(obj)));

    testSingleCase<t_MODEL, volatile t_TYPE&>(
                 &obj,
                 U::forward_like<t_MODEL>(static_cast<volatile t_TYPE&>(obj)));

    testSingleCase<t_MODEL, const volatile t_TYPE&>(
           &obj,
           U::forward_like<t_MODEL>(static_cast<const volatile t_TYPE&>(obj)));
}

// TEST CASES
template <class TYPE>
void TestDriver<TYPE>::testCase2()
    // ------------------------------------------------------------------------
    // TESTING 'forward_like'
    //
    // Concerns:
    //: 1 The 'forward_like' function template accepts arguments of any type.
    //:
    //: 2 The return value of the 'forward_like' has expected type and its
    //:   address is the same as the address of function argument.
    //:
    //: 3 'volatile' qualifier is ignored during result type deducing.
    //
    // Plan:
    //: 1 Exercise function parameterized with different 'model' types passing
    //:   arguments of various types and verify the results.  The results are
    //:   verified by
    //:   o comparing the stripped (with removed reference-ness and
    //:     constant-ness) types of passed object and result object
    //:   o checking if the resulting type is const if the 'model' type is
    //:     const or if the function argument type was const
    //:   o check if the resulting type is lvalue reference if the 'model' type
    //:     is lvalue reference  (C-1..3)
    //
    // Testing:
    //   template <class T, class U> forward_like(U&& value)
    // ------------------------------------------------------------------------
{
    testSingleModelType<                        TYPE  >();
    testSingleModelType<                        TYPE& >();
    testSingleModelType<                  const TYPE  >();
    testSingleModelType<                  const TYPE& >();
    testSingleModelType<                        TYPE *>();
    testSingleModelType<                  const TYPE *>();
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    testSingleModelType<                        TYPE&&>();
    testSingleModelType<                  const TYPE&&>();
#else
    testSingleModelType<bslmf::MovableRef<      TYPE> >();
    testSingleModelType<bslmf::MovableRef<const TYPE> >();
#endif
    testSingleModelType<         volatile       TYPE  >();
    testSingleModelType<         volatile       TYPE& >();
    testSingleModelType<         volatile const TYPE  >();
    testSingleModelType<         volatile const TYPE& >();
}

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bslmf::Util::forward
///---------------------------------------
// Clients should generally not use 'bsls::Util::forward' directly, instead it
// should be used via 'BSLS_COMPILERFEATURES_FORWARD' in conjunction with
// 'BSLS_COMPILERFEATURES_FORWARD_REF'.  Here we show a simple function using
// 'BSLS_COMPILERFEATURES_FORWARD':
//..
    template <class RESULT_TYPE>
    struct FactoryUtil {
//
       template <class ARG_TYPE>
       RESULT_TYPE create(BSLS_COMPILERFEATURES_FORWARD_REF(ARG_TYPE) arg) {
         return RESULT_TYPE(BSLS_COMPILERFEATURES_FORWARD(ARG_TYPE, arg));
       }
    };
//..
// Notice that 'bsls::Util::forward' is only used in conjunction with
// 'BSLS_COMPILERFEATURES_FORWARD_REF' because, in the example above, if the
// 'create' function's parameter type was 'ARG_TYPE&& ' then it is a
// C++11-only(!) forwarding reference, and we would simply use the standard
// 'std::forward'.  Alternatively, if the parameter type was
// 'MovableRef<ARG_TYPE>' then 'arg' is *not* a forwarding-reference to be
// forwarded (certainly not in C++03).
//
///Example 2: Using 'bslmf::Util::forwardAsReference'
///--------------------------------------------------
// Suppose we have a class 'S1' that has a regular copy constructor, and only
// if the compiler supports rvalue references has to move constructor.  We want
// to construct it with the move constructor if moves are supported and as a
// copy otherwise.  Then we use 'bslmf::Util::forwardAsReference':
//..
    struct S {
        S();
        S(const S&);
    #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        S(S&&);
    #endif
    };

    S::S() {}

    S::S(const S&)
    {
        printf("S copy c'tor\n");
    }

    #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    S::S(S&&)
    {
        printf("S move c'tor\n");
    }
    #endif

    void doThis2(S s)
    {
        // ...
(void) s;
    }

    void doThat2(bslmf::MovableRef<S> value)
    {
        doThis2(bslmf::Util::forwardAsReference<S>(value));
    }
//..
//
///Example 3: Using 'bslmf::Util::moveIfSupported'
///-----------------------------------------------
// Suppose we had a function that takes a non-const lvalue-ref, and only when
// the compiler supports rvalue references also has an overload that takes
// rvalue references:
//..
    void doSomething(S&)
    {
        printf("doSomething lvalue-ref\n");
    }

    #ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void doSomething(S&&)
    {
        printf("doSomething rvalue-ref\n");
    }
    #endif

    void doSomethingElse(S value)
    {
        doSomething(bslmf::Util::moveIfSupported(value));
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the usage of this component, and ensure that usage
        //:   examples in the header file are syntactically correct and work
        //:   properly, giving the output they are represent as giving.
        //
        // Plan:
        //: 1 Reproduce the code in the usage examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Then, in 'main':
//..
{
    S s;

    doThat2(bslmf::MovableRefUtil::move(s));
}
//..
// output in C++03:
//..
//    S copy c'tor
//..
// output in C++11:
//..
//    S move c'tor
//..

// Then, in 'main':
{
    S s;

    doSomethingElse(s);
}
//..
// output in C++03:
//..
//    S copy c'tor
//    doSomething lvalue-ref
//..
// output in C++11:
//    S copy c'tor
//    doSomething rvalue-ref
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'forward_like'
        //
        // Concerns:
        //: 1 The 'forward_like' function template accepts arguments of any
        //:   type.
        //:
        //: 2 The return value of the 'forward_like' has expected type and its
        //:   address is the same as the address of function argument.
        //:
        //: 3 'volatile' qualifier is ignored during result type deducing.
        //
        // Plan:
        //: 1 Exercise function parameterized with different 'model' types
        //:   passing arguments of various types and verify the results.  The
        //:   results are verified by
        //:   o comparing the stripped (with removed reference-ness and
        //:     constant-ness) types of passed object and result object
        //:   o checking if the resulting type is const if the 'model' type is
        //:     const or if the function argument type was const
        //:   o check if the resulting type is lvalue reference if the 'model'
        //:     type is lvalue reference  (C-1..3)
        //
        // Testing:
        //   template <class T, class U> forward_like(U&& value)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'forward_like'"
                            "\n======================\n");

        TestDriver<int   >::testCase2();
        TestDriver<int  *>::testCase2();
        TestDriver<int **>::testCase2();
        TestDriver<Obj   >::testCase2();
        TestDriver<Obj  *>::testCase2();
        TestDriver<Obj **>::testCase2();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality but *tests* *nothing*.
        //
        // Concerns:
        //: 1 That the functions exist with the documented signatures.
        //: 2 That the basic functionality works as documented.
        //
        // Plan:
        //: 1 Exercise each function in turn and devise an elementary test
        //:   sequence to ensure that the basic functionality is as documented.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
            printf("\tTest Util::forward\n");
        }
        {
            Obj        mA;
            const Obj& A = mA;
            ASSERT(e_NONE == A.type());

            bsls::ObjectBuffer<Obj> buf;
            Obj&                    mX = buf.object();
            const Obj&              X  = mX;

            CUtil::construct(buf.address(), A);
            ASSERT(e_CONST_LVALUE_REF == X.type());

            CUtil::construct(buf.address(), mA);
            ASSERT(e_CONST_LVALUE_REF == X.type());

            CUtil::construct(buf.address(), bslmf::MovableRefUtil::move(mA));
            ASSERT(e_RVALUE_REF == X.type());
        }

        if (verbose) {
            printf("\tTest Util::declval\n");
        }
        {
            ASSERT(sizeof(ParameterTypes) ==
                   sizeof(bslmf::Util::declval<Obj>().type()));
        }

        if (verbose) {
            printf("\tTest Util::forwardAsRef\n");
        }
        {
            Obj        mA;
            const Obj& A = mA;
            ASSERT(e_NONE == A.type());

            bsls::ObjectBuffer<Obj> buf;
            Obj&                    mX = buf.object();
            const Obj&              X  = mX;

            CUtil::construct(buf.address(),
                             bslmf::Util::forwardAsReference<Obj>(mA));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(e_RVALUE_REF == X.type());
#else
            ASSERT(e_CONST_LVALUE_REF  == X.type());
#endif

            CUtil::construct(buf.address(),
                             bslmf::Util::forwardAsReference<Obj>(
                                 bslmf::MovableRefUtil::move(mA)));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(e_RVALUE_REF == X.type());
#else
            ASSERT(e_CONST_LVALUE_REF  == X.type());
#endif
        }

        if (verbose) {
            printf("\tTest Util::moveIfSupported\n");
        }
        {
            Obj movedFrom;
            Obj movedTo(bslmf::Util::moveIfSupported(movedFrom));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ASSERT(e_RVALUE_REF == movedTo.type());
#else
            ASSERT(e_CONST_LVALUE_REF  == movedTo.type());
#endif
        }
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
// Copyright 2016 Bloomberg Finance L.P.
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
