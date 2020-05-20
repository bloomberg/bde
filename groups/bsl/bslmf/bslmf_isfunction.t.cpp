// bslmf_isfunction.t.cpp                                             -*-C++-*-
#include <bslmf_isfunction.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
# define BSLMF_ISFUNCTION_NO_ABOMINABLE_TYPES
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_function' and a
// template variable 'bsl::is_function_v', that determine whether a template
// parameter type is a function type.  Thus, we need to ensure that the value
// returned by this meta-function is correct for each possible category of
// types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_function::value
// [ 1] bsl::is_function_v
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

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.
};

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_function'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_function'.

typedef void (*FunctionPtrTestType) ();
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.

typedef void VoidRetNoParamFunctionTestType();
    // This function type having 'void' return type and no parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef void VoidRetParamFunctionTestType(int);
    // This function type having 'void' return type and two 'int' parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int IntRetNoParamFunctionTestType();
    // This function type having 'int' return type and no parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int IntRetParamFunctionTestType(int, int);
    // This function type having 'int' return type and two 'int' parameters is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_function'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_function'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_function'.

struct Abstract {
    // This abstract class is intended to be used for testing as the template
    // parameter 'TYPE' of 'bsl::is_function'.

    virtual ~Abstract() = 0;
};

class NotConstructible {
    // This awkward class is intended to be used for testing as the template
    // parameter 'TYPE' of 'bsl::is_function'.

    NotConstructible(const NotConstructible&);
};

class EvilOverloads {
    // This awkward class is intended to be used for testing as the template
    // parameter 'TYPE' of 'bsl::is_function'.

    void operator&();

    template <class OTHER>
    void operator,(const OTHER&);
};



}  // close unnamed namespace

extern "C" {
typedef int ExternCFunc();
typedef int ExternCFuncElipsis(...);
typedef int ExternCFunc14       (int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int);
typedef int ExternCFunc14Elipsis(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int...);
typedef int ExternCFunc15       (int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int);
typedef int ExternCFunc15Elipsis(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int...);

}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#define ASSERT_V_SAME(TYPE)                                                   \
    ASSERT( bsl::is_function<TYPE>::value == bsl::is_function_v<TYPE>)
    // 'ASSERT' that 'is_function_v' has the same value as
    // 'is_function::value'.
#else
#define ASSERT_V_SAME(TYPE)
#endif

#define TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE, result)                      \
    ASSERT(result == META_FUNC<TYPE>::value);                                 \
    ASSERT_V_SAME(TYPE)
    // Test that the result of 'META_FUNC' has the same value as the expected
    // 'result'.  Confirm that the result value of the 'META_FUNC' and the
    // value of the 'META_FUNC_v' variable are the same.

#define TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE, result)                       \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC,                TYPE, result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, const          TYPE, result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC,       volatile TYPE, result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, const volatile TYPE, result);
    // Test cv-qualified combinations on the specified 'TYPE'.

#define TYPE_ASSERT_CVQ_SUFFIX(META_FUNC, TYPE, result)                       \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE,                result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const,          result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE volatile,       result);          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const volatile, result);
    // Test cv-qualified combinations on the specified 'TYPE'.

#define TYPE_ASSERT_CVQ_REF(META_FUNC, TYPE, result)                          \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE&,                result);         \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const&,          result);         \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE volatile&,       result);         \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const volatile&, result);
    // Test references to cv-qualified combinations on the specified 'TYPE'.

#define TYPE_ASSERT_CVQ_REF_NOEXCEPT(META_FUNC, TYPE, result)                 \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE& noexcept,                result);\
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const& noexcept,          result);\
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE volatile& noexcept,       result);\
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const volatile& noexcept, result);
    // Test references to cv-qualified combinations on the specified 'noexcept'
    // 'TYPE'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TYPE_ASSERT_CVQ_RVALREF(META_FUNC, TYPE, result)                     \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE&&,                result);        \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const&&,          result);        \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE volatile&&,       result);        \
    TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const volatile&&, result);
    // Test an r-value  references to cv-qualified combinations on the
    // specified 'TYPE'.
