// bslmf_voidtype.t.cpp                                               -*-C++-*-
#include <bslmf_voidtype.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>   // `printf`, `puts`
#include <stdlib.h>  // `atoi`

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <type_traits> // `std::void_t`, `std::true_type`, `std::false_type`
#endif

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// The metafunction defined in this component does no actual calculation; it
// simply provides a `void` type member.  Testing it is trivial: simply
// instantiate it with 0 to 14 parameters and verify that it compiles.  The
// usefulness of this component is demonstrated in the usage examples.  Note
// that as this component is a primitive building block for metaprogramming, we
// wish to ensure there are no dependencies on any other `bslmf` component for
// testing purposes, so that a dependency on `bslmf_voidtype` has no further
// implication on levelization.  For this reason, we choose a testing strategy
// that does not depend on `bslmf_issame`.
//-----------------------------------------------------------------------------
// [2] VoidType::type
// [3] bsl::void_t
// [4] BSLMF_VOIDTYPE
// [5] BSLMF_VOIDTYPE2
// [6] BSLMF_VOIDTYPES
//-----------------------------------------------------------------------------
// [7] USAGE EXAMPLE
// [1] CONCERN: Test machinery works as expected

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

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
//                        DEFECT DETECTION MACROS
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)
// The Sun CC compiler forms both pointers and references to "abominable"
// function types, such as `void() const`, which the language forbids, so a
// SFINAE test that requires forming either one does not reject such a type.
# define BUG_FORMS_POINTERS_AND_REFERENCES_TO_ABOMINABLE_FUNCTIONS 1
#endif

// KNOWN DEFECT: Clang compilers prior to version 3.7 do not implement the
// resolution of CWG 1558, so will fail the SFINAE tests in test in cases 4, 5,
// 6, and 7.  We should update the minimum supported Clang version rather than
// cater to a long out-of-support early C++ compiler.  The trait fundamentally
// works even here, struggling only when too many template type arguments are
// supplied.

//=============================================================================
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)
// Test case 5 forms a reference to a reference type, which is ill-formed in
// C++03.  Several compilers collapse such a reference as an extension, but
// only Sun CC warns.  The tests give the same result either way, as forming a
// pointer to a reference type always fails.
# pragma error_messages(off, reftoref)
#endif

//=============================================================================
//                             USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma: push      // usage examples relax rules for doc clarity
// BDE_VERIFY pragma: -FD01     // function contracts are usage example

///Usage Example 1
///- - - - - - - -
// In this example, we demonstrate the use of `BSLMF_VOIDTYPE` to determine
// whether a given type `T` has a member type `T::iterator`.  Our goal is to
// create a metafunction, `HasIteratorType`, such that
// `HasIteratorType<T>::k_VALUE` is `true` if `T::iterator` is a valid type and
// `false` otherwise.  This example is adapted from the paper proposing
// `std::void_t` for the C++ Standard, N3911.
//
// First, we define the base-case metafunction that returns `false`:
// ```
    template <class TYPE, class = void>
    struct HasIteratorType {
        enum { k_VALUE = false };
    };
// ```
// Then, we create a partial specialization that uses `BSLMF_VOIDTYPE` to probe
// for `T::iterator`:
// ```
    template <class TYPE>
    struct HasIteratorType<TYPE, BSLMF_VOIDTYPE(typename TYPE::iterator)> {
        enum { k_VALUE = true };
    };
// ```
// Now, we define a class that has an `iterator` member and apply
// `HasIteratorType` to it:
// ```
    struct WithIterator {
        typedef short *iterator;
    };

    void usageExample1()
    {
        ASSERT(true == HasIteratorType<WithIterator>::k_VALUE);
// ```
// As `WithIterator::iterator` is a valid type,
// `BSLMF_VOIDTYPE(typename TYPE::iterator)` will be `void` and the second
// `HasIteratorType` template will be more specialized than the primary
// template, thus yielding a `k_VALUE` of `true`.
//
// Finally, we try to instantiate `HasIteratorType<int>`.  Any use of
// `BSLMF_VOIDTYPE(typename TYPE::iterator)` will result in a substitution
// failure.  Fortunately, the Substitution Failure Is Not An Error (SFINAE)
// rule applies and the partial specialization is eliminated from
// consideration, resulting in the primary template being instantiated and
// yielding a `k_VALUE` of `false`:
// ```
        ASSERT(false == HasIteratorType<int>::k_VALUE);
    }
