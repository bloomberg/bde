// bslstl_function_smallobjectoptimization.t.cpp                      -*-C++-*-

#include <bslstl_function_smallobjectoptimization.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bslstl_referencewrapper.h>

#include <limits.h> // 'INT_MAX'
#include <stddef.h> // 'size_t'
#include <stdint.h> // 'uintptr_t'
#include <stdio.h>  // 'printf', 'FILE', etc.
#include <stdlib.h> // 'atoi'
#include <string.h> // 'strncpy'

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
// This subordinate component provides a namespace for a suite of utility
// types, type traits, and constants used in the implementation of
// 'bslstl_function'.  This test driver verifies each utility type, type trait,
// and constant independently in its own test case.
//
// Throughout this test plan, the type 'Obj' refers to
// 'bslstl::Function_SmallObjectOptimization'.
// ----------------------------------------------------------------------------
// TYPES
// [ 3] Obj::InplaceBuffer
// [ 5] Obj::SooFuncSize
// [ 6] Obj::IsInplaceFunc
//
// CLASS DATA
// [ 4] Obj::k_NON_SOO_SMALL_SIZE
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] TEST MACHINERY
// ----------------------------------------------------------------------------

// ============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {
int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define ASSERT_EQ(X,Y) ASSERTV((X),(Y),(X) == (Y))
#define ASSERT_NE(X,Y) ASSERTV((X),(Y),(X) != (Y))
#define ASSERT_GT(X,Y) ASSERTV((X),(Y),(X) >  (Y))
#define ASSERT_GE(X,Y) ASSERTV((X),(Y),(X) >= (Y))
#define ASSERT_LT(X,Y) ASSERTV((X),(Y),(X) <  (Y))
#define ASSERT_LE(X,Y) ASSERTV((X),(Y),(X) <= (Y))

#define LOOP_ASSERT_EQ(L,X,Y) ASSERTV((L),(X),(Y),(X) == (Y))
#define LOOP_ASSERT_NE(L,X,Y) ASSERTV((L),(X),(Y),(X) != (Y))
#define LOOP_ASSERT_GT(L,X,Y) ASSERTV((L),(X),(Y),(X) >  (Y))
#define LOOP_ASSERT_GE(L,X,Y) ASSERTV((L),(X),(Y),(X) >= (Y))
#define LOOP_ASSERT_LT(L,X,Y) ASSERTV((L),(X),(Y),(X) <  (Y))
#define LOOP_ASSERT_LE(L,X,Y) ASSERTV((L),(X),(Y),(X) <= (Y))

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number


// ============================================================================
//                            MACROS FOR TESTING
// ----------------------------------------------------------------------------

#ifndef SIZE_MAX
# define SIZE_MAX static_cast<size_t>(-1)
#endif

// ============================================================================
//                           ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1800
#define MSVC_2013 1
#else
#define MSVC_2013 0
#endif

namespace {
namespace u {
using namespace BloombergLP;

                            // ====================
                            // class IncompleteType
                            // ====================

class IncompleteType;
    // 'IncompleteType' provides a declaration an incomplete class type, which
    // this test driver uses to construct other types for the purposes of
    // testing.

#if MSVC_2013
    // However MSVC 2013 requires 'IncompleteType' to be complete when
    // calculating the size of 'bsl::reference_wrapper<IncompleteType>' below.

class IncompleteType {
};

#endif

                               // =============
                               // class TypeTag
                               // =============

template <class TYPE>
class TypeTag {
    // This class provides a default-constructible and copyable type that other
    // functions in this test driver use for the purpose of function template
    // argument deduction.

  public:
    // CREATORS
    TypeTag();
        // Create a 'TypeTag<TYPE>' object.
};

                         // =========================
                         // class SmallObjectTestType
                         // =========================

template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
class SmallObjectTestType {
    // This class template provides a test type that has the specified 'SIZE',
    // for which the 'value' of 'bsl::is_nothrow_move_constructible' is equal
    // to the value of the specified 'NOTHROW_MOVE_CONSTRUCTIBLE' parameter,
    // and the 'value' of 'bslmf::IsBitwiseMoveable' is equal to the specified
    // 'BITWISE_MOVEABLE' parameter.  The behavior is undefined unless
    // 'SIZE != 0', and 'BITWISE_MOVEABLE == true' if 'SIZE == 1'.

    BSLMF_ASSERT(SIZE > 0);
    BSLMF_ASSERT(SIZE > 1 || BITWISE_MOVEABLE);

    // DATA
    unsigned char d_data[SIZE];

