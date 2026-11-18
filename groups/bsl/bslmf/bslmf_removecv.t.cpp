// bslmf_removecv.t.cpp                                               -*-C++-*-
#include <bslmf_removecv.h>

#include <bslmf_issame.h>
#include <bslmf_typeidentity.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_SUN)
// Needed only for `CvForms`; see `BUG_COMPILER_FORMS_CONST_REFERENCES`.
#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addvolatile.h>
#endif

#include <stdio.h>   // `printf`, `puts`
#include <stdlib.h>  // `atoi`

using namespace BloombergLP;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines metafunctions, `bsl::remove_cv` and
// `bsl::remove_cv_t`, that remove any top-level cv-qualifiers from a template
// parameter type.  Thus, we need to ensure that the values returned by the
// metafunction are correct for each possible category of types.
//
// ----------------------------------------------------------------------------
// PUBLIC TYPE
// [ 3] bsl::remove_cv::type
// [ 3] bsl::remove_cv_t
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 2] TEST MACHINERY
// [ 4] CONCERN: class template can be passed as a metafunction parameter

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
//                      WARNING SUPPRESSION
//-----------------------------------------------------------------------------

// This test driver intentionally creates types with unusual use of
// cv-qualifiers in order to confirm that there are no strange corners of the
// type system that are not addressed by this traits component.  Consequently,
// we disable certain warnings from common compilers.

#if defined(BSLS_PLATFORM_CMP_MSVC)
# pragma warning(disable : 4180)
#elif defined(BSLS_PLATFORM_CMP_SUN)
# pragma error_messages(off, functypequal)
#endif

//=============================================================================
//                        DEFECT DETECTION MACROS
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_SUN)
// The Sun CC compiler will form illegal types such as `int & const` when
// implementing metafunctions in the simplest way.  That problem is solved by
// using `typename add_const<T>::type` instead of `const T`, where `T` is a
// template parameter or other dependent type.
# define BUG_COMPILER_FORMS_CONST_REFERENCES 1
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

extern "C" {
/// This function type having C language linkage is used to validate
/// `bsl::remove_cv`.
typedef void CLinkageFunctionTestType(int);
}

namespace {

/// This user-defined `enum` type is used to validate `bsl::remove_cv`.
enum   EnumTestType {
};

/// This user-defined `struct` type is used to validate `bsl::remove_cv`.
struct StructTestType {
};

/// This user-defined `union` type is used to validate `bsl::remove_cv`.
union  UnionTestType {
};

/// This user-defined base class type is used to validate `bsl::remove_cv`.
class  BaseClassTestType {
};

/// This user-defined derived class type is used to validate
/// `bsl::remove_cv`.
class  DerivedClassTestType : public BaseClassTestType {
};

/// This user-defined abstract class type, of which no array can be formed, is
/// used to validate `bsl::remove_cv`.
class  AbstractClassTestType {
  public:
    virtual void f() = 0;
};

/// This `struct` template fails to compile if it is instantiated, and is used
/// to validate that `bsl::remove_cv` does not instantiate its (template
/// parameter) `t_TYPE`.
template <class t_TYPE>
struct PoisonTestType {
    typedef char Instantiated[sizeof(t_TYPE) == 0 ? 1 : -1];
};

#if BSLS_COMPILERFEATURES_FULL_CPP11
/// This user-defined scoped enumeration type is used to validate
/// `bsl::remove_cv`.
enum class ScopedEnumTestType {
};

/// This user-defined enumeration type, having a fixed underlying type, is used
/// to validate `bsl::remove_cv`.
enum FixedEnumTestType : short {
};

/// These opaque enumeration types, declared without enumerators, are used to
/// validate `bsl::remove_cv`.
enum class OpaqueScopedEnumTestType;
enum       OpaqueBoolEnumTestType       : bool;
enum class OpaqueScopedBoolEnumTestType : bool;
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

/// This pointer-to-member-function type is used to validate `bsl::remove_cv`.
typedef int (StructTestType::*MethodPtrTestType) ();

/// This function pointer type is used to validate `bsl::remove_cv`.
typedef void (*FunctionPtrTestType) ();

/// This pointer-to-data-member type is used to validate `bsl::remove_cv`.
typedef int StructTestType::* PMD;

/// This incomplete `struct` type is used to validate `bsl::remove_cv`.
struct Incomplete;

/// This incomplete `union` type is used to validate `bsl::remove_cv`.
union Uncomplete;

/// This `struct` template provides a `type` that is the `type` member of the
/// specified (template parameter) `t_METAFUNCTION` instantiated with the
/// specified (template parameter) `t_TYPE`.  `t_METAFUNCTION` shall be a class
/// template having a single type parameter and that provides its result as the
/// typedef-name `type`.
template <template <class> class t_METAFUNCTION, class t_TYPE>
struct ApplyMetafunction {