// ```
//
///Usage Example 2
///- - - - - - - -
// This example demonstrates the use of `BSLMF_VOIDTYPE2` to probe for more
// than one type at once.  As in the previous example, we are defining a
// metafunction.  We'll define `IsTraversable<T>::k_VALUE` to be `true` if
// `T::iterator` and `T::value_type` both exist.  First, we define a primary
// template that always yields `false`:
// ```
    template <class TYPE, class = void>
    struct IsTraversable {
        enum { k_VALUE = false };
    };
// ```
// Then, we create a partial specialization that uses `BSLMF_VOIDTYPE2` with
// two arguments:
// ```
    template <class TYPE>
    struct IsTraversable<TYPE,
                         BSLMF_VOIDTYPE2(typename TYPE::iterator,
                                         typename TYPE::value_type)> {
        enum { k_VALUE = true };
    };
// ```
// Now, we define a type that meets the requirements for being traversable:
// ```
    struct MyTraversable {
        typedef int  value_type;
        typedef int *iterator;
    };
// ```
// Finally, the `IsTraversable` metafunction yields `true` for `MyTraversable`
// but neither for `WithIterator`, which lacks a `value_type` member, nor for
// `int`, which lacks both `iterator` and `value_type` members:
// ```
    void usageExample2()
    {
        ASSERT(true  == IsTraversable<MyTraversable>::k_VALUE);
        ASSERT(false == IsTraversable<WithIterator>::k_VALUE);
        ASSERT(false == IsTraversable<int>::k_VALUE);
    }
// ```

// BDE_VERIFY pragma: pop  // Restore bde_verify rules

//=============================================================================
//                  GLOBAL CLASS TEMPLATES FOR TESTING
//-----------------------------------------------------------------------------

                        // ===================
                        // struct TestEnableIf
                        // ===================

/// Replicate the definition of `bsl::enable_if` to avoid a dependency
/// on `bslmf_enableif` as doing so would introduce a cyclic dependency into
/// this package.
template <bool CONDITION, class TYPE = void>
struct TestEnableIf
{

    typedef TYPE type;
};

template <class TYPE>
struct TestEnableIf<false, TYPE>
{
};

                        // ================================
                        // struct IsMediumClass<class TYPE>
                        // ================================

/// A "medium" class is a type that is a class type, that has a size larger
/// than a single byte, but is no larger than an `int`.
template <class TYPE, class = void>
struct IsMediumClass {

    enum { k_VALUE = false };
};

