// bslalg_autoarraymovedestructor.t.cpp                               -*-C++-*-

#include <bslalg_autoarraymovedestructor.h>

#include <bslalg_autoarraydestructor.h>          // for testing only
#include <bslalg_scalarprimitives.h>             // for testing only
#include <bslalg_typetraits.h>                   // for testing only
#include <bslalg_typetraitusesbslmaallocator.h>  // for testing only
#include <bslalg_typetraitbitwisemoveable.h>     // for testing only

#include <bslma_allocator.h>                     // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bsls_alignmentutil.h>                  // for testing only
#include <bsls_assert.h>
#include <bsls_asserttest.h>                     // for testing only
#include <bsls_stopwatch.h>                      // for testing only

#include <ctype.h>    // isalpha()
#include <stdio.h>
#include <stdlib.h>   // atoi()
#include <string.h>   // strlen()

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides a proctor to help with exception-safety
// guarantees.  The test sequence is very simple: we only have to ascertain
// that the index computation is correct and that the proctor does destroy its
// guarded range unless 'release has been called.  We use a test type that
// monitors the number of constructions and destructions, and that allocates in
// order to take advantage of the standard 'bslma' exception test.
//-----------------------------------------------------------------------------
// [ 2] bslalg::AutoArrayMoveDestructor(T *b, T *e);
// [ 2] ~bslalg::AutoArrayMoveDestructor();
// [ 2] T *advance();
// [ 2] T *begin() const;
// [ 2] T *middle() const;
// [ 2] T *end() const;
// [ 2] T *destination() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
int testStatus = 0;

