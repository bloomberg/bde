// bslmf_ismemberpointer.t.cpp                                        -*-C++-*-
#include <bslmf_ismemberpointer.h>

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_nullptr.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function, 'bsl::is_member_pointer'
// and a template variable 'bsl::is_member_pointer_v', that determine whether
// a template parameter type is a pointer to non-static member (object or
// function).  Thus, we need to ensure that the value returned by this
// meta-functions is correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bsl::is_member_pointer::value
// [ 1] bsl::is_member_pointer_v
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
//              PLATFORM-SPECIFIC MACROS FOR WORKAROUNDS
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
# define BSLMF_ISMEMBERPOINTER_NO_ABOMINABLE_TYPES 1
#endif

// Visual Studio 2017 and 2019 in C++17 mode (when 'noexcept' types are
// supported) runs out of heap space.  Factoring out the large test case (case
// 1) was attempted and it made no difference.  The conditions below are
// structured so that they remove tests for known (not future) compiler
// versions only, so we automatically put them back once the compilers are
// fixed.
#if defined(BSLS_PLATFORM_CMP_MSVC) &&                                        \
    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
// Visual Studio 2019 runs out of memory compiling this file.  If you have
// installed a new update and still runs out of memory, bump the upper limit
// number in the '#if' below to 1925, and do not forget to update the number in
// this comment as well.
# if (BSLS_PLATFORM_CMP_VERSION >= 1920) && (BSLS_PLATFORM_CMP_VERSION <= 1924)
#   define DO_FEWER_TESTS 1
# endif
// Visual Studio 2017 runs out of memory optimizing this file.  If you have
// installed a new update and still runs out of memory, bump the upper limit
// number in the '#if' below to 1917, and do not forget to update the number in
// this comment as well.
# if (BSLS_PLATFORM_CMP_VERSION >= 1910) && (BSLS_PLATFORM_CMP_VERSION <= 1916)
#   if defined(BDE_BUILD_TARGET_OPT)
#     define DO_FEWER_TESTS 1
#   endif
# endif
#endif

// #define BSLMF_ISMEMBERPOINTER_SHOW_FAIL_FOR_DIAGNOSTICS 1
//   Define this macro to generate expected test failures for types that might
//   have awkward diagnostics, to verify that all the necessary information is
//   available to correctly diagnose a test failure.

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
#endif

//=============================================================================
//                          MACROS USEFUL FOR TESTING
//-----------------------------------------------------------------------------

#define INT_00
#define INT_01 int
#define INT_02 INT_01, int
#define INT_03 INT_02, int
#define INT_04 INT_03, int
#define INT_05 INT_04, int
#define INT_06 INT_05, int
#define INT_07 INT_06, int
#define INT_08 INT_07, int
#define INT_09 INT_08, int
#define INT_10 INT_09, int
#define INT_11 INT_10, int
#define INT_12 INT_11, int
#define INT_13 INT_12, int
#define INT_14 INT_13, int
#define INT_15 INT_14, int
#define INT_16 INT_15, int
#define INT_17 INT_16, int
#define INT_18 INT_17, int
#define INT_19 INT_18, int
#define INT_20 INT_19, int
#define INT_21 INT_20, int
    // This set of macros will make it much simpler to declare function types
    // with a specific number of parameters, and to clearly see that number
    // without counting each individual parameter to audit coverage.  Various
    // BDE metaprogramming utilities support 10, 14, 16, and 20 arguments, so
    // provide a full set up to 21 in order to cover anticipated boundary
    // conditions.


// The following macros provide most of the test machinery used to validate the
// trait under test in this test driver.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES)
# define ASSERT_V_EQ_VALUE(TYPE)                                              \
    ASSERT(bsl::is_member_pointer  <TYPE>::value ==                           \
           bsl::is_member_pointer_v<TYPE>)
    // Test whether 'bsl::is_member_pointer_v<TYPE>' value equals to
    // 'bsl::is_member_pointer<TYPE>::value'.
#else
# define ASSERT_V_EQ_VALUE(TYPE)
#endif

#define TYPE_ASSERT(META_FUNC, TYPE, result)                                  \
    ASSERT(result == META_FUNC<TYPE>::value);                                 \
    ASSERT_V_EQ_VALUE(TYPE)
    // Test that the result of 'META_FUNC' has the same value as the expected
    // 'result'.  Confirm that the result value of the 'META_FUNC' and the
    // value of the 'META_FUNC_v' variable are the same.