template <class TYPE>
struct IsMediumClass<
TYPE,
BSLMF_VOIDTYPES(
    int TYPE::*,                                                 // is  "class"
    typename TestEnableIf<(sizeof(TYPE) >  sizeof(char))>::type, // not "small"
    typename TestEnableIf<(sizeof(TYPE) <= sizeof(int ))>::type  // not "large"
)> {
    enum { k_VALUE = true };
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

                        // ======================================
                        // struct IsMediumClassOracle<class TYPE>
                        // ======================================

/// A "medium" class is a type that is a class type, that has a size larger
/// than a single byte, but is no larger than an `int`.  This Oracle class
/// provides an implementation using the standard type traits when they are
/// available, to confirm that the component test above is giving a standard
/// conforming result.
template <class TYPE, class = void>
struct IsMediumClassOracle : std::false_type {
};

template <class TYPE>
struct IsMediumClassOracle<
TYPE,
std::void_t<
int TYPE::*,                                              // is  "class"
std::enable_if_t<(sizeof(TYPE) >  sizeof(char))>,         // not "small"
std::enable_if_t<(sizeof(TYPE) <= sizeof(int ))>          // not "large"
>> : std::true_type {
};
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// Sample Classes

class MyEmptyClass {
};

struct MyEmptyStruct {
};

struct MyMediumStruct {
    char d_data[3];
};

class MySmallClass {
  public:
    char d_data;
};

class MyMediumClass {
  public:
    int d_data;
};

class MyBigClass {
  public:
    int d_data1;
    int d_data2;
};

enum MyEnumNotClass {
    e_Value = 10
};

union MySmallUnion {
    char d_char;
};

union MyMediumUnion {
    char  d_char;
    short d_short;
};

union MyBigUnion {
    char  d_char;
    long  d_int[2];
};

// ----------------------------------------------------------------------------

/// This trait provides a nested enumerator `k_VALUE` that is `true` if the
/// (template parameter) `TYPE` is a (publicly copyable) non-abstract class
/// type, and is `false` otherwise.  For testing purposes, this trait uses
/// the `BSLMF_VOIDTYPES` macro with more than one argument.
template <class TYPE, class = void>
struct IsNonAbstractClass {

    enum { k_VALUE = false };
};

template <class TYPE>
struct IsNonAbstractClass<TYPE, BSLMF_VOIDTYPES(int TYPE::*, TYPE(), TYPE[])> {
    enum { k_VALUE = true };
};

/// This trait provides a nested enumerator `k_VALUE` that is `true` if the
/// (template parameter) `TYPE` is a class type, including if it is a union
/// type, and is `false` otherwise.  For testing purposes, this trait uses
/// the `BSLMF_VOIDTYPE` macro with exactly one argument.
template <class TYPE, class = void>
struct IsClassType {

    enum { k_VALUE = false };
};

template <class TYPE>
struct IsClassType<TYPE, BSLMF_VOIDTYPE(int TYPE::*)> {
    enum { k_VALUE = true };
};

/// This trait provides a nested enumerator `k_VALUE` that is `true` if the
/// (template parameter) `TYPE` is deemed reasonable, and is `false`
/// otherwise.  For this trait, a type is reasonable if we can form both a
/// pointer and a reference to it.  For example, `void` is not reasonable as
/// although we can form a `void *`, there are no references to `void`.
/// Similarly, reference types are not reasonable as we cannot form pointers
/// to references.  Regular function types are reasonable, while abominable
/// function types are not.  Most other types are reasonable.  For testing
/// purposes, this trait uses the `BSLMF_VOIDTYPE2` macro with exactly two
/// arguments.
template <class TYPE, class = void>
struct IsReasonable {

    enum { k_VALUE = false };
};

/// This partial specialization implements the test documented by the
/// primary template.
template <class TYPE>
struct IsReasonable<TYPE, BSLMF_VOIDTYPE2(TYPE&, TYPE *)> {

    enum { k_VALUE = true };
};

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

/// This `struct` is an abstract class type used for testing.
struct Abstract {
    /// Destroy this object.  Note that this destructor is pure virtual only
    /// to make this class abstract.
    virtual ~Abstract() = 0;
};

/// This `struct` is implicitly convertible to `void *`, and is used to verify
/// that `isVoidPointer` does not consider such a conversion.
struct ConvertibleToVoidStar {
    /// Return a `void *` null pointer.
    operator void*() const { return 0; }
};

class Incomplete;

enum MyEnum { E0, E1 };

struct MyStruct {
    int d_x;
};

union MyUnion {
    int d_x;
};

//=============================================================================
//                  GLOBAL FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

/// Return `false`.  Note that the (non-template) function below will be
/// selected in overload resolution if (the template parameter) `TYPE` is a
/// pointer to cv-unqualified `void`.
template <class TYPE>
inline bool isVoidPointer(const TYPE&)
{
    return false;
}

/// Return `true`.
inline bool isVoidPointer(void *)
{
    return true;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;          // suppress warning
    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) puts("\nUSAGE EXAMPLE"
                          "\n=============");

        usageExample1();
        usageExample2();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `BSLMF_VOIDTYPES`
        //   Due to the C++98 requirements that `typename` be used only where
        //   the compiler absolutely requires it (rather than allowing for
        //   redundant use) the `BSLMF_VOIDTYPES` macro is to be used only in
        //   type-dependent contexts.  That means testing must be in terms of
        //   instantiating templates declared outside `main`.  Note that the
        //   macro arguments supplied to `BSLMF_VOIDTYPES` cannot contain any
        //   mix of non-type expressions:
        //  - literal values
        //  - string literals
        //  - enumerators
        //  - uninstantiated templates
        //
        // Concerns:
        // 1. `BSLMF_VOIDTYPES(T1, T2... TN)` is an alias for `void`, for any
        //    number of *valid* arguments, at least one of which shall depend
        //    on the type parameters of the template being instantiated.
        //
        // 2. `BSLMF_VOIDTYPES(T1, T2... TN)` fails to instantiate in a SFINAE
        //    compatible manner, for all dialects of C++, when any of the types
        //    `T1, T2, ... TN` forms an invalid (dependent) type expression.
        //
        // Plan:
        // 1. Create a metafunction, `IsMediumClass`, using the macro
        //    `BSLMF_VOIDTYPES` to filter types on some set of properties.
        //    At least three properties are tested to distinguish this test
        //    from that of `BSLMF_VOIDTYPE2` (test case 5).
        //
        // 2. Instantiate that metafunction for a wide variety of types that
        //    cover the potential syntax space.
        //
        // Testing:
        //   BSLMF_VOIDTYPES
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `BSLMF_VOIDTYPES`"
                          "\n=========================");

        typedef Incomplete T;   // abbreviation for table below

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

    #define IS_MEDIUM_CLASS(TYPE, EXPECTED)                   \
            ASSERT(IsMediumClass<TYPE>::k_VALUE == EXPECTED); \
            ASSERT(IsMediumClass<TYPE>::k_VALUE ==            \
                   IsMediumClassOracle<TYPE>::value);