#else
# define TYPE_ASSERT_CVQ_RVALREF(META_FUNC, TYPE, result)
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(META_FUNC, TYPE, result)            \
   TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE&& noexcept,                result);\
   TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const&& noexcept,          result);\
   TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE volatile&& noexcept,       result);\
   TYPE_ASSERT_IS_FUNCTION(META_FUNC, TYPE const volatile&& noexcept, result);
    // Test an r-value  references to cv-qualified combinations on the
    //  specified 'noexcept' 'TYPE'.
#else
# define TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(META_FUNC, TYPE, result)
#endif

#define TYPE_ASSERT_CVQ(META_FUNC, TYPE, result)                     \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE,                result);  \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const,          result);  \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE       volatile, result);  \
    TYPE_ASSERT_CVQ_PREFIX(META_FUNC, TYPE const volatile, result);
    // Test all cv-qualified combinations on the specified 'TYPE'.

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
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for a non-function type
// and a function type, and assert the 'value' static data member of each
// instantiation:
//..
    ASSERT(false == bsl::is_function<int>::value);
    ASSERT(true  == bsl::is_function<int (int)>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_function_v' variable as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(false == bsl::is_function_v<int>);
    ASSERT(true  == bsl::is_function_v<int (int)>);
#endif
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bsl::is_function::value'
        //   Ensure that the static data member 'value' of 'bsl::is_function'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type.
        //:
        //: 3 'is_function::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer or pointer-to-member type.
        //:
        //: 4 'is_function::value' is 'false' when 'TYPE' is a reference type,
        //:   and 'false' for function references in particular.
        //:
        //: 5 'is_function::value' is 'true' when 'TYPE' is a function type.
        //:   Note that cv-qualified is irrelevant for a function type.
        //:
        //: 6  That 'is_function<T>::value' has the same value as
        //:    'is_function_v<T>' for a variety of template parameter types.
        //
        // Plan:
        //   Verify that 'bsl::is_function::value' has the correct value for
        //   each (template parameter) 'TYPE' in the concerns.
        //
        // Testing:
        //   bsl::is_function::value
        // --------------------------------------------------------------------

        if (verbose) printf("'bsl::is_function::value'\n"
                            "=========================\n");

        // C-1

        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void,      false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int,       false);
        TYPE_ASSERT_CVQ_REF   (bsl::is_function, int,       false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int[],     false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int[2],    false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int[][2],  false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, int[2][3], false);

        // C-2

        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, EnumTestType,        false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, EnumTestType,        false);
        TYPE_ASSERT_CVQ_RVALREF (bsl::is_function, EnumTestType,        false);
        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, StructTestType,      false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, StructTestType,      false);
        TYPE_ASSERT_CVQ_RVALREF (bsl::is_function, StructTestType,      false);
        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, UnionTestType,       false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, UnionTestType,       false);
        TYPE_ASSERT_CVQ_RVALREF (bsl::is_function, UnionTestType,       false);
        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, BaseClassTestType,   false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, BaseClassTestType,   false);
        TYPE_ASSERT_CVQ_RVALREF (bsl::is_function, BaseClassTestType,   false);
        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, DerivedClassTestType,false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, DerivedClassTestType,false);
        TYPE_ASSERT_CVQ_RVALREF (bsl::is_function, DerivedClassTestType,false);
        TYPE_ASSERT_CVQ_SUFFIX  (bsl::is_function, Incomplete,          false);
        TYPE_ASSERT_CVQ_REF     (bsl::is_function, Incomplete,          false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, Incomplete[],        false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, Incomplete[2],       false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, Incomplete[][2],     false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, Incomplete[2][3],    false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, Abstract,            false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, NotConstructible,    false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, NotConstructible[],  false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, NotConstructible[2], false);
        TYPE_ASSERT_CVQ_PREFIX(bsl::is_function, NotConstructible[][2], false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, NotConstructible[2][3],
                                                                        false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, EvilOverloads,       false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, EvilOverloads[],     false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, EvilOverloads[3],    false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, EvilOverloads[][3],  false);
        TYPE_ASSERT_CVQ_PREFIX  (bsl::is_function, EvilOverloads[3][2], false);

        // C-3

        TYPE_ASSERT_CVQ(bsl::is_function, int*,                         false);
        TYPE_ASSERT_CVQ(bsl::is_function, StructTestType*,              false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::*,        false);
        TYPE_ASSERT_CVQ(bsl::is_function, int StructTestType::* *,      false);
        TYPE_ASSERT_CVQ(bsl::is_function, UnionTestType*,               false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::*,     false);
        TYPE_ASSERT_CVQ(bsl::is_function, PMD BaseClassTestType::* *,   false);
        TYPE_ASSERT_CVQ(bsl::is_function, BaseClassTestType*,           false);
        TYPE_ASSERT_CVQ(bsl::is_function, DerivedClassTestType*,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, Incomplete*,           false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFunc*,          false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFuncElipsis*,   false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFunc14*,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFunc14Elipsis*, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFunc15*,        false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, ExternCFunc15Elipsis*, false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, MethodPtrTestType,     false);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, FunctionPtrTestType,   false);


        TYPE_ASSERT_CVQ_REF(bsl::is_function, int*,                     false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, StructTestType*,          false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int StructTestType::* *,  false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, UnionTestType*,           false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_REF(
                          bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, BaseClassTestType*,       false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, DerivedClassTestType*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, Incomplete*,              false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, MethodPtrTestType,        false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, FunctionPtrTestType,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFunc*,             false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFuncElipsis*,      false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFunc14*,           false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFunc14Elipsis*,    false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFunc15*,           false);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, ExternCFunc15Elipsis*,    false);

        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, int*,                 false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, StructTestType*,      false);
        TYPE_ASSERT_CVQ_RVALREF(
                               bsl::is_function, int StructTestType::*, false);
        TYPE_ASSERT_CVQ_RVALREF(
                             bsl::is_function, int StructTestType::* *, false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, UnionTestType*,       false);
        TYPE_ASSERT_CVQ_RVALREF(
                            bsl::is_function, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQ_RVALREF(
                          bsl::is_function, PMD BaseClassTestType::* *, false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, BaseClassTestType*,   false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, DerivedClassTestType*,false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, Incomplete*,          false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, MethodPtrTestType,    false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, FunctionPtrTestType,  false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFunc*,         false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFuncElipsis*,  false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFunc14*,       false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFunc14Elipsis*,false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFunc15*,       false);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, ExternCFunc15Elipsis*,false);

        // C-4

        // Functions cannot be cv-qualified, so it suffices to check a simple
        // reference to each kind of function signature, without cv-qualfiers.
        // Similarly, it is not possible to form a reference to an "sbominable"
        // function, so there are no cv-qualifiers to check elsewhere.

        ASSERT(false == bsl::is_function<int  (&)(int )>::value);
        ASSERT(false == bsl::is_function<void (&)(void)>::value);
        ASSERT(false == bsl::is_function<int  (&)(void)>::value);
        ASSERT(false == bsl::is_function<void (&)(int )>::value);

        ASSERT(false == bsl::is_function<int  (&)(int...)>::value);
        ASSERT(false == bsl::is_function<void (&)(   ...)>::value);
        ASSERT(false == bsl::is_function<int  (&)(   ...)>::value);
        ASSERT(false == bsl::is_function<void (&)(int...)>::value);

        ASSERT((false == bsl::is_function<
                         int (&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int...)>::value));

        ASSERT((false == bsl::is_function<
                         int (&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int...)>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(false == bsl::is_function<int  (&&)(int )>::value);
        ASSERT(false == bsl::is_function<void (&&)(void)>::value);
        ASSERT(false == bsl::is_function<int  (&&)(void)>::value);
        ASSERT(false == bsl::is_function<void (&&)(int )>::value);

        ASSERT(false == bsl::is_function<int  (&&)(int...)>::value);
        ASSERT(false == bsl::is_function<void (&&)(   ...)>::value);
        ASSERT(false == bsl::is_function<int  (&&)(   ...)>::value);
        ASSERT(false == bsl::is_function<void (&&)(int...)>::value);

        ASSERT((false == bsl::is_function<
                         int (&&)(int, int, int, int, int, int, int, int, int,
                                  int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (&&)(int, int, int, int, int, int, int, int, int,
                                  int, int, int, int, int...)>::value));

        ASSERT((false == bsl::is_function<
                         int (&&)(int, int, int, int, int, int, int, int, int,
                                  int, int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (&&)(int, int, int, int, int, int, int, int, int,
                                  int, int, int, int, int, int...)>::value));
#endif

        ASSERT(false == bsl::is_function<int  (*)(int )>::value);
        ASSERT(false == bsl::is_function<void (*)(void)>::value);
        ASSERT(false == bsl::is_function<int  (*)(void)>::value);
        ASSERT(false == bsl::is_function<void (*)(int )>::value);

        ASSERT(false == bsl::is_function<int  (*)(int...)>::value);
        ASSERT(false == bsl::is_function<void (*)(   ...)>::value);
        ASSERT(false == bsl::is_function<int  (*)(   ...)>::value);
        ASSERT(false == bsl::is_function<void (*)(int...)>::value);

        ASSERT((false == bsl::is_function<
                         int (*)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (*)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int...)>::value));

        ASSERT((false == bsl::is_function<
                         int (*)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int)>::value));
        ASSERT((false == bsl::is_function<
                         int (*)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int, int...)>::value));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        ASSERT(false == bsl::is_function<int  (&)(int ) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (&)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&)(int ) noexcept>::value);

        ASSERT(false == bsl::is_function<int  (&)(int...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (&)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&)(int...) noexcept>::value);

        ASSERT((false == bsl::is_function<
                         int (&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                      int (&)(int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...) noexcept>::value));

        ASSERT((false == bsl::is_function<
                      int (&)(int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                      int (&)(int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...) noexcept>::value));

        // If a compiler supports C++17 noexcept as part of the function type,
        // it should already support C++11 cv-qualifiers in the function type.
        ASSERT(false == bsl::is_function<int  (&&)(int ) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&&)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (&&)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&&)(int ) noexcept>::value);

        ASSERT(false == bsl::is_function<int  (&&)(int...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&&)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (&&)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (&&)(int...) noexcept>::value);

        ASSERT((false == bsl::is_function<
                         int (&&)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                        int (&&)(int, int, int, int, int, int, int, int, int,
                                int, int, int, int, int...) noexcept>::value));

        ASSERT((false == bsl::is_function<
                     int (&&)(int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                  int (&&)(int, int, int, int, int, int, int, int, int,
                           int, int, int, int, int, int...) noexcept>::value));

        ASSERT(false == bsl::is_function<int  (*)(int ) noexcept>::value);
        ASSERT(false == bsl::is_function<void (*)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (*)(void) noexcept>::value);
        ASSERT(false == bsl::is_function<void (*)(int ) noexcept>::value);

        ASSERT(false == bsl::is_function<int  (*)(int...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (*)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<int  (*)(   ...) noexcept>::value);
        ASSERT(false == bsl::is_function<void (*)(int...) noexcept>::value);

        ASSERT((false == bsl::is_function<
                         int (*)(int, int, int, int, int, int, int, int, int,
                                 int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                        int (*)(int, int, int, int, int, int, int, int, int,
                                int, int, int, int, int...) noexcept>::value));

        ASSERT((false == bsl::is_function<
                      int (*)(int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int, int) noexcept>::value));
        ASSERT((false == bsl::is_function<
                      int (*)(int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...) noexcept>::value));
#endif

        // C-5

        // Note: as the macros perform textual substitution on the source code,
        // we are actually testing "abominable" function types, rather than
        // cv-qualified function types (which do not exist) or function
        // references.

#if defined(BSLMF_ISFUNCTION_NO_ABOMINABLE_TYPES)
        ASSERT((bsl::is_function< int  (int )>::value));
        ASSERT((bsl::is_function< void (void)>::value));
        ASSERT((bsl::is_function< int  (void)>::value));
        ASSERT((bsl::is_function< void (int )>::value));

        ASSERT((bsl::is_function< int  (int...)>::value));
        ASSERT((bsl::is_function< void (   ...)>::value));
        ASSERT((bsl::is_function< int  (   ...)>::value));
        ASSERT((bsl::is_function< void (int...)>::value));

        ASSERT((bsl::is_function<
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int)>::value));
        ASSERT((bsl::is_function<
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...)>::value));

        ASSERT((bsl::is_function<
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int)>::value));
        ASSERT((bsl::is_function<
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...)>::value));
#else
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int  (int ),   true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void (void),   true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int  (void),   true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void (int ),   true);

        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int  (int...), true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void (   ...), true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, int  (   ...), true);
        TYPE_ASSERT_CVQ_SUFFIX(bsl::is_function, void (int...), true);

        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_SUFFIX(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...),
                         true);

