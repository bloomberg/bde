// ball_filteringobserver.t.cpp                                       -*-C++-*-
#include <ball_filteringobserver.h>

#include <ball_context.h>
#include <ball_patternutil.h>
#include <ball_record.h>
#include <ball_recordattributes.h>
#include <ball_severity.h>
#include <ball_testobserver.h>
#include <ball_userfields.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_asserttest.h>
#include <bsls_platform.h>

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
// The component under test is a simple observer that filters log records.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] FilteringObserver(observer, recordFilterCallback, allocator);
// [ 2] virtual ~FilteringObserver();
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

#define ASSERT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::FilteringObserver Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

class CountingPassFilter
    // This class provides 'operator()' that maintains its invocation count.
{
    //DATA
    int d_filterCounter;

  public:
    // CREATORS
    CountingPassFilter() : d_filterCounter(0) {}

    //! ~CountingPassFilter() = default;

    //! CountingPassFilter(const CountingPassFilter&) = default;

    //! CountingPassFilter& operator=(const CountingPassFilter&) = default;

    // MANIPULATORS
    bool operator()(const ball::Record&, const ball::Context&)
    {
        ++d_filterCounter;
        return true;
    }

    // ACCESSORS
    int filterCounter() const
    {
        return d_filterCounter;
    }
};

class CountingDropFilter
    // This class provides 'operator()' that maintains its invocation count.
{
    //DATA
    int d_filterCounter;

  public:
    CountingDropFilter() : d_filterCounter(0) {}


    //! ~CountingDropFilter() = default;

    //! CountingDropFilter(const CountingDropFilter&) = default;

    //! CountingDropFilter& operator=(const CountingDropFilter&) = default;

    // MANIPULATORS
    bool operator()(const ball::Record&, const ball::Context&)
    {
        ++d_filterCounter;
        return false;
    }

    // ACCESSORS
    int filterCounter() const
    {
        return d_filterCounter;
    }
};

class AllocatingFilter
    // This filter is used to test that the allocator is properly hooked up at
    // construction.
{
    //DATA
    int              *d_data_p;       // data member
    bslma::Allocator *d_allocator_p;  // allocator used to supply memory (held,
                                      // not owned)

  public:
    BSLMF_NESTED_TRAIT_DECLARATION(AllocatingFilter,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit AllocatingFilter(bslma::Allocator *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_data_p = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        *d_data_p = 0;
    }


    AllocatingFilter(const AllocatingFilter&  original,
                     bslma::Allocator        *basicAllocator = 0)
    : d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_data_p = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        *d_data_p = *original.d_data_p;
    }

    ~AllocatingFilter()
    {
        d_allocator_p->deallocate(d_data_p);
    }

    // MANIPULATORS
    AllocatingFilter& operator=(const AllocatingFilter& rhs)
    {
        if (&rhs != this)
        {
            int *newData = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
            d_allocator_p->deallocate(d_data_p);
            d_data_p = newData;
            *d_data_p = *rhs.d_data_p;
        }
        return *this;
    }

    bool operator()(const ball::Record&, const ball::Context&)
    {
        ++(*d_data_p);
        return true;
    }

    // ACCESSORS
    bslma::Allocator *allocator() const
    {
        return d_allocator_p;
    }

    int data() const
    {
        return *d_data_p;
    }
};

bool categoryFilter(const ball::Record& record, const ball::Context&)
{
    return ball::PatternUtil::isMatch(record.fixedFields().category(),
                                      "EQUITY.*");
}

}  // close unnamed namespace

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

    (void) veryVerbose;      // Suppress compiler warning.
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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

///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example shows how to use filtering observer to route some log records
// to a designated observer.  A common use case is routing all log records for
// a given category (or category pattern) to a separate log file.
//
// First, we create a filter that will match a log record's category against
// the pattern "EQUITY.*":
//..
//  bool categoryFilter(const ball::Record& record, const ball::Context&)
//  {
//      return ball::PatternUtil::isMatch(record.fixedFields().category(),
//                                         "EQUITY.*");
//  }
//..
// Then, we create the observer that will receive filtered log records and
// create a filtering observer:
//..
    bsl::shared_ptr<ball::TestObserver> innerObserver(
                                           new ball::TestObserver(&bsl::cout));

    ball::FilteringObserver filteringObserver(innerObserver, categoryFilter);