#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE)            \
    TYPE_ASSERT(META_FUNC, TYPE,                   false);      \
    TYPE_ASSERT(META_FUNC, TYPE &,                 false);
#else
# define TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE)            \
    TYPE_ASSERT(META_FUNC, TYPE,                   false);      \
    TYPE_ASSERT(META_FUNC, TYPE &,                 false);      \
    TYPE_ASSERT(META_FUNC, TYPE &&,                false);
#endif


#define TEST_CV_OBJECT(META_FUNC, TYPE)                         \
    TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE               ); \
    TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE const         ); \
    TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE       volatile); \
    TEST_OBJECT_AND_REFERENCES(META_FUNC, TYPE const volatile);

#define TEST_OBJECT(META_FUNC, TYPE)                            \
    TEST_CV_OBJECT(META_FUNC, TYPE  );                          \
    TEST_CV_OBJECT(META_FUNC, TYPE *);


#define TEST_ARRAY(META_FUNC, TYPE)                             \
    TYPE_ASSERT(META_FUNC, TYPE[],       false);                \
    TYPE_ASSERT(META_FUNC, TYPE[2],      false);                \
    TYPE_ASSERT(META_FUNC, TYPE[][2],    false);                \
    TYPE_ASSERT(META_FUNC, TYPE(*)[],    false);                \
    TYPE_ASSERT(META_FUNC, TYPE(*)[2],   false);                \
    TYPE_ASSERT(META_FUNC, TYPE(*)[][2], false);                \
    TYPE_ASSERT(META_FUNC, TYPE(&)[],    false);                \
    TYPE_ASSERT(META_FUNC, TYPE(&)[2],   false);                \
    TYPE_ASSERT(META_FUNC, TYPE(&)[][2], false);


#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TEST_MEMPTR(META_FUNC, TYPE)                                         \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type Incomplete::*,    true );     \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type Incomplete::* &,  false);     \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type Incomplete::*);
#else
# define TEST_MEMPTR(META_FUNC, TYPE)                                         \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type Incomplete::*,    true );     \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type Incomplete::* &,  false);     \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type Incomplete::* &&, false);     \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type Incomplete::*);
#endif


// Define the macro 'TEST_FUNCTION(META_FUNC, TYPE, result)' to incrementally
// support testing of cv-qualified abominable functions, cv-ref qualified
// abominable functions, and cv-ref-noexcept qualified abominable functions,
// according to the level of support offered by the compiler.
#if defined(BSLMF_ISMEMBERPOINTER_NO_ABOMINABLE_TYPES)

# define TEST_FUNCTION(META_FUNC, TYPE)                        \
    TYPE_ASSERT(META_FUNC, TYPE,                   false);     \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type &, false);     \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);            \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type *);

#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

# define TEST_FUNCTION(META_FUNC, TYPE)                        \
    TYPE_ASSERT(META_FUNC, TYPE,                   false);     \
    TYPE_ASSERT(META_FUNC, TYPE const,             false);     \
    TYPE_ASSERT(META_FUNC, TYPE       volatile,    false);     \
    TYPE_ASSERT(META_FUNC, TYPE const volatile,    false);     \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type &, false);     \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);            \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type *);

#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)

# define TEST_FUNCTION(META_FUNC, TYPE)                        \
    TYPE_ASSERT(META_FUNC, TYPE,                    false);    \
    TYPE_ASSERT(META_FUNC, TYPE const,              false);    \
    TYPE_ASSERT(META_FUNC, TYPE       volatile,     false);    \
    TYPE_ASSERT(META_FUNC, TYPE const volatile,     false);    \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type  &, false);    \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type &&, false);    \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);            \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type *);

#elif !defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)