#else // Oracle unavailable

    #define IS_MEDIUM_CLASS(TYPE, EXPECTED)                   \
            ASSERT(IsMediumClass<TYPE>::k_VALUE == EXPECTED);

#endif

        IS_MEDIUM_CLASS(MyEmptyClass,   false);
        IS_MEDIUM_CLASS(MyEmptyStruct,  false);
        IS_MEDIUM_CLASS(MySmallClass,   false);
        IS_MEDIUM_CLASS(MyBigClass,     false);
        IS_MEDIUM_CLASS(MyEnumNotClass, false);
        IS_MEDIUM_CLASS(MySmallUnion,   false);
        IS_MEDIUM_CLASS(MyBigUnion,     false);

        IS_MEDIUM_CLASS(      MyMediumClass,  true );
        IS_MEDIUM_CLASS(const MyMediumClass,  true );
        IS_MEDIUM_CLASS(      MyMediumStruct, true );
        IS_MEDIUM_CLASS(const MyMediumStruct, true );
        IS_MEDIUM_CLASS(      MyMediumUnion,  true );
        IS_MEDIUM_CLASS(const MyMediumUnion,  true );

        IS_MEDIUM_CLASS(      void    , false);
        IS_MEDIUM_CLASS(const void    , false);
        IS_MEDIUM_CLASS(const void *  , false);
        IS_MEDIUM_CLASS(      int     , false);
        IS_MEDIUM_CLASS(const int     , false);
        IS_MEDIUM_CLASS(const int *   , false);
        IS_MEDIUM_CLASS(const int&    , false);

        IS_MEDIUM_CLASS(Incomplete    , false);
        IS_MEDIUM_CLASS(      MyEnum  , false);
        IS_MEDIUM_CLASS(      MyStruct, true );
        IS_MEDIUM_CLASS(const MyStruct, true );
        IS_MEDIUM_CLASS(      MyUnion , true );

        IS_MEDIUM_CLASS(int T::*        , false);
        IS_MEDIUM_CLASS(int(T::*)()const, false);

        IS_MEDIUM_CLASS(int    [ ]      , false);
        IS_MEDIUM_CLASS(int (*)[ ]      , false);
        IS_MEDIUM_CLASS(int (&)[ ]      , false);
        IS_MEDIUM_CLASS(int    [2]      , false);
        IS_MEDIUM_CLASS(int (*)[3]      , false);
        IS_MEDIUM_CLASS(int (&)[4]      , false);

        IS_MEDIUM_CLASS(void   ()       , false);
        IS_MEDIUM_CLASS(void(*)()       , false);
        IS_MEDIUM_CLASS(void(&)()       , false);
        IS_MEDIUM_CLASS(void   ()const  , false);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE)
        IS_MEDIUM_CLASS(void   ()        noexcept, false);
        IS_MEDIUM_CLASS(void(*)()        noexcept, false);
        IS_MEDIUM_CLASS(void(&)()        noexcept, false);
        IS_MEDIUM_CLASS(void   ()  const noexcept, false);
        IS_MEDIUM_CLASS(int(T::*)()const noexcept, false);