  public:
    // TRAITS
#if ! defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
    BSLMF_NESTED_TRAIT_DECLARATION_IF(SmallObjectTestType,
                                      bsl::is_nothrow_move_constructible,
                                      NOTHROW_MOVE_CONSTRUCTIBLE);
#endif

    BSLMF_NESTED_TRAIT_DECLARATION_IF(SmallObjectTestType,
                                      bslmf::IsBitwiseMoveable,
                                      BITWISE_MOVEABLE);

    // CREATORS
    SmallObjectTestType();
        // Default constructor.

    SmallObjectTestType(const SmallObjectTestType&);
        // Copy constructor (would be suppressed by move constructor if not
        // present).

    SmallObjectTestType(bslmf::MovableRef<SmallObjectTestType>)
            BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(NOTHROW_MOVE_CONSTRUCTIBLE);
        // Move constructor that is declared non-throwing if
        // 'NOTHROW_MOVE_CONSTRUCTIBLE' is true.
};

                       // ==============================
                       // struct SmallObjectTestTypeUtil
                       // ==============================

struct SmallObjectTestTypeUtil {
    // This utility 'struct' provides a 'static' member function for obtaining
    // tag types for 'SmallObjectTestType' specializations, which this test
    // driver uses for function template argument deduction.

    // CLASS METHODS
    template <size_t SIZE,
              bool   NOTHROW_MOVE_CONSTRUCTIBLE,
              bool   BITWISE_MOVEABLE>
    static TypeTag<SmallObjectTestType<SIZE,
                                       NOTHROW_MOVE_CONSTRUCTIBLE,
                                       BITWISE_MOVEABLE> >
    tag();
        // Return 'TypeTag<T>()', where 'T' is 'SmallObjectTestType<S, N, B>',
        // 'S' is the specified 'SIZE' parameter, 'N' is the specified
        // 'NOTHROW_MOVE_CONSTRUCTIBLE' parameter, and 'B' is the specified
        // 'BITWISE_MOVEABLE' parameter.
};

                        // ===========================
                        // struct ReferenceWrapperUtil
                        // ===========================

struct ReferenceWrapperUtil {
    // This utility 'struct' provides a 'static' member function for obtaining
    // tag types for 'bsl::reference_wrapper' specializations, which this test
    // driver uses for function template argument deduction.

    // CLASS METHODS
    template <class REFERENCED_TYPE>
    static TypeTag<bsl::reference_wrapper<REFERENCED_TYPE> > tag();
        // Return 'TypeTag<T>()', where 'T' is 'bsl::reference_wrapper<R>',
        // and 'R' is the specified 'REFERENCED_TYPE' parameter.
};

                   // ======================================
                   // struct SmallObjectReferenceWrapperUtil
                   // ======================================

struct SmallObjectReferenceWrapperUtil {
    // This utility 'struct' provides a 'static' member function for obtaining
    // tag types for 'bsl::reference_wrapper' specializations with a 'T'
    // parameter of a 'SmallObjectTestType' specialization, which this test
    // driver uses for function template argument deduction.

    // CLASS METHODS
    template <size_t SIZE,
              bool   NOTHROW_MOVE_CONSTRUCTIBLE,
              bool   BITWISE_MOVEABLE>
    static TypeTag<
        bsl::reference_wrapper<SmallObjectTestType<SIZE,
                                                   NOTHROW_MOVE_CONSTRUCTIBLE,
                                                   BITWISE_MOVEABLE> > >
    tag();
        // Return 'TypeTag<bsl::reference_wrapper<T> >()', where 'T' is
        // 'SmallObjectTestType<S, N, B>', 'S' is the specified 'SIZE'
        // parameter, 'N' is the specified 'NOTHROW_MOVE_CONSTRUCTIBLE'
        // parameter, and 'B' is the specified 'BITWISE_MOVEABLE' parameter.
};

                       // =============================
                       // class SmallObjectTestTypeTest
                       // =============================

class SmallObjectTestTypeTest {
    // This class provides a function object that verifies
    // 'SmallObjectTestType' satisfies its contract.

  public:
    // CREATORS
    SmallObjectTestTypeTest();
        // Create a 'SmallObjectTestTypeTest' object.

    // ACCESSORS
    template <size_t SIZE,
              bool   NOTHROW_MOVE_CONSTRUCTIBLE,
              bool   BITWISE_MOVEABLE>
    void operator()(
        int line,
        SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE>
               obj,
        size_t expectedSize,
        bool   expectedNothrowMoveConstructability,
        bool   expectedBitwiseMovability) const;
        // Verify that the size of the specified 'T' is 'expectedSize', that
        // the 'value' of 'bsl::is_nothrow_move_constructible<T>' is
        // 'expectedNothrowMoveConstructibility', and the 'value' of
        // 'bslmf::IsBitwiseMoveable<T>' is 'expectedBitwiseMoveability', where
        // 'T' is the type of 'obj', and to report an error with the specified
        // 'line' number otherwise.
};

