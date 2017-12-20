// bslmf_isbitwisemoveable.t.cpp                                      -*-C++-*-
#include <bslmf_isbitwisemoveable.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addvolatile.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_bsltestutil.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>   // 'printf'
#include <stdlib.h>  // 'atoi'
#include <string.h>
#include <new>

using namespace BloombergLP;
using namespace bsl;

// BDE_VERIFY pragma : -IND01

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// The component under test defines a meta-function,
// 'bslmf::IsBitwiseMoveable', that determines whether a template parameter
// type is bitwise movable.  By default, the meta-function supports a
// restricted set of type categories and can be extended to support other types
// through either template specialization or use of the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Thus, we need to ensure that the natively supported types are correctly
// identified by the meta-function by testing the meta-function with each of
// the supported type categories.  We also need to verify that the
// meta-function can be correctly extended to support other types through
// either of the two supported mechanisms.  Finally, we need to test correct
// support for cv-qualified and array types, where the underlying type may be
// bitwise movable.
//
// ----------------------------------------------------------------------------
// PUBLIC CLASS DATA
// [ 1] bslmf::IsBitwiseMoveable::value
// ----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE
// [ 2] EXTENDING 'bslmf::IsBitwiseMoveable'

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
//                  COMPONENT-SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

// Each of the macros below will test the 'bslmf::IsBitwiseMoveable' trait with
// a set of variations on a type.  There are several layers of macros, as
// object types support the full range of variation, but function types cannot
// form an array, nor be cv-qualified.  Similarly, 'void' may be cv-qualified
// but still cannot form an array.  As macros are strictly text-substitution we
// must use the appropriate 'add_decoration' traits to transform types in a
// manner that is guaranteed to be syntactically valid.  Note that these are
// not type-dependent contexts, so there is no need to use 'typename' when
// fetching the result from any of the queried traits.

#define ASSERT_IS_BITWISE_MOVEABLE_TYPE(TYPE, RESULT)                         \
    ASSERT( bslmf::IsBitwiseMoveable<TYPE>::value == RESULT);                 \
    ASSERT( bslmf::IsBitwiseMoveable<bsl::add_pointer<TYPE>::type>::value);   \
    ASSERT(!bslmf::IsBitwiseMoveable<                                         \
                                bsl::add_lvalue_reference<TYPE>::type>::value);

#define ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE, RESULT)                      \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE(TYPE, RESULT);                            \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE(bsl::add_const<TYPE>::type, RESULT);      \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE(bsl::add_volatile<TYPE>::type, RESULT);   \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE(bsl::add_cv<TYPE>::type, RESULT);

// Two additional macros will allow testing on old MSVC compilers when 'TYPE'
// is an array of unknown bound.

#define ASSERT_IS_BITWISE_MOVEABLE_TYPE_NO_REF(TYPE, RESULT)                  \
    ASSERT( bslmf::IsBitwiseMoveable<TYPE>::value == RESULT);                 \
    ASSERT( bslmf::IsBitwiseMoveable<bsl::add_pointer<TYPE>::type>::value);

#define ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE_NO_REF(TYPE, RESULT)              \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE_NO_REF(TYPE, RESULT);                    \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE_NO_REF(                                  \
                                        bsl::add_const<TYPE>::type, RESULT); \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE_NO_REF(                                  \
                                     bsl::add_volatile<TYPE>::type, RESULT); \
    ASSERT_IS_BITWISE_MOVEABLE_TYPE_NO_REF(bsl::add_cv<TYPE>::type, RESULT);


#if defined(BSLMF_ISBITWISEMOVEABLE_NO_SUPPORT_FOR_ARRAY_OF_UNKNOWN_BOUND)
// Run a reduced set of test scenarios on compilers that do not support arrays
// of unknown bound for template parameters.

# define ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(TYPE, RESULT)                 \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE, RESULT)                          \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[128], RESULT)                     \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[12][8], RESULT)

