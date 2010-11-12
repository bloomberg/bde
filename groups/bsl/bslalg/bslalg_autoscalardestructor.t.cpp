// bslalg_autoscalardestructor.t.cpp                  -*-C++-*-

#include <bslalg_autoscalardestructor.h>

#include <bslalg_scalarprimitives.h>             // for testing only
#include <bslalg_typetraits.h>                   // for testing only
#include <bslalg_typetraitusesbslmaallocator.h>  // for testing only

#include <bslma_allocator.h>                     // for testing only
#include <bslma_default.h>                       // for testing only
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#include <bsls_alignmentutil.h>                  // for testing only
#include <bsls_stopwatch.h>                      // for testing only

#include <cstdio>
#include <cstdlib>     // atoi()
#include <cstring>     // strlen()
#include <ctype.h>     // isalpha()

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component to be tested provides a proctor to help with exception-safety
// guarantees.  The test sequence is very simple: we only have to ascertain
// that the proctor does destroy its guarded object unless 'release has been
// called.  We use a test type that monitors the number of constructions and
// destructions, and that allocates in order to take advantage of the standard
// 'bslma' exception test.
//-----------------------------------------------------------------------------
// [ 2] bslalg_AutoScalarDestructor(T *object);
// [ 2] ~AutoScalarDestructor();
// [ 3] void release();
// [ 2] void reset(T *object);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE

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

const int MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

static int numDefaultCtorCalls = 0;
static int numCharCtorCalls    = 0;
static int numCopyCtorCalls    = 0;
static int numAssignmentCalls  = 0;
static int numDestructorCalls  = 0;

bslma_TestAllocator *Z;  // initialized at the start of main()

                               // ==============
                               // class TestType
                               // ==============

class TestType {
    // This test type contains a 'char' in some allocated storage.  It counts
    // the number of default and copy constructions, assignments, and
    // destructions.  It has no traits other than using a 'bslma' allocator.
    // It could have the bit-wise moveable traits but we defer that trait to
    // the 'MoveableTestType'.

    char            *d_data_p;
    bslma_Allocator *d_allocator_p;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(TestType,
                                  bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    TestType(bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numDefaultCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = '?';
    }

    TestType(char c, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
    {
        ++numCharCtorCalls;
        d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
        *d_data_p = c;
    }

    TestType(const TestType& original, bslma_Allocator *ba = 0)
    : d_data_p(0)
    , d_allocator_p(bslma_Default::allocator(ba))
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

// TBD

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

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns: That the usage example compiles and runs as expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("Testing usage example.\n");

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'release'
        //
        // Concerns:  That the guard does not free guarded memory if 'release'
        //    has been called.
        //
        // Plan:
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'release'."
                            "\n==================\n");

        const int MAX_SIZE = 1;
        static union {
            char                               d_raw[MAX_SIZE * sizeof(T)];
            bsls_AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        if (verbose) printf("\tWith release.\n");
        {
            char c = 'a';

            for (int i = 0; i < MAX_SIZE; ++i, ++c) {
                new (&buf[i]) T(c, Z);
                if (veryVerbose) { buf[i].print(); }
            }

            bslalg_AutoScalarDestructor<T> mG(&buf[0]);
            mG.release();
        }
        ASSERT(0 <  testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose) printf("\tWithout release.\n");
        {
            bslalg_AutoScalarDestructor<T> mG(&buf[0]);
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'class bslalg_AutoScalarDestructor'
        //
        // Concerns:  That the guard frees guarded memory properly upon
        //   exceptions.
        //
        // Plan: After asserting that the interface behaves as intended and
        //   that the guard destruction indeed frees the memory, we set up an
        //   exception test that ensures that the guard indeed correctly guards
        //   a varying portion of an scalar.
        //
        // Testing:
        //   bslalg_AutoScalarDestructor(T *object);
        //   ~AutoScalarDestructor();
        //   void reset(T *object);
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslalg_AutoScalarDestructor'."
                            "\n=====================================\n");

        static union {
            char                               d_raw[sizeof(T)];
            bsls_AlignmentUtil::MaxAlignedType d_align;
        } u;
        T *buf = (T*)&u.d_raw[0];

        if (verbose)
            printf("\tSimple interface tests (from breathing test).\n");
        {
            new (buf) T('a', Z);
            if (veryVerbose) { buf->print(); }

            bslalg_AutoScalarDestructor<T> mG(buf);
            mG.reset(buf);
        }
        ASSERT(0 == testAllocator.numBytesInUse());
        ASSERT(0 == testAllocator.numMismatches());

        if (verbose)
            printf("\tException test.\n");
        {
            BEGIN_BSLMA_EXCEPTION_TEST
            {
                bslalg_AutoScalarDestructor<T> mG(0);
                const bslalg_AutoScalarDestructor<T>& G = mG;

                new (buf) T('a', Z);
                if (veryVerbose) { buf->print(); }

                mG.reset(buf);
            }
            END_BSLMA_EXCEPTION_TEST
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

        if (verbose) printf("\nclass bslalg_AutoScalarDestructor"
                            "\n--------------------------------\n");
        {
            static union {
                char                               d_raw[sizeof(T)];
                bsls_AlignmentUtil::MaxAlignedType d_align;
            } u;
            T *buf = (T*)&u.d_raw[0];

            new (buf) T('a', Z);
            if (veryVerbose) { buf->print(); }

            bslalg_AutoScalarDestructor<T> mG(buf);

        }  // deallocates buf

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
