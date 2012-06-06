// baem_streampublisher.t.cpp  -*-C++-*-
#include <baem_streampublisher.h>

#include <baem_metricsample.h>
#include <baem_metricformat.h>

#include <bdet_datetimetz.h>
#include <bdetu_systemtime.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_() cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef baem_StreamPublisher Obj;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

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
// with a 'baem_StreamPublisher'.  First we define a couple metric ids.  Note
// that we create the 'baem_MetricId' objects by hand, but in practice an id
// should be obtained from a 'baem_MetricRegistry' object (such as the
// one owned by a 'baem_MetricsManager').
//..
    baem_Category myCategory("MyCategory");
    baem_MetricDescription descA(&myCategory, "MetricA");
    baem_MetricDescription descB(&myCategory, "MetricB");

    baem_MetricId metricA(&descA);
    baem_MetricId metricB(&descB);
//..
// Now we create a 'baem_StreamPublisher' object, supplying it the stdout
// stream.
//..
    baem_StreamPublisher myPublisher(bsl::cout);
//..
// Next we create a metric sample containing a some records and send it to the
// 'baem_StreamPublisher' we created.  Note that because we are not actually
// collecting the metrics we set the elapsed time of the sample to an
// arbitrary 5s interval.
//..
    bslma_Allocator *allocator = bslma_Default::allocator(0);
    bsl::vector<baem_MetricRecord> records(allocator);
//
    records.push_back(baem_MetricRecord(metricA, 5, 25.0, 6.0, 25.0));
    records.push_back(baem_MetricRecord(metricB, 2,  7.0, 3.0, 11.0));

    baem_MetricSample sample(allocator);
//
    sample.setTimeStamp(
                   bdet_DatetimeTz(bdetu_SystemTime::nowAsDatetimeUtc(), 0));
    sample.appendGroup(records.data(),
                       records.size(),
                       bdet_TimeInterval(5, 0));

    myPublisher.publish(sample);
//..
// The output of this example would look similar to:
//..
// 28MAR2008_18:37:46.623+0000 - 2 Records   5s Elapsed Time
//         MyCategory.Metric1 (count = 5, total = 25, min = 6, max = 25)
//         MyCategory.Metric2 (count = 2, total = 7, min = 3, max = 11)
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

        bslma_TestAllocator ta, da;
        bslma_DefaultAllocatorGuard guard(&da);

        Obj mX(bsl::cout);
        baem_Category myCategory("MyCategory");
        bdet_DatetimeTz now(bdetu_SystemTime::nowAsDatetimeUtc(), 0);
        for (int i = 0; i < baem_PublicationType::BAEM_LENGTH; ++i) {
            baem_MetricDescription desc(&myCategory, "Test");

            desc.setPreferredPublicationType((baem_PublicationType::Value)i);
            baem_MetricId metricId(&desc);

            baem_MetricSample sample(&ta);
            sample.setTimeStamp(now);

            baem_MetricRecord record(metricId, 10, 100, -5, 5);
            bdet_TimeInterval elapsedTime(1,0);
            sample.appendGroup(&record, 1, elapsedTime);

            mX.publish(sample);

            for (int j = 0; j < baem_PublicationType::BAEM_LENGTH; ++j) {
                baem_MetricFormat format(&ta);
                format.setFormatSpec((baem_PublicationType::Value)j,
                                     baem_MetricFormatSpec(100, "TEST %f!"));
                bcema_SharedPtr<const baem_MetricFormat> format_p(
                                     &format, bcema_SharedPtrNilDeleter(), 0);

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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