namespace {
    void aSsErT(int c, const char *s, int i) {
    if (c) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\n", #I, I, #J, J); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { printf("%s: %d\t%s: %c\t%s: %c\n", #I, I, #J, J, #K, K); \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { printf("%s: %d\t%s: %d\t%s: %d\t%s: %d\n", \
                #I, I, #J, J, #K, K, #L, L); aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS/TYPES FOR TESTING
//-----------------------------------------------------------------------------

// TYPES
class TestType;

typedef TestType                      T;    // uses 'bslma' allocators

// STATIC DATA
static int verbose, veryVerbose, veryVeryVerbose;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma::TestAllocator *Z;  // initialized at the start of main()

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char             *d_data_p;
    bslma::Allocator *d_allocator_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(TestType,
                                  bslalg::TypeTraitUsesBslmaAllocator,
                                  bslalg::TypeTraitBitwiseMoveable);

    // CREATORS
    TestType(bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }

    TestType(char c, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma::Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma::Default::allocator(ba))
    {
        ++numCopyCtorCalls;
        if (&original != this) {
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = *original.d_data_p;
    }
    }

    ~TestType() {
        ++numDestructorCalls;
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p = 0;
    }

    // MANIPULATORS
    TestType& operator=(const TestType& rhs)
    {
        ++numAssignmentCalls;
        if (&rhs != this) {
        char *newData = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '_';
        d_allocator_p->deallocate(d_data_p);
        d_data_p  = newData;
        *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    void setDatum(char c) {
        *d_data_p = c;
    }

    // ACCESSORS
    char datum() const {
        return *d_data_p;
    }

    void print() const
    {
        if (d_data_p) {
        ASSERT(isalpha(*d_data_p));
            printf("%c (int: %d)\n", *d_data_p, (int)*d_data_p);
        } else {
            printf("VOID\n");
        }
    }
};

bool operator==(const TestType& lhs, const TestType& rhs)
{
    ASSERT(isalpha(lhs.datum()));
    ASSERT(isalpha(rhs.datum()));

    return lhs.datum() == rhs.datum();
}

//=============================================================================
//                            USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

    bslma::TestAllocator testAllocator(veryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns: That the usage example compiles and runs as expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE."
                            "\n======================\n");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'class AutoArrayDestructor'
        //
        // Concerns:  That the guard frees guarded memory properly upon
        //   exceptions.
        //
        // Plan: After asserting that the interface behaves as intended and
        //   that the guard destruction indeed frees the memory, we set up an
        //   exception test that ensures that the guard indeed correctly guards
        //   a varying portion of an array.
        //
        // Testing:
        //   AutoArrayMoveDestructor(T *d, T *b, T *m, T *e);
        //   ~AutoArrayMoveDestructor();
        //   T *advance();
        //   T *begin() const;
        //   T *middle() const;
        //   T *end() const;
        //   T *destination() const;
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING class bslalg::AutoArrayMoveDestructor"
                   "\n=============================================\n");

        const int GUARD_SIZE = 8;
        const int MAX_SIZE   = 2 * GUARD_SIZE;  // do not change

        static union {
            char                                d_raw[MAX_SIZE * sizeof(T)];
            bsls::AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        if (verbose)
            printf("\tSimple interface test.\n");
        {
            bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                  &buf[0],
                                                  &buf[0],
                                                  &buf[GUARD_SIZE]);
            const bslalg::AutoArrayMoveDestructor<T>& G = mG;

            ASSERT(&buf[0]          == G.begin());
            ASSERT(&buf[0]          == G.middle());
            ASSERT(&buf[GUARD_SIZE] == G.end());
            ASSERT(&buf[GUARD_SIZE] == G.destination());

            for (int i = 0; i < GUARD_SIZE; ++i) {
                mG.advance();

                ASSERT(&buf[GUARD_SIZE + i + 1] == G.destination());
                ASSERT(&buf[0]                  == G.begin());
                ASSERT(&buf[i + 1]              == G.middle());
                ASSERT(&buf[GUARD_SIZE]         == G.end());
            }

            ASSERT(G.middle() == G.end());  // no destruction, no move
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose)
            printf("\tException test.\n");

        {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator)
            {
                bslalg::AutoArrayDestructor<T> mExcGuard(&buf[0], &buf[0]);
                const bslalg::AutoArrayDestructor<T>& EXC_GUARD = mExcGuard;

                char c = 'a';
                for (int i = 0; i < GUARD_SIZE; ++i, ++c) {
                    new (&buf[i]) T(c, Z);
                    mExcGuard.moveEnd(1);
                    if (veryVerbose) { buf[i].print(); }
                }

                {
                    bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                          &buf[0],
                                                          &buf[0],
                                                          &buf[GUARD_SIZE]);
                    const bslalg::AutoArrayMoveDestructor<T>& G = mG;
                        // guards as follows (upon destruction): destroys first
                        // portion of first half, move second portion of first
                        // half beyond constructed elements in second half

                    mExcGuard.moveBegin(GUARD_SIZE);
                    mExcGuard.moveEnd(MAX_SIZE - GUARD_SIZE); // second half

                    ASSERT(&buf[GUARD_SIZE] == G.destination());
                    ASSERT(&buf[0]          == G.begin());
                    ASSERT(&buf[0]          == G.middle());
                    ASSERT(&buf[GUARD_SIZE] == G.end());

                    for (int i = 0; i < GUARD_SIZE; ++i, ++c) {
                        new (&buf[GUARD_SIZE + i]) T(c, Z);
                        mG.advance();

                        ASSERT(&buf[0]                  == G.begin());
                        ASSERT(&buf[i + 1]              == G.middle());
                        ASSERT(&buf[GUARD_SIZE]         == G.end());
                        if (veryVerbose) { buf[GUARD_SIZE + i].print(); }
                    }
                    ASSERT(G.middle() == G.end());
                }
                mExcGuard.moveBegin(-GUARD_SIZE);  // whole range
            }
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   This test exercises the component but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const int GUARD_SIZE = 8;
        const int MAX_SIZE   = 2 * GUARD_SIZE;  // do not change

        static union {
            char                                d_raw[MAX_SIZE * sizeof(T)];
            bsls::AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        {
            bslalg::AutoArrayMoveDestructor<T> mG(&buf[GUARD_SIZE],
                                                  &buf[0],
                                                  &buf[1],
                                                  &buf[GUARD_SIZE]);
            const bslalg::AutoArrayMoveDestructor<T>& G = mG;

            ASSERT(&buf[GUARD_SIZE] == G.destination());
            ASSERT(&buf[0]          == G.begin());
            ASSERT(&buf[1]          == G.middle());
            ASSERT(&buf[GUARD_SIZE] == G.end());

            for (int i = 1; i < GUARD_SIZE; ++i) {
                mG.advance();

                ASSERT(&buf[GUARD_SIZE + i] == G.destination());
                ASSERT(&buf[0]              == G.begin());
                ASSERT(&buf[i + 1]          == G.middle());
                ASSERT(&buf[GUARD_SIZE]     == G.end());
            }

            ASSERT(G.middle() == G.end());  // no destruction, no move
        }

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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