#endif

#undef IS_MEDIUM_CLASS
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `BSLMF_VOIDTYPE2`
        //   Due to the C++98 requirements that `typename` be used only where
        //   the compiler absolutely requires it (rather than allowing for
        //   redundant use) the `BSLMF_VOIDTYPE2` macro is to be used only in
        //   type-dependent contexts.  That means testing must be in terms of
        //   instantiating templates declared outside `main`.  Note that the
        //   macro arguments supplied to `BSLMF_VOIDTYPE2` cannot contain any
        //   mix of non-type expressions:
        //  - literal values
        //  - string literals
        //  - enumerators
        //  - uninstantiated templates
        //
        // Concerns:
        // 1. `BSLMF_VOIDTYPE2(T1, T2)` is an alias for `void`, for exactly two
        //    *valid* arguments, at least one of which shall depend on the type
        //    parameters of the template being instantiated.
        //
        // 2. `BSLMF_VOIDTYPE2(T1, T2)` fails to instantiate in a SFINAE
        //    compatible manner, for all dialects of C++, when either of the
        //    macro arguments, T1 or T2, forms an invalid (dependent) type
        //    expression.
        //
        // Plan:
        // 1. Create a metafunction, `IsReasonable`, using the macro
        //    `BSLMF_VOIDTYPE2` to filter types on some property.
        //
        // 2. Instantiate that metafunction for a wide variety of types that
        //    cover the potential syntax space.
        //
        // Testing:
        //   BSLMF_VOIDTYPE2
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `BSLMF_VOIDTYPE2`"
                          "\n=========================");

        typedef Incomplete T;   // abbreviation for table below

        ASSERT(!IsReasonable<      void    >::k_VALUE);
        ASSERT(!IsReasonable<const void    >::k_VALUE);
        ASSERT( IsReasonable<const void *  >::k_VALUE);
        ASSERT( IsReasonable<      int     >::k_VALUE);
        ASSERT( IsReasonable<const int     >::k_VALUE);
        ASSERT( IsReasonable<const int *   >::k_VALUE);
        ASSERT(!IsReasonable<const int&    >::k_VALUE);

        ASSERT( IsReasonable<Abstract      >::k_VALUE);
        ASSERT( IsReasonable<Incomplete    >::k_VALUE);
        ASSERT( IsReasonable<      MyEnum  >::k_VALUE);
        ASSERT( IsReasonable<      MyStruct>::k_VALUE);
        ASSERT( IsReasonable<const MyStruct>::k_VALUE);
        ASSERT( IsReasonable<      MyUnion >::k_VALUE);

        ASSERT( IsReasonable<int T::*      >::k_VALUE);
        ASSERT( IsReasonable<int(T::*)()const>::k_VALUE);

        ASSERT( IsReasonable<int    [ ]    >::k_VALUE);
        ASSERT( IsReasonable<int (*)[ ]    >::k_VALUE);
        ASSERT(!IsReasonable<int (&)[ ]    >::k_VALUE);
        ASSERT( IsReasonable<int    [2]    >::k_VALUE);
        ASSERT( IsReasonable<int (*)[3]    >::k_VALUE);
        ASSERT(!IsReasonable<int (&)[4]    >::k_VALUE);

        ASSERT( IsReasonable<void   ()     >::k_VALUE);
        ASSERT( IsReasonable<void(*)()     >::k_VALUE);
        ASSERT(!IsReasonable<void(&)()     >::k_VALUE);
#ifndef BUG_FORMS_POINTERS_AND_REFERENCES_TO_ABOMINABLE_FUNCTIONS
        ASSERT(!IsReasonable<void   ()const>::k_VALUE);
#endif

