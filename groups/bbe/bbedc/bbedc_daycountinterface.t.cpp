// bbedc_daycountinterface.t.cpp     -*-C++-*-

#include <bbedc_daycountinterface.h>
#include <bdet_date.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that (1) a
// concrete derived class compiles and links, and (2) that a usage example
// obtains the behavior specified by the protocol from the concrete subclass.
//-----------------------------------------------------------------------------
// [ 1] virtual ~bbedc_DayCountInterface();
// [ 1] virtual int daysDiff(const bdet_Date& d1, const bdet_Date& d2);
// [ 1] virtual double yearsDiff(const bdet_Date& d1, const bdet_Date& d2);
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure main usage example compiles and works properly.
//--------------------------------------------------------------------------


// =========================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline



//=============================================================================
//                       CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------

int my_ConventionDestructorCalled;

class my_Convention : public bbedc_DayCountInterface {

  public:
    my_Convention()  {
        my_ConventionDestructorCalled = 0;
    }

    ~my_Convention() {
        my_ConventionDestructorCalled = 1;
    }

    int daysDiff(const bdet_Date& beginDate, const bdet_Date& endDate) const {
        return 1;
    }

    double yearsDiff(const bdet_Date& beginDate,
                     const bdet_Date& endDate) const {
        return 2.0;
    }
};



//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

// my_daycountconvention.h

class my_DayCountConvention : public bbedc_DayCountInterface {
  public:
    my_DayCountConvention() { }
    virtual ~my_DayCountConvention();
    virtual int daysDiff(const bdet_Date& beginDate,
                         const bdet_Date& endDate) const;
        // Return the number of days between the specified ...
    virtual double yearsDiff(const bdet_Date& beginDate,
                             const bdet_Date& endDate) const;
        // Return the number of years between the specified ...
};

// my_daycountconvention.cpp

my_DayCountConvention::~my_DayCountConvention() { }

int my_DayCountConvention::daysDiff(const bdet_Date& beginDate,
                                    const bdet_Date& endDate) const
{
    return endDate - beginDate;
}

double my_DayCountConvention::yearsDiff(const bdet_Date& beginDate,
                                        const bdet_Date& endDate) const
{
    return (double)(endDate - beginDate) / 365.0;
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bbedc_DayCountInterface Obj;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
       // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "\nTesting Usage Examples"
                          << "\n======================" << endl;

        const bdet_Date d1(2003, 10, 19);
        const bdet_Date d2(2003, 12, 31);

        my_DayCountConvention myDcc;
        const bbedc_DayCountInterface& dcc = myDcc;

        const int daysDiff = dcc.daysDiff(d1, d2);
        ASSERT(73 == daysDiff);

        const double yearsDiff = dcc.yearsDiff(d1, d2);
        // Need fuzzy comparison since 'yearsDiff' is a double.  Expect
        // 0.2 == 'yearsDiff'.
        ASSERT(0.1999 < yearsDiff);
        ASSERT(0.2001 > yearsDiff);

        if (veryVerbose) {
            P_(daysDiff);
            P(yearsDiff);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        // Concerns:
        //   We must ensure that (1) a subclass of the
        //   'bbedc_DayCountInterface' class compiles and links when all
        //   virtual functions are defined, and (2) the functions are in fact
        //   virtual.
        //
        // Plan:
        //   Construct an object of a class derived from
        //   'bbedc_DayCountInterface' and bind a 'bbedc_DayCountInterface'
        //   reference to the object.  Using the base class reference, invoke
        //   the 'daysDiff' and 'yearsDiff' methods and the destructor.  Verify
        //   that the correct implementations of the methods are called.
        //
        // Testing:
        //   virtual ~bbedc_DayCountInterface();
        //   virtual int daysDiff(const bdet_Date& d1, const bdet_Date& d2);
        //   virtual double yearsDiff(const bdet_Date& d1,const bdet_Date& d2);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;


        if (verbose) cout << "\nTesting 'daysDiff(beginDate, endDate)'\n";
        {
            my_Convention mX;  const my_Convention& X = mX;
            const bbedc_DayCountInterface& protocol = X;

            ASSERT(1 == protocol.daysDiff(bdet_Date(), bdet_Date()));
        }

        if (verbose) cout << "\nTesting 'yearsDiff(beginDate, endDate)'\n";
        {
            my_Convention mX;  const my_Convention& X = mX;
            const bbedc_DayCountInterface& protocol = X;

            ASSERT(2.0 == protocol.yearsDiff(bdet_Date(), bdet_Date()));
        }

        if (verbose) cout << "\nTesting destructor" << endl;
        {
            bbedc_DayCountInterface *protocol = new my_Convention;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
