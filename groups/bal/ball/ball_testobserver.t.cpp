// ball_testobserver.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_testobserver.h>

#include <ball_recordattributes.h>
#include <ball_userfields.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmf_assert.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>       // time()

#include <bsl_iostream.h>
#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a test observer ('ball::TestObserver') that
// is intended for use in other 'ball' test drivers.  It is sufficient to
// verify the functionality of the test observer in a single test case (case 2)
// using ad hoc testing.
//-----------------------------------------------------------------------------
// [ 2] static int numInstances();
// [ 2] TestObserver(bsl::ostream *stream, bslma::Allocator *ba = 0);
// [ 2] virtual ~TestObserver();
// [ 2] virtual void publish(const Record&, const Context&);
// [ 2] virtual void publish(const shared_ptr<const Record>&, context);
// [ 2] virtual void releaseRecords();
// [ 2] void setVerbose(int flagValue);
// [ 2] int id() const;
// [ 2] const Context& lastPublishedContext() const;
// [ 2] const Record& lastPublishedRecord() const;
// [ 2] int numPublishedRecords() const;
// [ 2] int numReleases() const;
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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::TestObserver Obj;

const int SEQUENCE_LENGTH = 99;

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
    // The following helper functions are used to test the 'publish' method.

static
int nextRecord(ball::Record& record)
    // Modify the specified 'record' to have a value distinct from any other
    // record transmitted so far in the publication sequence and return the
    // 0-based sequence number of 'record'.
{
    static int sequenceNumber = 0;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    ball::RecordAttributes attr(&scratch);
    attr.setLineNumber(sequenceNumber);
    attr.setProcessID(sequenceNumber);
    attr.setSeverity(sequenceNumber);
    attr.setThreadID(sequenceNumber);

    // force a memory allocation in the test observer
    attr.setCategory(SUFFICIENTLY_LONG_STRING);

    record.setFixedFields(attr);

    return sequenceNumber++;
}

static
bool isNthRecord(const ball::Record& record, int nth)
    // Return 'true' if the specified 'record' is the specified 'nth' in the
    // publication sequence, and 'false' otherwise.
{
    const ball::RecordAttributes& attr = record.fixedFields();

    if (nth == attr.lineNumber() && nth == attr.processID()
     && nth == attr.severity()   && nth == static_cast<int>(attr.threadID())) {
        return true;                                                  // RETURN
    }

    return false;
}