# define TEST_FUNCTION(META_FUNC, TYPE)                        \
    TYPE_ASSERT(META_FUNC, TYPE,                    false);    \
    TYPE_ASSERT(META_FUNC, TYPE const,              false);    \
    TYPE_ASSERT(META_FUNC, TYPE       volatile,     false);    \
    TYPE_ASSERT(META_FUNC, TYPE const volatile,     false);    \
    TYPE_ASSERT(META_FUNC, TYPE                  &, false);    \
    TYPE_ASSERT(META_FUNC, TYPE const            &, false);    \
    TYPE_ASSERT(META_FUNC, TYPE       volatile   &, false);    \
    TYPE_ASSERT(META_FUNC, TYPE const volatile   &, false);    \
    TYPE_ASSERT(META_FUNC, TYPE                 &&, false);    \
    TYPE_ASSERT(META_FUNC, TYPE const           &&, false);    \
    TYPE_ASSERT(META_FUNC, TYPE volatile        &&, false);    \
    TYPE_ASSERT(META_FUNC, TYPE const volatile  &&, false);    \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type  &, false);    \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type &&, false);    \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);            \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type *);

#else

# define TEST_FUNCTION(META_FUNC, TYPE)                                \
    TYPE_ASSERT(META_FUNC, TYPE,                             false);   \
    TYPE_ASSERT(META_FUNC, TYPE const,                       false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile,              false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile,              false);   \
    TYPE_ASSERT(META_FUNC, TYPE                 &,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE const           &,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile  &,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile  &,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE                &&,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE const          &&,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile &&,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile &&,           false);   \
    TYPE_ASSERT(META_FUNC, TYPE                   noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const             noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile    noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile    noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE                 & noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const           & noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile  & noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile  & noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE                && noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const          && noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE       volatile && noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, TYPE const volatile && noexcept,  false);   \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type  &,          false);   \
    TYPE_ASSERT(META_FUNC, Identity<TYPE>::type &&,          false);   \
    TYPE_ASSERT(META_FUNC, Identity<TYPE noexcept>::type  &, false);   \
    TYPE_ASSERT(META_FUNC, Identity<TYPE noexcept>::type &&, false);   \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);                    \
    TEST_OBJECT(META_FUNC, Identity<TYPE noexcept>::type *);           \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type *);                    \
    TEST_ARRAY (META_FUNC, Identity<TYPE noexcept>::type *);
#endif


// Support macro for following tests
#define ASSERT_MEMFUNC_TYPE(META_FUNC, TYPE)                   \
    TYPE_ASSERT(META_FUNC, TYPE, true);                        \
    TEST_OBJECT(META_FUNC, Identity<TYPE>::type *);            \
    TEST_ARRAY (META_FUNC, Identity<TYPE>::type);


// Member function always support cv-qualifiers. so worry less about abominable
// function types.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
# define TEST_MEMFUN_NOEXCEPT(META_FUNC, TYPE)                 \
    ASSERT_MEMFUNC_TYPE(META_FUNC, TYPE         );             \
    ASSERT_MEMFUNC_TYPE(META_FUNC, TYPE noexcept);
#else
# define TEST_MEMFUN_NOEXCEPT(META_FUNC, TYPE)                 \
    ASSERT_MEMFUNC_TYPE(META_FUNC, TYPE         );
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
# define TEST_MEMFUN_REF(META_FUNC, TYPE)                      \
    TEST_MEMFUN_NOEXCEPT(META_FUNC, TYPE   );                  \
    TEST_MEMFUN_NOEXCEPT(META_FUNC, TYPE  &);                  \
    TEST_MEMFUN_NOEXCEPT(META_FUNC, TYPE &&);
#else
# define TEST_MEMFUN_REF(META_FUNC, TYPE)                      \
    ASSERT_MEMFUNC_TYPE (META_FUNC, TYPE);
#endif

# define TEST_MEMFUN(META_FUNC, TYPE)                          \
    TEST_MEMFUN_REF(META_FUNC, TYPE);                          \
    TEST_MEMFUN_REF(META_FUNC, TYPE const);                    \
    TEST_MEMFUN_REF(META_FUNC, TYPE       volatile);           \
    TEST_MEMFUN_REF(META_FUNC, TYPE const volatile);

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE_PARAMETER>
struct Identity {
    // This template provides an easy way to obtain a 'typedef' to the template
    // parameter 'TYPE_PARAMETER'.  This is most useful when the parameter type
    // is a function or array type, as it is difficult to add pointer or
    // reference qualifiers to such types with their regular syntax, but very
    // simple to add a trailing '*' or '&' using this type.  E.g.,
    //..
    //  static_assert(is_same_v<Identity<int()>::type *, int(*)()>);
    //..
    // It is very difficult to manipulate such types within macros, where all
    // we have access to is a stream of tokens (source code) without type
    // information.