#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700
// Old microsoft compilers do not support references to arrays of unknown
// bound.

# define ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(TYPE, RESULT)                 \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE, RESULT)                          \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[128], RESULT)                     \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[12][8], RESULT)                   \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE_NO_REF(TYPE[], RESULT)                 \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE_NO_REF(TYPE[][8], RESULT)

#else
# define ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(TYPE, RESULT)                 \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE, RESULT)                          \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[128], RESULT)                     \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[12][8], RESULT)                   \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[], RESULT)                        \
    ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(TYPE[][8], RESULT)
#endif

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

struct UserDefinedBwmTestType {
    // This user-defined type, which is marked to be bitwise movable using
    // template specialization (below), is used for testing.
};

struct UserDefinedBwmTestType2 {
    // This user-defined type, which is marked to be bitwise movable using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedBwmTestType2,
                                   BloombergLP::bslmf::IsBitwiseMoveable);
};

struct UserDefinedOneByteTestType {
    // One-byte simple structs should typically be bitwise moveable.
    bool d_dummy;
};


struct UserDefinedNotTriviallyCopyableOneByteTestType {
    // One-byte, non-trivially copyable type (used to test the one-byte
    // bitwise-movable heuristic)

    bool d_dummy;

    UserDefinedNotTriviallyCopyableOneByteTestType(
        const UserDefinedNotTriviallyCopyableOneByteTestType& rhs)
    : d_dummy(!rhs.d_dummy)
    {
    }
};
#if defined(BSLMF_ISTRIVIALLYCOPYABLE_NATIVE_IMPLEMENTATION)
static_assert(!bsl::is_trivially_copyable<
                  UserDefinedNotTriviallyCopyableOneByteTestType>::value,
              "Incorrect test type declaration.");
#endif

struct UserDefinedTcTestType {
    // This user-defined type, which is marked to be trivially copyable using
    // template specialization (below), is used for testing.  Note that
    // trivially copyable types are bitwise movable by default.  Also note that
    // 'empty' or small types are implicitly bitwise movable, so we add a
    // trivial data member to make this type large enough to avoid the size
    // base implicitly bitwise-movable test.

    char d_dummy[4];
};

struct UserDefinedTcTestType2 {
    // This user-defined type, which is marked to be trivially copyable using
    // the 'BSLMF_NESTED_TRAIT_DECLARATION' macro, is used for testing.  Note
    // that trivially copyable types are bitwise movable by default.

    BSLMF_NESTED_TRAIT_DECLARATION(UserDefinedTcTestType2,
                                   bsl::is_trivially_copyable);
};

struct UserDefinedNonTcTestType {
    // This user-defined type, which is not marked to be trivially copyable, is
    // used for testing.  Note that trivially copyable types are bitwise
    // movable by default.  Also note that, as 'empty' or small types are
    // implicitly bitwise movable, we add a trivial data member to make this
    // type large enough to remain non-bitwise-movable.
    UserDefinedNonTcTestType() : d_dummy() {}
    UserDefinedNonTcTestType(const UserDefinedNonTcTestType& other)
    : d_dummy()
    {}

    char d_dummy[4];
};

enum EnumTestType {
    // This 'enum' type is used for testing.
};

typedef int (UserDefinedNonTcTestType::*MethodPtrTestType) ();
    // This pointer to non-static function member type is used for testing.

}  // close unnamed namespace


// Provide explicit traits specializations, ensuring that both means of
// associating a trait with a type are tested.