# if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE)
        ASSERT( IsReasonable<void   ()        noexcept>::k_VALUE);
        ASSERT( IsReasonable<void(*)()        noexcept>::k_VALUE);
        ASSERT(!IsReasonable<void(&)()        noexcept>::k_VALUE);
#  ifndef BUG_FORMS_POINTERS_AND_REFERENCES_TO_ABOMINABLE_FUNCTIONS
        ASSERT(!IsReasonable<void   ()  const noexcept>::k_VALUE);
#  endif
        ASSERT( IsReasonable<int(T::*)()const noexcept>::k_VALUE);
# endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `BSLMF_VOIDTYPE`
        //   Due to the C++98 requirements that `typename` be used only where
        //   the compiler absolutely requires it (rather than allowing for
        //   redundant use) the `BSLMF_VOIDTYPE` macro is to be used only in
        //   type-dependent contexts.  That means testing must be in terms of
        //   instantiating templates declared outside `main`.  Note that the
        //   macro arguments supplied to `BSLMF_VOIDTYPE` cannot contain any
        //   mix of non-type expressions:
        //  - literal values
        //  - string literals
        //  - enumerators
        //  - uninstantiated templates
        //
        // Concerns:
        // 1. `BSLMF_VOIDTYPE(T)` is an alias for `void`, when `T` forms a
        //    valid (dependent) type expression.
        //
        // 2. `BSLMF_VOIDTYPE(T)` fails to instantiate in a SFINAE compatible
        //    manner, for all dialects of C++, when the macro argument `T`
        //    forms an invalid (dependent) type expression.
        //
        // Plan:
        // 1. Create a metafunction, `IsClassType`, using the macro
        //    `BSLMF_VOIDTYPE` to filter types on some property.
        //
        // 2. Instantiate that metafunction for a wide variety of types that
        //    cover the potential syntax space.
        //
        // Testing:
        //   BSLMF_VOIDTYPE
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `BSLMF_VOIDTYPE`"
                          "\n========================");

        typedef Incomplete T;   // abbreviation for table below

        ASSERT(!IsClassType<      void    >::k_VALUE);
        ASSERT(!IsClassType<const void    >::k_VALUE);
        ASSERT(!IsClassType<const void *  >::k_VALUE);
        ASSERT(!IsClassType<      int     >::k_VALUE);
        ASSERT(!IsClassType<const int     >::k_VALUE);
        ASSERT(!IsClassType<const int *   >::k_VALUE);
        ASSERT(!IsClassType<const int&    >::k_VALUE);

        ASSERT( IsClassType<Abstract      >::k_VALUE);
        ASSERT( IsClassType<Incomplete    >::k_VALUE);
        ASSERT(!IsClassType<      MyEnum  >::k_VALUE);
        ASSERT( IsClassType<      MyStruct>::k_VALUE);
        ASSERT( IsClassType<const MyStruct>::k_VALUE);
        ASSERT( IsClassType<      MyUnion >::k_VALUE);

        ASSERT(!IsClassType<int T::*      >::k_VALUE);
        ASSERT(!IsClassType<int(T::*)()const>::k_VALUE);

        ASSERT(!IsClassType<int    [ ]    >::k_VALUE);
        ASSERT(!IsClassType<int (*)[ ]    >::k_VALUE);
        ASSERT(!IsClassType<int (&)[ ]    >::k_VALUE);
        ASSERT(!IsClassType<int    [2]    >::k_VALUE);
        ASSERT(!IsClassType<int (*)[3]    >::k_VALUE);
        ASSERT(!IsClassType<int (&)[4]    >::k_VALUE);

        ASSERT(!IsClassType<void   ()     >::k_VALUE);
        ASSERT(!IsClassType<void(*)()     >::k_VALUE);
        ASSERT(!IsClassType<void(&)()     >::k_VALUE);
        ASSERT(!IsClassType<void   ()const>::k_VALUE);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE)
        ASSERT(!IsClassType<void   ()        noexcept>::k_VALUE);
        ASSERT(!IsClassType<void(*)()        noexcept>::k_VALUE);
        ASSERT(!IsClassType<void(&)()        noexcept>::k_VALUE);
        ASSERT(!IsClassType<void   ()  const noexcept>::k_VALUE);
        ASSERT(!IsClassType<int(T::*)()const noexcept>::k_VALUE);
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `bsl::void_t<T>`
        //   As `bsl::void_t` requires specific C++11 features, there are no
        //   concerns to test when building with C++03.
        //
        // Concerns:
        // 1. `bsl::void_t<T1, T2... TN>` is an alias for `void` (for any N).
        //
        // 2. The types used to instantiate `bsl::void_t` can be any mix of
        //    valid types, including (potentially cv-qualified):
        //    - fundamental types (including `void`)
        //    - enumerations
        //    - classes
        //    - unions
        //    - incomplete classes
        //    - pointers
        //    - array pointers
        //    - function pointers
        //    - pointers-to-data-member
        //    - pointers-to-function-member
        //    - arrays of known bound
        //    - arrays of unknown bound
        //    - reference types
        //    - function references
        //    - array references
        //    - regular function types
        //    - abominable function types
        //
        // 3. Duplicate types are allowed.
        //
        // Plan:
        // 1. For concern 1, instantiate `bsl::void_t<T1, T2... TN>` with 0 to
        //    14 parameters and verify that the result is type `void`.  (C-1)
        //
        // 2. For concern 2, ensure that P-1 has a healthy mix of parameter
        //    types.  (C-2)
        //
        // 3. For concern 3, ensure that P-1 includes a list of parameters in
        //    which a type is repeated.  (C-3)
        //
        // Testing:
        //   bsl::void_t
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `bsl::void_t<T>`"
                          "\n========================");

