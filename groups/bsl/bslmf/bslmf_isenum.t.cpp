// bslmf_isenum.t.cpp                                                 -*-C++-*-
#include <bslmf_isenum.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>

#include <bsls_bsltestutil.h>
#include <bsls_nullptr.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines two meta-functions, 'bsl::is_enum' and
// 'bslmf::IsEnum' and a template variable 'bsl::is_enum_v', that determine
// whether a template parameter type is an enumerated type.  Thus, we need to
// ensure that the value returned by these meta-functions are correct for each
// possible category of types.  Since the two meta-functions are functionally
// equivalent, we will use the same set of types for both.
//
//-----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 2] bslmf::IsEnum::VALUE
// [ 1] bsl::is_enum::value
// [ 1] bsl::is_enum_v
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

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
//                  COMPONENT SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
#define ASSERT_V_SAME(TYPE)                                                   \
    ASSERT( bsl::is_enum<TYPE>::value == bsl::is_enum_v<TYPE>)
    // 'ASSERT' that 'is_enum_v' has the same value as 'is_enum::value'.
#else
#define ASSERT_V_SAME(TYPE)
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER) &&                   \
    defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
# define TYPE_ASSERT_REF(META_FUNC, TYPE, result)                             \
    ASSERT(result == META_FUNC<                         TYPE       >::value); \
    ASSERT(false == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::value); \
    ASSERT(false == META_FUNC<bsl::add_rvalue_reference<TYPE>::type>::value); \
    ASSERT_V_SAME(                          TYPE       );                     \
    ASSERT_V_SAME(bsl::add_lvalue_reference<TYPE>::type);                     \
    ASSERT_V_SAME(bsl::add_rvalue_reference<TYPE>::type);
    // References are never 'enum' types, but must safely be parsed by the
    // metafunction under test, and give the expected ('false') result.  Also
    // test that the result value of the 'META_FUNC' on the specified 'TYPE'
    // and references to that type and the value of the 'META_FUNC_v' variable
    // instantiated with the same types are the same.
#else
# define TYPE_ASSERT_REF(META_FUNC, TYPE, result)                             \
    ASSERT(result == META_FUNC<                         TYPE       >::value); \
    ASSERT(false == META_FUNC<bsl::add_lvalue_reference<TYPE>::type>::value); \
    ASSERT_V_SAME(                          TYPE       );                     \
    ASSERT_V_SAME(bsl::add_lvalue_reference<TYPE>::type);
    // References are never 'enum' types, but must safely be parsed by the
    // metafunction under test, and give the expected ('false') result. Also
    // test that the result value of the 'META_FUNC' on the specified 'TYPE'
    // and a reference to that type and the value of the 'META_FUNC_v' variable
    // instantiated with the same types are the same.
#endif

# define TYPE_ASSERT_CVQ(META_FUNC,                   TYPE,        result)    \
         TYPE_ASSERT_REF(META_FUNC,                   TYPE,        result)    \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_const<   TYPE>::type, result)    \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_volatile<TYPE>::type, result)    \
         TYPE_ASSERT_REF(META_FUNC, bsl::add_cv<      TYPE>::type, result)
    // Test all cv-qualified combinations on a type, and references to those
    // same cv-qualified types.

# define TYPE_ASSERT_CVQP(META_FUNC,                  TYPE,        result)    \
         TYPE_ASSERT_CVQ (META_FUNC,                  TYPE,        result)    \
         TYPE_ASSERT_CVQ (META_FUNC, bsl::add_pointer<TYPE>::type, false)
    // Test whether a type as an 'enum', and confirm that pointers to such a
    // type is never an 'enum'.


#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_ISENUM_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND 1
        // The IBM xlC compiler does not handle arrays of unknown bounds as
        // template type parameters.
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This user-defined 'enum' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.
    ENUM_TEST_VALUE0 = 0,
    ENUM_TEST_VALUE1
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
enum class EnumClassType {
    // This 'enum' type is used for testing.
};
#endif

struct StructTestType {
    // This user-defined 'struct' type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.
};

union UnionTestType {
    // This user-defined 'union' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.
};

class BaseClassTestType {
    // This user-defined base class type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.
};

class DerivedClassTestType : public BaseClassTestType {
    // This user-defined derived class type is intended to be used for testing
    // as the template parameter 'TYPE' of 'bsl::is_enum'.
};

typedef int (StructTestType::*MethodPtrTestType) ();
    // This pointer to non-static member function type is intended to be used
    // for testing as the template parameter 'TYPE' of 'bsl::is_enum'.