namespace BloombergLP {
namespace bslmf {

template <>
struct IsBitwiseMoveable<UserDefinedBwmTestType> : bsl::true_type {
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <>
struct is_trivially_copyable<UserDefinedTcTestType> : bsl::true_type {
};

}  // close namespace bsl

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// BDE_VERIFY pragma : push
// BDE_VERIFY pragma : -AL01   // Strict aliasing (mostly) false-positives
// BDE_VERIFY pragma : -CC01   // C-style casts make this example readable
// BDE_VERIFY pragma : -FD01   // Function contracts are descriptive text
// BDE_VERIFY pragma : -IEC01  // Some example types have implicit ctors
// BDE_VERIFY pragma : -NT01   // Used examples provide additional packages

// Used to test detection of legacy traits
#define BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT)            \
    operator TRAIT::NestedTraitDeclaration<T>() const {   \
        return TRAIT::NestedTraitDeclaration<T>();        \
    }

///Example 1: Using the trait to implement `destructiveMoveArray`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Here, we use this trait in a simple algorithm called 'destructiveMoveArray',
// which moves elements from one array to another.  The algorithm is
// implemented using two implementation functions, one for types that are known
// to be bitwise moveable, and one for other types.  The first takes an extra
// function argument of type 'true_type', the second takes and extra function
// argument of type 'false_type':
//..
    namespace BloombergLP {

    template <class TYPE>
    void destructiveMoveArrayImp(TYPE *to,
                                 TYPE *from,
                                 int   size,
                                 bsl::true_type)
    {
        // Bitwise moveable types can be moved using 'memcpy'.

        memcpy(to, from, size * sizeof(TYPE));
    }

    template <class TYPE>
    void destructiveMoveArrayImp(TYPE *to,
                                 TYPE *from,
                                 int   size,
                                 bsl::false_type)
    {
        for (int i = 0; i < size; ++i) {
            ::new(to + i) TYPE(from[i]);
            from[i].~TYPE();
        }
    }
//..
// Now we can dispatch between the two Imp functions, using the
// 'IsBitwiseMoveable' trait metafunction to determine at compile time which of
// the implementations should be used:
//..
    template <class TYPE>
    void destructiveMoveArray(TYPE *to, TYPE *from, int size)
    {
        destructiveMoveArrayImp(to, from, size,
                                bslmf::IsBitwiseMoveable<TYPE>());
    }
//..
// Next, to check our work, we create three classes that we will use to
// instantiate 'destructiveMoveArray'.  All of the classes will log the number
// of constructor and destructor calls.  The first class will not be decorated
// with the 'IsBitwiseMoveable' trait:
//..
    class NonMoveableClass
    {
      private:
        int d_value;

        static int d_ctorCount;
        static int d_dtorCount;

      public:
        static int ctorCount() { return d_ctorCount; }
        static int dtorCount() { return d_dtorCount; }

        NonMoveableClass(int val = 0) : d_value(val) { ++d_ctorCount; }
        NonMoveableClass(const NonMoveableClass& other)
            : d_value(other.d_value) { ++d_ctorCount; }
        ~NonMoveableClass() { d_dtorCount++; }

        int value() const { return d_value; }
    };

    int NonMoveableClass::d_ctorCount = 0;
    int NonMoveableClass::d_dtorCount = 0;
//..
// The second class is similar except that we declare it to be bit-wise
// moveable by specializing 'IsBitwiseMoveable':
//..
    class MoveableClass1
    {
      private:
        int d_value;

        static int d_ctorCount;
        static int d_dtorCount;

      public:
        static int ctorCount() { return d_ctorCount; }
        static int dtorCount() { return d_dtorCount; }

        MoveableClass1(int val = 0) : d_value(val) { ++d_ctorCount; }
        MoveableClass1(const MoveableClass1& other)
            : d_value(other.d_value) { ++d_ctorCount; }
        ~MoveableClass1() { d_dtorCount++; }

        int value() const { return d_value; }
    };

    int MoveableClass1::d_ctorCount = 0;
    int MoveableClass1::d_dtorCount = 0;

    namespace bslmf {
        template <> struct IsBitwiseMoveable<MoveableClass1> : bsl::true_type {
        };
    }  // close namespace bslmf
//..
// The third class is also declared to be bitwise moveable, but this time we do
// it using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
//..
    class MoveableClass2
    {
      private:
        int d_value;

        static int d_ctorCount;
        static int d_dtorCount;

