// bslmt_readerwritermutex.t.cpp                                      -*-C++-*-

#include <bslmt_readerwritermutex.h>

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bslmt::ReaderWriterMutexImpl' is the templated-for-testing implementation
// of a reader-writer lock.  The templatization allows for the creation of a
// script-based testing object, 'TestImpl', that enables simplified testing of
// the concerns for each method.  The methods of 'bslmt::ReaderWriterMutexImpl'
// are tested by directly exercising the functionality or by using the
// depth-limited enumeration technique.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] ReaderWriterMutex();
// [ 2] ~ReaderWriterMutex();
//
// MANIPULATORS
// [ 2] void lockRead();
// [ 2] void lockWrite();
// [ 2] int tryLockRead();
// [ 2] int tryLockWrite();
// [ 2] void unlock();
// [ 2] void unlockRead();
// [ 2] void unlockWrite();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] FAIR LOCK ACQUISITION

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

// ============================================================================
//                   GLOBAL STRUCTS FOR TESTING
// ----------------------------------------------------------------------------

struct ThreadData {
    bslmt::ReaderWriterMutex *d_mutex_p;
    bsls::Types::size_type    d_writeCount;
    bsls::Types::size_type    d_readCount;

    ThreadData() : d_mutex_p(0), d_writeCount(0), d_readCount(0) {}

    bool isFair(bsls::Types::size_type numWriter,
                bsls::Types::size_type numReader)
    {
        bsls::Types::size_type count         = d_writeCount + d_readCount;
        bsls::Types::size_type num           = numWriter + numReader;
        bsls::Types::size_type expWriteCount = numWriter * count / num;
        bsls::Types::size_type expReadCount  = numReader * count / num;

        return d_writeCount * 5 >= 4 * expWriteCount
            && d_readCount  * 5 >= 4 * expReadCount;
    }
};

// ============================================================================
//                   GLOBAL METHODS FOR TESTING
// ----------------------------------------------------------------------------

extern "C" void *writeLockCount(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);
    return 0;
}

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslmt::ReaderWriterMutex Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    //int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create objects.
        //:
        //: 2 Exercise these objects using primary manipulators.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj obj;

        obj.lockRead();
        obj.unlock();

        obj.lockWrite();
        obj.unlock();
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