typedef int MultiParameterFunction(char, float...);
    // This function pointer type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

typedef int StructTestType::*PMD;
    // This pointer to member object type is intended to be used for testing as
    // the template parameter 'TYPE' of 'bsl::is_enum'.

struct Incomplete;
    // This incomplete 'struct' type is intended to be used for testing as the
    // template parameter 'TYPE' of 'bsl::is_enum'.

struct ConvertToIntTestType {
    // This 'struct' type, having a conversion operator to 'int', is intended
    // to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_enum'.  This type should not be determined to be an enumerated
    // type.

    operator int() { return 0; }
};

struct ConvertToEnumTestType {
    // This 'struct' type, having a conversion operator to 'EnumTestType', is
    // intended to be used for testing as the template parameter 'TYPE' of
    // 'bsl::is_enum'.  This type should not be determined to be an enumerated
    // type.

    operator EnumTestType() { return ENUM_TEST_VALUE0; }
};

struct ConvertToAnyType {
    template <class TYPE>
    operator TYPE() { return TYPE(); }
};

}  // close unnamed namespace

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Enumerated Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are 'enum' types.
//
// First, we create an enumerated type, 'MyEnum', and a class type, 'MyClass':
//..
    enum MyEnum { MY_ENUMERATOR = 5 };
    class MyClass { explicit MyClass(MyEnum); };
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

    (void) veryVerbose;          // eliminate unused variable warning
    (void) veryVeryVerbose;      // eliminate unused variable warning
    (void) veryVeryVeryVerbose;  // eliminate unused variable warning

    setbuf(stdout, NULL);        // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

// Now, we instantiate the 'bsl::is_enum' template for both types we defined
// previously, and assert the 'value' static data member of each instantiation:
//..
    ASSERT(true  == bsl::is_enum<MyEnum>::value);
    ASSERT(false == bsl::is_enum<MyClass>::value);
//..
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above as follows:
//..
#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
    ASSERT(true  == bsl::is_enum_v<MyEnum>);
    ASSERT(false == bsl::is_enum_v<MyClass>);
#endif
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::IsEnum::VALUE'
        //   Ensure that the static data member 'VALUE' of 'bslmf::IsEnum'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   primitive type.
        //:
        //: 2 'IsEnum::VALUE' is 1 when 'TYPE' is a (possibly cv-qualified)
        //:   'enum' type, and is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   reference to an enumerated type.
        //:
        //: 3 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'class', 'struct', or 'union' type.
        //:
        //: 4 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   pointer or pointer-to-member type.
        //:
        //: 5 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   user-defined type having conversions to integral or enumerated
        //:   type, or a reference to such a user-defined type.
        //:
        //: 6 'IsEnum::VALUE' is 0 when 'TYPE' is a function or function
        //:   reference type.
        //:
        //: 7 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   array type.
        //:
        //: 8 'IsEnum::VALUE' is 0 when 'TYPE' is a (possibly cv-qualified)
        //:   'void' type.
        //
        // Plan:
        //: 1 Verify that 'bslmf::IsEnum::VALUE' has the correct value for each
        //:   (template parameter) 'TYPE' in the concerns.  (C-1..5)
        //
        // Testing:
        //   bslmf::IsEnum::VALUE
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'bslmf::IsEnum::VALUE'\n"
                            "==============================\n");

        // C-1
        TYPE_ASSERT_CVQP(bslmf::IsEnum, bool, 0);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TYPE_ASSERT_CVQP(bslmf::IsEnum, char8_t, 0);
#endif
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int,  0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, long double, 0);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TYPE_ASSERT_CVQP(bslmf::IsEnum, char16_t,  0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, char32_t,  0);
#endif

        // 'void' is fundamental
        TYPE_ASSERT_CVQP(bslmf::IsEnum, void, 0);

        // 'bsl::nullptr_t' should be fundamental
        TYPE_ASSERT_CVQP(bslmf::IsEnum, bsl::nullptr_t, 0);

        // C-2
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumTestType, 1);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumClassType, 1);
#endif

        // C-3
        TYPE_ASSERT_CVQP(bslmf::IsEnum, StructTestType,       0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, UnionTestType,        0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, Incomplete,           0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, BaseClassTestType,    0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, DerivedClassTestType, 0);

        // C-4
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int StructTestType::*,     0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, PMD BaseClassTestType::*,  0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, MethodPtrTestType,   0);

        // C-5
        TYPE_ASSERT_CVQP(bslmf::IsEnum, ConvertToIntTestType,  0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, ConvertToEnumTestType, 0);

        // can't use TYPE_ASSERT_CVQP because it adds volatile and
        // ConvertibleToAny doesn't have a volatile operator()
        ASSERT(! bslmf::IsEnum<ConvertToAnyType>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType const>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType &>::value);
        ASSERT(! bslmf::IsEnum<ConvertToAnyType const &>::value);

        // C-6
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int(int),  0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, void(...), 0);

        typedef int MultiParameterFunction(char, float...);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, MultiParameterFunction, 0);

        // C-7
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int[2], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int[4][2], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumTestType[2], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumTestType[4][2], 0);

