// balm_metricrecord.t.cpp                                            -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_metricrecord.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_limits.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'balm::MetricRecord' is a simple value object holding to
// properties that can be written and accessed freely, i.e., there are no
// constraints placed up their values.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 3]  balm::MetricRecord();
// [ 5]  balm::MetricRecord(const balm::MetricRecord&  original);
// [ 3]  balm::MetricRecord(const balm::MetricId& metric);
// [ 3]  balm::MetricRecord(const balm::MetricId& metric,
//                         int                  count,
//                         double               total,
//                         double               min,
//                         double               max);
// [ 2]  ~balm::MetricRecord();
//
//  MANIPULATORS
// [ 6]  balm::MetricRecord& operator=(const balm::MetricRecord& rhs);
// [ 2]  balm::MetricId& metric();
// [ 2]  int& count();
// [ 2]  double& total();
// [ 2]  double& max();
// [ 2]  double& min();
//
// ACCESSORS
// [ 2]  const balm::MetricId& metric() const;
// [ 2]  const int& count() const;
// [ 2]  const double& total() const;
// [ 2]  const double& max() const;
// [ 2]  const double& min() const;
// [ 7]  bsl::ostream& print(bsl::ostream &stream) const;
//
// FREE OPERATORS
// [ 4]  bool operator==(const balm::MetricRecord&,
//                       const balm::MetricRecord&);
// [ 4]  bool operator!=(const balm::MetricRecord&,
//                       const balm::MetricRecord&);
// [ 7]  bsl::ostream& operator<<(const bsl::ostream&      stream,
//                                const balm::MetricRecord& rhs);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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

