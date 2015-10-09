// bsls_blockgrowth.t.cpp                                             -*-C++-*-

#include <bsls_blockgrowth.h>

#include <iostream>

#include <cstdlib>
#include <cstring>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component implements a standard enumeration that, in addition to its
// enumeration values, provide a count of enumeration values and a 'toAscii'
// method.
//-----------------------------------------------------------------------------
// [1] enum Strategy { ... };
// [1] char *toAscii(bsls::BlockGrowth::Strategy)
//-----------------------------------------------------------------------------
// [2] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------
typedef bsls::BlockGrowth::Strategy Enum;
typedef bsls::BlockGrowth           Class;

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
// Memory block growth strategies are often used in memory managers and
// containers to control memory usage.  First of all, suppose we have a
// 'my_BlockList' class that manages a link list of memory blocks:
//..
    class my_BlockList {
        // ...
    };
//..
// We can then create a memory manager class 'my_SequentialPool' that manages a
// pool of memory:
//..
    class my_SequentialPool {
        // This class implements a memory pool that dispenses (heterogeneous)
        // blocks of memory (of varying, user-specified-sized) from a sequence
        // of dynamically allocated buffers.

        // DATA
        char         *d_currentBuffer_p;    // pointer to current buffer

        int           d_currentBufferSize;  // size of current buffer

        bsls::BlockGrowth::Strategy
                      d_growthStrategy;     // growth strategy

        my_BlockList  d_blockList;          // manager for all allocated memory
                                            // blocks

        // NOT IMPLEMENTED
        my_SequentialPool(const my_SequentialPool&);
        my_SequentialPool& operator=(const my_SequentialPool&);

      private:
        // PRIVATE MANIPULATORS
        int calculateNextSize(int size);
            // Return the next buffer size sufficient to satisfy a memory
            // allocation request of the specified 'size' (in bytes).

      public:
        // CREATORS
        my_SequentialPool(bsls::BlockGrowth::Strategy  strategy);
            // Create a pool with the specified memory block growth 'strategy'.

        // ...

        // MANIPULATORS
        void *allocate(int size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).  If the pool cannot return the
            // requested number of bytes, 'std::bad_alloc' will be thrown in an
            // exception-enabled build, or the program will be aborted.  The
            // behavior is undefined unless 'size > 0'.
    };
//..
// The implementation for the rest of the class is elided as the function
// 'calculateNextSize' alone is sufficient to illustrate the use of this
// component:
//..
    // PRIVATE MANIPULATORS
    int my_SequentialPool::calculateNextSize(int size)
    {
        if (bsls::BlockGrowth::BSLS_CONSTANT == d_growthStrategy) {
            return d_currentBufferSize;                               // RETURN
        }
//..
// Note that, if the growth strategy in effect is constant growth
// ('BSLS_CONSTANT'), the size of the internal buffers will always be the same.
// If 'size' is greater than the buffer size, the implementation of 'allocate'
// will return a block having the exact 'size' from the internal block list:
//..
        int nextSize = d_currentBufferSize;

        do {
            nextSize *= 2;  // growth factor of 2
        } while (nextSize < size);
//..
// Note that, if the growth strategy in effect is geometric growth
// ('BSLS_GEOMETRIC'), the size of the internal buffer grows geometrically by a
// factor of 2:
//..
        return nextSize;
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Copy usage example from the header file, uncomment the code and
        //   change all 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // VALUE TEST
        //
        // Concerns:
        //   1) Enumerators hold expected values.
        //   2) 'toAscii' returns the correct ascii representation of the
        //      enumerators.
        //
        // Plan:
        //   Using the table driven technique, create test vectors that contain
        //   the enumerators, expected enumerator values and ascii
        //   representations.  Verify that the enumerators have the expected
        //   values and output.
        //
        // Testing:
        //   enum Strategy { ... };
        //   char *toAscii(bsls::BlockGrowth::Strategy)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "VALUE TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "\nTesting enumerator value and 'toAscii'."
                          << endl;

        int invalidEnumValues[] = { 2, -1, 10 };
        static const struct {
            int         d_line;   // line number
            Enum        d_enum;   // enumerator
            const char *d_ascii;  // ascii representation
            int         d_value;  // expected value
        } DATA[] = {
           // LINE    ENUMERATOR                  ASCII              VALUE
           // ----    ----------                  -----              -----
            { L_,     Class::BSLS_GEOMETRIC,      "GEOMETRIC",        0  },
            { L_,     Class::BSLS_CONSTANT,       "CONSTANT",         1  },
            { L_,     (Enum)invalidEnumValues[0], "(* UNKNOWN *)",   -1  },
            { L_,     (Enum)invalidEnumValues[1], "(* UNKNOWN *)",   -1  },
            { L_,     (Enum)invalidEnumValues[2], "(* UNKNOWN *)",   -1  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const Enum ENUMERATOR = DATA[ti].d_enum;
            const char *ASCII     = DATA[ti].d_ascii;
            const int VALUE       = DATA[ti].d_value;

            if (veryVerbose) {
                T_ P_(LINE) P_((int) ENUMERATOR) P_(ASCII) P(VALUE)
            }

            const char *result = bsls::BlockGrowth::toAscii(ENUMERATOR);
            LOOP3_ASSERT(LINE, ASCII, result, strlen(ASCII) == strlen(result));
            LOOP3_ASSERT(LINE, ASCII, result, 0 == strcmp(ASCII, result));
            if (VALUE >= 0) {
                ASSERT(VALUE == (int) ENUMERATOR);
            }
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