    // PUBLIC TYPES
    typedef typename t_METAFUNCTION<t_TYPE>::type type;
};

                            // ===================
                            // struct ShowBadConst
                            // ===================

/// This `struct` metafunction provides a `type` that is the specified
/// (template parameter) `t_TYPE` with a `const`-qualifier applied directly, as
/// `t_TYPE const`.  This template will demonstrate the bug signified by the
/// macro `BUG_COMPILER_FORMS_CONST_REFERENCES` when it is defined.
template <class t_TYPE>
struct ShowBadConst {

    // PUBLIC TYPES
    typedef t_TYPE const type;
};

                              // ==============
                              // struct CvForms
                              // ==============

/// This `struct` template provides the three cv-qualified forms of the
/// (template parameter) `t_TYPE`, for a dependent template argument.  Note
/// that the qualifiers apply to `t_TYPE` as a whole: an array `t_TYPE` has
/// qualified elements, and the qualifiers are ignored for a reference or
/// function `t_TYPE`.
template <class t_TYPE>
struct CvForms {

    // PUBLIC TYPES
#if defined(BUG_COMPILER_FORMS_CONST_REFERENCES)
    typedef typename bsl::add_const<t_TYPE>::type    C;
    typedef typename bsl::add_volatile<t_TYPE>::type V;
    typedef typename bsl::add_cv<t_TYPE>::type       CV;
#else
    typedef t_TYPE const          C;
    typedef t_TYPE       volatile V;
    typedef t_TYPE const volatile CV;
#endif
};

                           // ===================
                           // struct RemoveCvTest
                           // ===================

/// This `struct` template provides a namespace for functions that verify the
/// result of `bsl::remove_cv`, and (in C++11 and later) `bsl::remove_cv_t`,
/// for the (template parameter) `t_TYPE` and for types produced from it,
/// reporting any failure against the specified `line`.  Cv-qualified forms of
/// `t_TYPE` are those provided by `CvForms`.  Note that every result is
/// verified for a dependent template argument.
template <class t_TYPE>
struct RemoveCvTest {

    // PUBLIC TYPES
    typedef typename CvForms<t_TYPE>::C  C;
    typedef typename CvForms<t_TYPE>::V  V;
    typedef typename CvForms<t_TYPE>::CV CV;

    // CLASS METHODS

    /// Verify that the result for `t_TYPE` is the (template parameter)
    /// `t_RESULT`.
    template <class t_RESULT>
    static void testExpect(int line)
    {
        ASSERTV(line, (bsl::is_same<typename bsl::remove_cv<t_TYPE>::type,
                                    t_RESULT>::value));
#if BSLS_COMPILERFEATURES_FULL_CPP11
        ASSERTV(line, (bsl::is_same<bsl::remove_cv_t<t_TYPE>,
                                    t_RESULT>::value));
#endif
    }

    /// Verify that the result for `t_TYPE`, and for each cv-qualified form of
    /// `t_TYPE`, is `t_TYPE`.  `t_TYPE` must not be cv-qualified.
    static void testCvq(int line)
    {
        testExpect<t_TYPE>(line);
        RemoveCvTest<C >::template testExpect<t_TYPE>(line);
        RemoveCvTest<V >::template testExpect<t_TYPE>(line);
        RemoveCvTest<CV>::template testExpect<t_TYPE>(line);
    }

    /// Verify that the result for each cv-qualified form of a pointer to each
    /// cv-qualified form of `t_TYPE` is that pointer, retaining the
    /// qualifiers of the pointed-to type.  `t_TYPE` must not be cv-qualified,
    /// nor a reference type, nor an abominable function type.
    static void testPtr(int line)
    {
        RemoveCvTest<t_TYPE *>::testCvq(line);
        RemoveCvTest<C      *>::testCvq(line);
        RemoveCvTest<V      *>::testCvq(line);
        RemoveCvTest<CV     *>::testCvq(line);
    }

    /// Verify that the result for each cv-qualified form of an lvalue
    /// reference to each cv-qualified form of `t_TYPE` is that reference,
    /// unchanged.  `t_TYPE` must not be cv-qualified, nor a reference type,
    /// nor cv-`void`, nor an abominable function type.
    static void testLref(int line)
    {
        RemoveCvTest<t_TYPE &>::testCvq(line);
        RemoveCvTest<C      &>::testCvq(line);
        RemoveCvTest<V      &>::testCvq(line);
        RemoveCvTest<CV     &>::testCvq(line);
    }

    /// In C++11 and later, verify that the result for each cv-qualified form
    /// of an rvalue reference to each cv-qualified form of `t_TYPE` is that
    /// reference, unchanged; otherwise, do nothing.  `t_TYPE` must not be
    /// cv-qualified, nor a reference type, nor cv-`void`, nor an abominable
    /// function type.
    static void testRref(int line)
    {
#if BSLS_COMPILERFEATURES_FULL_CPP11
        RemoveCvTest<t_TYPE &&>::testCvq(line);
        RemoveCvTest<C      &&>::testCvq(line);
        RemoveCvTest<V      &&>::testCvq(line);
        RemoveCvTest<CV     &&>::testCvq(line);
#else
        (void)line;
#endif
    }