      public:
        BSLMF_NESTED_TRAIT_DECLARATION(MoveableClass2,
                                       bslmf::IsBitwiseMoveable);

        static int ctorCount() { return d_ctorCount; }
        static int dtorCount() { return d_dtorCount; }

        MoveableClass2(int val = 0) : d_value(val) { ++d_ctorCount; }
        MoveableClass2(const MoveableClass2& other)
            : d_value(other.d_value) { ++d_ctorCount; }
        ~MoveableClass2() { d_dtorCount++; }

        int value() const { return d_value; }
    };

    int MoveableClass2::d_ctorCount = 0;
    int MoveableClass2::d_dtorCount = 0;
//..
// Finally, invoke 'destructiveMoveArray' on arrays of all three classes:
//..
    enum MoveableEnum { A_VALUE };

    int usageExample1()
    {
        using namespace bslmf;

        // First, check the basic operation of 'IsBitwiseMoveable':
        ASSERT(  IsBitwiseMoveable<int>::value);
        ASSERT(  IsBitwiseMoveable<int*>::value);
        ASSERT(  IsBitwiseMoveable<const int*>::value);
        ASSERT(  IsBitwiseMoveable<MoveableEnum>::value);
        ASSERT(! IsBitwiseMoveable<int&>::value);
        ASSERT(! IsBitwiseMoveable<const int&>::value);
        ASSERT(  IsBitwiseMoveable<MoveableClass1>::value);
        ASSERT(  IsBitwiseMoveable<const MoveableClass1>::value);
        ASSERT(  IsBitwiseMoveable<MoveableClass2>::value);
        ASSERT(  IsBitwiseMoveable<volatile MoveableClass2>::value);
        ASSERT(! IsBitwiseMoveable<NonMoveableClass>::value);
        ASSERT(! IsBitwiseMoveable<const NonMoveableClass>::value);

        // For each of our test classes, allocate an array, construct three
        // objects into it, then move it into another array.
        const int nObj = 3;

        {
            NonMoveableClass *p1 = (NonMoveableClass*)
                ::operator new(nObj * sizeof(NonMoveableClass));
            NonMoveableClass *p2 =  (NonMoveableClass*)
                ::operator new(nObj * sizeof(NonMoveableClass));

            for (int i = 0; i < nObj; ++i) {
                new(p1 + i) NonMoveableClass(i);
            }

            ASSERT(nObj == NonMoveableClass::ctorCount());
            ASSERT(0    == NonMoveableClass::dtorCount());

            ASSERT(! IsBitwiseMoveable<NonMoveableClass>::value);
            destructiveMoveArray(p2, p1, nObj);

            // Verify that constructor and destructor were called on each move
            ASSERT(2 * nObj == NonMoveableClass::ctorCount());
            ASSERT(nObj     == NonMoveableClass::dtorCount());

            // Verify contents
            for (int i = 0; i < nObj; ++i) {
                ASSERT(i == p2[i].value());
            }

            // Destroy and deallocate
            for (int i = 0; i < nObj; ++i) {
                p2[i].~NonMoveableClass();
            }
            ::operator delete(p1);
            ::operator delete(p2);
        }

        {
            MoveableClass1 *p1 = (MoveableClass1*)
                ::operator new(nObj * sizeof(MoveableClass1));
            MoveableClass1 *p2 = (MoveableClass1*)
                ::operator new(nObj * sizeof(MoveableClass1));

            for (int i = 0; i < nObj; ++i) {
                ::new(p1 + i) MoveableClass1(i);
            }

            ASSERT(nObj == MoveableClass1::ctorCount());
            ASSERT(0    == MoveableClass1::dtorCount());

            ASSERT(IsBitwiseMoveable<MoveableClass1>::value);
            destructiveMoveArray(p2, p1, nObj);

            // Verify that constructor and destructor were NOT called on each
            // move
            ASSERT(nObj == MoveableClass1::ctorCount());
            ASSERT(0    == MoveableClass1::dtorCount());

            // Verify contents
            for (int i = 0; i < nObj; ++i) {
                ASSERT(i == p2[i].value());
            }

            // Destroy and deallocate
            for (int i = 0; i < nObj; ++i) {
                p2[i].~MoveableClass1();
            }
            ::operator delete(p1);
            ::operator delete(p2);
        }

        {
            MoveableClass2 *p1 = (MoveableClass2*)
                ::operator new(nObj * sizeof(MoveableClass2));
            MoveableClass2 *p2 = (MoveableClass2*)
                ::operator new(nObj * sizeof(MoveableClass2));

            for (int i = 0; i < nObj; ++i) {
                ::new(p1 + i) MoveableClass2(i);
            }

            ASSERT(nObj == MoveableClass2::ctorCount());
            ASSERT(0    == MoveableClass2::dtorCount());

            ASSERT(IsBitwiseMoveable<MoveableClass2>::value);
            destructiveMoveArray(p2, p1, nObj);

            // Verify that constructor and destructor were NOT called on each
            // move
            ASSERT(nObj == MoveableClass2::ctorCount());
            ASSERT(0    == MoveableClass2::dtorCount());

            // Verify contents
            for (int i = 0; i < nObj; ++i) {
                ASSERT(i == p2[i].value());
            }

            // Destroy and deallocate
            for (int i = 0; i < nObj; ++i) {
                p2[i].~MoveableClass2();
            }
            ::operator delete(p1);
            ::operator delete(p2);
        }

        return 0;
    }

    }  // close enterprise namespace
//..
//
///Example 2: Associating a Trait with a Class Template
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we associate a trait not with a class, but with a class
// *template*.  We create three class templates, each of which uses a different
// mechanisms for being associated with the 'IsBitwiseMoveable' trait, plus a
// "control" template that is not bitwise moveable.  First, we define the
// non-bitwise-moveable template, 'NonMoveableTemplate':
//..
    namespace BloombergLP {

