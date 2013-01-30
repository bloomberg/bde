// bslmf_isbitwisemoveable.t.cpp                  -*-C++-*-

#include "bslmf_isbitwisemoveable.h"

#include <new>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace BloombergLP;
using namespace std;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//

//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and must not be called during exception testing.

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                    \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                  \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                \
    if (!(X)) { printf("%s", #I ": "); dbg_print(I); printf("\t"); \
                printf("%s", #J ": "); dbg_print(J); printf("\t"); \
                printf("%s", #K ": "); dbg_print(K); printf("\t"); \
                printf("%s", #L ": "); dbg_print(L); printf("\t"); \
                printf("%s", #M ": "); dbg_print(M); printf("\n"); \
                fflush(stdout); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");     // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n")  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", ") // P(X) without '\n'
#define L_ __LINE__                        // current Line number
#define T_ putchar('\t');                  // Print a tab (w/o newline)

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%hd", val); fflush(stdout); }
inline void dbg_print(unsigned short val) {printf("%hu", val); fflush(stdout);}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%lu", val); fflush(stdout); }
inline void dbg_print(unsigned long val) { printf("%lu", val); fflush(stdout);}
// inline void dbg_print(Int64 val) { printf("%lld", val); fflush(stdout); }
// inline void dbg_print(Uint64 val) { printf("%llu", val); fflush(stdout); }
inline void dbg_print(float val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

// Used to test detection of legacy traits
#define BSLALG_DECLARE_NESTED_TRAITS(T, TRAIT)            \
    operator TRAIT::NestedTraitDeclaration<T>() const {   \
        return TRAIT::NestedTraitDeclaration<T>();        \
    }

//..
// Now we use this trait in a simple algorithm called 'destructiveMoveArray',
// which moves elements from one array to another.  The algorithm is
// implemented using two implementation functions, one for types that are
// known to be bit-wise moveable, and one for other types.  THe first takes an
// extra function argument of type 'true_type', the second takes and extra
// function argument of type 'false_type':
//..
    namespace BloombergLP {

    template <class TYPE>
    void destructiveMoveArrayImp(TYPE *to, TYPE *from, int size, bsl::true_type)
    {
        // Bitwize moveable types can be moved using memcpy
        std::memcpy(to, from, size * sizeof(TYPE));
    }

    template <class TYPE>
    void destructiveMoveArrayImp(TYPE *to, TYPE *from, int size, bsl::false_type)
    {
        for (int i = 0; i < size; ++i) {
            ::new(to + i) TYPE(from[i]);
            from[i].~TYPE();
        }
    }
//..
// Now we can dispatch between the two Imp functions, using the
// 'IsBitwiseMoveable' trait metafunction to determine at compile time which
// of the implementations should be used:
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
        template <> struct IsBitwiseMoveable<MoveableClass1> : bsl::true_type { };
    }
//..
// The third class is also declared to be bitwise moveable, but this time we
// do it using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
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

        // For each of our test classes, allocate an array, construct three
        // objects into it, then move it into another array.
        const int nObj = 3;

        {
            NonMoveableClass* p1 = (NonMoveableClass*)
                ::operator new(nObj * sizeof(NonMoveableClass));
            NonMoveableClass* p2 =  (NonMoveableClass*)
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
            MoveableClass1* p1 = (MoveableClass1*)
                ::operator new(nObj * sizeof(MoveableClass1));
            MoveableClass1* p2 = (MoveableClass1*)
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
            MoveableClass2* p1 = (MoveableClass2*)
                ::operator new(nObj * sizeof(MoveableClass2));
            MoveableClass2* p2 = (MoveableClass2*)
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
//..
//
///Example 2: Associating a trait with a class template
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we associate a trait not with a class, but with a class
// *template*.  We create three class templates, each of which uses a
// different mechanisms for being associated with the 'IsBitwiseMoveable'
// trait, plus a "control" template that is not bit-wise moveable.  First, we
// define the non-bit-wise-moveable template, 'NonMoveableTemplate':
//..
    template <class TYPE>
    struct NonMoveableTemplate
    {
    };
//..
// Second, we define a 'MoveableTemplate1', which uses partial template
// specialization to associate the 'IsBitwiseMoveable' trait with each
// instantiation:
//..
    template <class TYPE>
    struct MoveableTemplate1
    {
    };

    namespace bslmf {
        template <class TYPE>
        struct IsBitwiseMoveable<MoveableTemplate1<TYPE> > : bsl::true_type { };
    }
//..
// Third, we define 'MoveableTemplate2', which uses the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the 'IsBitwiseMoveable'
// trait with each instantiation:
//..
    template <class TYPE>
    struct MoveableTemplate2
    {
        BSLMF_NESTED_TRAIT_DECLARATION(MoveableTemplate2,
                                       bslmf::IsBitwiseMoveable);
    };
//..
// Fourth, we define 'MoveableTemplate3', which is bit-wise moveable iff its
// 'TYPE' template parameter is bit-wise moveable.  There is no way to get
// this effect using 'BSLMF_NESTED_TRAITS_DECLARATION', so we use partial
// specialization combined with inheritence to "inherit" the trait from
// 'TYPE':
//..
    template <class TYPE>
    struct MoveableTemplate3
    {
    };

    namespace bslmf {
        template <class TYPE>
        struct IsBitwiseMoveable<MoveableTemplate3<TYPE> > :
            IsBitwiseMoveable<TYPE>::type { };
    }
//..
// Now, we check that the traits are correctly associated by instantiating
// each class with both bit-wise moveable and non-moveable types and verifying
// the value of 'IsBitwiseMoveable<T>::value':
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
//
///Example 3: Support for legacy traits
/// - - - - - - - - - - - - - - - - - -
// 'IsBitwiseMoveable' is not a new trait.  Within the legacy type traits
// system, 'bslalg::TypeTraitBitwiseMoveable' conveys the same
// information. The example shows how the legacy trait can be built on top of
// the new trait, how the legacy system for associating traits with types can
// be supported, and how the new system can be used to query for a trait that
// was associated using the legacy system.
//
// First, we define the 'bslalg::TypeTraitBitwiseMoveable' trait, using a
// special nested type to tie the legacy trait to 'IsBitwiseMoveable':
//..
    namespace bslalg {

    struct TypeTraitBitwiseMoveable {
        // Objects of a type with this trait can be "moved" from one memory
        // location to another using 'memmove' or 'memcpy'.  Although the
        // result of such a bitwise copy is two copies of the same object,
        // this trait guarantees only that one of the copies can be destroyed.
        // The other copy must be considered invalid and its destructor must
        // not be called.  Most types, even those that contain pointers, are
        // bitwise moveable.  Undefined behavior may result if this trait is
        // assigned to a type that contains pointers to its own internals,
        // uses virtual inheritance, or places pointers to itself within other
        // data structures.

        template <class TYPE>
        struct NestedTraitDeclaration :
            bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseMoveable>
        {
            // Ties this trait to the 'bslmf::IsBitwiseMoveable' trait.
        };
    };

    } // Close package namespace
//..
// Second, we declare a class and a class template that use the legacy
// 'BSLALG_DECLARE_NESTED_TRAITS' macro to associate the above trait with
// themselves.  (Note: the legacy macro is in 'bslalg', not 'bslmf')
//..
    struct MoveableClass3
    {
        BSLALG_DECLARE_NESTED_TRAITS(MoveableClass3,
                                     bslalg::TypeTraitBitwiseMoveable);
    };

    template <class TYPE>
    struct MoveableTemplate4
    {
        BSLALG_DECLARE_NESTED_TRAITS(MoveableTemplate4,
                                     bslalg::TypeTraitBitwiseMoveable);
    };
//..
// Third, we use 'IsBitwiseMoveable' to check for the trait.  The
// 'BSLALG_DECLARE_NESTED_TRAITS' macro has been rewritten to allow for this
// detection of the 'bslalg::TypeTraitBitwiseMoveable' trait:
//..
    int usageExample3()
    {
        using namespace bslmf;

        ASSERT(  IsBitwiseMoveable<MoveableClass3>::value);
        ASSERT(  IsBitwiseMoveable<MoveableTemplate4<int> >::value);

        return 0;
    }

    }  // close enterprise namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
	//
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        usageExample1();
        usageExample2();
        usageExample3();

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