#if !defined(BSLMF_ISENUM_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND)
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int[], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, int[][2], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumTestType[], 0);
        TYPE_ASSERT_CVQP(bslmf::IsEnum, EnumTestType[][2], 0);
#else
        ASSERT(! bslmf::IsEnum<int[2]>::value);
        ASSERT(! bslmf::IsEnum<const int[2]>::value);
        ASSERT(! bslmf::IsEnum<volatile int[2]>::value);
        ASSERT(! bslmf::IsEnum<const volatile int[2]>::value);

        ASSERT(! bslmf::IsEnum<int[4][2]>::value);
        ASSERT(! bslmf::IsEnum<const int[4][2]>::value);
        ASSERT(! bslmf::IsEnum<volatile int[4][2]>::value);
        ASSERT(! bslmf::IsEnum<const volatile int[4][2]>::value);

        ASSERT(! bslmf::IsEnum<EnumTestType[2]>::value);
        ASSERT(! bslmf::IsEnum<const EnumTestType[2]>::value);
        ASSERT(! bslmf::IsEnum<volatile EnumTestType[2]>::value);
        ASSERT(! bslmf::IsEnum<const volatile EnumTestType[2]>::value);

        ASSERT(! bslmf::IsEnum<EnumTestType[4][2]>::value);
        ASSERT(! bslmf::IsEnum<const EnumTestType[4][2]>::value);
        ASSERT(! bslmf::IsEnum<volatile EnumTestType[4][2]>::value);
        ASSERT(! bslmf::IsEnum<const volatile EnumTestType[4][2]>::value);
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bsl::is_enum::value'
        //   Ensure that the static data member 'value' of 'bsl::is_enum'
        //   instantiations having various (template parameter) 'TYPE's has the
        //   correct value.
        //
        // Concerns:
        //: 1 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) primitive type.
        //:
        //: 2 'is_enum::value' is 'true' when 'TYPE' is a (possibly
        //:   cv-qualified) 'enum' type, and is 'false' when 'TYPE' is a
        //:   (possibly cv-qualified) reference to an enumerated type.
        //:
        //: 3 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) 'class', 'struct', or 'union' type.
        //:
        //: 4 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) pointer or pointer-to-member type.
        //:
        //: 5 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) user-defined type having conversions to integral or
        //:   enumerated type, or a reference to such a user-defined type.
        //:
        //: 6 'is_enum::value' is 'false' when 'TYPE' is a function or function
        //:   reference type.
        //:
        //: 7 'is_enum::value' is 'false' when 'TYPE' is a (possibly
        //:   cv-qualified) array type.
        //:
        //: 9  That 'is_enum<T>::value' has the same value as 'is_enum_v<T>'
        //:    for a variety of template parameter types.
        //
        // Plan:
        //: 1 Verify that 'bsl::is_enum::value' has the correct value for each
        //:   (template parameter) 'TYPE' in the concerns.  (C-1..5)
        //
        // Testing:
        //   bsl::is_enum::value
        //   bsl::is_enum_v
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING 'bsl::is_enum::value'\n"
                            "=============================\n");

        // C-1
        TYPE_ASSERT_CVQP(bsl::is_enum, bool,  false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        TYPE_ASSERT_CVQP(bsl::is_enum, char8_t,  false);
#endif
        TYPE_ASSERT_CVQP(bsl::is_enum, int,  false);
        TYPE_ASSERT_CVQP(bsl::is_enum, long double, false);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        TYPE_ASSERT_CVQP(bsl::is_enum, char16_t,  false);
        TYPE_ASSERT_CVQP(bsl::is_enum, char32_t,  false);
#endif

        // 'void' is fundamental
        TYPE_ASSERT_CVQP(bsl::is_enum, void, false);

        // C-2
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumTestType, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ENUM_CLASS)
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumClassType, true);
#endif

        // C-3
        TYPE_ASSERT_CVQP(bsl::is_enum, StructTestType,       false);
        TYPE_ASSERT_CVQP(bsl::is_enum, UnionTestType,        false);
        TYPE_ASSERT_CVQP(bsl::is_enum, Incomplete,           false);
        TYPE_ASSERT_CVQP(bsl::is_enum, BaseClassTestType,    false);
        TYPE_ASSERT_CVQP(bsl::is_enum, DerivedClassTestType, false);

        // C-4
        TYPE_ASSERT_CVQP(bsl::is_enum, int StructTestType::*,    false);
        TYPE_ASSERT_CVQP(bsl::is_enum, PMD BaseClassTestType::*, false);
        TYPE_ASSERT_CVQP(bsl::is_enum, MethodPtrTestType,        false);

        // C-5
        TYPE_ASSERT_CVQP(bsl::is_enum, ConvertToIntTestType,  false);
        TYPE_ASSERT_CVQP(bsl::is_enum, ConvertToEnumTestType, false);

        // can't use TYPE_ASSERT_CVQP because it adds volatile and
        // ConvertibleToAny doesn't have a volatile operator()
        ASSERT(! bsl::is_enum<ConvertToAnyType>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType const>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType &>::value);
        ASSERT(! bsl::is_enum<ConvertToAnyType const &>::value);
        ASSERT_V_SAME(ConvertToAnyType);
        ASSERT_V_SAME(ConvertToAnyType const);
        ASSERT_V_SAME(ConvertToAnyType &);
        ASSERT_V_SAME(ConvertToAnyType const &);

        // C-6
        TYPE_ASSERT_CVQP(bsl::is_enum, int(int),  false);
        TYPE_ASSERT_CVQP(bsl::is_enum, void(...), false);
        TYPE_ASSERT_CVQP(bsl::is_enum, MultiParameterFunction, false);

        // C-7
        TYPE_ASSERT_CVQP(bsl::is_enum, int[2], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, int[4][2], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumTestType[2], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumTestType[4][2], false);

