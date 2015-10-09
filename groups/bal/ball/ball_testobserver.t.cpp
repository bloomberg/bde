// ball_testobserver.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_testobserver.h>

#include <ball_context.h>                       // for testing only
#include <ball_record.h>                        // for testing only
#include <ball_recordattributes.h>              // for testing only

#include <bdlt_datetimeutil.h>

#include <bslim_testutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <ball_userfields.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD
//-----------------------------------------------------------------------------
// [  ] ball::TestObserver(bslma::Allocator *ba = 0);
// [  ] ball::TestObserver(ball::Observer *observer, *ba = 0);
// [  ] virtual ~ball::TestObserver();
// [  ] virtual void publish(const record&, const attributes&) = 0;
// [  ] int registerObserver(ball::Observer *observer);
// [  ] int deregisterObserver(ball::Observer *observer);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST - BUG
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::TestObserver Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
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
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            ball::RecordAttributes attributes;
            ball::UserFields       fieldValues;
            ball::Record           record(attributes, fieldValues);
            ball::Context          context;

                                              ASSERT(0 == Obj::numInstances());
            ball::TestObserver to1(bsl::cout); ASSERT(1 == to1.id());
                                              ASSERT(1 == Obj::numInstances());
            ball::TestObserver to2(bsl::cout); ASSERT(2 == to2.id());
                                              ASSERT(2 == Obj::numInstances());
            ball::TestObserver to3(bsl::cout); ASSERT(3 == to3.id());
                                              ASSERT(3 == Obj::numInstances());

                                        ASSERT(0 == to1.numPublishedRecords());
                                        ASSERT(0 == to2.numPublishedRecords());
                                        ASSERT(0 == to3.numPublishedRecords());

            to1.setVerbose(verbose); // silences 'publish' in production
            to1.publish(record, context);
                                        ASSERT(1 == to1.numPublishedRecords());
            to2.publish(record, context);
                                        ASSERT(1 == to2.numPublishedRecords());
                                        ASSERT(0 == to3.numPublishedRecords());
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: BUG
        //   We must ensure that (1) a subclass of the 'ball::Observer' class
        //   compiles and links when all virtual functions are defined, and
        //   (2) the functions are in fact virtual.
        //
        // Plan: BUG
        //   Construct an object of a class derived from 'ball::Observer' and
        //   bind a 'ball::Observer' reference to the object.  Using the base
        //   class reference, invoke the 'publish' method and destructor.
        //   Verify that the correct implementations of the methods are called.
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            ball::RecordAttributes attributes;
            ball::UserFields  fieldValues;
            ball::Record           record(attributes, fieldValues);
            ball::Context          context;

            ASSERT(0 == Obj::numInstances());

            if (verbose)
                cout << "\nInstantiate a test observer 'mX1':" << endl;

            Obj mX1(bsl::cout);  const Obj& X1 = mX1;
            ASSERT(1 == Obj::numInstances());
            ASSERT(1 == X1.id());
            ASSERT(0 == X1.numPublishedRecords());
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose)
                cout << "\nInstantiate a test observer 'mX2':" << endl;

            Obj mX2(bsl::cout);  const Obj& X2 = mX2;
            ASSERT(2 == Obj::numInstances());
            ASSERT(2 == X2.id());
            ASSERT(0 == X2.numPublishedRecords());
            if (verbose) { T_; P_(X2.id());  P(X2.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX1':" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(1 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord() == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish the record again:" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord() == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX2':" << endl;

            mX2.setVerbose(veryVerbose);
            mX2.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(1 == X2.numPublishedRecords());
            ASSERT(X2.lastPublishedRecord() == record);
            ASSERT(X2.lastPublishedContext() == context);
            if (verbose) { T_; P_(X2.id());  P(X2.numPublishedRecords()); }

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