#ifndef BSLS_COMPILERFEATURES_FULL_CPP11
        if (verbose) puts("`bsl::void_t` requires full C++11 support");
#else
# define TEST(...) \
        ASSERT(isVoidPointer((bsl::void_t<__VA_ARGS__> *)0))

        // Preserve the structure of test case 2 for the initial test cases up
        // to 14 arguments.

        TEST();
        TEST(signed long long);
        TEST(signed long long,   signed long long);  // test for duplicates
        TEST(signed long long,   MyEnum);
        TEST(signed long long,   MyEnum,   MyStruct);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *
            );
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &
            );

        // As we can assume variadic templates, all the remaining type concerns
        // can be verified with just one more parameter pack.

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &,
             const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*,
             int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...),
             int() const, int(int...) volatile, int(int, int(&)()...) const
             );

# undef TEST
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `VoidType<T>::type`
        //
        // Concerns:
        // 1. `bslmf::VoidType<T1, T2... TN>::type` is `void` for N from 0 to
        //    14.
        //
        // 2. The types used to instantiate `bslmf::VoidType` can be any mix of
        //    valid types, including (potentially cv-qualified):
        //    - fundamental types (including `void`)
        //    - enumerations
        //    - classes
        //    - unions
        //    - incomplete classes
        //    - pointers
        //    - array pointers
        //    - function pointers
        //    - pointers-to-data-member
        //    - pointers-to-function-member
        //    - arrays of known bound
        //    - arrays of unknown bound
        //    - reference types
        //    - function references
        //    - array references
        //    - regular function types
        //    - abominable function types
        //
        // 3. Duplicate types are allowed.
        //
        // Plan:
        // 1. For concern 1, instantiate `bslmf::VoidType<T1, T2... TN>` with 0
        //    to 14 parameters and verify that the result `type` is `void`.
        //    (C-1)
        //
        // 2. For concern 2, ensure that P-1 has a healthy mix of parameter
        //    types.  (C-2)
        //
        // 3. For concern 3, ensure that P-1 includes a list of parameters in
        //    which a type is repeated.  (C-3)
        //
        // Testing:
        //   VoidType::type
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING `VoidType<T>::type`"
                          "\n===========================");

#define TEST(...) \
        ASSERT(isVoidPointer((bslmf::VoidType<__VA_ARGS__>::type *)0))

        // Incrementally test up to 14 types

        TEST();
        TEST(signed long long);
        TEST(signed long long,   signed long long);  // test for duplicates
        TEST(signed long long,   MyEnum);
        TEST(signed long long,   MyEnum,   MyStruct);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *
            );
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &);
        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &);

