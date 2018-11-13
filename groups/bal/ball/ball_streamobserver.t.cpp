// ball_streamobserver.t.cpp                                          -*-C++-*-
#include <ball_streamobserver.h>

#include <ball_context.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_userfields.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

// Note: on Windows -> WinGDI.h:#define PASSTHROUGH 19
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(PASSTHROUGH)
#undef PASSTHROUGH
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under the test is a simple observer that prints the log
// records to the 'bsl::ostream' supplied at construction.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] StreamObserver(bsl::ostream *stream)
// [ 2] virtual ~StreamObserver()
//
// MANIPULATORS
// [ 2] virtual void publish(const shared_ptr<const Record>&, Context&);
// [ 2] virtual void releaseRecords();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::StreamObserver Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;      // Supress compiler warning.
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

        ball::RecordAttributes attributes;
        ball::UserFields       fieldValues;
        ball::Context          context;

        bslma::TestAllocator ga("scratch", veryVeryVeryVerbose);

        const bsl::shared_ptr<const ball::Record>
            record(new (ga) ball::Record(attributes, fieldValues, &ga), &ga);

        ball::StreamObserver observer(&bsl::cout);

        observer.publish(record, context);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 Log record is correctly formatted.
        //:
        //: 2 Formatted log record is written to the output stream.
        //
        // Plan:
        //: 1 Create the observer object and publish log record.
        //
        // Testing:
        //   StreamObserver(bsl::ostream *stream)
        //   virtual ~StreamObserver()
        //   virtual void publish(const shared_ptr<const Record>&, Context&);
        //   virtual void releaseRecords();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================" << endl;

        {
            Obj mX(&bsl::cout);
        }

        if (verbose) cout << "Publish a single message." << endl;
        {
            bsl::ostringstream os;
            Obj mX(&os);

            ball::RecordAttributes fixed;

            bdlt::Datetime timestamp(2017, 4, 1);
            fixed.setTimestamp(timestamp);
            fixed.setProcessID(1);
            fixed.setThreadID(2);
            fixed.setSeverity(ball::Severity::e_INFO);
            fixed.setFileName("test.cpp");
            fixed.setLineNumber(189);
            fixed.setMessage("Log Message");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            mX.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                             0,
                                             1));

            ASSERTV(os.str(),
                    "\n01APR2017_00:00:00.000 1 2 INFO test.cpp 189"
                    "  Log Message \n"
                    == os.str());
        }

        if (verbose) cout << "\nNegative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_SAFE_PASS((Obj((&bsl::cout))));  // most vexing parse
                ASSERT_SAFE_FAIL((Obj((         0))));
            }

            {
                ball::Record        mR;
                const ball::Context C;

                Obj mX(&bsl::cout);

                ASSERT_OPT_FAIL_RAW(mX.publish(mR, C));
            }

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::shared_ptr<ball::Record>
                            mR(new (scratch) ball::Record(&scratch), &scratch);

                const ball::Context C;

                Obj mX(&bsl::cout);

                ASSERT_PASS(mX.publish(mR, C));
                mR.reset();
                ASSERT_FAIL(mX.publish(mR, C));
            }
        }
      } break;
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
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;


        if (verbose) cout << "Publish a single message." << endl;
        {
            bsl::ostringstream os;
            Obj X(&os);

            ball::RecordAttributes fixed;

            bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
            fixed.setTimestamp(now);
            fixed.setProcessID(100);
            fixed.setThreadID(0);
            fixed.setMessage("Log Message (single)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();
            record->setFixedFields(fixed);

            X.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                            0,
                                            1));
        }

        if (verbose) cout << "Publish a sequence of three messages." << endl;
        {
            bsl::ostringstream os;
            Obj X(&os);

            ball::RecordAttributes fixed;

            const int NUM_MESSAGES = 3;
            for (int n = 0; n < NUM_MESSAGES; ++n) {
                bdlt::Datetime now =
                                    bdlt::EpochUtil::convertFromTimeT(time(0));
                fixed.setTimestamp(now);
                fixed.setProcessID(201 + n);
                fixed.setThreadID(31 + n);
                fixed.setMessage("Log Message (series)");

                bsl::shared_ptr<ball::Record> record;
                record.createInplace();
                record->setFixedFields(fixed);

                X.publish(record, ball::Context(ball::Transmission::e_TRIGGER,
                                                n,
                                                NUM_MESSAGES));
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
// Copyright 2017 Bloomberg Finance L.P.
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
