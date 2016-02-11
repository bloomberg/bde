// bsls_alignedbuffer.t.cpp                                           -*-C++-*-

#include <bsls_alignedbuffer.h>

#include <bsls_alignmenttotype.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>

#include <stddef.h>
#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// bsls::AlignedBuffer is a simple template class that provides a few
// compile-time invariants and two trivial run-time functions.  Our tests
// involve instantiating it with various representative template parameters
// and verifying the invariants.
//-----------------------------------------------------------------------------
// [2] char *buffer();
// [2] const char *buffer() const;
//
// [1] CLASS INVARIANTS
// [3] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//                              USAGE EXAMPLES
//-----------------------------------------------------------------------------

// The 'allocateFromBuffer' function below uses an aligned buffer as a small
// heap from which objects can be allocated.  We choose 'int' alignment
// (4-byte alignment) for our buffer because the objects we are allocating
// are composed of 'char', 'short', and 'int' values only.  If no alignment
// were specified, the buffer would be maximally aligned, which could be
// wasteful on some platforms.
//..
    const int MY_ALIGNMENT = bsls::AlignmentFromType<int>::VALUE;
    bsls::AlignedBuffer<1000, MY_ALIGNMENT> my_AllocBuffer;
    const char* my_AllocEnd = my_AllocBuffer.buffer() + 1000;
    char *my_AllocPtr = my_AllocBuffer.buffer();
        // Invariant: my_AllocPtr is always aligned on a multiple of 4 bytes

    static void *allocateFromBuffer(int size)
    {
        if (size > my_AllocEnd - my_AllocPtr)
            return 0;       // Out of buffer space                    // RETURN

        void *result = my_AllocPtr;
        my_AllocPtr += size;
        if (size % MY_ALIGNMENT) {
            // re-establish invariant by re-aligning my_AllocPtr
            my_AllocPtr += MY_ALIGNMENT - size % MY_ALIGNMENT;
        }

        ASSERT(0 == size_t(my_AllocPtr) % MY_ALIGNMENT);     // Test invariant

        return result;
    }
