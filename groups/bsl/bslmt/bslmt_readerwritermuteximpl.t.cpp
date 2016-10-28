// bslmt_readerwritermuteximpl.t.cpp                                  -*-C++-*-

#include <bslmt_readerwritermuteximpl.h>

#include <bslmt_barrier.h>
#include <bslmt_readerwriterlock.h>
#include <bslmt_semaphore.h>
#include <bslmt_threadattributes.h>
#include <bslmt_threadutil.h>
#include <bslmt_threadgroup.h>
#include <bslmt_platform.h>

#include <bslim_testutil.h>

#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_algorithm.h>
#include <bsl_functional.h>
#include <bsl_numeric.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

// ============================================================================
//                   GLOBAL STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

struct TestImpl {
    static const bsls::Types::Int64 k_READER         = 0x0000000000000001LL;
    static const bsls::Types::Int64 k_PENDING_WRITER = 0x0000000100000000LL;
    static const bsls::Types::Int64 k_WRITER         = 0x1000000000000000LL;

    static bsls::AtomicOperations::AtomicTypes::Int64 *s_pState;

    static std::vector<int>                            s_script;
    static int                                         s_scriptAt;

    enum {
        k_INIT    =  -1,
        k_GET     =  -2,
        k_ADDAR   =  -3,
        k_ADDNV   =  -4,
        k_ADDNVAR =  -5,
        k_CAS     =  -6,
        k_LOCK    =  -7,
        k_UNLOCK  =  -8,
        k_WAIT    =  -9,
        k_POST    = -10
    };

    static void assignScript(const std::vector<int>& script) {
        s_script = script;
        s_scriptAt = 0;
    }

    static void verifyFunction(const int expectedFunctionId) {
        ASSERT(0 != s_pState);

        if (s_pState) {
            while (   s_scriptAt < s_script.size()
                   && s_script[s_scriptAt] >= 0) {
                int script           = s_script[s_scriptAt];
                int numReader        = script % 10;
                int numPendingWriter = (script / 10) % 10;
                int numWriter        = (script / 100) % 10;
                bsls::AtomicOperations::setInt64(
                                         s_pState,
                                         (  k_READER * numReader
                                          + k_PENDING_WRITER * numPendingWriter
                                          + k_WRITER * numWriter));
                ++s_scriptAt;
            }

            ASSERT(s_scriptAt         <  s_script.size());
            ASSERT(expectedFunctionId == s_script[s_scriptAt++]);
        }
    }

    // ATOMIC_OP implementations
    static void initInt64(bsls::AtomicOperations::AtomicTypes::Int64 *pState) {
        s_pState = pState;

        verifyFunction(k_INIT);

        bsls::AtomicOperations::initInt64(s_pState);
    }

    static bsls::Types::Int64 getInt64(
                          bsls::AtomicOperations::AtomicTypes::Int64 *pState) {
        ASSERT(pState == s_pState);

        verifyFunction(k_GET);
        return bsls::AtomicOperations::getInt64(s_pState);
    }

    static void addInt64AcqRel(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_pState);

        verifyFunction(k_ADDAR);
        bsls::AtomicOperations::addInt64AcqRel(s_pState, value);
    }

    static bsls::Types::Int64 addInt64Nv(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_pState);

        verifyFunction(k_ADDNV);
        return bsls::AtomicOperations::addInt64Nv(s_pState, value);
    }

    static bsls::Types::Int64 addInt64NvAcqRel(
                           bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                           bsls::Types::Int64                          value) {
        ASSERT(pState == s_pState);

        verifyFunction(k_ADDNVAR);
        return bsls::AtomicOperations::addInt64NvAcqRel(s_pState, value);
    }

    static bsls::Types::Int64 testAndSwapInt64AcqRel(
                        bsls::AtomicOperations::AtomicTypes::Int64 *pState,
                        bsls::Types::Int64                          value,
                        bsls::Types::Int64                          newValue) {
        ASSERT(pState == s_pState);

        verifyFunction(k_CAS);
        return bsls::AtomicOperations::testAndSwapInt64AcqRel(s_pState,
                                                              value,
                                                              newValue);
    }

    // MUTEX
    static void lock() {
        verifyFunction(k_LOCK);
    }

    static void unlock() {
        verifyFunction(k_UNLOCK);
    }

    // SEMAPHORE
    static void wait() {
        verifyFunction(k_WAIT);
    }

    static void post() {
        verifyFunction(k_POST);
    }
};

bsls::AtomicOperations::AtomicTypes::Int64 *TestImpl::s_pState   = 0;
std::vector<int>                            TestImpl::s_script;
int                                         TestImpl::s_scriptAt = 0;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ReaderWriterMutexImpl<TestImpl, TestImpl, TestImpl>  Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    int readers = 5, writers = 1; // for negative cases
    if (argc > 2) {
       readers = atoi(argv[2]);
    }
    if (argc > 3) {
       cout << "WARNING: setting # of writers not supported yet" << endl;
       //writers = atoi(argv[3]);
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
          Obj obj;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
