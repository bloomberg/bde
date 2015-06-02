// bbldc_daycountinterface.t.cpp                                      -*-C++-*-

#include <bbldc_daycountinterface.h>

#include <bdls_testutil.h>

#include <bdlt_date.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;


// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
// We are testing a pure protocol class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) that a usage example
// obtains the behavior specified by the protocol from the concrete subclass.
// ----------------------------------------------------------------------------
// [ 1] virtual ~DayCountInterface();
// [ 1] virtual int daysDiff(beginDate, endDate);
// [ 1] virtual double yearsDiff(beginDate, endDate);
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

class my_Convention : public bbldc::DayCountInterface {

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
//..
//  1. Define a concrete day-count type derived from 'bbldc_DayCountInterface'.
//  2. Implement the pure virtual 'daysDiff' and 'yearsDiff' methods.
//  3. Instantiate and use an object of the concrete type.
//..
// First define the (derived) 'my_DayCountConvention' class and implement its
// constructor inline (for convenience, directly within the derived-class
// definition):
//..
    // my_daycountconvention.h

    class my_DayCountConvention : public bbldc::DayCountInterface {
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
// calculate these values.
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

typedef bbldc::DayCountInterface Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

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

// Then, create two 'bdlt::Date' variables 'd1' and 'd2' to use with the
// 'my_DayCountConvention' object and its day-count convention methods.
//..
    const bdlt::Date d1(2003, 10, 19);
    const bdlt::Date d2(2003, 12, 31);
//..
// Next, we obtain a 'bbldc_DayCountInterface' reference from an instantiated
// 'my_DayCountConvention':
//..
    my_DayCountConvention           myDcc;
    const bbldc::DayCountInterface& dcc = myDcc;
//..
// Now, we compute the day-count between these two dates:
//..
    const int daysDiff = dcc.daysDiff(d1, d2);
    ASSERT(73 == daysDiff);
//..
// Finally, we compute the year fraction between these two dates:
//..
    const double yearsDiff = dcc.yearsDiff(d1, d2);
    // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
    // '0.2 == yearsDiff'.
    ASSERT(0.1999 < yearsDiff);
    ASSERT(0.2001 > yearsDiff);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST
        //   Verify the interface works as expected.
        //
        // Concerns:
        //: 1 A subclass of the 'bbldc::DayCountInterface' class compiles and
        //:   links when all virtual functions are defined.
        //:
        //: 2 The functions are in fact virtual.
        //
        // Plan:
        //: 1 Construct an object of a class derived from
        //:   'bbldc::DayCountInterface' and bind a 'bbldc::DayCountInterface'
        //:   reference to the object.  Using the base class reference, invoke
        //:   the 'daysDiff' and 'yearsDiff' methods and the destructor.
        //:   Verify that the correct implementations of the methods are
        //:   called.  (C-1..2)
        //
        // Testing:
        //   virtual ~DayCountInterface();
        //   virtual int daysDiff(beginDate, endDate);
        //   virtual double yearsDiff(beginDate, endDate);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;


        if (verbose) cout << "\nTesting 'daysDiff(beginDate, endDate)'\n";
        {
            my_Convention mX;  const my_Convention& X = mX;

            const bbldc::DayCountInterface& protocol = X;

            ASSERT(1 == protocol.daysDiff(bdlt::Date(), bdlt::Date() + 1));
        }

        if (verbose) cout << "\nTesting 'yearsDiff(beginDate, endDate)'\n";
        {
            my_Convention mX;  const my_Convention& X = mX;

            const bbldc::DayCountInterface& protocol = X;

            ASSERT(2.0 == protocol.yearsDiff(bdlt::Date(), bdlt::Date() + 1));
        }

        if (verbose) cout << "\nTesting destructor" << endl;
        {
            bbldc::DayCountInterface *protocol = new my_Convention;

            ASSERT(0 == my_ConventionDestructorCalled);

            delete protocol;

            ASSERT(1 == my_ConventionDestructorCalled);
        }

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