# ifdef BSLS_COMPILERFEATURES_FULL_CPP11
        // Additional test for 15 types, demonstrating variadic support

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &
            );

        // Test remaining types that may have interesting parses.  We do not
        // need to break this remaining list of types into smaller fragments as
        // variadic templates will consume as many arguments as we supply (up
        // to a compiler limit that must be at least 1000)

        TEST(signed long long,   MyEnum,   MyStruct,   MyUnion,   Incomplete,
             signed long long *, MyEnum *, MyStruct *, MyUnion *, Incomplete *,
             signed long long &, MyEnum &, MyStruct &, MyUnion &, Incomplete &,
             const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*,
             int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...),
             int() const, int(int...) volatile, int(int, int(&)()...) const
             );
# else
        // Test remaining types that may have interesting parses, splitting
        // tests into sub-lists that fit the supported limit of 14 types.

        TEST(const int, volatile MyEnum, const MyStruct *volatile,
             void, const void, volatile void ******, volatile void **const ***,
             int MyStruct::*, int MyStruct::* const, const int MyStruct::*);

        TEST(int (MyStruct::*)(), int (MyStruct::*)(int,...) const volatile,
             unsigned   [], unsigned   [2], unsigned   (), unsigned   (...),
             unsigned(*)[], unsigned(*)[2], unsigned(*)(), unsigned(*)(...),
             unsigned(&)[], unsigned(&)[2], unsigned(&)(), unsigned(&)(...));

        TEST(int() const, int(int...) volatile, int(int, int(&)()...) const);
# endif

#undef TEST
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING THE TEST MACHINERY
        //
        // Concerns:
        // 1. `isVoidPointer(arg)` returns `true` if (and only if) `arg` has
        //    the type `void *`.
        //
        // 2. `isVoidPointer(arg)` returns `false` if `arg` has the type of a
        //    pointer to cv-qualified `void`.
        //
        // 3. `isVoidPointer(arg)` returns `false` if `arg` has the type of a
        //    pointer to any object type other than `void`.
        //
        // 4. `isVoidPointer(arg)` returns `false` if `arg` has the type of a
        //    pointer to any function type.
        //
        // 5. `isVoidPointer(arg)` returns `false` if `arg` has the type of a
        //    class with a conversion operator to `void *`.
        //
        // 6. `isVoidPointer(arg)` returns `false` if `arg` is a null pointer
        //    constant such as `0` or `nullptr`, which are not actually
        //    pointers.
        //
        // Plan:
        // 1. Call `isVoidPointer` with a variety of null pointers, cast to
        //    pointers to cv-qualified `void`, asserting that only the pointer
        //    to unqualified `void` returns `true`.  (C-1,2)
        //
        // 2. Call `isVoidPointer` with a variety of null pointers, cast to
        //    types that may fool a careless human into thinking they are
        //    related to `void *`, and verify that all return `false`.  (C-3,4)
        //
        // 3. Call `isVoidPointer` with 0 and, where supported, `nullptr`, and
        //    verify that the result is `false` in each case.  (C-6)
        //
        // 4. Call `isVoidPointer` with an object of a class type implicitly
        //    convertible to `void *`, and verify that it returns `false`.
        //    (C-5)
        //
        // Testing:
        //   CONCERN: Test machinery works as expected
        // --------------------------------------------------------------------

        if (verbose) puts("\nTESTING THE TEST MACHINERY"
                          "\n==========================");

        ASSERT( isVoidPointer((void *)0));
        ASSERT(!isVoidPointer((const void *)0));
        ASSERT(!isVoidPointer((volatile void *)0));
        ASSERT(!isVoidPointer((const volatile void *)0));

        ASSERT(!isVoidPointer((void **)0));
        ASSERT(!isVoidPointer((int *)0));
        ASSERT(!isVoidPointer((void *(*)[])0));
        ASSERT(!isVoidPointer((void  (*)())0));
        ASSERT(!isVoidPointer((void *(*)())0));

        ASSERT(!isVoidPointer((MyStruct *)0));

        const ConvertibleToVoidStar X = {};
        ASSERT(!isVoidPointer(X));
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