typedef balm::MetricRecord      Obj;
typedef balm::MetricId          Id;
typedef balm::MetricDescription Desc;

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The following example demonstrates how a 'balm::MetricRecord' can be used
// to describe a set of metric values.  In the example we create a
// 'RequestProcessor' class that collects information about the sizes of the
// requests it has processed.  The 'RequestProcessor' also provides a
// 'loadRequestSizeInformation' method that populates a 'balm::MetricRecord'
// object describing the sizes of the requests it has processed.
//..
    class RequestProcessor {
        // This class defines a request processor that provides metric
        // information about the sizes of the requests it has processed.

        // DATA
        unsigned int d_numRequests;       // number of requests processed
        unsigned int d_totalRequestSize;  // total size of the requests
        unsigned int d_minRequestSize;    // minimum request size
        unsigned int d_maxRequestSize;    // maximum request size

      public:
        // CREATORS
        RequestProcessor()
            // Create this 'RequestProcessor'.
        : d_numRequests(0)
        , d_totalRequestSize(0)
        , d_minRequestSize(INT_MAX)
        , d_maxRequestSize(0)
        {
        }

        // ...

        // MANIPULATORS
        void processRequest(const bsl::string& request)
            // Process the specified 'request'.
        {
            ++d_numRequests;
            d_totalRequestSize += request.size();
            d_minRequestSize   =  bsl::min(d_minRequestSize,
                                           (unsigned int)request.size());
            d_maxRequestSize   =  bsl::max(d_maxRequestSize,
                                           (unsigned int)request.size());

            // Process the request
        }
//..
// Now we declare a function that populates a 'balm::MetricRecord' describing
// the sizes of the requests that the request processor has processed.
//..
        // ACCESSORS
        void loadRequestSizeInformation(balm::MetricRecord *record) const
            // Populate the specified 'record' with information regarding
            // the sizes of the request processed by this request.
        {
            if (0 < d_numRequests) {
                record->count()  = d_numRequests;
                record->total()  = d_totalRequestSize;
                record->min()    = d_minRequestSize;
                record->max()    = d_maxRequestSize;
            }
        }

        // ...
    };

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    balm::Category cA("A", true);
    Desc mA(&cA, "A"); const Desc *MA = &mA;
    Desc mB(&cA, "B"); const Desc *MB = &mB;
    Desc mC(&cA, "C"); const Desc *MC = &mC;
    Desc mD(&cA, "D"); const Desc *MD = &mD;
    Desc mE(&cA, "E"); const Desc *ME = &mE;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT VALUES
        //
        // Concerns:
        //   The hand-coded literals supplied for '(k_)DEFAULT_*' match the
        //   corresponding values provided by 'bsl::numeric_limits'.
        //
        // Plan:
        //   Compare the values of the '(k_)DEFAULT_*' constants with the
        //   values returned by 'bsl::numeric_limits'.
        //
        // Testing:
        //   CONCERN: DEFAULT VALUES
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Default Values"
                          << "\n======================" << endl;

        ASSERTV(balm::MetricRecord::k_DEFAULT_MIN,
                balm::MetricRecord::k_DEFAULT_MIN
                    == bsl::numeric_limits<double>::infinity());
        ASSERTV(balm::MetricRecord::k_DEFAULT_MAX,
                balm::MetricRecord::k_DEFAULT_MAX
                    == -bsl::numeric_limits<double>::infinity());


      } break;
      case 8: {
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

//..
// We can create an instance of this 'RequestProcessor' class and use it to
// process a couple requests.
//..
    RequestProcessor requestProcessor;

    requestProcessor.processRequest("123");
    requestProcessor.processRequest("12345");
//..
// Now we create 'balm::MetricRecord' to hold our aggregated metrics values.
// Note that we create 'balm::MetricId' object by hand; however, in practice an
// id should be obtained from a 'balm::MetricRegistry' object (such as the one
// owned by a 'balm::MetricsManager').
//..
    balm::Category           myCategory("MyCategory");
    balm::MetricDescription  description(&myCategory, "RequestSize");
    balm::MetricId           requestSizeId(&description);

    // In practice, get 'requestSizeId' from a 'balm::MetricRegistry' object.
    balm::MetricRecord requestSize(requestSizeId);
//..
// Finally we retrieve the information about the request sizes of the
// requests processed by 'requestProcessor'.  Note that the count of requests
// should be 2, the total size of the requests should be 8 (3 + 5), the
// minimum size should be 3, and the maximum size should be 5.
//..
    requestProcessor.loadRequestSizeInformation(&requestSize);
        ASSERT(2 == requestSize.count());
        ASSERT(8 == requestSize.total());
        ASSERT(3 == requestSize.min());
        ASSERT(5 == requestSize.max());
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PRINT AND OUTPUT (<<) OPERATOR:
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   operator<<(ostream&, const balm::MetricRecord&);
        // --------------------------------------------------------------------

        if (veryVerbose) cout << "Testing 'print'." << endl;
        bsl::ostringstream buffer;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
            const char *d_exptdVal;
        } VALUES [] = {
            {  0, 0, 0, 0, 0, "[ INVALID_ID: 0 0 0 0 ]"},
            { MA, 1, 1, 1, 1, "[ A.A: 1 1 1 1 ]"},
            { MB, 2, 3, 4, 5, "[ A.B: 2 3 4 5 ]"},
            { MC, 9, 1.1, 4.6, 5.7, "[ A.C: 9 1.1 4.6 5.7 ]"}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& U = u;

            bsl::ostringstream buf1, buf2;
            buf2 << U;
            U.print(buf1);
            LOOP2_ASSERT(i, buf1.str(), VALUES[i].d_exptdVal == buf1.str());
            LOOP2_ASSERT(i, buf2.str(), VALUES[i].d_exptdVal == buf2.str());
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   balm::MetricRecord& operator=(const balm::MetricRecord& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator" << endl;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
        } VALUES [] = {
            {  0, 0, 0, 0, 0},
            { MA, 1, 1, 1, 1},
            { MB, 2, 3, 4, 5},
            { MC, 9, 1.1, 4.6, 5.7},
            { MD, 32, -12314.0, 143.0, 23849123.0},
            { ME, 12312, 12312312314.0, 12312343.0, -123123123.0}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj u(Id(VALUES[i].d_metric),
                      VALUES[i].d_count,
                      VALUES[i].d_total,
                      VALUES[i].d_min,
                      VALUES[i].d_max);
                const Obj& U = u;
                Obj v(Id(VALUES[j].d_metric),
                      VALUES[j].d_count,
                      VALUES[j].d_total,
                      VALUES[j].d_min,
                      VALUES[j].d_max);
                const Obj& V = v;
                Obj w(v); const Obj& W = w;

                u = v;
                ASSERT(V == U);
                ASSERT(W == U);
            }
        }

        if (veryVerbose) cout << "\tTesting assignment u = u (Aliasing)."
                              << endl;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& U = u;
            Obj w(U);  const Obj& W = w;              // control
            u = u;
            LOOP_ASSERT(i, W == U);
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   balm::MetricRecord(const balm::MetricRecord& );
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor" << endl;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
        } VALUES [] = {
            {  0, 0, 0, 0, 0},
            { MA, 1, 1, 1, 1},
            { MB, 2, 3, 4, 5},
            { MC, 9, 1.1, 4.6, 5.7},
            { MD, 32, -12314.0, 143.0, 23849123.0},
            { ME, 12312, 12312312314.0, 12312343.0, -123123123.0}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& X = x;
            Obj w(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& W = w;

            Obj y(X); const Obj& Y = y;

            ASSERT(W == Y);
            ASSERT(X == Y);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATOR
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   bool operator==(const balm::MetricRecord&,
        //                   const balm::MetricRecord&);
        //   bool operator!=(const balm::MetricRecord&,
        //                   const balm::MetricRecord&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting equality." << endl;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
        } VALUES [] = {
            {  0, 0, 0, 0, 0},
            { MA, 1, 1, 1, 1},
            { MB, 2, 3, 4, 5},
            { MC, 9, 1.1, 4.6, 5.7},
            { MD, 32, -12314.0, 143.0, 23849123.0},
            { ME, 12312, 12312312314.0, 12312343.0, -123123123.0}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj u(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& U = u;

            for (int j = 0; j < NUM_VALUES; ++j) {
                Obj v(Id(VALUES[j].d_metric),
                      VALUES[j].d_count,
                      VALUES[j].d_total,
                      VALUES[j].d_min,
                      VALUES[j].d_max);
                const Obj& V = v;

                bool isEqual = i == j;
                LOOP2_ASSERT(i, j, isEqual  == (U == V));
                LOOP2_ASSERT(i, j, !isEqual == (U != V))
            }
        }
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS
        //
        // Concerns:
        //   Test the four varieties of constructors
        //
        // Plan:
        //   Verify the default constructor.
        //
        //   Next, for a sequence of independent test values, use the
        //   alternative constructor to create and object with a specific value
        //   and verify the values using a basic accessor.
        //
        // Testing:
        //   balm::MetricRecord()
        //   balm::MetricRecord(const balm::MetricId& );
        //   balm::MetricRecord(const balm::MetricId&  ,
        //                     int,
        //                     double,
        //                     double,
        //                     double);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting constructors." << endl;

        {
            if (veryVerbose) cout << "\tTesting default constructor." << endl;
            Obj x; const Obj& X = x;
            ASSERT(false            == X.metricId().isValid());
            ASSERT(0                == X.count());
            ASSERT(0                == X.total());
            ASSERT(Obj::k_DEFAULT_MIN == X.min());
            ASSERT(Obj::k_DEFAULT_MAX == X.max());
        }
        if (veryVerbose) cout << "\tTesting secondary constructor." << endl;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
        } VALUES [] = {
            {  0, 0, 0, 0, 0},
            { MA, 1, 1, 1, 1},
            { MB, 2, 3, 4, 5},
            { MC, 9, 1.1, 4.6, 5.7},
            { MD, 32, -12314.0, 143.0, 23849123.0},
            { ME, 12312, 12312312314.0, 12312343.0, -123123123.0}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x(Id(VALUES[i].d_metric)); const Obj& X = x;
            Obj y(Id(VALUES[i].d_metric),
                  VALUES[i].d_count,
                  VALUES[i].d_total,
                  VALUES[i].d_min,
                  VALUES[i].d_max);
            const Obj& Y = y;

            ASSERT(VALUES[i].d_metric == X.metricId().description());
            ASSERT(0                  == X.count());
            ASSERT(0                  == X.total());
            ASSERT(Obj::k_DEFAULT_MIN   == X.min());
            ASSERT(Obj::k_DEFAULT_MAX   == X.max());

            ASSERT(VALUES[i].d_metric == Y.metricId().description());
            ASSERT(VALUES[i].d_count  == Y.count());
            ASSERT(VALUES[i].d_total  == Y.total());
            ASSERT(VALUES[i].d_min    == Y.min());
            ASSERT(VALUES[i].d_max    == Y.max());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulators
        //   to set their values.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   balm::MetricRecord()
        //   ~balm::MetricRecord()
        //   balm::MetricId& metricId();
        //   int&    count();
        //   double& total();
        //   double& min();
        //   double& max();
        //   const balm::MetricId& metricId() const;
        //   const int&    count() const;
        //   const double& total() const;
        //   const double& min() const;
        //   const double& max() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting mainpulator methods." << endl;

        struct {
            const Desc *d_metric;
            int         d_count;
            double      d_total;
            double      d_min;
            double      d_max;
        } VALUES [] = {
            {  0, 0, 0, 0, 0},
            { MA, 1, 1, 1, 1},
            { MB, 2, 3, 4, 5},
            { MC, 9, 1.1, 4.6, 5.7},
            { MD, 32, -12314.0, 143.0, 23849123.0},
            { ME, 12312, 12312312314.0, 12312343.0, -123123123.0}
        };
        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj x; const Obj& X = x;
            x.metricId() = Id(VALUES[i].d_metric);
            x.count()    = VALUES[i].d_count;
            x.total()    = VALUES[i].d_total;
            x.min()      = VALUES[i].d_min;
            x.max()      = VALUES[i].d_max;

            ASSERT(VALUES[i].d_metric == X.metricId().description());
            ASSERT(VALUES[i].d_count  == X.count());
            ASSERT(VALUES[i].d_total  == X.total());
            ASSERT(VALUES[i].d_min    == X.min());
            ASSERT(VALUES[i].d_max    == X.max());
        }
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

        Id VA_METRIC(MA);
        int    VA_COUNT = 2;
        double VA_TOTAL = 3.0;
        double VA_MIN   = 4.0;
        double VA_MAX   = 5.0;

        Id VB_METRIC(MB);
        int    VB_COUNT = 7;
        double VB_TOTAL = 8.0;
        double VB_MIN   = 9.0;
        double VB_MAX   = 10.0;

        Id VC_METRIC(MC);
        int    VC_COUNT = 12;
        double VC_TOTAL = 13.0;
        double VC_MIN   = 14.0;
        double VC_MAX   = 15.0;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA_METRIC, VA_COUNT, VA_TOTAL, VA_MIN, VA_MAX);
        const Obj& X1 = mX1;

        if (veryVerbose) { cout << '\t';  P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(VA_METRIC == mX1.metricId());
        ASSERT(VA_METRIC == X1.metricId());
        ASSERT(VA_COUNT  == mX1.count());
        ASSERT(VA_COUNT  == X1.count());
        ASSERT(VA_TOTAL  == mX1.total());
        ASSERT(VA_TOTAL  == X1.total());
        ASSERT(VA_MIN    == mX1.min());
        ASSERT(VA_MIN    == X1.min());
        ASSERT(VA_MAX    == mX1.max());
        ASSERT(VA_MAX    == X1.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(VA_METRIC == mX2.metricId());
        ASSERT(VA_METRIC == X2.metricId());
        ASSERT(VA_COUNT  == mX2.count());
        ASSERT(VA_COUNT  == X2.count());
        ASSERT(VA_TOTAL  == mX2.total());
        ASSERT(VA_TOTAL  == X2.total());
        ASSERT(VA_MIN    == mX2.min());
        ASSERT(VA_MIN    == X2.min());
        ASSERT(VA_MAX    == mX2.max());
        ASSERT(VA_MAX    == X2.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;

        mX1.metricId() = VB_METRIC;
        mX1.count()    = VB_COUNT;
        mX1.total()    = VB_TOTAL;
        mX1.min()      = VB_MIN;
        mX1.max()      = VB_MAX;

        if (veryVerbose) { cout << '\t';  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(VB_METRIC == mX1.metricId());
        ASSERT(VB_METRIC == X1.metricId());
        ASSERT(VB_COUNT  == mX1.count());
        ASSERT(VB_COUNT  == X1.count());
        ASSERT(VB_TOTAL  == mX1.total());
        ASSERT(VB_TOTAL  == X1.total());
        ASSERT(VB_MIN    == mX1.min());
        ASSERT(VB_MIN    == X1.min());
        ASSERT(VB_MAX    == mX1.max());
        ASSERT(VB_MAX    == X1.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(0 == mX3.metricId());
        ASSERT(0 == X3.metricId());
        ASSERT(0 == mX3.count());
        ASSERT(0 == X3.count());
        ASSERT(0 == mX3.total());
        ASSERT(0 == X3.total());
        ASSERT(balm::MetricRecord::k_DEFAULT_MIN == mX3.min());
        ASSERT(balm::MetricRecord::k_DEFAULT_MIN == X3.min());
        ASSERT(balm::MetricRecord::k_DEFAULT_MAX == mX3.max());
        ASSERT(balm::MetricRecord::k_DEFAULT_MAX == X3.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t';  P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == mX4.metricId());
        ASSERT(0 == X4.metricId());
        ASSERT(0 == mX4.count());
        ASSERT(0 == X4.count());
        ASSERT(0 == mX4.total());
        ASSERT(0 == X4.total());
        ASSERT(balm::MetricRecord::k_DEFAULT_MIN == mX4.min());
        ASSERT(balm::MetricRecord::k_DEFAULT_MIN == X4.min());
        ASSERT(balm::MetricRecord::k_DEFAULT_MAX == mX4.max());
        ASSERT(balm::MetricRecord::k_DEFAULT_MAX == X4.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.metricId() = VC_METRIC;
        mX3.count()    = VC_COUNT;
        mX3.total()    = VC_TOTAL;
        mX3.min()      = VC_MIN;
        mX3.max()      = VC_MAX;

        if (veryVerbose) { cout << '\t';  P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(VC_METRIC == mX3.metricId());
        ASSERT(VC_METRIC == X3.metricId());
        ASSERT(VC_COUNT  == mX3.count());
        ASSERT(VC_COUNT  == X3.count());
        ASSERT(VC_TOTAL  == mX3.total());
        ASSERT(VC_TOTAL  == X3.total());
        ASSERT(VC_MIN    == mX3.min());
        ASSERT(VC_MIN    == X3.min());
        ASSERT(VC_MAX    == mX3.max());
        ASSERT(VC_MAX    == X3.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(VB_METRIC == mX2.metricId());
        ASSERT(VB_METRIC == X2.metricId());
        ASSERT(VB_COUNT  == mX2.count());
        ASSERT(VB_COUNT  == X2.count());
        ASSERT(VB_TOTAL  == mX2.total());
        ASSERT(VB_TOTAL  == X2.total());
        ASSERT(VB_MIN    == mX2.min());
        ASSERT(VB_MIN    == X2.min());
        ASSERT(VB_MAX    == mX2.max());
        ASSERT(VB_MAX    == X2.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t';  P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(VC_METRIC == mX2.metricId());
        ASSERT(VC_METRIC == X2.metricId());
        ASSERT(VC_COUNT  == mX2.count());
        ASSERT(VC_COUNT  == X2.count());
        ASSERT(VC_TOTAL  == mX2.total());
        ASSERT(VC_TOTAL  == X2.total());
        ASSERT(VC_MIN    == mX2.min());
        ASSERT(VC_MIN    == X2.min());
        ASSERT(VC_MAX    == mX2.max());
        ASSERT(VC_MAX    == X2.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (veryVerbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { cout << '\t';  P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(VB_METRIC == mX1.metricId());
        ASSERT(VB_METRIC == X1.metricId());
        ASSERT(VB_COUNT  == mX1.count());
        ASSERT(VB_COUNT  == X1.count());
        ASSERT(VB_TOTAL  == mX1.total());
        ASSERT(VB_TOTAL  == X1.total());
        ASSERT(VB_MIN    == mX1.min());
        ASSERT(VB_MIN    == X1.min());
        ASSERT(VB_MAX    == mX1.max());
        ASSERT(VB_MAX    == X1.max());

        if (veryVerbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
