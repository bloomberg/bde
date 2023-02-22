// bslmf_isbitwiseequalitycomparable.t.cpp                            -*-C++-*-
#include <bslmf_isbitwiseequalitycomparable.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nameof.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'
#include <string.h>  // 'memcmp'

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a metafunction,
// 'bslmf::IsBitwiseEqualityComparable', which determines whether a template
// parameter type is bitwise EqualityComparable.  By default, the metafunction
// supports a restricted set of type categories and can be extended to support
// other types through either template specialization or use of the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by checking the metafunction correctly returns 'true_type' for
// each of the supported type categories, and returns 'false_type' for all
// other type categories.  We also need to verify that the metafunction can be
// correctly extended to support user-defined types through either of the two
// supported mechanisms.  Finally, we need to test correct support for
// cv-qualified types and array types, where the underlying type may be bitwise
// EqualityComparable.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslmf::IsBitwiseEqualityComparable::value
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] EXTENDING 'bslmf::IsBitwiseEqualityComparable'

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
//                  COMPILER DEFECT MACROS TO GUIDE TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
# define BSLMF_ISBITWISEEQUALITYCOMPARABLE_ABOMINABLE_FUNCTION_MATCH_CONST 1
// The Solaris CC compiler matches 'const' qualified abominable functions as
// 'const'-qualified template parameters, but does not strip the 'const'
// qualifier when passing that template parameter onto the next instantiation.
// Therefore, 'IsBitwiseEqualityComparable<void() const>' requests infinite
// template recursion.  We opt to not try a workaround in the header for this
// platform, where we would delegate to the same implementation as the primary
// template, as that would leave an awkward difference in behavior for 'const'
// qualified class types between using a nested trait and directly specializing
// the trait.  Abominable function types are a sufficiently unlikely corner
// case in production code that the risk from simply silencing this test case
// (on just this broken platform) is negligible.
#endif

#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 0x1900)   \
 || defined(BSLMF_ISBITWISEEQUALITYCOMPARABLE_ABOMINABLE_FUNCTION_MATCH_CONST)
# define BSLMF_ISBITWISEEQUALITYCOMPARABLE_NO_ABOMINABLE_FUNCTIONS  1
// Older MSVC compilers do not parse abominable function types, so it does not
// matter whether trait would support them or not, we can simply disable such
// tests on this platform.
#endif

//=============================================================================
//                  MACROS TO CONFIGURE TESTING
//-----------------------------------------------------------------------------

//#define BSLMF_ISBITWISEEQUALITYCOMPARABLE_TEST_INCOMPLETE_TYPES_FAIL    1
// Define this macro to enable test coverage of incomplete class types, which
// should produce a compile-error.  Testing with this macro must be enabled
// outside the regular nightly regression cycle, as by its existence, it would
// cause the test driver to fail.

//=============================================================================
//                  COMPONENT-SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below will test the 'bslmf::IsBitwiseEqualityComparable'
// trait with a set of variations on a type.  There are several layers of
// macros, as object types support the full range of variation, but function
// types cannot form an array, nor be cv-qualified.  Similarly, 'void' may be
// cv-qualified but still cannot form an array.  As macros are strictly
// text-substitution we must use the appropriate 'add_decoration' traits to
// transform types in a manner that is guaranteed to be syntactically valid.
// Note that these are not type-dependent contexts, so there is no need to use
// 'typename' when fetching the result from any of the queried traits.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
#define ASSERT_IS_BITWISE_COMPARABLE_TYPE(TYPE, RESULT)                       \
    ASSERT( bslmf::IsBitwiseEqualityComparable<TYPE>::value == RESULT);       \
    ASSERT( bslmf::IsBitwiseEqualityComparable<                               \
                              bsl::add_pointer<TYPE>::type>::value);          \
    ASSERT(!bslmf::IsBitwiseEqualityComparable<                               \
                              bsl::add_lvalue_reference<TYPE>::type>::value); \
    ASSERT(!bslmf::IsBitwiseEqualityComparable<                               \
                              bsl::add_rvalue_reference<TYPE>::type>::value)
