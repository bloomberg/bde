// bdlqq_semaphore.t.cpp                                              -*-C++-*-
#include <bdlqq_semaphore.h>

#include <bdlqq_threadattributes.h>     // for testing only
#include <bdlqq_threadutil.h>           // for testing only

#include <bsls_timeinterval.h>
#include <bdlf_bind.h>
#include <bsls_atomic.h>
#include <bsl_iostream.h>
#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//
//
//-----------------------------------------------------------------------------
// [1] Breathing test
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlqq::Semaphore Obj;

void waitAndSet(bdlqq::Semaphore *obj,
                bsls::AtomicInt *val)
{
    obj->wait();
    (*val) = 1;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) {
      case 2: {
        // --------------------------------------------------------------------
        // BLOCKING TEST
        //
        // Test that a thread can block on the semaphore.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Blocking Test" << endl
                 << "=============" << endl;
        }
        {
            Obj X;
            bdlqq::ThreadUtil::Handle h;
            bsls::AtomicInt flag;
            ASSERT(0 == bdlqq::ThreadUtil::create(
                            &h, bdlf::BindUtil::bind(&waitAndSet, &X, &flag)));
            bdlqq::ThreadUtil::sleep(bsls::TimeInterval(1));
            ASSERT(0 == flag);
            X.post();
            bdlqq::ThreadUtil::join(h);
            ASSERT(1 == flag);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Breathing Test" << endl
                 << "==============" << endl;
        }
        {
            Obj X;
            X.post();
            X.post(2);
            X.wait();
            X.wait();
            ASSERT(0 == X.tryWait());
            ASSERT(0 != X.tryWait());
        }
      } break;

      default: {
          testStatus = -1; break;
      }

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