    /// Verify the result for each cv-qualified form of `t_TYPE`, for each
    /// cv-qualified pointer to such a form, and for lvalue and (in C++11 and
    /// later) rvalue references to any of those.  `t_TYPE` must not be
    /// cv-qualified, nor a reference type, nor cv-`void`, nor an abominable
    /// function type.
    static void testAll(int line)
    {
        testCvq(line);
        testPtr(line);
        testLref(line);
        testRref(line);
        RemoveCvTest<t_TYPE *>::testLref(line);
        RemoveCvTest<t_TYPE *>::testRref(line);
    }
};

}  // close unnamed namespace

// The following macros spell out function types from a function `SIGNATURE`
// (e.g., `int(char *, ...)`) by appending qualifiers to it, which is the only
// way to form an "abominable" function type deliberately.  All other testing
// uses `RemoveCvTest`.

/// Verify the result for the ordinary function type given by `SIGNATURE`, for
/// references and pointers to it, and for a pointer to member function of that
/// type.
#define TEST_ORDINARY_FUNCTION(SIGNATURE)                                     \
    {                                                                         \
        typedef bsl::type_identity<SIGNATURE>::type Function;                 \
        typedef Function StructTestType::*          MethodPtr;                \
                                                                              \
        RemoveCvTest<Function>::testAll(L_);                                  \
        RemoveCvTest<MethodPtr>::testAll(L_);                                 \
    }

/// Verify that `bsl::remove_cv` leaves unchanged the "abominable" function
/// type given by `SIGNATURE`, a function signature followed by qualifiers,
/// including through an added cv-qualifier, and verify the result for a
/// pointer to member function of that type.
#define TEST_ABOMINABLE(SIGNATURE)                                            \
    {                                                                         \
        typedef bsl::type_identity<SIGNATURE>::type Abominable;               \
        typedef Abominable StructTestType::*        AbominableMethodPtr;      \
                                                                              \
        RemoveCvTest<Abominable>::testCvq(L_);                                \
        RemoveCvTest<AbominableMethodPtr>::testAll(L_);                       \
    }

#if BSLS_COMPILERFEATURES_FULL_CPP11
/// Apply `TEST_ABOMINABLE` to each ref-qualified form of the given function
/// `SIGNATURE`.
# define TEST_ABOMINABLE_REF_FORMS(SIGNATURE)                                 \
    TEST_ABOMINABLE(SIGNATURE                &)                               \
    TEST_ABOMINABLE(SIGNATURE                &&)                              \
    TEST_ABOMINABLE(SIGNATURE const          &)                               \
    TEST_ABOMINABLE(SIGNATURE const          &&)                              \
    TEST_ABOMINABLE(SIGNATURE       volatile &)                               \
    TEST_ABOMINABLE(SIGNATURE       volatile &&)                              \
    TEST_ABOMINABLE(SIGNATURE const volatile &)                               \
    TEST_ABOMINABLE(SIGNATURE const volatile &&)
#else
# define TEST_ABOMINABLE_REF_FORMS(SIGNATURE)
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE)
/// Apply `TEST_ORDINARY_FUNCTION` to the `noexcept` form of the function
/// `SIGNATURE`, and `TEST_ABOMINABLE` to each of its cv- and ref-qualified
/// `noexcept` forms.
# define TEST_NOEXCEPT_FUNCTION_FORMS(SIGNATURE)                              \
    TEST_ORDINARY_FUNCTION(SIGNATURE noexcept)                                \
    TEST_ABOMINABLE(SIGNATURE const             noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE       volatile    noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE const volatile    noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE                &  noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE                && noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE const          &  noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE const          && noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE       volatile &  noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE       volatile && noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE const volatile &  noexcept)                     \
    TEST_ABOMINABLE(SIGNATURE const volatile && noexcept)
#else
# define TEST_NOEXCEPT_FUNCTION_FORMS(SIGNATURE)
#endif

/// Apply `TEST_ORDINARY_FUNCTION` to the function `SIGNATURE`, and
/// `TEST_ABOMINABLE` to each of its cv- and (in C++11 and later) ref-qualified
/// forms.  In C++17 and later, do the same for the `noexcept` forms of
/// `SIGNATURE`.
#define TEST_FUNCTION_FORMS(SIGNATURE)                                        \
    TEST_ORDINARY_FUNCTION(SIGNATURE)                                         \
    TEST_ABOMINABLE(SIGNATURE const)                                          \
    TEST_ABOMINABLE(SIGNATURE       volatile)                                 \
    TEST_ABOMINABLE(SIGNATURE const volatile)                                 \
    TEST_ABOMINABLE_REF_FORMS(SIGNATURE)                                      \
    TEST_NOEXCEPT_FUNCTION_FORMS(SIGNATURE)