#else
#define ASSERT_IS_BITWISE_COMPARABLE_TYPE(TYPE, RESULT)                       \
    ASSERT( bslmf::IsBitwiseEqualityComparable<TYPE>::value == RESULT);       \
    ASSERT( bslmf::IsBitwiseEqualityComparable<                               \
                              bsl::add_pointer<TYPE>::type>::value);          \
    ASSERT(!bslmf::IsBitwiseEqualityComparable<                               \
                              bsl::add_lvalue_reference<TYPE>::type>::value)
#endif
    // Pointers are always bitwise EqualityComparable, and references are
    // never bitwise EqualityComparable.  The primary type under test must
    // specify whether or not it expects to be bitwise EqualityComparable.

#define ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE, RESULT)                    \
    ASSERT_IS_BITWISE_COMPARABLE_TYPE(TYPE, RESULT);                          \
    ASSERT_IS_BITWISE_COMPARABLE_TYPE(bsl::add_const<TYPE>::type, RESULT);    \
    ASSERT_IS_BITWISE_COMPARABLE_TYPE(bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_BITWISE_COMPARABLE_TYPE(bsl::add_cv<TYPE>::type, RESULT)
    // Confirm that all cv-qualified variations on a type produce the same
    // result as for the specified 'TYPE' itself.


#define ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(TYPE, RESULT)                \
    if (RESULT) { checkFootprint<TYPE>(); }                                   \
    ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE, RESULT);                       \
    ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE[128], RESULT);                  \
    ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE[12][8], RESULT);                \
    ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE[], RESULT);                     \
    ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(TYPE[][8], RESULT)
    // For an object type, confirm that specified 'TYPE' has the expected
    // result for the tested trait, and arrays of that type produce a matching
    // result.

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma : push
// BDE_VERIFY pragma : -FD01   // Function contracts are descriptive text
// BDE_VERIFY pragma : -NT01   // Used examples provide additional packages