                          // =======================
                          // class IsInplaceFuncTest
                          // =======================

class IsInplaceFuncTest {
    // This class provides a function object that verifies 'Obj::IsInplaceFunc'
    // satisfies its contract, where 'Obj' is
    // '::BloombergLP::bslstl::Function_SmallObjectOptimization'.

  public:
    // CREATORS
    IsInplaceFuncTest();
        // Create a 'IsInplaceFuncTest' object.

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator()(int                  line,
                    TypeTag<OBJECT_TYPE>,
                    bool                 expectedIsInplaceFuncValue) const;
        // Verify that the 'value' of 'Obj::IsInplaceFunc<OBJECT_TYPE>' is
        // 'expectedIsInplaceFuncValue', and report an error with the specified
        // 'line' number otherwise.
};

                           // =====================
                           // class SooFuncSizeTest
                           // =====================

struct SooFuncSizeTest {
    // This class provides a function object that verifies 'Obj::SooFuncSize'
    // satisfies its contract, where 'Obj' is
    // '::BloombergLP::bslstl::Function_SmallObjectOptimization'.

    // CREATORS
    SooFuncSizeTest();
        // Create a 'SooFuncSizeTest' object.

    // ACCESSORS
    template <class OBJECT_TYPE>
    void operator()(int                  line,
                    TypeTag<OBJECT_TYPE>,
                    size_t               expectedSooFuncSize) const;
        // Verify that the 'value' of 'Obj::SooFuncSize<OBJECT_TYPE>' is
        // 'expectedSooFuncSize', and report an error with the specified 'line'
        // number otherwise.
};

// ============================================================================
//                      ENTITY DEFINITIONS FOR TESTING
// ----------------------------------------------------------------------------

                               // -------------
                               // class TypeTag
                               // -------------

// CREATORS
template <class TYPE>
TypeTag<TYPE>::TypeTag()
{
}

                         // -------------------------
                         // class SmallObjectTestType
                         // -------------------------

// CREATORS
template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE>::
    SmallObjectTestType()
{
}

template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE>::
    SmallObjectTestType(const SmallObjectTestType&)
{
}

template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE>::
    SmallObjectTestType(bslmf::MovableRef<SmallObjectTestType>)
        BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(NOTHROW_MOVE_CONSTRUCTIBLE)
{
}

                       // ------------------------------
                       // struct SmallObjectTestTypeUtil
                       // ------------------------------

// CLASS METHODS
template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
TypeTag<
    SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE> >
SmallObjectTestTypeUtil::tag()
{
    typedef SmallObjectTestType<SIZE,
                                NOTHROW_MOVE_CONSTRUCTIBLE,
                                BITWISE_MOVEABLE>
        Type;

    return TypeTag<Type>();
}


                        // ---------------------------
                        // struct ReferenceWrapperUtil
                        // ---------------------------

// CLASS METHODS
template <class REFERENCED_TYPE>
TypeTag<bsl::reference_wrapper<REFERENCED_TYPE> > ReferenceWrapperUtil::tag()
{
    typedef bsl::reference_wrapper<REFERENCED_TYPE> Type;

    return TypeTag<Type>();
}

                   // --------------------------------------
                   // struct SmallObjectReferenceWrapperUtil
                   // --------------------------------------

// CLASS METHODS
template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
TypeTag<bsl::reference_wrapper<
    SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE> > >
SmallObjectReferenceWrapperUtil::tag()
{
    typedef SmallObjectTestType<SIZE,
                                NOTHROW_MOVE_CONSTRUCTIBLE,
                                BITWISE_MOVEABLE>
                                                   ReferencedType;
    typedef bsl::reference_wrapper<ReferencedType> Type;

    return TypeTag<Type>();
}

