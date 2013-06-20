// bslalg_dequeimputil.t.cpp                                          -*-C++-*-

#include <bslalg_dequeimputil.h>

#include <bslmf_issame.h>

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component provides an extremely simple 'struct' with only types and
// integral 'enum' values.  The concerns for testing are merely that the
// computation of 'BLOCK_LENGTH' be correct and that the names are as expected.
//-----------------------------------------------------------------------------
// [ 2] 'bslalg::DequeImp::BLOCK_LENGTH'
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
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
//--------------------------------------------------------------------------
// NOTE: This implementation of LOOP_ASSERT macros must use printf since
//       cout uses new and be called during exception testing.
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { printf("%s: %d\n", #I, I); aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
// #define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) printf("<| " #X " |>\n");  // Quote identifier literally.
//#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ printf("\t");             // Print a tab (w/o newline)

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bslstl::DequeImpUtil' TEMPLATE
        //
        // Concerns:  That all the names are defined.
        //
        // Plan:  Since the component under test is a template, we cannot use
        //   the table data selection method, but instead use a test macro,
        //   with two parameters containing the value of the template
        //   arguments.  We then instantiate the macro for various parameters
        //   that exercise the various branches and boundary conditions.
        //
        // Testing:
        //   class template 'bslstl::DequeImp'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'bslalg::DequeImp"
                            "\n=========================\n");

#define TEST_DEQUE_IMP(LINE_NUM, VALUE_TYPE, BLOCK_LENGTH)  {                \
                const int LINE               = LINE_NUM;                     \
                                                                             \
                typedef bslalg::DequeImpUtil<VALUE_TYPE,                     \
                                            BLOCK_LENGTH> Obj;               \
                                                                             \
                LOOP_ASSERT(LINE, (bsl::is_same<VALUE_TYPE,                 \
                                                Obj::ValueType>::value));    \
                                                                             \
                LOOP_ASSERT(LINE, Obj::BLOCK_BYTES == sizeof(Obj::Block));   \
                LOOP_ASSERT(LINE, (bsl::is_same<Obj::BlockPtr,              \
                                               Obj::Block *>::value));       \
            }

        TEST_DEQUE_IMP(L_, char, 1);
        TEST_DEQUE_IMP(L_, char, 4096);

        TEST_DEQUE_IMP(L_, void *, 1);
        TEST_DEQUE_IMP(L_, void *, sizeof(void *));
        TEST_DEQUE_IMP(L_, void *, 200);
        TEST_DEQUE_IMP(L_, void *, 4095);
        TEST_DEQUE_IMP(L_, void *, 4096);

        typedef char XXL[4096];
        TEST_DEQUE_IMP(L_, XXL, 1);
        TEST_DEQUE_IMP(L_, XXL, 200);
        TEST_DEQUE_IMP(L_, XXL, 4095);
        TEST_DEQUE_IMP(L_, XXL, sizeof(XXL));
        TEST_DEQUE_IMP(L_, XXL, 8192);
        TEST_DEQUE_IMP(L_, XXL, 16384);

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //   That the basic usage is functional and correct.
        //
        // Plan:  Exercise basic usage of this component.
        //
        // Testing:
        //   This test exercises basic usage but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============");

        enum { BLOCK_LENGTH = 200 };
        typedef bslalg::DequeImpUtil<char, BLOCK_LENGTH> Obj;

        Obj::Block block[4];
        std::memset(block[0].d_data, 'A', BLOCK_LENGTH);
        std::memset(block[1].d_data, 'B', BLOCK_LENGTH);
        std::memset(block[2].d_data, 'C', BLOCK_LENGTH);
        std::memset(block[3].d_data, 'D', BLOCK_LENGTH);

        ASSERT(200 == Obj::BLOCK_BYTES);

        Obj::BlockPtr blockPtr = &block[0];
        ASSERT('A' == blockPtr->d_data[0]);
        ++blockPtr;
        ASSERT('B' == blockPtr->d_data[1]);
        ++blockPtr;
        ASSERT('C' == blockPtr->d_data[2]);
        ++blockPtr;
        ASSERT('D' == blockPtr->d_data[3]);

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
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