    typedef TYPE_PARAMETER type;
};

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum class EnumClassTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};
#else
enum EnumClassTestType {
    // Simpler to have a redundant test than use macros to determine test
    // support everywhere.
};
#endif

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_member_object_pointer'.
};

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_member_object_pointer'.

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Example 1: Verify Member Pointer Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are member pointer
// types.
//
// First, we create a user-defined type 'MyStruct':
//..
struct MyStruct
{
};
//..

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

// Now, we create three 'typedef's -- a member object pointer type, a member
// function pointer type and a general function pointer type:
//..
    typedef int MyStruct::* DataMemPtr;
    typedef int (MyStruct::*MyStructMethodPtr) ();
    typedef int (*MyFunctionPtr) ();
//..
// Finally, we instantiate the 'bsl::is_member_pointer' template for various
// types and assert the 'value' static data member of each instantiation:
//..
    ASSERT(false == bsl::is_member_pointer<int*>::value);
    ASSERT(false == bsl::is_member_pointer<MyFunctionPtr>::value);
    ASSERT(true  == bsl::is_member_pointer<DataMemPtr>::value);
    ASSERT(true  == bsl::is_member_pointer<MyStructMethodPtr>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature then we can re-write the snippet of code above using the
// 'bsl::is_member_pointer_v' variable as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(false == bsl::is_member_pointer_v<int*>);
    ASSERT(false == bsl::is_member_pointer_v<MyFunctionPtr>);
    ASSERT(true  == bsl::is_member_pointer_v<DataMemPtr>);
    ASSERT(true  == bsl::is_member_pointer_v<MyStructMethodPtr>);
#endif
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_member_pointer::value'
        //   Ensure that the static data member 'value' of
        //   'bsl::is_member_pointer<TYPE>' instantiations having
        //    various (template parameter) 'TYPE's has the correct value.
        //
        // Concerns:
        //:  1 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    (possibly cv-qualified) primitive type or reference to such.
        //:  2 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    (possibly cv-qualified) user-defined type or reference to such.
        //:  3 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    (possibly cv-qualified) pointer type or reference to such.
        //:  4 'is_member_pointer::value' is 'false' when 'TYPE' is an
        //:    array type or reference to such, or pointer to such.
        //:  5 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    (possibly cv-qualified) function type or reference to function
        //:    type, including abominable function types.
        //:  6 'is_member_pointer::value' is 'true' when 'TYPE' is a
        //:    (possibly cv-qualified) pointer to non-static data member type.
        //:    However, if 'TYPE' is a reference or pointer to such a type, or
        //:    an array of such a type, then the result is 'false.
        //:  7 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    pointer to pointer to data member or a member pointer to pointer
        //:    to data member.  This should be a superfluous concern, but you
        //:    never know what compiler corner cases you might uncover.
        //:  8 'is_member_pointer::value' is 'false' when 'TYPE' is a
        //:    (possibly cv-qualified) pointer to non-static member function
        //:    type, or a pointer or reference to such a type, or an array of
        //:    such a type.  Allow for the full range of function types
        //:    including zero and multiple arguments, C-style elipses, trailing
        //:    cv-qualifiers, reference-qualifiers for C++11, and 'noexcept'
        //:    specifications in C++17.
        //:  9 Concern 8 applies when the member function (not the pointer) is
        //:    cv-qualfied.
        //: 10 Concern 8 applies when the member function (not the pointer) is
        //:    ref-qualified (in C++11 and later).
        //: 11 That 'is_member_pointer<T>::value' has the same value
        //:    as 'is_member_object_pointer_v<T>'.
        //
        // Plan:
        //: 1 Create a set of test macros that can verify the expected result
        //:   of 'value' member of the 'is_member_pointer' trait for
        //:   the distinct syntax patterns of object types, array types,
        //:   function types, pointer-to-data-member types, and
        //:   pointer-to-member-function types; such macros should also handle
        //:   (recursively) cv-qualified versions of such types, and
        //:   (cv-qualified) pointers and references to such types.  Those
        //:   macros should also verify the 'bool' value of the
        //:   'is_member_object_pointer_v' variable template on build
        //:   configurations that support it.
        //: 2 Use the test macro for object types to verify the trait(s) for a
        //:   representative cross-section of fundamental types.  Note that
        //:   cv-qualfied 'void' types must be tested fairly directly, due to
        //:   the lack of support for reference-to-'void' types that would fall
        //:   out of most top-level test macro invocations.
        //: 3 Use the test macro for object types to verify the trait(s) for a
        //:   representative cross-section of user-defined types, including
        //:   enumerations, class types, union types, and incomplete class
        //:   types.
        //: 4 Use the test macro for array types to verify the trait(s) for a
        //:   representative cross-section of types that can be stored in an
        //:   array.
        //: 5 Use the test macro for function types to verify the trait(s) for
        //:   a representative cross-section of function types, including all
        //:   abominable (cv-ref-qualified) function types supported by the
        //:   current build mode; parameter lists that cover the boundaries
        //:   around common meta-programming limits used in BDE code (10, 14,
        //:   16, and 20); and functions with C-style elipses; and functions
        //:   with 'noexcept' specifications (when supported).  Such macros
        //:   should recursively handle pointers and references to such typs
        //:   (where valid), and array od pointers.
        //: 6 Use the test macro for pointer-to-data-member types to verify the
        //:   trait(s) for a representative cross-section of
        //:   pointer-to-data-member types.  While there is a surprisingly
        //:   sparse set of obviously interesting types to test, special
        //:   attention should be given to types that that may be confused as
        //:   pointer-to-member-pbject types by old compilers.  The test macros
        //:   should also test pointers and references to such types, arrays of
        //:   such types, and recursively pointers and references to arrays of
        //:   such types.
        //: 7 Finally, use the test macro for pointer-to-member-function types
        //:   to verify the trait(s) for a representative cross-section of
        //:   pointer-to-member-function types.  Test for the same variety of
        //:   parameters as highlighed for regular (and abominable) function
        //:   types, and verify that pointers and references to such types do
        //:   *not* satisfy the trait(s), nor do arrays of such types.
        //
        // Testing:
        //   bsl::is_member_pointer::value
        //   bsl::is_member_object_pointer_v
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'bsl::is_member_pointer::value'\n"
                            "=======================================\n");

        // C-1, 3
        TEST_OBJECT(bsl::is_member_pointer, bool);
        TEST_OBJECT(bsl::is_member_pointer, int);
        TEST_OBJECT(bsl::is_member_pointer, long double);