                       // -----------------------------
                       // class SmallObjectTestTypeTest
                       // -----------------------------

// CREATORS
SmallObjectTestTypeTest::SmallObjectTestTypeTest()
{
}

// ACCESSORS
template <size_t SIZE, bool NOTHROW_MOVE_CONSTRUCTIBLE, bool BITWISE_MOVEABLE>
void SmallObjectTestTypeTest::operator()(
    int line,
    SmallObjectTestType<SIZE, NOTHROW_MOVE_CONSTRUCTIBLE, BITWISE_MOVEABLE>,
    size_t expectedSize,
    bool   expectedNothrowMoveConstructability,
    bool   expectedBitwiseMovability) const
{
    typedef SmallObjectTestType<SIZE,
                                NOTHROW_MOVE_CONSTRUCTIBLE,
                                BITWISE_MOVEABLE>
        Type;

    const size_t ACTUAL_SIZE = sizeof(Type);
    const bool   ACTUAL_NOTHROW_MOVE_CONSTRUCTABILITY =
        bsl::is_nothrow_move_constructible<Type>::value;
    const bool ACTUAL_BITWISE_MOVABILITY =
        bslmf::IsBitwiseMoveable<Type>::value;

    LOOP_ASSERT_EQ(line, ACTUAL_SIZE, expectedSize);
    LOOP_ASSERT_EQ(line,
                   ACTUAL_NOTHROW_MOVE_CONSTRUCTABILITY,
                   expectedNothrowMoveConstructability);
    LOOP_ASSERT_EQ(line, ACTUAL_BITWISE_MOVABILITY, expectedBitwiseMovability);
}

                          // -----------------------
                          // class IsInplaceFuncTest
                          // -----------------------

// CREATORS
IsInplaceFuncTest::IsInplaceFuncTest()
{}

// ACCESSORS
template <class OBJECT_TYPE>
void IsInplaceFuncTest::operator()(
                         int                  line,
                         TypeTag<OBJECT_TYPE>,
                         bool                 expectedIsInplaceFuncValue) const
{
    typedef bslstl::Function_SmallObjectOptimization Obj;

    LOOP_ASSERT_EQ(line,
                   Obj::IsInplaceFunc<OBJECT_TYPE>::value,
                   expectedIsInplaceFuncValue);
}

                           // ---------------------
                           // class SooFuncSizeTest
                           // ---------------------

// CREATORS
SooFuncSizeTest::SooFuncSizeTest()
{
}

// ACCESSORS
template <class OBJECT_TYPE>
void SooFuncSizeTest::operator()(
                                int                  line,
                                TypeTag<OBJECT_TYPE>,
                                size_t               expectedSooFuncSize) const
{
    typedef bslstl::Function_SmallObjectOptimization Obj;

    LOOP_ASSERT_EQ(line,
                   Obj::SooFuncSize<OBJECT_TYPE>::value,
                   expectedSooFuncSize);
}

}  // close namespace u
}  // close unnamed namesapce

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test                = argc > 1 ? atoi(argv[1]) : 0;
    const int verbose             = argc > 2;
    const int veryVerbose         = argc > 3;
    const int veryVeryVerbose     = argc > 4;
    const int veryVeryVeryVerbose = argc > 5;

    static_cast<void>(verbose);
    static_cast<void>(veryVerbose);
    static_cast<void>(veryVeryVerbose);
    static_cast<void>(veryVeryVeryVerbose);

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'IsInplaceFunc'
        //   This case verifies that 'IsInplaceFunc' is 'true' for all functor
        //   types that are small enough to fit in an 'InplaceBuffer', and are
        //   either nothrow-move constructible or bit-wise moveable.
        //
        // Concerns:
        //: 1 'IsInplaceFunc' is 'true' for 'bsl::nullptr_t', function pointer,
        //:   member-function pointer, and member-data pointer types.
        //:
        //: 2 'IsInplaceFunc' is 'true' for all functor types that are small
        //:   enough to fit in an 'InplaceBuffer', and are either nothrow-move
        //:   constructible or bit-wise moveable.
        //:
        //: 3 'IsInplaceFunc' is 'false' for all functor types that are too big
        //:   to fit in an 'InplaceBuffer', or are neither nothrow-move
        //:   constructible nor bit-wise moveable.
        //:
        //: 4 'IsInplaceFunc' for any 'bsl::reference_wrapper' specialization
        //:   is always 'true'.
        //
        // Plan:
        //: 1 Verify that 'IsInplaceFunc' is 'true' for 'bsl::nullptr_t', as
        //:   well as one function-pointer type, one member-function-pointer
        //:   type, and one data-member-pointer type.
        //:
        //: 2 For each type 'T' in a set of types that provide a large number
        //:   of permutations of size, nothrow-move-constructibility, and
        //:   bitwise-moveability, verify that 'IsInplaceFunc<T>' satisfies
        //:   concerns 2 and 3.
        //:
        //: 3 For several specializations of 'bsl::reference_wrapper', 'R',
        //:   verify that 'IsInplaceFunc<R>' satisfies concern 4.
        //
        // Testing:
        //   Obj::IsInplaceFunc
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING 'IsInplaceFunc'"
                   "\n=======================\n");
        }

        const u::IsInplaceFuncTest TEST;

        typedef u::SmallObjectTestTypeUtil         SOTTU;
        typedef u::SmallObjectReferenceWrapperUtil SORWU;

        typedef void (*FuncPtr)();
        typedef void (u::IncompleteType:: *MemFuncPtr)();
        typedef int u::IncompleteType:: *MemDataPtr;

        static const bool T = true;
        static const bool F = false;

        typedef bslstl::Function_SmallObjectOptimization Obj;
        static const size_t SOO_SIZE = sizeof(Obj::InplaceBuffer);

        //   LINE                    TYPE           'IsInplaceFunc<TYPE>'
        //   ----             ------------------    ---------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        TEST(L_ , u::TypeTag<bsl::nullptr_t    >(), T                   );