//..
// Next, we issue a series of log records and verify that only records with the
// category matching the pattern are published to the inner observer:
//..
    const ball::Context context(ball::Transmission::e_PASSTHROUGH, 0, 1);

    ball::RecordAttributes fixedFields;

    bsl::shared_ptr<ball::Record> record;
    record.createInplace();

    fixedFields.setCategory("CURNCY.USDGBP");
    record->setFixedFields(fixedFields);
    filteringObserver.publish(record, context);

    ASSERT(0 == innerObserver->numPublishedRecords());  // dropped

    fixedFields.setCategory("EQUITY.NYSE");
    record->setFixedFields(fixedFields);
    filteringObserver.publish(record, context);

    ASSERT(1 == innerObserver->numPublishedRecords());  // forwarded

    fixedFields.setCategory("EQUIT.");
    record->setFixedFields(fixedFields);
    filteringObserver.publish(record, context);

    ASSERT(1 == innerObserver->numPublishedRecords());  // dropped
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // Concerns:
        //: 1 The allocator is hooked up correctly.
        //:
        //: 2 Log record is correctly formatted.
        //:
        //: 3 Formatted log record is written to the output stream.
        //:
        //: 4 The filtering observer drops all records if the filter functor is
        //:   empty.
        //
        // Plan:
        //: 1 Create the observer object and publish log record.
        //
        // Testing:
        //   FilteringObserver(observer, recordFilterCallback, allocator);
        //   virtual ~FilteringObserver();
        //   virtual void publish(const shared_ptr<const Record>&, Context&);
        //   virtual void releaseRecords();
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING PRIMARY MANIPULATORS"
                          << "\n============================" << endl;

        if (verbose) cout << "Publish a single message." << endl;

        {
            if (veryVerbose) cout << "\tPass filter." << endl;

            bsl::ostringstream                  os;
            bsl::shared_ptr<ball::TestObserver> innerObserver(
                                                  new ball::TestObserver(&os));

            Obj X(innerObserver, CountingPassFilter());

            ball::RecordAttributes fixed;

            fixed.setMessage("Log Message (passed)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();

            record->setFixedFields(fixed);

            X.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                            0,
                                            1));

            ASSERTV(innerObserver->numPublishedRecords(),
                    1 == innerObserver->numPublishedRecords());
        }

        {
            if (veryVerbose) cout << "\tDrop filter." << endl;

            bsl::ostringstream                  os;
            bsl::shared_ptr<ball::TestObserver> innerObserver(
                                                  new ball::TestObserver(&os));

            Obj X(innerObserver, CountingDropFilter());

            ball::RecordAttributes fixed;

            fixed.setMessage("Log Message (dropped)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();

            record->setFixedFields(fixed);

            X.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                            0,
                                            1));

            ASSERTV(innerObserver->numPublishedRecords(),
                    0 == innerObserver->numPublishedRecords());
        }

        {
            if (veryVerbose) cout << "\tPass filter(via bind)." << endl;

            bsl::ostringstream                  os;
            bsl::shared_ptr<ball::TestObserver> innerObserver(
                                                  new ball::TestObserver(&os));

            CountingPassFilter passFilter;

            Obj X(innerObserver, bdlf::BindUtil::bind(
                                               &CountingPassFilter::operator(),
                                               &passFilter,
                                               bdlf::PlaceHolders::_1,
                                               bdlf::PlaceHolders::_2));

            ball::RecordAttributes fixed;

            fixed.setMessage("Log Message (passed)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();

            record->setFixedFields(fixed);

            X.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                            0,
                                            1));

            ASSERTV(passFilter.filterCounter(),
                    1 == passFilter.filterCounter());
            ASSERTV(innerObserver->numPublishedRecords(),
                    1 == innerObserver->numPublishedRecords());
        }

        {
            if (veryVerbose) cout << "\tPass empty filter." << endl;

            bsl::ostringstream                  os;
            bsl::shared_ptr<ball::TestObserver> innerObserver(
                                                  new ball::TestObserver(&os));

            Obj X(innerObserver, Obj::RecordFilterCallback());

            ball::RecordAttributes fixed;

            fixed.setMessage("Log Message (empty functor)");

            bsl::shared_ptr<ball::Record> record;
            record.createInplace();

            record->setFixedFields(fixed);

            X.publish(record, ball::Context(ball::Transmission::e_PASSTHROUGH,
                                            0,
                                            1));

            ASSERTV(innerObserver->numPublishedRecords(),
                    0 == innerObserver->numPublishedRecords());
        }

        if (verbose)
            cout << "\nTesting with various allocator configurations." << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
            const char CONFIG = cfg;  // how we specify the allocator

            // Allocating temporary objects before allocators
            bsl::ostringstream                  os;
            bsl::shared_ptr<ball::TestObserver> innerObserver(
                                                  new ball::TestObserver(&os));
            AllocatingFilter                    filter;
            const Obj::RecordFilterCallback     callback(filter);

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj(innerObserver, callback);
              } break;
              case 'b': {
                objPtr = new (fa) Obj(innerObserver, callback, 0);
              } break;
              case 'c': {
                objPtr = new (fa) Obj(innerObserver, callback, &sa);
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            ASSERTV(CONFIG, oa.numBlocksInUse(),  0 !=  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(CONFIG, fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(CONFIG, oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());
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