/// Verify that the result of `bsl::remove_cv`, and (in C++11 and later)
/// `bsl::remove_cv_t`, for the non-dependent template argument `TYPE` is
/// `RESULT`.
#if BSLS_COMPILERFEATURES_FULL_CPP11
# define TEST_NON_DEPENDENT(TYPE, RESULT)                                     \
    ASSERT((bsl::is_same<bsl::remove_cv<TYPE>::type, RESULT>::value));        \
    ASSERT((bsl::is_same<bsl::remove_cv_t<TYPE>,     RESULT>::value))
#else
# define TEST_NON_DEPENDENT(TYPE, RESULT)                                     \
    ASSERT((bsl::is_same<bsl::remove_cv<TYPE>::type, RESULT>::value))
#endif

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

    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);       // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
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

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers of a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two `typedef`s -- a `const`-qualified and
// `volatile`-qualified type (`MyCvType`) and the same type without the
// cv-qualifiers (`MyType`):
// ```
    typedef int                MyType;
    typedef const volatile int MyCvType;
// ```
// Now, we remove the cv-qualifiers from `MyCvType` using `bsl::remove_cv` and
// verify that the resulting type is the same as `MyType`:
// ```
    ASSERT(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
                                 MyType>::value));
// ```
// Finally, in C++11 and later, we remove the cv-qualifiers from `MyCvType`
// using the alias template `bsl::remove_cv_t` and verify that the resulting
// type is the same as `MyType`:
// ```
#if BSLS_COMPILERFEATURES_FULL_CPP11
    ASSERT(true == (bsl::is_same<bsl::remove_cv_t<MyCvType>, MyType>::value));
#endif
// ```
// Note that `bsl::remove_cv_t` avoids the `::type` suffix and `typename`
// prefix when we want to use the result of the `bsl::remove_cv` metafunction
// in templates.
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: CLASS TEMPLATE CAN BE PASSED AS A METAFUNCTION PARAMETER
        //
        // Concerns:
        // 1. `bsl::remove_cv` can be passed as the argument for a template
        //    template parameter having a single type parameter (i.e.,
        //    `template <class> class`).
        //
        // 2. The `type` member of `bsl::remove_cv` instantiated through such a
        //    template template parameter is the same as that of a direct
        //    instantiation.
        //
        // Plan:
        // 1. Instantiate `ApplyMetafunction`, which has a
        //    `template <class> class` template template parameter, with
        //    `bsl::remove_cv` and a selection of types, including types that
        //    `bsl::remove_cv` leaves unchanged, and verify that its `type`
        //    member is the same as `bsl::remove_cv<TYPE>::type`.  (C-1..2)
        //
        // Testing:
        //   CONCERN: class template can be passed as a metafunction parameter
        // --------------------------------------------------------------------

        if (verbose) puts(
        "\nCONCERN: CLASS TEMPLATE CAN BE PASSED AS A METAFUNCTION PARAMETER"
        "\n=================================================================");

        typedef int AbomC() const;

#define TEST_APPLY(TYPE)                                                      \
        ASSERT((bsl::is_same<ApplyMetafunction<bsl::remove_cv, TYPE>::type,   \
                             bsl::remove_cv<TYPE>::type>::value))

        TEST_APPLY(int               );
        TEST_APPLY(const volatile int);
        TEST_APPLY(const int &       );
        TEST_APPLY(const void        );
        TEST_APPLY(AbomC             );
#if BSLS_COMPILERFEATURES_FULL_CPP11
        TEST_APPLY(const int &&      );
#endif