# if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int (int ), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, void(void), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int (void), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, void(int ), true);

        TYPE_ASSERT_CVQ_REF(bsl::is_function, int (int...), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, void(   ...), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, int (   ...), true);
        TYPE_ASSERT_CVQ_REF(bsl::is_function, void(int...), true);

        TYPE_ASSERT_CVQ_REF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_REF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_REF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_REF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, int (int ), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, void(void), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, int (void), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, void(int ), true);

        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, int (int...), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, void(   ...), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, int (   ...), true);
        TYPE_ASSERT_CVQ_RVALREF(bsl::is_function, void(int...), true);

        TYPE_ASSERT_CVQ_RVALREF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_RVALREF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_RVALREF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_RVALREF(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...),
                         true);
# endif

# if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        // If a compiler supports C++17 noexcept as part of the function type,
        // it should already support C++11 cv-qualifiers in the function type.

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, int (int ), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, void(void), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, int (void), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, void(int ), true);

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, int (int...), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, void(   ...), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, int (   ...), true);
        TYPE_ASSERT_CVQ_REF_NOEXCEPT(bsl::is_function, void(int...), true);

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_REF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...),
                         true);


        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, int (int ), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, void(void), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, int (void), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, void(int ), true);

        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, int (int...), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, void(   ...), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, int (   ...), true);
        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function, void(int...), true);

        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int...),
                         true);

        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int),
                         true);

        TYPE_ASSERT_CVQ_RVALREF_NOEXCEPT(
                         bsl::is_function,
                         int (int, int, int, int, int, int, int, int, int, int,
                              int, int, int, int, int...),
                         true);
# endif

#endif

        // Confirm that there are no surprises when function types are supplied
        // using a typedef-name.

        ASSERT(true == bsl::is_function<IntRetParamFunctionTestType>::value);
        ASSERT(
              true == bsl::is_function<VoidRetNoParamFunctionTestType>::value);
        ASSERT(true == bsl::is_function<IntRetNoParamFunctionTestType>::value);
        ASSERT(true == bsl::is_function<VoidRetParamFunctionTestType>::value);

        ASSERT(true == bsl::is_function<ExternCFunc>::value);
        ASSERT(true == bsl::is_function<ExternCFuncElipsis>::value);
        ASSERT(true == bsl::is_function<ExternCFunc14>::value);
        ASSERT(true == bsl::is_function<ExternCFunc14Elipsis>::value);
        ASSERT(true == bsl::is_function<ExternCFunc15>::value);
        ASSERT(true == bsl::is_function<ExternCFunc15Elipsis>::value);

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
// Copyright 2017 Bloomberg Finance L.P.
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