#endif
        TEST(L_ , u::TypeTag<FuncPtr           >(), T                   );
        TEST(L_ , u::TypeTag<MemFuncPtr        >(), T                   );
        TEST(L_ , u::TypeTag<MemDataPtr        >(), T                   );

        //                   NOTHROW MOVE       BITWISE MOVE
        //                   ------------.     .------------
        //                       SIZE     \   /
        //                   ------------ -- --
        TEST(L_ , SOTTU::tag<           1, F, T>(), T                   );
        TEST(L_ , SOTTU::tag<           1, T, T>(), T                   );
        TEST(L_ , SOTTU::tag<           2, F, F>(), F                   );
        TEST(L_ , SOTTU::tag<           2, F, T>(), T                   );
        TEST(L_ , SOTTU::tag<           2, T, F>(), T                   );
        TEST(L_ , SOTTU::tag<           2, T, T>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE - 1, F, F>(), F                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE - 1, F, T>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE - 1, T, F>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE - 1, T, T>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE    , F, F>(), F                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE    , F, T>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE    , T, F>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE    , T, T>(), T                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE + 1, F, F>(), F                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE + 1, F, T>(), F                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE + 1, T, F>(), F                   );
        TEST(L_ , SOTTU::tag<SOO_SIZE + 1, T, T>(), F                   );

        TEST(L_ , SORWU::tag<           1, F, T>(), T                   );
        TEST(L_ , SORWU::tag<           1, T, T>(), T                   );
        TEST(L_ , SORWU::tag<           2, F, F>(), T                   );
        TEST(L_ , SORWU::tag<           2, F, T>(), T                   );
        TEST(L_ , SORWU::tag<           2, T, F>(), T                   );
        TEST(L_ , SORWU::tag<           2, T, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE - 1, F, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE - 1, F, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE - 1, T, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE - 1, T, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE    , F, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE    , F, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE    , T, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE    , T, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE + 1, F, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE + 1, F, T>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE + 1, T, F>(), T                   );
        TEST(L_ , SORWU::tag<SOO_SIZE + 1, T, T>(), T                   );
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'SooFuncSize'
        //   This case verifies that 'SooFuncSize' is equal to the size of its
        //   parameter 'T' if 'T' is small enough to fit in an 'InplaceBuffer',
        //   and is either nothrow-move-constructible or bit-wise movable, and
        //   is equal to 'Obj::k_NON_SOO_SMALL_SIZE + sizeof(T)' otherwise.
        //
        // Concerns:
        //: 1 'SooFuncSize' is 'sizeof(T)' when 'T' is 'bsl::nullptr_t', a
        //:   function-pointer type, a member-function-pointer type, or a
        //:   member-data-pointer type.
        //:
        //: 2 'SooFuncSize' is 'sizeof(T)' for all functor types 'T' that are
        //:   small enough to fit in an 'InplaceBuffer', and are either
        //:   nothrow-move constructible or bit-wise moveable.
        //:
        //: 3 'SooFuncSize' is 'Obj::k_NON_SOO_SMALL_SIZE + sizeof(T)' for all
        //:   functor types 'T' that are too big to fit in an 'InplaceBuffer',
        //:   or are neither nothrow-move constructible nor bit-wise moveable.
        //:
        //: 4 'SooFuncSize' is 'sizeof(void *)' for any
        //:   'bsl::reference_wrapper' specialization.
        //
        // Plan:
        //: 1 Verify that 'SooFuncSize' is 'sizeof(T)' when 'T' is
        //:   'bsl::nullptr_t', as well as one function-pointer type, one
        //:   member-function-pointer type, and one data-member-pointer type.
        //:
        //: 2 For each type 'T' in a set of types that provide a large number
        //:   of permutations of size, nothrow-move-constructibility, and
        //:   bitwise-moveability, verify that 'SooFuncSize' satisfies
        //:   concerns 2 and 3.
        //:
        //: 3 For several specializations of 'bsl::reference_wrapper', 'R',
        //:   verify that 'SooFuncSize<R>' satisfies concern 4.
        //
        // Testing:
        //   Obj::SooFuncSize
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING 'SooFuncSize'"
                   "\n=====================\n");
        }

        const u::SooFuncSizeTest   TEST;

        typedef u::SmallObjectTestTypeUtil         SOTTU;
        typedef u::SmallObjectReferenceWrapperUtil SORWU;

        typedef void (*FuncPtr)();
        typedef void (u::IncompleteType:: *MemFuncPtr)();
        typedef int u::IncompleteType:: *MemDataPtr;

        static const bool T = true;
        static const bool F = false;

        typedef bslstl::Function_SmallObjectOptimization Obj;
        static const size_t SOO_SZ = sizeof(Obj::InplaceBuffer);
        static const size_t MAX_SZ = SIZE_MAX;

        //   LINE                    TYPE           'SooFuncSize<TYPE>'
        //   ----             -----------------    ---------------------
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NULLPTR
        TEST(L_ , u::TypeTag<bsl::nullptr_t  >(), sizeof(bsl::nullptr_t));