static
bool isNthContext(const ball::Context& context, int nth)
    // Return 'true' if the specified 'context' is the specified 'nth' in the
    // publication sequence, and 'false' otherwise.
{
    if (ball::Transmission::e_TRIGGER == context.transmissionCause()
        &&                        nth == context.recordIndex()
        &&            SEQUENCE_LENGTH == context.sequenceLength()) {
        return true;                                                  // RETURN
    }

    return false;
}

}  // close unnamed namespace

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int         test = argc > 1 ? atoi(argv[1]) : 0;
    const bool     verbose = argc > 2;
    const bool veryVerbose = argc > 3;

    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        {
            ball::RecordAttributes attributes;
            ball::UserFields       fieldValues;
            ball::Context          context;

            bslma::TestAllocator ga("scratch", veryVeryVeryVerbose);

            const bsl::shared_ptr<const ball::Record>
              record(new (ga) ball::Record(attributes, fieldValues, &ga), &ga);

            ASSERT(0 == Obj::numInstances());

            ball::TestObserver to1(&bsl::cout);
            ASSERT(1 == to1.id());
            ASSERT(1 == Obj::numInstances());

            ball::TestObserver to2(&bsl::cout);
            ASSERT(2 == to2.id());
            ASSERT(2 == Obj::numInstances());

            ball::TestObserver to3(&bsl::cout);
            ASSERT(3 == to3.id());
            ASSERT(3 == Obj::numInstances());

            ASSERT(0 == to1.numPublishedRecords());
            ASSERT(0 == to2.numPublishedRecords());
            ASSERT(0 == to3.numPublishedRecords());

            to1.setVerbose(verbose);  // silences 'publish' in production
            to1.publish(record, context);
            ASSERT(1 == to1.numPublishedRecords());

            to2.publish(record, context);
            ASSERT(1 == to2.numPublishedRecords());
            ASSERT(0 == to3.numPublishedRecords());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // METHODS TEST
        //
        // Concerns:
        //: 1 All 'virtual' methods in the 'ball::Observer' protocol are
        //:   implemented.
        //:
        //: 2 The 'publish' and 'releaseRecords' methods have the expected
        //:   effect.
        //:
        //: 3 All accessors return the expected values.
        //:
        //: 4 Each object is given a unique id.
        //:
        //: 5 All memory comes from the allocator supplied (explicitly or
        //:   implicitly) at construction.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use ad hoc testing to verify the correct operation of all methods
        //:   of the type under test.  (C-1..5)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   static int numInstances();
        //   TestObserver(bsl::ostream *stream, bslma::Allocator *ba = 0);
        //   virtual ~TestObserver();
        //   virtual void publish(const Record&, const Context&);
        //   virtual void publish(const shared_ptr<const Record>&, context);
        //   virtual void releaseRecords();
        //   void setVerbose(int flagValue);
        //   int id() const;
        //   const Context& lastPublishedContext() const;
        //   const Record& lastPublishedRecord() const;
        //   int numPublishedRecords() const;
        //   int numReleases() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHODS TEST" << endl
                          << "============" << endl;

        ASSERT(0 == Obj::numInstances());

        if (verbose) cout << "\t'publish' method taking 'shared_ptr'." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&bsl::cout);  const Obj& X = mX;

            ASSERT(1 == Obj::numInstances());
            ASSERT(1 == X.id());
            ASSERT(0 == X.numPublishedRecords());
            ASSERT(0 == X.numReleases());
            ASSERT(0 == da.numBlocksTotal());

            mX.setVerbose(veryVerbose);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsl::shared_ptr<ball::Record>
                            mR(new (scratch) ball::Record(&scratch), &scratch);
            const bsl::shared_ptr<const ball::Record>& R = mR;

            ball::Context mC(ball::Transmission::e_TRIGGER,
                             0, SEQUENCE_LENGTH);
            const ball::Context& C = mC;

            const int I1 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I1));
            mC.setRecordIndexRaw(I1);        ASSERT(1 == isNthContext(C, I1));

            ball::Observer& mB = mX; // call 'virtual' methods through protocol

            mB.publish(R, C);

            ASSERT(1 == X.numPublishedRecords());
            ASSERT(0 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I1));
            ASSERT(     isNthContext(X.lastPublishedContext(), I1));

            mB.releaseRecords();

            ASSERT(1 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I1));
            ASSERT(     isNthContext(X.lastPublishedContext(), I1));

            const int I2 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I2));
            mC.setRecordIndexRaw(I2);        ASSERT(1 == isNthContext(C, I2));

            mB.publish(R, C);

            ASSERT(2 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I2));
            ASSERT(     isNthContext(X.lastPublishedContext(), I2));

            const int I3 = nextRecord(*mR);  ASSERT(1 == isNthRecord(*R, I3));
            mC.setRecordIndexRaw(I3);        ASSERT(1 == isNthContext(C, I3));

            mB.publish(R, C);

            ASSERT(3 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I3));
            ASSERT(     isNthContext(X.lastPublishedContext(), I3));

            mB.releaseRecords();

            ASSERT(3 == X.numPublishedRecords());
            ASSERT(2 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I3));
            ASSERT(     isNthContext(X.lastPublishedContext(), I3));

            ASSERT(0 < da.numBlocksTotal());
        }
        ASSERT(1 == Obj::numInstances());

        if (verbose) cout << "\tDEPRECATED 'publish' method." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&bsl::cout, &oa);  const Obj& X = mX;

            ASSERT(2 == Obj::numInstances());
            ASSERT(2 == X.id());
            ASSERT(0 == X.numPublishedRecords());
            ASSERT(0 == X.numReleases());
            ASSERT(0 == da.numBlocksTotal());
            ASSERT(0 == oa.numBlocksTotal());

            mX.setVerbose(veryVerbose);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            ball::Record mR(&scratch);  const ball::Record& R = mR;

            ball::Context mC(ball::Transmission::e_TRIGGER,
                             0,
                             SEQUENCE_LENGTH);
            const ball::Context& C = mC;

            const int I1 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I1));
            mC.setRecordIndexRaw(I1);       ASSERT(1 == isNthContext(C, I1));

            ball::Observer& mB = mX; // call 'virtual' methods through protocol

            mB.publish(R, C);

            ASSERT(1 == X.numPublishedRecords());
            ASSERT(0 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I1));
            ASSERT(     isNthContext(X.lastPublishedContext(), I1));

            mB.releaseRecords();

            ASSERT(1 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I1));
            ASSERT(     isNthContext(X.lastPublishedContext(), I1));

            const int I2 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I2));
            mC.setRecordIndexRaw(I2);       ASSERT(1 == isNthContext(C, I2));

            mB.publish(R, C);

            ASSERT(2 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I2));
            ASSERT(     isNthContext(X.lastPublishedContext(), I2));

            const int I3 = nextRecord(mR);  ASSERT(1 == isNthRecord(R, I3));
            mC.setRecordIndexRaw(I3);       ASSERT(1 == isNthContext(C, I3));

            mB.publish(R, C);

            ASSERT(3 == X.numPublishedRecords());
            ASSERT(1 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I3));
            ASSERT(     isNthContext(X.lastPublishedContext(), I3));

            mB.releaseRecords();

            ASSERT(3 == X.numPublishedRecords());
            ASSERT(2 == X.numReleases());
            ASSERT(     isNthRecord( X.lastPublishedRecord(),  I3));
            ASSERT(     isNthContext(X.lastPublishedContext(), I3));

            ASSERT(0 == da.numBlocksTotal());
            ASSERT(0 <  oa.numBlocksTotal());
        }
        ASSERT(2 == Obj::numInstances());

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        // Use DEPRECATED constructor.
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            const Obj X(bsl::cout, &oa);
            ASSERT(3 == Obj::numInstances());

            const Obj Y(bsl::cout);
            ASSERT(4 == Obj::numInstances());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

        if (verbose) cout << "\nNegative Testing" << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_SAFE_PASS(Obj(&bsl::cout, 0));
                ASSERT_SAFE_FAIL(Obj(         0, 0));
            }

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bsl::shared_ptr<ball::Record>
                            mR(new (scratch) ball::Record(&scratch), &scratch);

                const ball::Context C;

                Obj mX(&bsl::cout);

                             ASSERT_PASS(mX.publish(mR, C));
                mR.reset();  ASSERT_FAIL(mX.publish(mR, C));
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
        //: 1 Create two test observers and exercise them such that sufficient
        //:   confidence is gained that they function correctly.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {
            ball::RecordAttributes attributes;
            ball::UserFields       fieldValues;
            ball::Record           record(attributes, fieldValues);
            ball::Context          context;

            ASSERT(0 == Obj::numInstances());

            if (verbose)
                cout << "\nInstantiate a test observer 'mX1':" << endl;

            Obj mX1(&bsl::cout);  const Obj& X1 = mX1;
            ASSERT(1 == Obj::numInstances());
            ASSERT(1 == X1.id());
            ASSERT(0 == X1.numPublishedRecords());
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose)
                cout << "\nInstantiate a test observer 'mX2':" << endl;

            Obj mX2(&bsl::cout);  const Obj& X2 = mX2;
            ASSERT(2 == Obj::numInstances());
            ASSERT(2 == X2.id());
            ASSERT(0 == X2.numPublishedRecords());
            if (verbose) { T_; P_(X2.id());  P(X2.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX1':" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(1 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord()  == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish the record again:" << endl;

            mX1.setVerbose(veryVerbose);
            mX1.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(0 == X2.numPublishedRecords());
            ASSERT(X1.lastPublishedRecord()  == record);
            ASSERT(X1.lastPublishedContext() == context);
            if (verbose) { T_; P_(X1.id());  P(X1.numPublishedRecords()); }

            if (verbose) cout << "\nPublish a record on 'mX2':" << endl;

            mX2.setVerbose(veryVerbose);
            mX2.publish(record, context);
            ASSERT(2 == X1.numPublishedRecords());
            ASSERT(1 == X2.numPublishedRecords());
            ASSERT(X2.lastPublishedRecord()  == record);
            ASSERT(X2.lastPublishedContext() == context);
            if (verbose) { T_; P_(X2.id());  P(X2.numPublishedRecords()); }
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

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