//..
// Below, we use our allocation function to allocate arrays of 'char',
// 'short', and user-defined 'Object' types from the static buffer.  Note that
// our 'Object' structure is composed of members that have alignment
// requirements less than or equal to 'int's alignment requirements.
//..
    struct Object {
        char  d_c;
        short d_s;
        int   d_i;
    };

    int usageExample()
    {
        // Allocate three 'char's from the buffer.
        char *charPtr   = (char *)   allocateFromBuffer(3 * sizeof(char));
        ASSERT(0 == size_t(charPtr) % MY_ALIGNMENT);

        // Allocate three 'short's from the buffer.
        short *shortPtr = (short *)  allocateFromBuffer(3 * sizeof(short));
        ASSERT(0 == size_t(shortPtr) % MY_ALIGNMENT);

        // Allocate three 'Object's from the buffer
        Object *objPtr = (Object *)  allocateFromBuffer(3 * sizeof(Object));
        ASSERT(0 == size_t(objPtr) % MY_ALIGNMENT);

        if (!charPtr || !shortPtr || !objPtr) {
            fprintf(stderr, "Global buffer is not large enough.\n");
            return -1;                                                // RETURN
        }

        // ...

        return 0;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Simple example showing how one might use bsls::AlignedBuffer
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        usageExample();

      } break;

      case 2: {
        // --------------------------------------------------------------------
        // buffer() TEST
        //
        // Concerns:
        //   - buffer() returns the address of the first byte of the buffer
        //
        // Plan:
        //   - Create 'bsls::AlignedBuffer' objects with different sizes and
        //     alignments.
        //   - For each object, verify that buffer() returns the address of
        //     the object.
        //
        // Testing:
        //   char *buffer();
        //   const char *buffer() const;
        // --------------------------------------------------------------------

#       define TEST_METHODS(SIZE, ALIGNMENT)                                  \
            do {                                                              \
                bsls::AlignedBuffer<SIZE, ALIGNMENT> buff;                    \
                const bsls::AlignedBuffer<SIZE, ALIGNMENT>& BUFF = buff;      \
                ASSERT((const char*) &BUFF == buff.buffer());                 \
                ASSERT((const char*) &BUFF == BUFF.buffer());                 \
            } while (false)

        TEST_METHODS(1, 1);
        TEST_METHODS(2, 1);
        TEST_METHODS(999, 1);
        TEST_METHODS(1, 2);
        TEST_METHODS(2, 2);
        TEST_METHODS(3, 2);
        TEST_METHODS(4, 2);
        TEST_METHODS(999, 2);
        TEST_METHODS(1, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_METHODS(2, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_METHODS(3, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_METHODS(64, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_METHODS(999, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // INVARIANT TEST
        //
        // Concerns:
        //   - aligned buffer actually has specified alignment
        //   - aligned buffer size >= specified size
        //   - aligned buffer size is a multiple of alignment
        //   - difference between aligned buffer size and specified size <
        //     specified alignment (i.e., no unnecessary padding).
        //
        // Plan:
        //   For a representative set of combinations of SIZE and ALIGNMENT,
        //   instantiate bsls::AlignedBuffer<SIZE, ALIGNMENT> (a.k.a. Buff) and
        //   verify that:
        //   - bsls::AlignmentFromType<Buff>::VALUE == ALIGNMENT
        //   - sizeof(Buff) >= SIZE
        //   - sizeof(Buff) % ALIGNMENT == 0
        //   - sizeof(Buff) - SIZE < ALIGNMENT
        //
        // Testing:
        //   Class Invariants
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CLASS INVARIANTS"
                            "\n========================\n");
#if defined(BSLS_PLATFORM_CMP_MSVC)
#define ASSERT_ALIGN(TYPE, ALIGNMENT) ASSERT(__alignof(TYPE) == ALIGNMENT);
#else
#define ASSERT_ALIGN(TYPE, ALIGNMENT) ASSERT(__alignof__(TYPE) == ALIGNMENT);
#endif

#       define TEST_INVARIANTS(SIZE, ALIGNMENT)                               \
            do {                                                              \
              typedef bsls::AlignedBuffer<SIZE, ALIGNMENT> Buff;              \
              Buff buffer;                                                    \
              if (veryVeryVerbose) {                                          \
                  int size = SIZE;                                            \
                  int alignment = ALIGNMENT;                                  \
                  P_(size) P_(alignment) P_(reinterpret_cast<size_t>(&buffer))\
                  P_((int) bsls::AlignmentFromType<Buff>::VALUE)              \
                  P(sizeof(Buff))                                             \
              }                                                               \
              ASSERTV(reinterpret_cast<size_t>(&buffer),                      \
                      reinterpret_cast<size_t>(&buffer) % ALIGNMENT == 0);    \
              ASSERTV((int) bsls::AlignmentFromType<Buff>::VALUE,             \
                      bsls::AlignmentFromType<Buff>::VALUE == (int)ALIGNMENT);\
              ASSERTV(sizeof(Buff), sizeof(Buff) >= SIZE);                    \
              ASSERTV(sizeof(Buff), sizeof(Buff) % ALIGNMENT == 0);           \
              ASSERTV(sizeof(Buff), sizeof(Buff) - SIZE < (int) ALIGNMENT);   \
              ASSERT_ALIGN(Buff, ALIGNMENT);                                  \
            } while (0)

        TEST_INVARIANTS(1, 1);
        TEST_INVARIANTS(2, 1);
        TEST_INVARIANTS(999, 1);
        TEST_INVARIANTS(1, 2);
        TEST_INVARIANTS(2, 2);
        TEST_INVARIANTS(3, 2);
        TEST_INVARIANTS(4, 2);
        TEST_INVARIANTS(999, 2);
        TEST_INVARIANTS(1, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_INVARIANTS(2, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_INVARIANTS(3, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_INVARIANTS(64, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
        TEST_INVARIANTS(999, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);

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