    template <class TYPE>
    struct NonMoveableTemplate
    {
        TYPE d_p;
    };
//..
// Second, we define a 'MoveableTemplate1', which uses partial template
// specialization to associate the 'IsBitwiseMoveable' trait with each
// instantiation:
//..
    template <class TYPE>
    struct MoveableTemplate1
    {
        TYPE *d_p;
    };

    namespace bslmf {
        template <class TYPE>
        struct IsBitwiseMoveable<MoveableTemplate1<TYPE> > : bsl::true_type {
        };
    }  // close namespace bslmf
//..
// Third, we define 'MoveableTemplate2', which uses the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the 'IsBitwiseMoveable'
// trait with each instantiation:
//..
    template <class TYPE>
    struct MoveableTemplate2
    {
        TYPE *d_p;

        BSLMF_NESTED_TRAIT_DECLARATION(MoveableTemplate2,
                                       bslmf::IsBitwiseMoveable);
    };
//..
// Fourth, we define 'MoveableTemplate3', which is bitwise moveable iff its
// 'TYPE' template parameter is bitwise moveable.  There is no way to get this
// effect using 'BSLMF_NESTED_TRAIT_DECLARATION', so we use partial
// specialization combined with inheritance to "inherit" the trait from 'TYPE':
//..
    template <class TYPE>
    struct MoveableTemplate3
    {
        TYPE d_p;
    };