///Example 1: Using the trait to optimize range comparison
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to compare two sequences of the same object type to
// determine whether or not they hold the same values.  The simplest solution
// would be to iterate over both sequences, comparing each member, and return
// 'false' as soon as any pair of elements do not compare equal; if we walk all
// the way to the end of both sequences, then they hold the same values.  If we
// want to perform this comparison most efficiently though, we would rather not
// invoke 'operator==' on each member, and instead defer to the 'memcmp'
// function in the standard library that is highly optimized (often to take
// advantage of platform-specific instructions) for comparing ranges of raw
// memory.  We can switch to this other technique only if we know that the
// value representations of a type are unique, rely on all of the bits in their
// representation, and do not have strange values like 'NaN' that self-compare
// as 'false'.  This property is denoted by the 'IsBitwiseEqualityComparable'
// trait.
//
// First, we create a simple 'struct' that contains a 'char' and a 'short' as
// its two data members, and supported comparison with 'operator=='.  Note that
// there will be a byte of padding between the 'char' and the 'short' members
// to ensure proper alignment.  We insert telemetry to count the number of
// times 'operator==' is called:
//..
    namespace BloombergLP {

    struct SimpleType {
        // This 'struct' holds two data members with a byte of padding, and can
        // be compared using the overloaded 'operator=='.

        char  d_dataC;
        short d_dataS;

        static int s_comparisons;

        friend bool operator==(const SimpleType& a, const SimpleType& b)
            // Return 'true' if the specified 'a' has the same value as the
            // specified 'b'.  Two 'SimpleType' objects have the same value if
            // their corresponding 'd_dataC' and 'd_dataS' members have the
            // same value.  The static data member 's_comparisons' is
            // incremented by one each time this function is called.
        {
            ++s_comparisons;
            return a.d_dataC == b.d_dataC
                && a.d_dataS == b.d_dataS;
        }

        friend bool operator!=(const SimpleType& a, const SimpleType& b)
            // Return 'true' if the specified 'a' does not have the same value
            // as the specified 'b'.  Two 'SimpleType' objects do not have the
            // same value if their corresponding 'd_dataC' and 'd_dataS'
            // members do not have the same value.  The static data member
            // 's_comparisons' is incremented by one each time this function is
            // called.
        {
            ++s_comparisons;
            return a.d_dataC != b.d_dataC
                || a.d_dataS != b.d_dataS;
        }
    };

    int SimpleType::s_comparisons = 0;
//..
// Then, we create another 'struct' that wraps a single 'int' as its only data
// member, and supports comparison with 'operator==', inserting telemetry to
// count the number of times 'operator==' is called:
//..
    struct SecondType {
        // This 'struct' holds a single 'int' member, 'd_data', and can be
        // compared using the overloaded 'operator=='.

//..
// We associate the bitwise EqualityComparable trait with 'SecondType' using
// the BDE nested trait declaration facility:
//..
        BSLMF_NESTED_TRAIT_DECLARATION(SecondType,
                                       bslmf::IsBitwiseEqualityComparable);

        int d_data;

        static int s_comparisons;

        friend bool operator==(const SecondType& a, const SecondType& b)
            // Return 'true' if the specified 'a' has the same value as the
            // specified 'b'.  Two 'SecondType' objects have the same value if
            // their corresponding 'd_data' elements have the same value.  The
            // static data member 's_comparisons' is incremented by one each
            // time this function is called.
        {
            ++s_comparisons;
            return a.d_data == b.d_data;
        }

        friend bool operator!=(const SecondType& a, const SecondType& b)
            // Return 'true' if the specified 'a' does not have the same value
            // as the specified 'b'.  Two 'SecondType' objects do not have the
            // same value if their corresponding 'd_data' elements do not have
            // the same value.  The static data member 's_comparisons' is
            // incremented by one each time this function is called.
        {
            ++s_comparisons;
            return a.d_data != b.d_data;
        }
    };

    int SecondType::s_comparisons = 0;
//..
// Next, we create another 'struct' that wraps a single 'int' as its only data
// member, and supports comparison with 'operator==', inserting telemetry to
// count the number of times 'operator==' is called:
//..
    struct ThirdType {
        // This 'struct' holds a single 'int' member, 'd_data', and can be
        // compared using the overloaded 'operator=='.

        int d_data;

        static int s_comparisons;

        friend bool operator==(const ThirdType& a, const ThirdType& b)
            // Return 'true' if the specified 'a' has the same value as the
            // specified 'b'.  Two 'SecondType' objects have the same value if
            // their corresponding 'd_data' elements have the same value.  The
            // static data member 's_comparisons' is incremented by one each
            // time this function is called.
        {
            ++s_comparisons;
            return a.d_data == b.d_data;
        }

        friend bool operator!=(const ThirdType& a, const ThirdType& b)
            // Return 'true' if the specified 'a' does not have the same value
            // as the specified 'b'.  Two 'ThirdType' objects do not have the
            // same value if their corresponding 'd_data' elements do not have
            // the same value.  The static data member 's_comparisons' is
            // incremented by one each time this function is called.
        {
            ++s_comparisons;
            return a.d_data != b.d_data;
        }
    };

    int ThirdType::s_comparisons = 0;
//..
// We associate the bitwise EqualityComparable trait with 'ThirdType' by
// explicitly specializing the trait:
//..
    namespace bslmf {
        template <>
        struct IsBitwiseEqualityComparable<ThirdType> : bsl::true_type {};
    }  // close namespace bslmf
//..
// Now, we write a function template to compare two arrays of the same type:
//..
    template <class TYPE>
    bool rangeCompare(const TYPE *start, size_t length, const TYPE *other)
    {
//..
// If we detect the bitwise EqualityComparable trait, we rely on the optimized
// 'memcmp' function:
//..
        if (bslmf::IsBitwiseEqualityComparable<TYPE>::value) {
            return 0 == memcmp(start,
                               other,
                               length * sizeof(TYPE));                // RETURN
        }
//..
// Otherwise we iterate over the range directly until we find a pair of
// elements that do not have the same value, and return 'true' if we reach the
// end of the range.
//..
        if (0 != length)  {
            while (*start++ == *other++) {
                if (!--length) {
                    return true;                                      // RETURN
                }
            }
        }
        return false;
    }
//..
// Finally, we write a test to confirm that two arrays containing different
// values do not compare as equal (using our array comparison function), and
// that an array compares equal to itself, as it does comprise elements all
// having the same value.  By inspecting the static data members provided for
// telemetry, we can confirm that 'operator==' is called only for 'SimpleType'
// as the other two 'struct's dispatch to 'memcmp' instead:
//..
    int usageExample1()
    {
//..
// We confirm the initial state of the telemetry:
//..
        ASSERT(0 == SimpleType::s_comparisons);
        ASSERT(0 == SecondType::s_comparisons);
        ASSERT(0 == ThirdType ::s_comparisons);
//..
// Then we create zero-initialized arrays for each of the types to be tested,
// and a second array for each type with a set of values distinct from all
// zeroes:
//..
        const SimpleType simpleZeroes[10] = { };
        const SecondType secondZeroes[10] = { };
        const ThirdType  thirdZeroes [10] = { };

        const SimpleType simpleValues[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        const SecondType secondValues[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        const ThirdType  thirdValues [10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
//..
// Next we confirm that the two arrays (of each type) do not compare equal, and
// inspect the telemetry to confirm that the comparison operator was called for
// only the 'SimpleType' without the bitwise EqualityComparable trait:
//..
        ASSERT(!rangeCompare(simpleZeroes, 10u, simpleValues) );
        ASSERT(!rangeCompare(secondZeroes, 10u, secondValues) );
        ASSERT(!rangeCompare(thirdZeroes,  10u, thirdValues)  );

        ASSERT(0 <  SimpleType::s_comparisons);
        ASSERT(0 == SecondType::s_comparisons);
        ASSERT(0 == ThirdType ::s_comparisons);

//..
// Then we reset the telemetry and confirm that an array of each type compares
// equal to itself, and inspect the telemetry to confirm that the comparison
// operator was called for only the 'SimpleType' without the bitwise
// EqualityComparable trait:
//..
        SimpleType::s_comparisons = 0;

        ASSERT( rangeCompare(simpleValues, 10u, simpleValues) );
        ASSERT( rangeCompare(secondValues, 10u, secondValues) );
        ASSERT( rangeCompare(thirdZeroes,  10u, thirdZeroes)  );

        ASSERT(0 <  SimpleType::s_comparisons);
        ASSERT(0 == SecondType::s_comparisons);
        ASSERT(0 == ThirdType ::s_comparisons);

        return 0;
    }

    }  // close enterprise namespace
//..
//
///Example 2: Associating a Trait with a Class Template
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we associate a trait not with a class, but with a class
// *template*.  We create a "control" template that is not bitwise
// EqualityComparable, and two class templates, each of which uses a different
// mechanisms for being associated with the 'IsBitwiseEqualityComparable'
// trait.  First, we define a class template that is not bitwise
// EqualityComparable, 'NotComparable':
//..
    namespace BloombergLP {

    template <class TYPE>
    struct NotComparable
    {
        TYPE d_value;
    };
//..
// Then, we define the class template 'PotentiallyComparable1', which uses
// partial template specialization to associate the
// 'IsBitwiseEqualityComparable' trait with each instantiation on a 'TYPE' that
// is itself bitwise EqualityComparable:
//..
    template <class TYPE>
    struct PotentiallyComparable1
    {
        TYPE d_value;
    };

    namespace bslmf {
        template <class TYPE>
        struct IsBitwiseEqualityComparable<PotentiallyComparable1<TYPE> >
            : IsBitwiseEqualityComparable<TYPE>::type {
        };
    }  // close namespace bslmf
//..
// Next, we define the class template'PotentiallyComparable2', which uses the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the
// 'IsBitwiseEqualityComparable' trait with each instantiation on a 'TYPE' that
// is itself bitwise EqualityComparable:
//..
    template <class TYPE>
    struct PotentiallyComparable2
    {
        TYPE d_value;

        BSLMF_NESTED_TRAIT_DECLARATION_IF(
                              PotentiallyComparable2,
                              bslmf::IsBitwiseEqualityComparable,
                              bslmf::IsBitwiseEqualityComparable<TYPE>::value);
    };
//..
// Finally, we check that the traits are correctly associated by instantiating
// each template with types that are bitwise EqualityComparable and with types
// that are not not bitwise EqualityComparable, verifying the value of
// 'IsBitwiseEqualityComparable<T>::value' in each case:
//..
    int usageExample2()
    {
        using namespace bslmf;

        ASSERT(!IsBitwiseEqualityComparable<NotComparable<int> >::value);
        ASSERT(!IsBitwiseEqualityComparable<
                              NotComparable<NotComparable<int> > >::value);

        ASSERT( IsBitwiseEqualityComparable<
                                   PotentiallyComparable1<int> >::value);
        ASSERT(!IsBitwiseEqualityComparable<
                     PotentiallyComparable1<NotComparable<int> > >::value);

        ASSERT( IsBitwiseEqualityComparable<
                                   PotentiallyComparable2<int> >::value);
        ASSERT(!IsBitwiseEqualityComparable<
                     PotentiallyComparable2<NotComparable<int> > >::value);

        return 0;
    }

    }  // close enterprise namespace
//..

// BDE_VERIFY pragma : pop

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

enum EnumTestType {
    // This 'enum' type is used for testing.
};

struct Incomplete;
    // This type supports testing the 'IsBitwiseEqualityComparable' trait for
    // incomplete class types; it is deliberately declared, but never defined.

union Uncomplete;
    // This type supports testing the 'IsBitwiseEqualityComparable' trait for
    // incomplete union types; it is deliberately declared, but never defined.

struct UserDefinedBwEqStruct {
    // This user-defined type, which is marked to be bitwise
    // EqualityComparable using template specialization (below), is used for
    // testing.

    char d_c;   // class must have a bitwise EqualityComparable data member
};

struct UserDefinedBwEqStruct2 {
    // This user-defined type, which is marked to be bitwise
    // EqualityComparable using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is
    // used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(
                              UserDefinedBwEqStruct2,
                              BloombergLP::bslmf::IsBitwiseEqualityComparable);

    int d_i;    // class must have a bitwise EqualityComparable data member
};

union UserDefinedBwEqUnion {
    // This user-defined type, which is marked to be bitwise
    // EqualityComparable using template specialization (below), is used for
    // testing.

    char d_c;   // class must have a bitwise EqualityComparable data member
};

union UserDefinedBwEqUnion2 {
    // This user-defined type, which is marked to be bitwise
    // EqualityComparable using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is
    // used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(
                              UserDefinedBwEqUnion2,
                              BloombergLP::bslmf::IsBitwiseEqualityComparable);

    int d_i;    // class must have a bitwise EqualityComparable data member
};

struct UserDefinedNonEqStruct {
    // This user-defined type, which is not marked as bitwise
    // EqualityComparable, is used for testing.  Note that this type could be
    // safely marked as bitwise EqualityComparable, but should not be
    // implicitly detected as such without user guidance.

    char d_dummy1;
    int  d_dummy2;
};

union UserDefinedNonEqUnion {
    // This user-defined type, which is not marked as bitwise
    // EqualityComparable, is used for testing.  Note that this type could be
    // safely marked as bitwise EqualityComparable, but should not be
    // implicitly detected as such without user guidance.

    char d_dummy1;
    int  d_dummy2;
};

typedef int UserDefinedNonEqStruct::* MemberDataTestType;
    // This pointer to non-static data member type is used for testing.

typedef int (UserDefinedNonEqStruct::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

}  // close unnamed namespace


// Provide explicit traits specializations, ensuring that both means of
// associating a trait with a type are tested.

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseEqualityComparable<UserDefinedBwEqStruct> : bsl::true_type {};

template <>
struct IsBitwiseEqualityComparable<UserDefinedBwEqUnion> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

//=============================================================================
//                  GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
void checkFootprint()
    // Perform a runtime check that (template parameter) 'TYPE' does not have
    // any padding bytes.  This implementation requires that 'TYPE' is trivial,
    // value-initializable, and that the value-initialized representation is
    // all zero bytes.  Note that this check does not guarantee to find padding
    // bytes as compilers are free to give them values consistent with this
    // test; the best we can do is offer a chance to find an unexpected fail.
{
    // First create a union with an array of 'unsigned char', the C++ type for
    // raw memory, and the supplied 'TYPE'.  Note that this would require C++11
    // for non-trival 'TYPE's.

    union TestWrapper {
        unsigned char buffer[sizeof(TYPE)];
        TYPE          value;
    };

    // Then initialize an object of the union type, 'x', with the array of raw
    // memory as the active element, and fill that memory with a non-zero bit
    // pattern.

    TestWrapper x = {};
    for (int i = 0; i != sizeof(TYPE); ++i) {
        x.buffer[i] = 0xff;
    }

    // Next, assign to the 'TYPE' member of the union with a default
    // constructed object.

    x.value = TYPE();

    // Then repeat the process for a second object, 'y', using a different bit
    // pattern to mark the raw array.

    TestWrapper y = {};
    for (int i = 0; i != sizeof(TYPE); ++i) {
        y.buffer[i] = 0xa0;
    }
    y.value = TYPE();

    // Finally, compare the bytes of the whole union object (rather than
    // inspecting through its members) to verify that all bytes have been
    // overwritten.  Note that we use a second object for comparison, rather
    // than simply checking the for the byte values changing, to avoid false
    // negatives when those byte-patterns happen to match part of the state of
    // a default constructed object, for example with null pointer values for
    // pointer-to-member types on xlC.

    const unsigned char *cursorX = reinterpret_cast<unsigned char *>(&x);
    const unsigned char *cursorY = reinterpret_cast<unsigned char *>(&y);
    for (int i = 0; i != sizeof(TYPE); ++i) {
        ASSERTV(bsls::NameOf<TYPE>(), i, cursorX[i],   cursorY[i],
                                         cursorX[i] == cursorY[i]);
    }
}
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

    setbuf(stdout, NULL);       // Use unbuffered output

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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample1();
        usageExample2();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // EXTENDING 'bslmf::IsBitwiseEqualityComparable'
        //   Ensure the 'bslmf::IsBitwiseEqualityComparable' metafunction
        //   returns the correct value for types explicitly specialized to
        //   support this trait.  Note that the only user-defined types that
        //   can be customized are class types and union types.
        //
        // Concerns:
        //: 1 The metafunction returns 'false' for plain user-defined types,
        //:   which may be classes or unions.
        //:
        //: 2 The metafunction returns 'true' for a user-defined type, if a
        //:   specialization for 'bslmf::IsBitwiseEqualityComparable' on that
        //:   type is defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The metafunction returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 The metafunction returns the same result for a cv-qualified type
        //:   as for the corresponding cv-unqualified type.
        //:
        //: 5 The metafunction returns the same result for an array type as for
        //:   the array's element type.
        //:
        //: 6 The metafunction gives a hard error when instantiated with an
        //:   incomplete class type, rather than risk differing results when
        //:   the type is completed.
        //
        // Plan:
        //:  1 Create a macro that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C-1..5)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //: 2 For user defined types with no customization, use the test macro
        //:   to confirm the metafunction returns 'false' in each case.
        //:
        //: 3 For user defined types utilizing each customization point in
        //:   turn, use the test macro to confirm the metafunction returns
        //:   'true' in each case.
        //:
        //:  4 Guarded by a configuration macro, which defaults to unchecked,
        //:    provide additional tests to ensure hard errors are diagnosed
        //:    for incomplete object types. (C-6)
        //
        // Testing:
        //   EXTENDING 'bslmf::IsBitwiseEqualityComparable'
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nEXTENDING 'bslmf::IsBitwiseEqualityComparable'"
                         "\n==============================================\n");

        // C-1
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedNonEqStruct,
                                                 false);

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedNonEqUnion,
                                                 false);

        // C-2
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedBwEqStruct,
                                                 true);

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedBwEqUnion,
                                                 true);

        // C-3
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedBwEqStruct2,
                                                 true);

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(UserDefinedBwEqUnion2,
                                                 true);

        // C-6 Verify there are no surprises for incomplete types.