#if !defined(BSLMF_ISENUM_DO_NOT_TEST_ARRAY_OF_UNKNOWN_BOUND)
        TYPE_ASSERT_CVQP(bsl::is_enum, int[], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, int[][2], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumTestType[], false);
        TYPE_ASSERT_CVQP(bsl::is_enum, EnumTestType[][2], false);
#else
        // The IBM xlC compiler does not handle arrays of unknown bounds as
        // template type parameters.  MSVC has problems with references to
        // arrays of unknown bound that fall out of the template metaprograms
        // used to implement this trait.

        ASSERT(! bsl::is_enum<int[2]>::value);
        ASSERT(! bsl::is_enum<const int[2]>::value);
        ASSERT(! bsl::is_enum<volatile int[2]>::value);
        ASSERT(! bsl::is_enum<const volatile int[2]>::value);

        ASSERT_V_SAME(int[2]);
        ASSERT_V_SAME(const int[2]);
        ASSERT_V_SAME(volatile int[2]);
        ASSERT_V_SAME(const volatile int[2]);

        ASSERT(! bsl::is_enum<int[4][2]>::value);
        ASSERT(! bsl::is_enum<const int[4][2]>::value);
        ASSERT(! bsl::is_enum<volatile int[4][2]>::value);
        ASSERT(! bsl::is_enum<const volatile int[4][2]>::value);

        ASSERT_V_SAME(int[4][2]);
        ASSERT_V_SAME(const int[4][2]);
        ASSERT_V_SAME(volatile int[4][2]);
        ASSERT_V_SAME(const volatile int[4][2]);

        ASSERT(! bsl::is_enum<EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[2]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[2]>::value);

        ASSERT_V_SAME(EnumTestType[2]);
        ASSERT_V_SAME(const EnumTestType[2]);
        ASSERT_V_SAME(volatile EnumTestType[2]);
        ASSERT_V_SAME(const volatile EnumTestType[2]);

        ASSERT(! bsl::is_enum<EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<const EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<volatile EnumTestType[4][2]>::value);
        ASSERT(! bsl::is_enum<const volatile EnumTestType[4][2]>::value);

        ASSERT_V_SAME(EnumTestType[4][2]);
        ASSERT_V_SAME(const EnumTestType[4][2]);
        ASSERT_V_SAME(volatile EnumTestType[4][2]);
        ASSERT_V_SAME(const volatile EnumTestType[4][2]);
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