#endif
        TEST(L_ , u::TypeTag<FuncPtr         >(), sizeof(FuncPtr)   );
        TEST(L_ , u::TypeTag<MemFuncPtr      >(), sizeof(MemFuncPtr));
        TEST(L_ , u::TypeTag<MemDataPtr      >(), sizeof(MemDataPtr));

        //                 NOTHROW MOVE       BITWISE MOVE
        //                 ------------.     .------------
        //                     SIZE     \   /
        //                   ---------- -- --
        TEST(L_ , SOTTU::tag<         1, F, T>(),                     1);
        TEST(L_ , SOTTU::tag<         1, T, T>(),                     1);
        TEST(L_ , SOTTU::tag<         2, F, F>(), (MAX_SZ - SOO_SZ) + 2);
        TEST(L_ , SOTTU::tag<         2, F, T>(),                     2);
        TEST(L_ , SOTTU::tag<         2, T, F>(),                     2);
        TEST(L_ , SOTTU::tag<         2, T, T>(),                     2);

        TEST(L_ , SOTTU::tag<SOO_SZ - 1, F, F>(),            MAX_SZ - 1);
        TEST(L_ , SOTTU::tag<SOO_SZ - 1, F, T>(),            SOO_SZ - 1);
        TEST(L_ , SOTTU::tag<SOO_SZ - 1, T, F>(),            SOO_SZ - 1);
        TEST(L_ , SOTTU::tag<SOO_SZ - 1, T, T>(),            SOO_SZ - 1);
        TEST(L_ , SOTTU::tag<SOO_SZ    , F, F>(),            MAX_SZ    );
        TEST(L_ , SOTTU::tag<SOO_SZ    , F, T>(),            SOO_SZ    );
        TEST(L_ , SOTTU::tag<SOO_SZ    , T, F>(),            SOO_SZ    );
        TEST(L_ , SOTTU::tag<SOO_SZ    , T, T>(),            SOO_SZ    );
        TEST(L_ , SOTTU::tag<SOO_SZ + 1, F, F>(),            SOO_SZ + 1);
        TEST(L_ , SOTTU::tag<SOO_SZ + 1, F, T>(),            SOO_SZ + 1);
        TEST(L_ , SOTTU::tag<SOO_SZ + 1, T, F>(),            SOO_SZ + 1);
        TEST(L_ , SOTTU::tag<SOO_SZ + 1, T, T>(),            SOO_SZ + 1);

        TEST(L_ , SORWU::tag<         1, F, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<         1, T, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<         2, F, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<         2, F, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<         2, T, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<         2, T, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ - 1, F, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ - 1, F, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ - 1, T, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ - 1, T, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ    , F, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ    , F, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ    , T, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ    , T, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ + 1, F, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ + 1, F, T>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ + 1, T, F>(),        sizeof(void *));
        TEST(L_ , SORWU::tag<SOO_SZ + 1, T, T>(),        sizeof(void *));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'k_NON_SOO_SMALL_SIZE'
        //   This case verifies that 'k_NON_SOO_SMALL_SIZE' is a positive
        //   integral constant that satisfies the criteria that
        //   'S + k_NON_SOO_SMALL_SIZE > sizeof(InplaceBuffer)' for all
        //   positive, integral object sizes 'S <= sizeof(InplaceBuffer)'.
        //
        // Concerns:
        //: 1 'k_NON_SOO_SMALL_SIZE' is an integral value greater than 0.
        //:
        //: 2 'k_NON_SOO_SMALL_SIZE' is equal to
        //:   'SIZE_MAX - sizeof(Obj::InplaceBuffer)'.
        //
        // Plan:
        //: 1 Directly verify both concerns.
        //
        // Testing:
        //   Obj::k_NON_SOO_SMALL_SIZE
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING 'k_NON_SOO_SMALL_SIZE'"
                   "\n==============================\n");
        }

        typedef bslstl::Function_SmallObjectOptimization Obj;

        ASSERT_LT(0, Obj::k_NON_SOO_SMALL_SIZE);
        ASSERT_EQ(SIZE_MAX - sizeof(Obj::InplaceBuffer),
                  Obj::k_NON_SOO_SMALL_SIZE);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'InplaceBuffer'
        //   This case verifies that 'InplaceBuffer' is large enough to hold
        //   common small-functor types, and that it has the maximum possible
        //   alignment for the platform.
        //
        // Concerns:
        //: 1 'InplaceBuffer' is no smaller than a pointer, function pointer,
        //:   member-function pointer, member-data pointer, or reference
        //:   wrapper.
        //:
        //: 2 'InplaceBuffer' is 6 pointers in size.
        //:
        //: 3 'InplaceBuffer' has the maximum possible alignment for the
        //:   platform.
        //
        // Plan:
        //: 1 Verify that the size of an 'InplaceBuffer' is greater than or
        //:   equal to the size of a void pointer, one function pointer type,
        //:   one member-function-pointer type, one member-data-pointer type,
        //:   and one reference-wrapper specialization.
        //:
        //: 2 Verify that the size of an 'InplaceBuffer' is equal to the size
        //:   of 6 void pointers.
        //:
        //: 3 Verify that 'InplaceBuffer' has the maximum alignment for the
        //:   platform using the value calculated by 'bsls::AlignmentFromType'.
        //
        // Testing:
        //   Obj::InplaceBuffer
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING 'InplaceBuffer'"
                   "\n=======================\n");
        }

        typedef bslstl::Function_SmallObjectOptimization::InplaceBuffer Obj;

        typedef void (*FuncPtr)();
        typedef void (u::IncompleteType:: *MemFuncPtr)();
        typedef int u::IncompleteType:: *MemDataPtr;
        typedef bsl::reference_wrapper<u::IncompleteType> ReferenceWrapper;

        ASSERT_GE(sizeof(Obj), sizeof(void *));
        ASSERT_GE(sizeof(Obj), sizeof(FuncPtr));
        ASSERT_GE(sizeof(Obj), sizeof(MemFuncPtr));
        ASSERT_GE(sizeof(Obj), sizeof(MemDataPtr));
        ASSERT_GE(sizeof(Obj), sizeof(ReferenceWrapper));

        ASSERT_EQ(sizeof(Obj), 6 * sizeof(void *));

        ASSERT_EQ(std::size_t(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT),
                  std::size_t(bsls::AlignmentFromType<Obj>::VALUE));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING TEST MACHINERY
        //   This case verifies that the entities defined for use in this test
        //   driver conform to their respective contracts.
        //
        // Concerns:
        //: 1 The type trait 'bsl::is_nothrow_move_constructible' has the
        //:   'value' 'true' for 'u::SmallObjectTestType' specializations with
        //:   a 'NOTHROW_MOVE_CONSTRUCTIBLE' parameter of 'true'.
        //:
        //: 2 The type trait 'bsl::is_nothrow_move_constructible' has the
        //:   'value' 'false' for 'u::SmallObjectTestType' specializations with
        //:   a 'NOTHROW_MOVE_CONSTRUCTIBLE' parameter of 'false'.
        //:
        //: 3 The type trait 'bslmf::IsBitwiseMoveable' has the 'value' 'true'
        //:   for 'u::SmallObjectTestType' specializations with a
        //:   'BITWISE_MOVEABLE' parameter of 'true' or a 'SIZE' parameter of
        //:   1.
        //:
        //: 4 The type trait 'bslmf::IsBitwiseMoveable' has the 'value' 'false'
        //:   for 'u::SmallObjectTestType' specializations with a
        //:   'BITWISE_MOVEABLE' parameter of 'false'.
        //:
        //: 5 The size of a 'u::SmallObjectTestType' specialization is equal to
        //:   its 'SIZE' parameter.
        //:
        //: 6 The numeric limits provided by 'SIZE_MAX' are accurate.
        //
        // Plan:
        //: 1 Instantiate a 'u::SmallObjectTestType' with template parameters
        //:   that enumerate several permutations of object size, nothrow-move
        //:   constructibility, and bitwise-movability, and verify that the
        //:   type does indeed have the corresponding traits.
        //:
        //: 2 Verify that the value of 'SIZE_MAX' is the maximum unsigned
        //:   32-bit number in 32-bit builds, and the maximum unsigned 64-bit
        //:   number in 64-bit builds.
        //
        // Testing:
        //   TEST MACHINERY
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nTESTING TEST MACHINERY"
                   "\n======================\n");
        }

        {
            if (veryVerbose) {
                printf("\nTESTING 'u::SmallObjectTestType'"
                       "\n--------------------------------\n");
            }

            const u::SmallObjectTestTypeTest TEST;

            static const bool   T   = true;
            static const bool   F   = false;
            static const size_t BIG = 256;

            //                      BITWISE MOVE
            //                      ------------.-------------.
            //                     NOTHROW MOVE  \             \.
            //                     ------------.--\----------.  \.
            //                            SIZE  \  \          \  \.
            //                            ----.--\--\------.   \  \.
            //   LINE        TEST TYPE         \  \  \      \   \  \.
            //   ---- ------------------------ -- -- --    ---- -- --
            TEST(L_  , u::SmallObjectTestType<  1, F, T>(),   1, F, T);
            TEST(L_  , u::SmallObjectTestType<  1, T, T>(),   1, T, T);

            TEST(L_  , u::SmallObjectTestType<  2, F, F>(),   2, F, F);
            TEST(L_  , u::SmallObjectTestType<  2, F, T>(),   2, F, T);
            TEST(L_  , u::SmallObjectTestType<  2, T, F>(),   2, T, F);
            TEST(L_  , u::SmallObjectTestType<  2, T, T>(),   2, T, T);
            TEST(L_  , u::SmallObjectTestType<BIG, F, F>(), BIG, F, F);
            TEST(L_  , u::SmallObjectTestType<BIG, F, T>(), BIG, F, T);
            TEST(L_  , u::SmallObjectTestType<BIG, T, F>(), BIG, T, F);
            TEST(L_  , u::SmallObjectTestType<BIG, T, T>(), BIG, T, T);
            TEST(L_  , u::SmallObjectTestType<BIG, F, F>(), BIG, F, F);
            TEST(L_  , u::SmallObjectTestType<BIG, F, T>(), BIG, F, T);
            TEST(L_  , u::SmallObjectTestType<BIG, T, F>(), BIG, T, F);
            TEST(L_  , u::SmallObjectTestType<BIG, T, T>(), BIG, T, T);
        }

        {
            if (veryVerbose) {
                printf("\nTESTING 'SIZE_MAX'"
                       "\n------------------\n");
            }

#if defined(BSLS_PLATFORM_CPU_32_BIT)
            ASSERT_EQ(0xFFFFFFFFU, SIZE_MAX);
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
            ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, SIZE_MAX);