    namespace bslmf {
        template <class TYPE>
        struct IsBitwiseMoveable<MoveableTemplate3<TYPE> > :
            IsBitwiseMoveable<TYPE>::type { };
    }  // close namespace bslmf
//..
// Now, we check that the traits are correctly associated by instantiating each
// class with both bitwise moveable and non-moveable types and verifying the
// value of 'IsBitwiseMoveable<T>::value':
//..
    int usageExample2()
    {
        using namespace bslmf;

        ASSERT(! IsBitwiseMoveable<
               NonMoveableTemplate<NonMoveableClass> >::value);
        ASSERT(! IsBitwiseMoveable<
               NonMoveableTemplate<MoveableClass1> >::value);

        ASSERT(  IsBitwiseMoveable<
               MoveableTemplate1<NonMoveableClass> >::value);
        ASSERT(  IsBitwiseMoveable<
               MoveableTemplate1<MoveableClass1> >::value);

        ASSERT(  IsBitwiseMoveable<
               MoveableTemplate2<NonMoveableClass> >::value);
        ASSERT(  IsBitwiseMoveable<
               MoveableTemplate2<MoveableClass1> >::value);

        ASSERT(! IsBitwiseMoveable<
               MoveableTemplate3<NonMoveableClass> >::value);
        ASSERT(  IsBitwiseMoveable<
               MoveableTemplate3<MoveableClass1> >::value);

        return 0;
    }

    }  // close enterprise namespace
//..
//
///Example 3: Avoiding False Positives on One-Byte Classes
/// -- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we define an empty class that has a non-trivial copy
// constructor that has a global side effect.  The side effect should not be
// omitted, even in a destructive-move situation, so 'IsBitwiseMoveable' should
// be false.  However, the heuristic described above would deduce any one-byte
// class (including an empty class) as bitwise-moveable by default, so we must
// take specific action to set the trait to false in this (rare) case.
//
// First, we declare a normal empty class which *is* bitwise moveable:
//..
    namespace BloombergLP {
    namespace xyza {

    class MoveableEmptyClass
    {
        // This class is implicitly moveable by virtue of being only one byte
        // in size.
    };
//..
// The class above requires no special treatment.  Next, we define an empty
// class that is not bitwise moveable:
//..
    class NonMoveableEmptyClass
    {
        // This class is empty, which normally would imply bitwise moveability.
        // However, because it has a non-trivial move/copy constructor, it
        // should not be bitwise moved.

        static int d_count;

      public:
        NonMoveableEmptyClass() { ++d_count; }
        NonMoveableEmptyClass(const NonMoveableEmptyClass&) { ++d_count; }
    };

    int NonMoveableEmptyClass::d_count = 0;

    }  // close package namespace
//..
// Now, we specialize the 'IsBitwiseMoveable' trait so that
// 'NonMoveableEmptyClass' is not incorrectly flagged by trait deduction as
// having the 'IsBitwiseMoveable' trait:
//..
    namespace bslmf {

    template <>
    struct IsBitwiseMoveable<xyza::NonMoveableEmptyClass> : bsl::false_type
    {
    };

    }  // close namespace bslmf
//..
// Finally, we show that the first class has the 'IsBitwiseMoveable' trait and
// the second class does not:
//..
    int usageExample3()
    {
        using namespace bslmf;
        ASSERT(  IsBitwiseMoveable<xyza::MoveableEmptyClass>::value);
        ASSERT(! IsBitwiseMoveable<xyza::NonMoveableEmptyClass>::value);

        return 0;
    }

    }  // close enterprise namespace
//..

//=============================================================================
//                              TEST CLASSES
//-----------------------------------------------------------------------------


// BDE_VERIFY pragma : pop

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
        usageExample3();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // EXTENDING 'bslmf::IsBitwiseMoveable'
        //   Ensure the 'bslmf::IsBitwiseMoveable' meta-function returns the
        //   correct value for types explicitly specialized to support this
        //   trait.
        //
        // Concerns:
        //: 1 The meta-function returns 'false' for normal user-defined types.
        //:
        //: 2 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bslmf::IsBitwiseMoveable' on that type is
        //:   defined to inherit from 'bsl::true_type'.
        //:
        //: 3 The meta-function returns 'true' for a user-defined type that
        //:   specifies it has the trait using the
        //:   'BSLMF_NESTED_TRAIT_DECLARATION' macro.
        //:
        //: 4 The meta-function returns 'true' for a user-defined type, if a
        //:   specialization for 'bslmf::IsBitwiseMoveable' on that type is
        //:   defined to inherit from 'bsl::true_type'.
        //:
        //: 5 For cv-qualified types, the meta-function returns 'true' if the
        //:   corresponding cv-unqualified type is bitwise movable, and
        //:   'false' otherwise.
        //:
        //: 6 For array types, the meta-function returns 'true' if the array
        //:   element is bitwise movable, and 'false' otherwise.
        //:
        //: 7 A static assertion is not generated in C++11 builds because of a
        //:   false positive IsBitwiseMoveable for a one-byte type. (I.e., the
        //:   C++03 heuristic never gives a false positive, when compared
        //:   to the compiler defined trait).  See 'bslmf_isbitwisemoveable.h'
        //:   'k_ValueWithoutOnebyteHeuristic'.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C5,6)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-4, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   EXTENDING 'bslmf::IsBitwiseMoveable'
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nEXTENDING 'bslmf::IsBitwiseMoveable'"
                   "\n====================================\n");