#if defined(BSLMF_ISBITWISEEQUALITYCOMPARABLE_TEST_INCOMPLETE_TYPES_FAIL)
        // It is impossible to test for a nested trait in an incomplate type,
        // so the trait should fail to instantiate.
        ASSERT(!bslmf::IsBitwiseEqualityComparable<Incomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<const Incomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<
                                                  volatile Incomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<
                                            const volatile Incomplete>::value);

        ASSERT(!bslmf::IsBitwiseEqualityComparable<Uncomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<const Uncomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<
                                                  volatile Uncomplete>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<
                                            const volatile Uncomplete>::value);
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'bslmf::IsBitwiseEqualityComparable::value'
        //   Ensure the 'bslmf::IsBitwiseEqualityComparable' metafunction
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The metafunction returns 'true' for fundamental object types.
        //:
        //:  2 The metafunction returns 'false' for cv-qualified 'void' types.
        //:
        //:  3 The metafunction returns 'true' for enumerated types.
        //:
        //:  4 The metafunction returns 'true' for pointer to member types.
        //:
        //:  5 The metafunction returns 'false' for function types.
        //:
        //:  6 The metafunction returns 'true' for pointer types.
        //:
        //:  7 The metafunction returns 'false' for reference types.
        //:
        //:  8 The metafunction returns the same result for array types as it
        //:    would for the array's element type.
        //:
        //:  9 The metafunction returns the same result for a cv-qualified type
        //:    as for the corresponding cv-unqualified type.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all (legal) variants of a type:  (C-7..9; partial for 6)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and unknown bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-5, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types. (C-1..5)
        //:
        //:  3 To complete testing of pointers not covered by earlier tests,
        //:    provide additional tests for pointers to function types, and for
        //:    pointers to array types. (C-6)
        //
        // Testing:
        //   bslmf::IsBitwiseEqualityComparable::value
        // --------------------------------------------------------------------

        if (verbose) printf(
                    "\nTESTING 'bslmf::IsBitwiseEqualityComparable::value'"
                    "\n===================================================\n");

        // C-1 : Test all fundamental types to be sure there are no accidental
        // gaps in coverage.

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(char, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(signed char, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(unsigned char, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UTF8_CHAR_TYPE)
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(char8_t, true);
#endif
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(wchar_t, true);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES)
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(char16_t, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(char32_t, true);
#endif

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(short,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(unsigned short,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(int,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(unsigned int,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(long,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(unsigned long ,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(long long,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(unsigned long long,  true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(long, true);

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(bool, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(bsl::nullptr_t, true);

        // floating-point types are not bitwise EqualityComparable.

        // Revert of {DRQS 143286899}. Once clients are fixed, change to false.
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(float, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(double, true);
        // This test relates to the above DRQS - long double is 10 bytes long
        // and to not pass "no padding" test. Uncomment with other floating
        // types.
        //ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(long double, true);

        // C-2 : 'void' is not an object type, but can be cv-qualified.

        ASSERT_IS_BITWISE_COMPARABLE_CV_TYPE(void, false);

        // C-3
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(EnumTestType, true);

        // C-4
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(MemberDataTestType, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(MethodPtrTestType, true);

        // C-5 : Function types are neither object types nor cv-qualifiable, so
        // must use the simplest test macro.  Abominable function types support
        // neither references nor pointers, so must be tested directly.

        ASSERT_IS_BITWISE_COMPARABLE_TYPE(void(), false);
        ASSERT_IS_BITWISE_COMPARABLE_TYPE(bool(float, double...), false);
#if !defined(BSLMF_ISBITWISEEQUALITYCOMPARABLE_NO_ABOMINABLE_FUNCTIONS)
        ASSERT(!bslmf::IsBitwiseEqualityComparable<void() volatile>::value);
        ASSERT(!bslmf::IsBitwiseEqualityComparable<void(...) const>::value);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_TYPES)
        ASSERT_IS_BITWISE_COMPARABLE_TYPE(void() noexcept, false);
        ASSERT_IS_BITWISE_COMPARABLE_TYPE(
                                       bool(float, double...) noexcept, false);
        ASSERT(
            !bslmf::IsBitwiseEqualityComparable<void() const noexcept>::value);
#endif

        // C-6 : Pointer types are mostly tested as object types in the macros
        // above, but we should add a few extra cases to recursively validate
        // them as object types in their own right.

        // Typedefs are needed to avoid strange parsings in the test macros.
        typedef int Array1D[42];
        typedef int Array2D[42][13];
        typedef int ArrayUB[];
        typedef int ArrayU2[][13];
        typedef void VoidFn();
        typedef bool MoreFn(float, double...);

        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(int *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(void *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(Incomplete *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(Array1D *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(Array2D *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(ArrayUB *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(ArrayU2 *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(VoidFn *, true);
        ASSERT_IS_BITWISE_COMPARABLE_OBJECT_TYPE(MoreFn *, true);
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