#else
#error "'bslstl_function_smallobjectoptimization.t.cpp' does not support " \
       "this platform."
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 'Obj::InplaceBuffer' is an object type.
        //:
        //: 2 'Obj::k_NON_SOO_SMALL_SIZE' is a non-zero integral constant.
        //:
        //: 3 'Obj::SooFuncSize' is a class template with 1 parameter.
        //:
        //: 4 'Obj::IsInplaceFunc' is a class template with 1 parameter.
        //
        // Plan:
        //: 1 Create an object of 'Obj::InplaceBuffer' type.
        //:
        //: 2 Verify that 'Obj::k_NON_SOO_SMALL_SIZE' is greater than 0.
        //:
        //: 3 Verify that 'Obj::SooFuncSize<void *>' is instantiable.
        //:
        //: 4 Verify that 'Obj::IsInplaceFunc<void *>' is instantiable.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) {
            printf("\nBREATHING TEST"
                   "\n==============\n");
        }

        typedef bslstl::Function_SmallObjectOptimization Obj;

        Obj::InplaceBuffer inplaceBuffer;
        inplaceBuffer.d_object_p = 0;
        static_cast<void>(inplaceBuffer);

        ASSERT_LT(0             , Obj::k_NON_SOO_SMALL_SIZE        );
        ASSERT_EQ(sizeof(void *), Obj::SooFuncSize<void *>::value  );
        ASSERT_EQ(true          , Obj::IsInplaceFunc<void *>::value);

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
// Copyright 2020 Bloomberg Finance L.P.
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