#undef TEST_APPLY
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // `bsl::remove_cv::type`
        //   Ensure that the `typedef` `type` of `bsl::remove_cv` has the
        //   correct type for a variety of template parameter types.
        //
        // Concerns:
        // 1. `bsl::remove_cv` leaves unchanged a type that is not
        //    cv-qualified, for each category of type.
        //
        // 2. `bsl::remove_cv` removes any top-level `const`, `volatile`, or
        //    `const volatile` qualifier from an object type, including
        //    cv-`void`.
        //
        // 3. `bsl::remove_cv` removes the cv-qualifiers from an array type,
        //    which are those of its elements, without decaying the array.
        //
        // 4. `bsl::remove_cv` does not remove cv-qualifiers that are not
        //    top-level, such as those of the type pointed to by a pointer, of
        //    the type referred to by a reference, or of a member function.
        //
        // 5. `bsl::remove_cv` leaves reference types unchanged.
        //
        // 6. `bsl::remove_cv` leaves function types unchanged, including
        //    "abominable" function types (with a trailing cv- or
        //    ref-qualifier).
        //
        // 7. `bsl::remove_cv_t` represents the return type of the
        //    `bsl::remove_cv` metafunction.
        //
        // 8. `bsl::remove_cv` does not instantiate the underlying template if
        //    the `t_TYPE` argument is a template specialization.
        //
        // 9. `bsl::remove_cv` gives the same results for a non-dependent
        //    template argument, used outside any template, as for a dependent
        //    one.
        //
        // Plan:
        // 1. Using the class template `RemoveCvTest`, verify results for a
        //    (template parameter) `t_TYPE`, and for types produced from it,
        //    against a given line, so that every result so verified is for a
        //    dependent template argument.  (C-9)
        //    - `testExpect<t_RESULT>` verifies that `bsl::remove_cv<t_TYPE>`
        //      yields `t_RESULT` and, in C++11 and later, that
        //      `bsl::remove_cv_t<t_TYPE>` also yields `t_RESULT`.  (C-7)
        //    - `testCvq` applies `testExpect` to `t_TYPE` and to each of its
        //      three cv-qualified forms, given by `CvForms` (verified in case
        //      2), expecting `t_TYPE`.
        //    - `testPtr`, `testLref`, and `testRref` apply `testCvq` to a
        //      pointer, lvalue reference, and (in C++11 and later) rvalue
        //      reference to each cv-qualified form of `t_TYPE`, and `testAll`
        //      applies `testCvq`, `testPtr`, `testLref`, and `testRref` to
        //      `t_TYPE`, and `testLref` and `testRref` to a pointer to
        //      `t_TYPE`.
        //
        // 2. The macro `TEST_FUNCTION_FORMS` spells out function types by
        //    appending qualifiers to a function signature (e.g.,
        //    `int(char *, ...)`), and so is the only means of deliberately
        //    forming "abominable" function types, which it names via the
        //    `bsl::type_identity` class template.  Via
        //    `TEST_ORDINARY_FUNCTION`, it applies `RemoveCvTest::testAll` to
        //    the ordinary function type and to a pointer to member function of
        //    that type, and, via `TEST_ABOMINABLE`, applies
        //    `RemoveCvTest::testCvq` to each of its cv- and (in C++11 and
        //    later) ref-qualified abominable forms, and
        //    `RemoveCvTest::testAll` to a pointer to member function of that
        //    form.  In C++17 and later, it does the same for the `noexcept`
        //    forms of the signature.
        //
        // 3. Using `RemoveCvTest::testCvq` and `RemoveCvTest::testPtr`, verify
        //    the result for `void`.  (C-1, 2, 4)
        //
        // 4. Using `RemoveCvTest::testAll`, verify the result for a variety
        //    of object types, and pointers and references to them.  (C-1, 2,
        //    4, 5)  Object types include:
        //    - fundamental, including `bool`, `char`, `double`, and
        //      `bsl::nullptr_t`
        //    - enumeration, including (C++11 and later) scoped, having a fixed
        //      underlying type, and opaque (declared without enumerators),
        //      including with a `bool` underlying type
        //    - class, including `struct`, `union`, base, derived, and abstract
        //      classes
        //    - incomplete class, including `union`
        //    - pointer to object, including pointer to cv-qualified object
        //    - pointer to function
        //    - pointer to member function
        //    - pointer to data member, and pointers to such pointers
        //
        // 5. Using `RemoveCvTest::testAll`, verify the result for arrays of
        //    known and unknown bound, of one or more dimensions, having
        //    elements of fundamental, class, pointer, and pointer-to-member
        //    type, and for pointers to such arrays.  (C-1..5)
        //
        // 6. Using `TEST_FUNCTION_FORMS`, verify the results for function
        //    types returning `void`, an object, a reference, or a pointer,
        //    having zero or more parameters, with or without a C-style
        //    ellipsis, and (C++17 and later) `noexcept`, together with
        //    references and pointers to them, their abominable forms, and
        //    pointers to member functions of all those forms.  Using
        //    `RemoveCvTest::testAll`, verify the result for a function type
        //    having C language linkage.  (C-1, 2, 4..6)
        //
        // 7. Using `TEST_NON_DEPENDENT`, directly in the test case, verify the
        //    result for non-dependent template arguments of each category,
        //    both unqualified and cv-qualified: cv-`void`, fundamental,
        //    enumeration, class, incomplete class, pointer, pointer to data
        //    member, pointer to ordinary and abominable member function, array
        //    of known and unknown bound, ordinary and (C++17 and later)
        //    `noexcept` function, abominable function (including, in C++11 and
        //    later, ref-qualified and, in C++17 and later, `noexcept` forms),
        //    and lvalue and (C++11 and later) rvalue reference, including to a
        //    function.  (C-9)
        //
        // 8. Using `RemoveCvTest::testAll`, verify the result for a class
        //    template specialization that would fail to compile if
        //    instantiated.  (C-8)
        //
        // Testing:
        //   bsl::remove_cv::type
        //   bsl::remove_cv_t
        // --------------------------------------------------------------------

        if (verbose) puts("\n`bsl::remove_cv::type`"
                          "\n======================");

        // C-1, 2, 4

        if (veryVerbose) puts("\tvoid");
        {
            RemoveCvTest<void>::testCvq(L_);
            RemoveCvTest<void>::testPtr(L_);
        }

        // C-1, 2, 4, 5

        if (veryVerbose) puts("\tFundamental types");
        {
            RemoveCvTest<bool          >::testAll(L_);
            RemoveCvTest<char          >::testAll(L_);
            RemoveCvTest<int           >::testAll(L_);
            RemoveCvTest<double        >::testAll(L_);
            RemoveCvTest<bsl::nullptr_t>::testAll(L_);
        }

        if (veryVerbose) puts("\tEnumeration types");
        {
            RemoveCvTest<EnumTestType>::testAll(L_);
#if BSLS_COMPILERFEATURES_FULL_CPP11
            RemoveCvTest<ScopedEnumTestType>::testAll(L_);
            RemoveCvTest<FixedEnumTestType >::testAll(L_);
#endif
        }

