// bbldc_basicdaycount.t.cpp                                          -*-C++-*-

#include <bbldc_basicdaycount.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

#include <bsls_protocoltest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test defines a protocol class the purpose of which is to
// provide an interface for computing day count and year fraction.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o It is possible to create a concrete implementation of the protocol.
// ----------------------------------------------------------------------------
// [ 1] virtual ~BasicDayCount();
// [ 1] virtual int daysDiff(beginDate, endDate) const = 0;
// [ 1] virtual double yearsDiff(beginDate, endDate) const = 0;
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// ----------------------------------------------------------------------------

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

int my_ConventionDestructorCalled;

class my_Convention : public bbldc::BasicDayCount {

  public:
    // CREATORS
    my_Convention();
        // Create this object.

    ~my_Convention();
        // Destroy this object.

    int daysDiff(const bdlt::Date& beginDate, const bdlt::Date& endDate) const;
        // Return the (signed) number of days between the specified 'beginDate'
        // and 'endDate'.  If 'beginDate <= endDate' then the result is
        // non-negative.  Note that reversing the order of 'beginDate' and
        // 'endDate' negates the result.

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const;
        // Return the (signed fractional) number of years between the specified
        // 'beginDate' and 'endDate' according to the Actual/360 convention.
        // If 'beginDate <= endDate' then the result is non-negative.  Note
        // that reversing the order of 'beginDate' and 'endDate' negates the
        // result; specifically
        // '|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15' for all dates 'b'
        // and 'e'.
};

my_Convention::my_Convention()  {
    my_ConventionDestructorCalled = 0;
}

my_Convention::~my_Convention() {
    my_ConventionDestructorCalled = 1;
}

int my_Convention::daysDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate) const {
    if (beginDate < endDate) {
        return  1;                                                    // RETURN
    }
    if (beginDate > endDate) {
        return -1;                                                    // RETURN
    }
    return 0;
}

double my_Convention::yearsDiff(const bdlt::Date& beginDate,
                                const bdlt::Date& endDate) const {
    if (beginDate < endDate) {
        return  2.0;                                                  // RETURN
    }
    if (beginDate > endDate) {
        return -2.0;                                                  // RETURN
    }
    return 0.0;
}

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

typedef bbldc::BasicDayCount ProtocolClass;

struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
    int daysDiff(const bdlt::Date& beginDate,
                 const bdlt::Date& endDate) const {
        (void)beginDate;
        (void)endDate;
        return markDone();
    }

    double yearsDiff(const bdlt::Date& beginDate,
                     const bdlt::Date& endDate) const {
        (void)beginDate;
        (void)endDate;
        return markDone();
    }
};

}  // close unnamed namespace

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Definition and Use of a Concrete Day-Count Convention
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example shows the definition and use of a simple concrete day-count
// convention.  This functionality suffices to demonstrate the requisite steps
// for having a working day-count convention:
//: * Define a concrete day-count type derived from 'bbldc::BasicDayCount'.
//:
//: * Implement the pure virtual 'daysDiff' and 'yearsDiff' methods.
//:
//: * Instantiate and use an object of the concrete type.
//
// First, define the (derived) 'my_DayCountConvention' class and implement its
// constructor inline (for convenience, directly within the derived-class
// definition):
//..
    // my_daycountconvention.h

    class my_DayCountConvention : public bbldc::BasicDayCount {
      public:
        my_DayCountConvention() { }
        virtual ~my_DayCountConvention();
        virtual int daysDiff(const bdlt::Date& beginDate,
                             const bdlt::Date& endDate) const;
            // Return the (signed) number of days between the specified ...
        virtual double yearsDiff(const bdlt::Date& beginDate,
                                 const bdlt::Date& endDate) const;
            // Return the (signed fractional) number of years between the ...
    };
//..
// Then, implement the destructor.  Note, however, that we always implement a
// virtual destructor (non-inline) in the .cpp file (to indicate the *unique*
// location of the class's virtual table):
//..
    // my_daycountconvention.cpp

    // ...

    my_DayCountConvention::~my_DayCountConvention() { }
//..
// Next, we implement the (virtual) 'daysDiff' and 'yearsDiff' methods, which
// incorporate the "policy" of what it means for this day-count convention to
// calculate day count and year fraction:
//..
    int my_DayCountConvention::daysDiff(const bdlt::Date& beginDate,
                                        const bdlt::Date& endDate) const
    {
        return endDate - beginDate;
    }

    double my_DayCountConvention::yearsDiff(const bdlt::Date& beginDate,
                                            const bdlt::Date& endDate) const
    {
        return static_cast<double>((endDate - beginDate) / 365.0);
    }
//..

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bbldc::BasicDayCount Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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

// Then, create two 'bdlt::Date' variables, 'd1' and 'd2', to use with the
// 'my_DayCountConvention' object and its day-count convention methods:
//..
    const bdlt::Date d1(2003, 10, 19);
    const bdlt::Date d2(2003, 12, 31);
//..
// Next, we obtain a 'bbldc::BasicDayCount' reference from an instantiated
// 'my_DayCountConvention':
//..
    my_DayCountConvention       myDcc;
    const bbldc::BasicDayCount& dcc = myDcc;
//..
// Now, we compute the day count between the two dates:
//..
    const int daysDiff = dcc.daysDiff(d1, d2);
    ASSERT(73 == daysDiff);
//..
// Finally, we compute the year fraction between the two dates:
//..
    const double yearsDiff = dcc.yearsDiff(d1, d2);
    // Need fuzzy comparison since 'yearsDiff' is a 'double'.
    ASSERT(0.1999 < yearsDiff && 0.2001 > yearsDiff);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //:
        //: 2 The protocol has no data members.
        //:
        //: 3 The protocol has a virtual destructor.
        //:
        //: 4 All methods of the protocol are pure virtual.
        //:
        //: 5 All methods of the protocol are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
        //:   of the protocol.
        //:
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'ProtocolClassTestImp', and use it to verify
        //:   that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:
        //:   2 The protocol has no data members. (C-2)
        //:
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:
        //:   2 publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~BasicDayCount();
        //   virtual int daysDiff(beginDate, endDate) const = 0;
        //   virtual double yearsDiff(beginDate, endDate) const = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nCreate a test object.\n";

        bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);

        if (verbose) cout << "\nVerify that the protocol is abstract.\n";

        ASSERT(testObj.testAbstract());

        if (verbose) cout << "\nVerify that there are no data members.\n";

        ASSERT(testObj.testNoDataMembers());

        if (verbose) cout << "\nVerify that the destructor is virtual.\n";

        ASSERT(testObj.testVirtualDestructor());

        if (verbose) cout << "\nVerify that methods are public and virtual.\n";

        BSLS_PROTOCOLTEST_ASSERT(testObj, daysDiff(bdlt::Date(2012, 1, 1),
                                                   bdlt::Date(2012, 1, 1)));

        BSLS_PROTOCOLTEST_ASSERT(testObj, yearsDiff(bdlt::Date(2012, 1, 1),
                                                    bdlt::Date(2012, 1, 1)));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT == FOUND." << endl;
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
