// btlso_timemetrics.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_timemetrics.h>

#include <bslmt_threadutil.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// [ 2] USAGE TEST
// [ 1] BREATHING TEST

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
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// The usage shows the basic usage with a 'btlso::TimeMetrics'.
//
// First, we specify an enumeration listing the categories that we want to
//measure.  For this example, we will specify two categories:
//..
    enum {
        e_CPU_CATEGORY   = 0,
        e_IO_CATEGORY    = 1,
        e_NUM_CATEGORIES = e_IO_CATEGORY + 1
    };
//..
// Then, create a metrics object, keeping two categories, and initially
// measuring time in the 'CPU' category:
//..
    btlso::TimeMetrics metrics(e_NUM_CATEGORIES, e_CPU_CATEGORY);
//..
// In order to measure time spent in I/O, e.g., doing 'select' calls, we do:
//..
    // perform initializations for 'select'
//
    metrics.switchTo(e_IO_CATEGORY);
//
    // do some IO, e.g., 'select'
//..
// To switch to measuring CPU time, e.g., doing event dispatch, we do:
//..
    metrics.switchTo(e_CPU_CATEGORY);
//
    // dispatch events
//..
// At the end of the computation, or periodically, one may report the time
// spent in each category as follows:
//..
    bsl::cout << "The total time spent in IO was "
              << metrics.percentage(e_IO_CATEGORY)
              << bsl::endl;
//
    bsl::cout << "The total time spent in CPU was "
              << metrics.percentage(e_CPU_CATEGORY)
              << bsl::endl;
//..
// This metrics may be reset to its initial state by:
//..
    metrics.resetAll();
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //  BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        enum {
            CPU_CATEGORY = 0,
            IO_CATEGORY = 1,
            NUM_CATEGORIES = IO_CATEGORY + 1
        };

        btlso::TimeMetrics mX(NUM_CATEGORIES, CPU_CATEGORY);
        const btlso::TimeMetrics& X = mX;

        ASSERT(NUM_CATEGORIES == X.numCategories());
        ASSERT(CPU_CATEGORY   == X.currentCategory());

        double x;
        for (int i = 0; i < 5000000; ++i) {
            x = (i + 2) * (i + 1) * i;
        }
        ASSERT(x);

        mX.switchTo(IO_CATEGORY);
        ASSERT(IO_CATEGORY == X.currentCategory());
        bslmt::ThreadUtil::microSleep(1000000); // 1 second

        mX.switchTo(CPU_CATEGORY);

        ASSERT(100 >= mX.percentage(CPU_CATEGORY));
        ASSERT(0   <= mX.percentage(CPU_CATEGORY));

        ASSERT(100 >= mX.percentage(IO_CATEGORY));
        ASSERT(0   <= mX.percentage(IO_CATEGORY));

        if (veryVerbose) {
            P(mX.percentage(IO_CATEGORY))
            P(mX.percentage(CPU_CATEGORY))
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
// Copyright 2015 Bloomberg Finance L.P.
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
