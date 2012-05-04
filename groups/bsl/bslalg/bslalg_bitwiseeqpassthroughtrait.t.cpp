// bslalg_bitwiseeqpassthroughtrait.t.cpp                             -*-C++-*-

#include <bslalg_bitwiseeqpassthroughtrait.h>

#include <bslalg_hastrait.h>
#include <bslalg_typetraits.h>

#include <bslmf_issame.h>

#include <bsls_types.h>  // for testing only

#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

# define LOOP_ASSERT(I,X) \
    if (!(X)) { printf("%s = %s\n", #I, (I)); aSsErT(!(X), #X, __LINE__); }

# define LOOP2_ASSERT(I,J,X) \
    if (!(X)) { printf("%s = %s, %s = %d\n", #I, (I), #J, (J));  \
                aSsErT(!(X), #X, __LINE__); }

//--------------------------------------------------------------------------

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

int verbose = 0;
int veryVerbose = 0;
int veryVeryVerbose = 0;

//=============================================================================
//                       HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class my_PaddingClass {
    // This class is not bitwise equality comparable.

    // DATA
    char d_a;
    int  d_b;
};

class my_NoPaddingClass {
    // This class is bitwise equality comparable.

    // DATA
    char d_a;
    char d_b;

  public:
    BSLALG_DECLARE_NESTED_TRAITS(my_NoPaddingClass,
                                 bslalg::TypeTraitBitwiseEqualityComparable);
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
// This class simplifies the declaration of
// 'bslalg::TypeTraitBitwiseEqualityComparable' trait for a class with multiple
// data members.  This is especially useful for attribute classes.  For
// example, suppose we have an unconstrained attribute class 'Point':
//..
    class my_Point {

        // DATA
        int d_x;
        int d_y;

      public:
        BSLALG_DECLARE_NESTED_TRAITS(
                                   my_Point,
                                   bslalg::TypeTraitBitwiseEqualityComparable);

        // ...
    };
//..
// Note that the methods of the 'Point' class is elided as they are not
// relevant to this usage example.  Then we have another attribute class
// 'ColorPoint' where color is a 32-bit number.
//..
    class my_ColorPoint {

        // PRIVATE TYPES
        typedef bslalg::BitwiseEqPassthroughTrait<my_Point, int>::Type BEC;

        // DATA
        my_Point d_point;
        int      d_color;

      public:
        BSLALG_DECLARE_NESTED_TRAITS(my_ColorPoint, BEC);

        // ...
    };
//..
// Instead of having to check whether 'Point' is bitwise equality comparable,
// and whether the combination of 'Point' and 'int' inside the class results in
// a bitwise equality comparable class, the use of
// 'bslalg::BitwiseEqPassthroughTrait' encapsulates all the meta-programming
// check.

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Assert that the class 'my_ColorPoint' does have the
        //   'bslalg::TypeTraitBitwiseEqualityComparable' trait.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============");

        ASSERT((1 == bslalg::HasTrait<
                        my_ColorPoint,
                        bslalg::TypeTraitBitwiseEqualityComparable>::VALUE));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That the class properly pass on
        //   'bslalg::TypeTraitBitwiseEqualityComparable' when applicable, and
        //   does not pass on any trait when not applicable.
        //
        // Plan:
        //   Assert the different scenarios.
        //
        // Testing:
        //   class bslalg::BitwiseEqPassthroughTrait;
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        #define OBJ bslalg::BitwiseEqPassthroughTrait
        typedef bslalg::TypeTraitBitwiseEqualityComparable Bec;
        typedef my_NoPaddingClass Npc;
        typedef my_PaddingClass Pc;

        // Check fundamental types
        ASSERT((1 == bslmf::IsSame<OBJ<char>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<unsigned char>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<signed char>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<short>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<unsigned short>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<int>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<unsigned int>::Type, Bec>::VALUE));
        ASSERT((1 ==
                bslmf::IsSame<OBJ<bsls::Types::Int64>::Type, Bec>::VALUE));
        ASSERT((1 ==
                bslmf::IsSame<OBJ<bsls::Types::Uint64>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<float>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<double>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<char *>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<void *>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<const void *>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<void * const>::Type, Bec>::VALUE));

        // Check for types without padding
        ASSERT((0 == bslmf::IsSame<OBJ<my_PaddingClass>::Type, Bec>::VALUE));
        ASSERT((1 == bslmf::IsSame<OBJ<my_NoPaddingClass>::Type, Bec>::VALUE));

        // Check for a combination of all 9
        ASSERT((1 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Npc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));

        // Check with a type that is not bitwise equality comparable.
        ASSERT((0 == bslmf::IsSame<OBJ<Pc,Npc,Npc,Npc,Npc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Pc,Npc,Npc,Npc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Pc,Npc,Npc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Pc,Npc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Pc,Npc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Npc,Pc,Npc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Npc,Npc,Pc,Npc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Npc,Npc,Npc,Pc,Npc
                                                        >::Type, Bec>::VALUE));
        ASSERT((0 == bslmf::IsSame<OBJ<Npc,Npc,Npc,Npc,Npc,Npc,Npc,Npc, Pc
                                                        >::Type, Bec>::VALUE));

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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
