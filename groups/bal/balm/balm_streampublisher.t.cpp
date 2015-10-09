// balm_streampublisher.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_streampublisher.h>

#include <balm_metricsample.h>
#include <balm_metricformat.h>

#include <bdlt_datetimetz.h>
#include <bdlt_currenttime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::StreamPublisher Obj;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// In the following example we illustrate how to create and publish records
// with a 'balm::StreamPublisher'.  First we define a couple metric ids.  Note
// that we create the 'balm::MetricId' objects by hand, but in practice an id
// should be obtained from a 'balm::MetricRegistry' object (such as the
// one owned by a 'balm::MetricsManager').
//..
    balm::Category myCategory("MyCategory");
    balm::MetricDescription descA(&myCategory, "MetricA");
    balm::MetricDescription descB(&myCategory, "MetricB");

    balm::MetricId metricA(&descA);
    balm::MetricId metricB(&descB);
//..
// Now we create a 'balm::StreamPublisher' object, supplying it the stdout
// stream.
//..
    balm::StreamPublisher myPublisher(bsl::cout);
//..
// Next we create a metric sample containing a some records and send it to the
// 'balm::StreamPublisher' we created.  Note that because we are not actually
// collecting the metrics we set the elapsed time of the sample to an
// arbitrary 5s interval.
//..
    bslma::Allocator *allocator = bslma::Default::allocator(0);
    bsl::vector<balm::MetricRecord> records(allocator);
//
    records.push_back(balm::MetricRecord(metricA, 5, 25.0, 6.0, 25.0));
    records.push_back(balm::MetricRecord(metricB, 2,  7.0, 3.0, 11.0));

    balm::MetricSample sample(allocator);
//
    sample.setTimeStamp(bdlt::DatetimeTz(bdlt::CurrentTime::utc(), 0));
    sample.appendGroup(records.data(), records.size(),
                       bsls::TimeInterval(5, 0));

    myPublisher.publish(sample);
//..
// The output of this example would look similar to:
//..
// 05FEB2009_19:52:11.723+0000 2 Records
//     Elapsed Time: 5s
//         MyCategory.MetricA [ count = 5, total = 25, min = 6, max = 25 ]
//         MyCategory.MetricB [ count = 2, total = 7, min = 3, max = 11 ]
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using the default, make and
        //   copy constructors.  Exercise these objects using primary
        //   manipulators, basic accessors, equality operators, and the
        //   assignment operator.  Invoke the primary (black box) manipulator
        //   [3&5], copy constructor [2&8], and assignment operator [10&9] in
        //   situations where the internal data (i) does *not* and (ii) *does*
        //   have to resize.  Try aliasing with assignment for a non-empty
        //   instance [11] and allow the result to leave scope, enabling the
        //   destructor to assert internal object invariants.  Display object
        //   values frequently in verbose mode:
        //    1. Create an object x1.                       x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1.           x1:A x2:
        //    4. Append the same element value A to x2.     x1:A x2:A
        //    5. Append another element value B to x2.      x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3.                  x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bslma::TestAllocator ta, da;
        bslma::DefaultAllocatorGuard guard(&da);

        Obj mX(bsl::cout);
        balm::Category myCategory("MyCategory");
        bdlt::DatetimeTz now(bdlt::CurrentTime::utc(), 0);
        for (int i = 0; i < balm::PublicationType::k_LENGTH; ++i) {
            balm::MetricDescription desc(&myCategory, "Test");

            desc.setPreferredPublicationType((balm::PublicationType::Value)i);
            balm::MetricId metricId(&desc);

            balm::MetricSample sample(&ta);
            sample.setTimeStamp(now);

            balm::MetricRecord record(metricId, 10, 100, -5, 5);
            bsls::TimeInterval elapsedTime(1,0);
            sample.appendGroup(&record, 1, elapsedTime);

            mX.publish(sample);

            for (int j = 0; j < balm::PublicationType::k_LENGTH; ++j) {
                balm::MetricFormat format(&ta);
                format.setFormatSpec((balm::PublicationType::Value)j,
                                     balm::MetricFormatSpec(100, "TEST %f!"));
                bsl::shared_ptr<const balm::MetricFormat> format_p(
                                    &format, bslstl::SharedPtrNilDeleter(), 0);

                desc.setFormat(format_p);
                mX.publish(sample);
            }
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