        // C-1
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedNonTcTestType,
                                               false);

        // C-2
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedBwmTestType, true);

        // C-3
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedBwmTestType2, true);

        // C-4
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedTcTestType, true);
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedTcTestType2, true);

        // C-7 (1-byte type)
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(UserDefinedOneByteTestType,
                                               true);
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(
            UserDefinedNotTriviallyCopyableOneByteTestType, true);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // 'bslmf::IsBitwiseMoveable::value'
        //   Ensure the 'bslmf::IsBitwiseMoveable' meta-function
        //   returns the correct value for intrinsically supported types.
        //
        // Concerns:
        //:  1 The meta-function returns 'true' for fundamental types.
        //:
        //:  2 The meta-function returns 'true' for enumerated types.
        //:
        //:  3 The meta-function returns 'true' for pointer to member types.
        //:
        //:  4 The meta-function returns 'false' for cv-qualified 'void' types.
        //:
        //:  5 The meta-function returns 'false' for function types.
        //
        //:  6 The meta-function returns 'true' for pointer types.
        //:
        //:  7 The meta-function returns 'false' for reference types.
        //:
        //:  8 The meta-function returns the same result for array types as it
        //:    would for the array's element type.
        //:
        //:  9 The meta-function returns the same result for cv-qualified
        //:    types that it would return 'true' for the corresponding
        //:    cv-unqualified type.
        //
        // Plan:
        //:  1 Create a set of macros that will generate an 'ASSERT' test for
        //:    all variants of a type:  (C6-9)
        //:    o  reference and pointer types
        //:    o  all cv-qualified combinations
        //:    o  arrays, of fixed and runtime bounds, and multiple dimensions
        //:
        //:  2 For each category of type in concerns 1-5, use the appropriate
        //:    test macro for confirm the correct result for a representative
        //:    sample of types.
        //
        // Testing:
        //   bslmf::IsBitwiseMoveable::value
        // --------------------------------------------------------------------

        if (verbose)
            printf("\n'bslmf::IsBitwiseMoveable::value'"
                   "\n=================================\n");

        // C-1
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(int, true);
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(char, true);
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(long double, true);

        // C-2
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(EnumTestType, true);

        // C-3
        ASSERT_IS_BITWISE_MOVEABLE_OBJECT_TYPE(MethodPtrTestType, true);

        // C-4 : 'void' is not an object type, but can be cv-qualified.
        ASSERT_IS_BITWISE_MOVEABLE_CV_TYPE(void, false);

        // C-5 : Function types are not object types, nor cv-qualifiable.
        // Note that this particular test stresses compilers handling of
        // function types, and function reference types, in the template type
        // system.
        ASSERT( bslmf::IsBitwiseMoveable<void(*)()>::value);
        ASSERT( bslmf::IsBitwiseMoveable<int(*)(float, double...)>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<void()>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<int(float, double...)>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<void(&)()>::value);
        ASSERT(!bslmf::IsBitwiseMoveable<int(&)(float, double...)>::value);
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