#if defined(BSLS_NULLPTR_USING_NATIVE_NULLPTR_T)
        TEST_OBJECT(bsl::is_member_pointer, bsl::nullptr_t);
#else
        TEST_MEMPTR(bsl::is_member_pointer, bsl::nullptr_t);
#endif

        // Test 'void' separately as cannot form reference to cv-'void'.

        TYPE_ASSERT(bsl::is_member_pointer, void, false);
        TYPE_ASSERT(bsl::is_member_pointer, const void, false);
        TYPE_ASSERT(bsl::is_member_pointer, volatile void, false);
        TYPE_ASSERT(bsl::is_member_pointer, const volatile void, false);
        TEST_OBJECT(bsl::is_member_pointer, void *);

        // C-2, 3
        TEST_OBJECT(bsl::is_member_pointer, EnumTestType);
        TEST_OBJECT(bsl::is_member_pointer, EnumClassTestType);
        TEST_OBJECT(bsl::is_member_pointer, StructTestType);
        TEST_OBJECT(bsl::is_member_pointer, UnionTestType);
        TEST_OBJECT(bsl::is_member_pointer, Incomplete);
        TEST_OBJECT(bsl::is_member_pointer, BaseClassTestType);
        TEST_OBJECT(bsl::is_member_pointer, DerivedClassTestType);


        // C-4
        TEST_ARRAY(bsl::is_member_pointer, int);
        TEST_ARRAY(bsl::is_member_pointer, void *);
        TEST_ARRAY(bsl::is_member_pointer, EnumTestType);
        TEST_ARRAY(bsl::is_member_pointer, UnionTestType);
        TEST_ARRAY(bsl::is_member_pointer, StructTestType);
        TEST_ARRAY(bsl::is_member_pointer, Incomplete *);


        // C-5
        typedef Incomplete X;  // Smaller name to fit tests onto a single line.
        typedef void (X::*MemFnType)();

        TEST_FUNCTION(bsl::is_member_pointer, int  (int)      );
        TEST_FUNCTION(bsl::is_member_pointer, void (void)     );
        TEST_FUNCTION(bsl::is_member_pointer, int  (void)     );
        TEST_FUNCTION(bsl::is_member_pointer, void (int)      );
        TEST_FUNCTION(bsl::is_member_pointer, MemFnType (int) );
        TEST_FUNCTION(bsl::is_member_pointer, int (MemFnType) );

        TEST_FUNCTION(bsl::is_member_pointer, int (INT_09)    );