#if BSLS_COMPILERFEATURES_FULL_CPP11
        if (veryVerbose) puts("\tOpaque enumeration types");
        {
            RemoveCvTest<OpaqueScopedEnumTestType    >::testAll(L_);
            RemoveCvTest<OpaqueBoolEnumTestType      >::testAll(L_);
            RemoveCvTest<OpaqueScopedBoolEnumTestType>::testAll(L_);
        }
#endif

        if (veryVerbose) puts("\tClass types");
        {
            RemoveCvTest<StructTestType       >::testAll(L_);
            RemoveCvTest<UnionTestType        >::testAll(L_);
            RemoveCvTest<BaseClassTestType    >::testAll(L_);
            RemoveCvTest<DerivedClassTestType >::testAll(L_);
            RemoveCvTest<AbstractClassTestType>::testAll(L_);
        }

        if (veryVerbose) puts("\tIncomplete class types");
        {
            RemoveCvTest<Incomplete>::testAll(L_);
            RemoveCvTest<Uncomplete>::testAll(L_);
        }

        if (veryVerbose) puts("\tPointer types");
        {
            RemoveCvTest<int                *>::testAll(L_);
            RemoveCvTest<const          int *>::testAll(L_);
            RemoveCvTest<      volatile int *>::testAll(L_);
            RemoveCvTest<const volatile int *>::testAll(L_);

            RemoveCvTest<EnumTestType         *>::testAll(L_);
            RemoveCvTest<StructTestType       *>::testAll(L_);
            RemoveCvTest<UnionTestType        *>::testAll(L_);
            RemoveCvTest<BaseClassTestType    *>::testAll(L_);
            RemoveCvTest<DerivedClassTestType *>::testAll(L_);
            RemoveCvTest<AbstractClassTestType*>::testAll(L_);
            RemoveCvTest<Incomplete           *>::testAll(L_);
            RemoveCvTest<Uncomplete           *>::testAll(L_);

            RemoveCvTest<FunctionPtrTestType  >::testAll(L_);
            RemoveCvTest<FunctionPtrTestType *>::testAll(L_);
        }

        if (veryVerbose) puts("\tPointer-to-member types");
        {
            RemoveCvTest<MethodPtrTestType    >::testAll(L_);
            RemoveCvTest<MethodPtrTestType   *>::testAll(L_);

            RemoveCvTest<PMD                        >::testAll(L_);
            RemoveCvTest<PMD                       *>::testAll(L_);
            RemoveCvTest<int StructTestType::*      >::testAll(L_);
            RemoveCvTest<int StructTestType::* *    >::testAll(L_);
            RemoveCvTest<int StructTestType::* * *  >::testAll(L_);
            RemoveCvTest<const int StructTestType::*>::testAll(L_);
            RemoveCvTest<PMD BaseClassTestType::*   >::testAll(L_);
            RemoveCvTest<PMD BaseClassTestType::* * >::testAll(L_);
        }

        // C-1..5

        if (veryVerbose) puts("\tArray types");
        {
            typedef int Array1D[5];
            typedef int Array2D[13][42];
            typedef int Array3D[5][2][3];
            typedef int ArrayUnknownBound1D[];
            typedef int ArrayUnknownBound2D[][8];

            typedef int            *PtrArray1D[3];
            typedef StructTestType  ClassArray1D[2];
            typedef PMD             PMDArrayUnknownBound1D[];

            RemoveCvTest<Array1D               >::testAll(L_);
            RemoveCvTest<Array2D               >::testAll(L_);
            RemoveCvTest<Array3D               >::testAll(L_);
            RemoveCvTest<ArrayUnknownBound1D   >::testAll(L_);
            RemoveCvTest<ArrayUnknownBound2D   >::testAll(L_);
            RemoveCvTest<PtrArray1D            >::testAll(L_);
            RemoveCvTest<ClassArray1D          >::testAll(L_);
            RemoveCvTest<PMDArrayUnknownBound1D>::testAll(L_);

            RemoveCvTest<Array1D            *>::testAll(L_);
            RemoveCvTest<ArrayUnknownBound1D*>::testAll(L_);
            RemoveCvTest<PtrArray1D         *>::testAll(L_);
        }

        // C-1, 2, 4..6

        if (veryVerbose) puts("\tFunction types and their abominable forms");
        {
            TEST_FUNCTION_FORMS(void()                          );
            TEST_FUNCTION_FORMS(int(int)                        );
            TEST_FUNCTION_FORMS(int&(int, char *, const double&));
            TEST_FUNCTION_FORMS(int(...)                        );
            TEST_FUNCTION_FORMS(const char *(const char *, ...) );

            RemoveCvTest<CLinkageFunctionTestType>::testAll(L_);
        }

        // C-9

        if (veryVerbose) puts("\tNon-dependent template arguments");
        {
            typedef int  AbomC() const;
            typedef void (StructTestType::*MethodPtr)();
            typedef AbomC StructTestType::*AbomMethodPtr;

            TEST_NON_DEPENDENT(void,                    void                );
            TEST_NON_DEPENDENT(const void,              void                );
            TEST_NON_DEPENDENT(volatile void,           void                );
            TEST_NON_DEPENDENT(const volatile void,     void                );
            TEST_NON_DEPENDENT(int,                     int                 );
            TEST_NON_DEPENDENT(const int,               int                 );
            TEST_NON_DEPENDENT(volatile int,            int                 );
            TEST_NON_DEPENDENT(const volatile int,      int                 );
            TEST_NON_DEPENDENT(const EnumTestType,      EnumTestType        );
            TEST_NON_DEPENDENT(volatile StructTestType, StructTestType      );
            TEST_NON_DEPENDENT(const Incomplete,        Incomplete          );
            TEST_NON_DEPENDENT(const int *,             const int *         );
            TEST_NON_DEPENDENT(int * const,             int *               );
            TEST_NON_DEPENDENT(const int * volatile,    const int *         );
            TEST_NON_DEPENDENT(PMD const,               PMD                 );
            TEST_NON_DEPENDENT(const int StructTestType::*,
                               const int StructTestType::*                  );
            TEST_NON_DEPENDENT(MethodPtr volatile,      MethodPtr           );
            TEST_NON_DEPENDENT(AbomMethodPtr const,     AbomMethodPtr       );
            TEST_NON_DEPENDENT(const int[5],            int[5]              );
            TEST_NON_DEPENDENT(volatile int[5][2],      int[5][2]           );
            TEST_NON_DEPENDENT(const volatile int[],    int[]               );
            TEST_NON_DEPENDENT(const int (*)[5],        const int (*)[5]    );
            TEST_NON_DEPENDENT(void(),                  void()              );
            TEST_NON_DEPENDENT(void (* const)(),        void (*)()          );
            TEST_NON_DEPENDENT(AbomC,                   AbomC               );
            TEST_NON_DEPENDENT(const int &,             const int &         );
            TEST_NON_DEPENDENT(void (&)(),              void (&)()          );
#if BSLS_COMPILERFEATURES_FULL_CPP11
            TEST_NON_DEPENDENT(const int &&,            const int &&        );
            TEST_NON_DEPENDENT(void (&&)(),             void (&&)()         );
            TEST_NON_DEPENDENT(int() &,                 int() &             );
            TEST_NON_DEPENDENT(int() const &&,          int() const &&      );
#endif
#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT_IN_FNC_TYPE)
            TEST_NON_DEPENDENT(int() noexcept,          int() noexcept      );
            TEST_NON_DEPENDENT(int() const noexcept,    int() const noexcept);
            TEST_NON_DEPENDENT(int() && noexcept,       int() && noexcept   );
#endif
        }

        // C-8

        if (veryVerbose) puts("\tClass template not to be instantiated");
        {
            RemoveCvTest<PoisonTestType<int> >::testAll(L_);
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TEST MACHINERY
        //   Ensure that the test machinery defined outside `main` behaves as
        //   documented, so that it can be relied upon in subsequent test
        //   cases.
        //
        // Concerns:
        // 1. `BUG_COMPILER_FORMS_CONST_REFERENCES` is defined if and only if
        //    applying a `const`-qualifier to a dependent reference type forms
        //    a type other than that reference type.
        //
        // 2. `CvForms<t_TYPE>` provides, as `C`, `V`, and `CV`, the `const`,
        //    `volatile`, and `const volatile` forms of an object type
        //    `t_TYPE`, retaining any qualifier of `t_TYPE`.
        //
        // 3. `CvForms<t_TYPE>` provides `t_TYPE` as each form of a reference
        //    type `t_TYPE`, whether or not the referenced type is
        //    cv-qualified.
        //
        // 4. `ApplyMetafunction<t_METAFUNCTION, t_TYPE>::type` is
        //    `t_METAFUNCTION<t_TYPE>::type`.
        //
        // 5. `PoisonTestType<t_TYPE>` can be named, and used as a template
        //    argument of the test machinery, without being instantiated.
        //
        // Plan:
        // 1. Verify that `ShowBadConst<const int>::type` is `const int`, and
        //    that `ShowBadConst<int &>::type` is `int &` if and only if
        //    `BUG_COMPILER_FORMS_CONST_REFERENCES` is not defined.  (C-1)
        //
        // 2. Verify each form provided by `CvForms` for `int` and for
        //    `const int`.  (C-2)
        //
        // 3. Verify that each form provided by `CvForms` for `int &` and for
        //    `const int &` is that reference type.  (C-3)
        //
        // 4. Verify that `ApplyMetafunction<bsl::type_identity, TYPE>::type`
        //    is `TYPE` for `int`, `const int`, and `PoisonTestType<int>`.
        //    (C-4..5)
        //
        // Note that `RemoveCvTest` and the `TEST_*` macros defined outside
        // `main` verify only results of `bsl::remove_cv`, which are the
        // subject of case 3, for types formed using `CvForms` and
        // `bsl::type_identity`, so are not verified separately.
        //
        // Testing:
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) puts("\nTEST MACHINERY"
                          "\n==============");

        // C-1

        if (veryVerbose) puts("\tBUG_COMPILER_FORMS_CONST_REFERENCES");
        {
#if defined(BUG_COMPILER_FORMS_CONST_REFERENCES)
            const bool EXP = false;
#else
            const bool EXP = true;
#endif

            ASSERT((bsl::is_same<ShowBadConst<const int>::type,
                                 const int>::value));
            ASSERT(EXP == (bsl::is_same<ShowBadConst<int &>::type,
                                        int &>::value));
        }

        // C-2

        if (veryVerbose) puts("\tCvForms of object types");
        {
            ASSERT((bsl::is_same<CvForms<int>::C,  const int>::value));
            ASSERT((bsl::is_same<CvForms<int>::V,  volatile int>::value));
            ASSERT((bsl::is_same<CvForms<int>::CV,
                                 const volatile int>::value));

            ASSERT((bsl::is_same<CvForms<const int>::C,
                                 const int>::value));
            ASSERT((bsl::is_same<CvForms<const int>::V,
                                 const volatile int>::value));
            ASSERT((bsl::is_same<CvForms<const int>::CV,
                                 const volatile int>::value));
        }

        // C-3

        if (veryVerbose) puts("\tCvForms of reference types");
        {
            ASSERT((bsl::is_same<CvForms<int &>::C,  int &>::value));
            ASSERT((bsl::is_same<CvForms<int &>::V,  int &>::value));
            ASSERT((bsl::is_same<CvForms<int &>::CV, int &>::value));

            ASSERT((bsl::is_same<CvForms<const int &>::C,
                                 const int &>::value));
            ASSERT((bsl::is_same<CvForms<const int &>::V,
                                 const int &>::value));
            ASSERT((bsl::is_same<CvForms<const int &>::CV,
                                 const int &>::value));
        }

        // C-4..5

        if (veryVerbose) puts("\tApplyMetafunction");
        {
            typedef PoisonTestType<int> Poison;

            ASSERT((bsl::is_same<
                           ApplyMetafunction<bsl::type_identity, int>::type,
                           int>::value));
            ASSERT((bsl::is_same<
                           ApplyMetafunction<bsl::type_identity,
                                             const int>::type,
                           const int>::value));
            ASSERT((bsl::is_same<
                           ApplyMetafunction<bsl::type_identity, Poison>::type,
                           Poison>::value));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Directly, without any test machinery, verify that
        //    `bsl::remove_cv`, and (in C++11 and later) `bsl::remove_cv_t`,
        //    yield `float`, a scalar type not otherwise tested, for `float`
        //    and for each of its cv-qualified forms.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) puts("\nBREATHING TEST"
                          "\n==============");

        ASSERT((bsl::is_same<bsl::remove_cv<               float>::type,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv<const          float>::type,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv<      volatile float>::type,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv<const volatile float>::type,
                             float>::value));

#if BSLS_COMPILERFEATURES_FULL_CPP11
        ASSERT((bsl::is_same<bsl::remove_cv_t<               float>,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv_t<const          float>,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv_t<      volatile float>,
                             float>::value));
        ASSERT((bsl::is_same<bsl::remove_cv_t<const volatile float>,
                             float>::value));
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