#if !DO_FEWER_TESTS
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_10)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_11)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_12)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_13)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_14)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_15)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_16)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_17)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_18)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_19)    );
#endif
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_20)    );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_21)    );


        TEST_FUNCTION(bsl::is_member_pointer, int  (...)      );
        TEST_FUNCTION(bsl::is_member_pointer, void (...)      );
        TEST_FUNCTION(bsl::is_member_pointer, int  (int...)   );
        TEST_FUNCTION(bsl::is_member_pointer, void (int...)   );

        TEST_FUNCTION(bsl::is_member_pointer, int (INT_09...) );
#if !DO_FEWER_TESTS
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_10...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_11...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_12...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_13...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_14...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_15...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_16...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_17...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_18...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_19...) );
#endif
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_20...) );
        TEST_FUNCTION(bsl::is_member_pointer, int (INT_21...) );


        // C-6
        TEST_MEMPTR(bsl::is_member_pointer, int);
        TEST_MEMPTR(bsl::is_member_pointer, void *);
        TEST_MEMPTR(bsl::is_member_pointer, int (*)());

        TEST_MEMPTR(bsl::is_member_pointer, EnumTestType);
        TEST_MEMPTR(bsl::is_member_pointer, EnumClassTestType);
        TEST_MEMPTR(bsl::is_member_pointer, StructTestType);
        TEST_MEMPTR(bsl::is_member_pointer, UnionTestType);
        TEST_MEMPTR(bsl::is_member_pointer, Incomplete);
        TEST_MEMPTR(bsl::is_member_pointer, BaseClassTestType);
        TEST_MEMPTR(bsl::is_member_pointer, DerivedClassTestType);

        // C-7
        typedef Incomplete X;  // Smaller name to fit tests onto a single line.

        TEST_MEMPTR(bsl::is_member_pointer, int X::*);
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)());
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)(...));
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)(INT_21...));
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)() volatile);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_REF_QUALIFIERS)
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)() &&);
# if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        TEST_MEMPTR(bsl::is_member_pointer, int (X::*)() & noexcept);
# endif
#endif

#if defined(BSLMF_ISMEMBERPOINTER_SHOW_FAIL_FOR_DIAGNOSTICS)
        // Test diagnostics for readability with an interesting failure
        TEST_MEMPTR(bsl::is_member_pointer, int (int)             );
# if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        TEST_MEMPTR(bsl::is_member_pointer, int (int) && noexcept );
# endif
#endif

        // C-8, 9, 10
        typedef Incomplete X;  // Smaller name to fit tests onto a single line.

        TEST_MEMFUN(bsl::is_member_pointer, int  (X::*)(int)      );
        TEST_MEMFUN(bsl::is_member_pointer, void (X::*)(void)     );
        TEST_MEMFUN(bsl::is_member_pointer, int  (X::*)(void)     );
        TEST_MEMFUN(bsl::is_member_pointer, void (X::*)(int)      );

        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_09)    );
#if !DO_FEWER_TESTS
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_10)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_11)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_12)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_13)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_14)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_15)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_16)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_17)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_18)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_19)    );
#endif
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_20)    );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_21)    );


        TEST_MEMFUN(bsl::is_member_pointer, int  (X::*)(...)      );
        TEST_MEMFUN(bsl::is_member_pointer, void (X::*)(...)      );
        TEST_MEMFUN(bsl::is_member_pointer, int  (X::*)(int...)   );
        TEST_MEMFUN(bsl::is_member_pointer, void (X::*)(int...)   );

        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_09...) );
#if !DO_FEWER_TESTS
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_10...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_11...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_12...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_13...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_14...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_15...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_16...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_17...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_18...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_19...) );
#endif
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_20...) );
        TEST_MEMFUN(bsl::is_member_pointer, int (X::*)(INT_21...) );
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
// Copyright 2019 Bloomberg Finance L.P.
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
