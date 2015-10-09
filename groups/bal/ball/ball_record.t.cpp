// ball_record.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_record.h>

#include <ball_severity.h>                                 // for testing only
#include <ball_userfields.h>

#include <bdlt_currenttime.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetimeutil.h>
#include <bdlt_epochutil.h>

#include <bslmt_threadutil.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bdls_processutil.h>

#include <bslim_testutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strlen(), memset(), memcpy(), memcmp()
#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>     // getpid()
#endif

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test has value-semantic and contains value-semantic
// members.  In addition to standard value-semantic methods, this component
// also implements a method to provide a (tight) upper bound to the dynamic
// memory required to hold the current instance, which is implemented with a
// tested component (ball_countingallocator).  Our concerns regarding the
// implementation of this component are that (1) all the standard
// value-semantic methods are implemented correctly (2) the bookkeeping of the
// dynamically allocated memory are done correctly and (3) the bookkeeping is
// done correctly when exception are thrown.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 2] ball::Record(bslma::Allocator *ba = 0);
// [ 2] ball::Record(const RecordAttributes&, const UserFields&, *ba = 0);
// [ 7] ball::Record(const ball::Record& original, *ba = 0);
// [ 2] ~ball::Record();
// [ 8] ball::Record& operator=(const ball::Record& rhs);
// [ 1] ball::RecordAttributes& fixedFields();
// [ 2] void setFixedFields(const ball::RecordAttributes& fixedFields);
// [ 2] void setUserFields(const ball::UserFields& userFields);
// [ 1] ball::UserFields& userFields();
// [ 4] const ball::RecordAttributes& fixedFields() const;
// [ 4] const ball::UserFields& userFields() const;
// [ 9] int numAllocatedBytes() const;
// [  ] bsl::ostream& print(bsl::ostream& stream, int level, int spl) const;
// [ 6] bool operator==(const ball::Record& lhs, const ball::Record& rhs);
// [ 6] operator!=(const ball::Record& lhs, const ball::Record& rhs);
// [ 9] STREAM& operator>>(STREAM& stream, ball::Record& rhs);
// [ 9] STREAM& operator<<(STREAM& stream, const ball::Record& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('ball::UserFields')
// [10] USAGE EXAMPLE 1
// [11] USAGE EXAMPLE 2

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

typedef ball::Record           Obj;
typedef ball::RecordAttributes Record_Attr;
typedef ball::UserFields  Values;
typedef bsls::Types::Int64     Int64;


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    // ------------------------------------------------------------------------
    // The following "global constants for testing" are in 'main' because the
    // test allocator (defined in 'main') is used in the constructors of the
    // test lists and tables.
    //
    // Make three sets of useful constants for testing: Ui, Xi, Yi
    // ------------------------------------------------------------------------

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    // TBD:
    testAllocator.setQuiet(1);
    bslma::Allocator     *Z = &testAllocator;

    Values VALUES_A(Z);
    Values VALUES_B(Z);
    VALUES_B.appendInt64(1);
    Values VALUES_C(Z);
    VALUES_C.appendInt64(1);
    VALUES_C.appendDouble(2.0);
    VALUES_C.appendString("A");
    VALUES_C.appendDatetimeTz(bdlt::DatetimeTz(bdlt::Datetime(1970,1,1), -10));

    Values *VALUES_DATA[] = { &VALUES_A, &VALUES_B, &VALUES_C };
    const int NUM_VALUES_DATA = sizeof(VALUES_DATA) / sizeof(*VALUES_DATA);

    struct {
        int d_pid;  int d_tid;   int d_lineNum;   int d_severity;
        int d_year; int d_month; int d_day;
        const char *d_fileName;
        const char *d_category;
        const char *d_message;
    } ATTR_VALUES[] = {
///pid     tid     line sev year  month day  file name   category  message
///------- ------- ---- --- ----  ----  ---  ----------  --------  ----------
 { 1,      0,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      1,      0,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      1,   0,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   1,  2004, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2000, 1,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 2,    1,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    2,   "",         "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "FILE1",    "",       ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "CATE1",  ""        },
 { 0,      0,      0,   0,  2004, 1,    1,   "",         "",       "MSG1"    }
        };

    const int NUM_ATTRS = sizeof ATTR_VALUES / sizeof ATTR_VALUES[0];
    Record_Attr REC_ATTRS[10];
    for (int jj = 0; jj < NUM_ATTRS; ++jj) {
        const int PID        = ATTR_VALUES[jj].d_pid;
        const int TID        = ATTR_VALUES[jj].d_tid;
        const int YEAR       = ATTR_VALUES[jj].d_year;
        const int MONTH      = ATTR_VALUES[jj].d_month;
        const int DAY        = ATTR_VALUES[jj].d_day;
        const int LINE       = ATTR_VALUES[jj].d_lineNum;
        const int SEVERITY   = ATTR_VALUES[jj].d_severity;
        const char *FILENAME = ATTR_VALUES[jj].d_fileName;
        const char *CATEGORY = ATTR_VALUES[jj].d_category;
        const char *MESSAGE  = ATTR_VALUES[jj].d_message;
        Record_Attr mY(
                bdlt::Datetime(YEAR, MONTH, DAY),
                PID,
                TID,
                FILENAME,
                LINE,
                CATEGORY,
                SEVERITY,
                MESSAGE);

        REC_ATTRS[jj] = mY;
    }

    const Record_Attr AA = REC_ATTRS[0];
    const Record_Attr AB = REC_ATTRS[1];
    const Record_Attr AC = REC_ATTRS[2];
    const Record_Attr AD = REC_ATTRS[3];
    const Record_Attr AE = REC_ATTRS[4];
    const Record_Attr AF = REC_ATTRS[5];
    const Record_Attr AG = REC_ATTRS[6];
    const Record_Attr AH = REC_ATTRS[7];
    const Record_Attr AI = REC_ATTRS[8];
    const Record_Attr AJ = REC_ATTRS[9];
    const Record_Attr AU;   // default value

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example-2 provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example-2 from header into driver, remove
        //   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example-2" << endl
                                  << "=======================" << endl;


///Usage
///------
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::Record'
/// - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and set the properties of
// a 'ball::Record'.  Note that users of the 'ball' logging subsystem are not
// expected to create records directly.
//
// First we default create a 'ball::Record', 'record', and verify it has a
// default set of attributes:
//..
    ball::Record record;

    ASSERT(ball::RecordAttributes() == record.fixedFields());
    ASSERT(0                        == record.userFields().length());
//..
// Then, we set the fixed fields of the record to contain a simple message:
//..
    int                 processId = bdls::ProcessUtil::getProcessId();
    bsls::Types::Uint64 threadId  = bslmt::ThreadUtil::selfIdAsUint64();

    ball::RecordAttributes attributes(bdlt::CurrentTime::utc(), // time stamp
                                      processId,                // process id
                                      threadId,                 // thread id
                                      __FILE__,                 // filename
                                      __LINE__,                 // line number
                                      "ExampleCategory",        // category
                                      ball::Severity::e_WARN,   // severity
                                      "Simple Test Message");   // message
    record.setFixedFields(attributes);

    ASSERT(attributes == record.fixedFields());
//..
// Finally, we write the record to a stream:
//..
    bsl::ostringstream output;
    output << record << bsl::endl;
//..
    if (verbose) {
        bsl::cout << output.str() << bsl::endl;
    }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION numAllocatedBytes()
        //   Verify the memory usage report is correct.
        //
        // Plan:
        //   Create a sequence of objects with a test allocator.  Call the
        //   method under testing and compare the results with those from
        //   the test allocator.
        //
        // Testing:
        //   int numAllocatedBytes() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "\nTesting 'numAllocatedBytes'." << endl
                 << "==============================" << endl;
        }

        const Obj VU(AU, VALUES_A);
        const Obj VA(AA, VALUES_B);
        const Obj VB(AB, VALUES_C);
        const Obj VC(AC, VALUES_A);
        const Obj VD(AD, VALUES_B);
        const Obj VE(AE, VALUES_C);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VU, VA, VB, VC, VD, VE };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bslma::TestAllocator localTestAllocator;

        for (int i = 0; i < NUM_VALUES; ++i) {
            Obj X(VALUES[i], &localTestAllocator);
            const int memUsage =
                X.numAllocatedBytes();
            const int memAlloc = localTestAllocator.numBytesInUse();

            if (veryVerbose) { P_(memUsage); P(memAlloc); }
            LOOP_ASSERT(i, memUsage == memAlloc);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
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
        //   bdlt::Date& operator=(const bdlt::Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];

            int i;
            for (i = 0; i < NUM_RECATTRS; ++i) {
                Obj v;  const Obj& V = v;
                int ii = i % NUM_VALUES_DATA;

                v.setFixedFields(REC_ATTRS[i]);
                v.setUserFields(*VALUES_DATA[ii]);
                for (int j = 0; j < NUM_RECATTRS; ++j) {
                    Obj u;  const Obj& U = u;
                    int jj = j % NUM_VALUES_DATA;
                    u.setFixedFields(REC_ATTRS[j]);
                    u.setUserFields(*VALUES_DATA[jj]);
                    if (veryVerbose) { T_;  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_RECATTRS; ++i) {
                Obj u;  const Obj& U = u;
                int ii = i % NUM_VALUES_DATA;
                u.setFixedFields(REC_ATTRS[i]);
                u.setUserFields(*VALUES_DATA[ii]);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_;  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct and object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   ball::Record(const ball::Record&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;
        {
            const int NUM_RECATTRS = sizeof REC_ATTRS / sizeof REC_ATTRS[0];

            for (int i = 0; i < NUM_RECATTRS; ++i) {
                Obj w;  const Obj& W = w;           // control
                int j = i % NUM_VALUES_DATA;
                w.setFixedFields(REC_ATTRS[i]);
                w.setUserFields(*VALUES_DATA[j]);

                Obj x;  const Obj& X = x;
                x.setFixedFields(REC_ATTRS[i]);
                x.setUserFields(*VALUES_DATA[j]);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);  LOOP_ASSERT(i, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   Any subtle variation in value must be detected by the equality
        //   operators.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //   S X S
        //
        // Testing:
        //   operator==(const ball::Record&, const ball::Record&);
        //   operator!=(const ball::Record&, const ball::Record&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;
        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {

            for (int i = 0; i < 3; ++i) {
                Obj u;  const Obj& U = u;
                u.setFixedFields(REC_ATTRS[i]);
                u.setUserFields(*VALUES_DATA[i]);
                for (int j = 0; j < 3; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setFixedFields(REC_ATTRS[j]);
                    v.setUserFields(*VALUES_DATA[j]);
                    int isSame = i == j;
                    if (veryVerbose) { T_;  P_(i);  P_(j);  P_(U);  P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                    LOOP2_ASSERT(i, j,  isSame == (V == U));
                    LOOP2_ASSERT(i, j, !isSame == (V != U));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Plan:  BUG
        //
        // Testing:
        //   operator<<(ostream&, const ball::Record&);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        //   We want to verify that each individual field is returned
        //   correctly.
        //
        // Plan:
        //   For a sequence of unique object values, verify that each of the
        //   basic accessors returns the correct value.
        //
        // Testing:
        //   const ball::RecordAttributes& fixedFields();
        //   const ball::UserFields& userFields();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Basic Accessors" << endl
            << "=======================" << endl;


        for (int ii = 0; ii < NUM_VALUES_DATA; ++ii) {
            const Values& X = *VALUES_DATA[ii];

            if (verbose) { cout << "\t"; P(X); }

            for (int jj = 0; jj < NUM_ATTRS; ++jj) {

                const Record_Attr& Y = REC_ATTRS[jj];

                Obj mT(Y, X, &testAllocator);

                if (veryVeryVerbose) { P(mT); }
                LOOP2_ASSERT(ii, jj, mT.fixedFields() == Y);
                LOOP2_ASSERT(ii, jj, mT.userFields()  == X);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'GG' AND 'GGG' ('ball::UserFields')
        // Plan:
        //   Test the behavior of 'gg' and 'ggg'.
        //   * Verify that all test values are distinct.
        //   * Show 'gg' returns a correct reference to its first argument.
        //   * Enumerate representative input, ordered by length.
        //   * Show the correct accessing of all 66 unique values (of 22
        //     unique types) that are a part of the 'gg' language.
        //   * Test large legal input strings.
        //   * Test 'ggg' syntax checking with syntactically illegal inputs.
        //
        // Testing:
        //   ball::UserFields& gg(ball::UserFields *address, const char *spec);
        //   int ggg(UserFields *address, const char *spec, int showErrorFlag);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
            << "Testing 'gg' and 'ggg' generator functions" << endl
            << "==========================================" << endl;


      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        //   We want to verify the constructor and the primary manipulators set
        //   the member fields correctly.
        //
        // Plan:
        //   Create three RecordAttributes and UserFields objects with
        //   different values - one unset value and two distinct values.
        //   Construct ball::Record objects with these values and with default
        //   constructor, verify the values with the basic accessors, verify
        //   the equality and inequality of these objects.  Use the primary
        //   manipulators to set values, verify the values are correctly set.
        //   The destructor is exercised as the objects being tested leave
        //   scope.
        //
        // Testing:
        //   ball::Record(const RecordAttributes &, const UserFields &);
        //   ball::Record();
        //   void setFixedFields(const ball::RecordAttributes &);
        //   void setUserFields(const ball::UserFields &);
        //   ~ball::Record();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Primary Manipulators" << endl
                                  << "============================" << endl;

        Record_Attr FA;
        Record_Attr FB(bdlt::Datetime(2004, 1, 21, 12, 30, 25, 150),
                       bdls::ProcessUtil::getProcessId(),
                       0,  // threadID
                       __FILE__,
                       __LINE__,
                       "MyCategory1",
                       ball::Severity::e_INFO,
                       "Distinct Message 1");
        Record_Attr FC(bdlt::Datetime(2000, 2, 29, 10, 13, 55, 111),
                       bdls::ProcessUtil::getProcessId(),
                       0,  // threadID
                       __FILE__,
                       __LINE__,
                       "MyCategory2",
                       ball::Severity::e_ERROR,
                       "Distinct Message 2");

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X((bslma::Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Default Ctor" << endl;
            const Obj X(&testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif
        }

        if (verbose) cout << "\nTesting ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            const Obj X(FB, *VALUES_DATA[0], (bslma::Allocator *)0);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            const Obj X(FB, *VALUES_DATA[1], &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Ctor" << endl;
            const Obj X(FB, *VALUES_DATA[1], &testAllocator);
            if (veryVerbose) { cout << "\t\t"; P(X); }
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif
        }
        if (verbose) cout << "\nTesting primary manipulator." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            Obj mX;
            Obj mY(FB, *VALUES_DATA[1]);
            Obj mZ(FC, *VALUES_DATA[2]);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);
            ASSERT(mY.fixedFields() == FB);
            ASSERT(mZ.fixedFields() == FC);

            ASSERT(mX.userFields()  == Values());
            ASSERT(mY.userFields()  == *VALUES_DATA[1]);
            ASSERT(mZ.userFields()  == *VALUES_DATA[2]);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                  << endl;
            mX.setFixedFields(FB);
            ASSERT(mX.fixedFields() == FB);

            mX.setUserFields(*VALUES_DATA[1]);
            ASSERT(mX.userFields() == *VALUES_DATA[1]);

            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                                  << endl;
            mX.setFixedFields(FC);
            ASSERT(mX.fixedFields() == FC);

            mX.setUserFields(*VALUES_DATA[2]);
            ASSERT(mX.userFields()  == *VALUES_DATA[2]);

            ASSERT(mX == mZ);
        }

        if (verbose) cout << "\tWith an allocator." << endl;
        if (verbose) cout << "\t\tWithout exceptions." << endl;
        {


            bslma::TestAllocator testAllocatorX(veryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVerbose);
            Obj mX(&testAllocatorX);
            Obj mY(FB, *VALUES_DATA[1], &testAllocatorY);
            Obj mZ(FC, *VALUES_DATA[2], &testAllocatorZ);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);
            ASSERT(mY.fixedFields() == FB);
            ASSERT(mZ.fixedFields() == FC);

            ASSERT(mX.userFields()  == Values());
            ASSERT(mY.userFields()  == *VALUES_DATA[1]);
            ASSERT(mZ.userFields()  == *VALUES_DATA[2]);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                  << endl;
            mX.setFixedFields(FB);
            ASSERT(mX.fixedFields() == FB);

            mX.setUserFields(*VALUES_DATA[1]);
            ASSERT(mX.userFields() == *VALUES_DATA[1]);

            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                                  << endl;
            mX.setFixedFields(FC);
            ASSERT(mX.fixedFields() == FC);

            mX.setUserFields(*VALUES_DATA[2]);
            ASSERT(mX.userFields()  == *VALUES_DATA[2]);

            ASSERT(mX == mZ);
        }
        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            bslma::TestAllocator testAllocatorX(veryVeryVerbose);
            bslma::TestAllocator testAllocatorY(veryVeryVerbose);
            bslma::TestAllocator testAllocatorZ(veryVeryVerbose);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
#endif
            if (veryVerbose) cout <<
                "\tTesting Exceptions In Primary Manipulator" << endl;
            Obj mX(&testAllocatorX);
            Obj mY(FB, *VALUES_DATA[1], &testAllocatorY);
            Obj mZ(FC, *VALUES_DATA[2], &testAllocatorZ);

            if (veryVeryVerbose) { P(mX); P(mY); P(mZ); }

            ASSERT(FA != FB); ASSERT(FA != FC); ASSERT(FC != FB);
            ASSERT(0 == (mX == mY)); ASSERT(0 == (mX == mZ));
            ASSERT(0 == (mY == mZ));

            ASSERT(mX.fixedFields() == FA);
            ASSERT(mY.fixedFields() == FB);
            ASSERT(mZ.fixedFields() == FC);

            ASSERT(mX.userFields()  == Values());
            ASSERT(mY.userFields()  == *VALUES_DATA[1]);
            ASSERT(mZ.userFields()  == *VALUES_DATA[2]);

            if (veryVerbose) cout << "\tSetting mX with mY's initializer."
                                  << endl;
            mX.setFixedFields(FB);
            ASSERT(mX.fixedFields() == FB);

            mX.setUserFields(*VALUES_DATA[1]);
            ASSERT(mX.userFields() == *VALUES_DATA[1]);

            ASSERT(mX == mY);

            if (veryVerbose) cout << "\tSetting mX with mZ's initializer."
                            << endl;
            mX.setFixedFields(FC);
            ASSERT(mX.fixedFields() == FC);

            mX.setUserFields(*VALUES_DATA[2]);
            ASSERT(mX.userFields()  == *VALUES_DATA[2]);

            ASSERT(mX == mZ);
#if !defined(BSLS_PLATFORM_CMP_MSVC) || defined(BDE_BUILD_TARGET_OPT)
          } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
#endif
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular, we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //     - constructor/destructor
        //     - assignment operator
        //     - userFields()/fixedFields() (non-const versions)
        //     - userFields()/fixedFields() (const versions)
        //     - setuserFields()/setFixedFields()
        //     - numAllocatedBytes()
        //     - equality/inequality operator (== and !=)
        //     - etc.
        //
        // Plan:
        //   Create list FA and FB with different schemas.  Create record
        //   attributes UA and UB with different values.  Instantiate test
        //   objects RA from FA and UA and RB from FB and UB.  Use accessors to
        //   verify the correct values.  Copy construct RC from RA, verify
        //   their equality.  Use manipulators to set RA to values hold by RB,
        //   verify RA's inequality with RC, and equality with RB.  Reset RA
        //   original values.  Get the dynamic memory usage.  To test non-const
        //   versions of the 'userFields' and 'fixedFields' methods, create a
        //   record RA from FA and *VALUES_DATA[1], get modifiables references
        //   to fixed field and user field and assign them FB and
        //   *VALUES_DATA[2] and finally verify that record is appropriately
        //   modified.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.

        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        Record_Attr FA(bdlt::Datetime(),
                       bdls::ProcessUtil::getProcessId(),
                       0,  // threadID
                       __FILE__, __LINE__,
                       "EQUITY.NASD",
                       ball::Severity::e_INFO,
                       "Ticker Summary");
        bdlt::Datetime now = bdlt::EpochUtil::convertFromTimeT(time(0));
        FA.setTimestamp(now);

        if (verbose) { P(FA) ; }

        Obj RA(FA, *VALUES_DATA[1]);

        ASSERT(FA == RA.fixedFields());
        ASSERT(*VALUES_DATA[1] == RA.userFields());

        Record_Attr FB(bdlt::Datetime(),
                       bdls::ProcessUtil::getProcessId(),
                       0,  // threadID
                       __FILE__, __LINE__,
                       "USER_SESSION",
                       ball::Severity::e_TRACE,
                       "User Session Info");

        now = bdlt::EpochUtil::convertFromTimeT(time(0));
        FB.setTimestamp(now);

        if (verbose) { P(FB) ; }

        Obj RB(FB, *VALUES_DATA[2]);
        ASSERT(FB == RB.fixedFields());
        ASSERT(*VALUES_DATA[2] == RB.userFields());

        Obj RC(RA);
        ASSERT(RC == RA);               ASSERT(RC != RB);

        RA.setFixedFields(FB);
        RA.setUserFields(*VALUES_DATA[2]);
        ASSERT(FB == RA.fixedFields());
        ASSERT(*VALUES_DATA[2] == RA.userFields());
        ASSERT(RA == RB);               ASSERT(RA != RC);

        RA.setFixedFields(FA);
        RA.setUserFields(*VALUES_DATA[1]);
        ASSERT(FA == RA.fixedFields());
        ASSERT(*VALUES_DATA[1] == RA.userFields());
        ASSERT(RA != RB);               ASSERT(RA == RC);

        RC = RB;
        ASSERT(FB == RC.fixedFields());
        ASSERT(*VALUES_DATA[2] == RC.userFields());
        ASSERT(RA != RC);               ASSERT(RB == RC);

        const int sizea = RA.numAllocatedBytes();
        const int sizeb = RB.numAllocatedBytes();
        if (verbose) { P_(sizea); P(sizeb); }
        ASSERT(0 < sizea);              ASSERT(0 < sizeb);

        {
            if (verbose) {
                cout << "testing const versions of 'fixedFields'"
                     << "and 'userFields'"
                     << endl;
            }

            Obj RA(FA, *VALUES_DATA[1]);
            Record_Attr &ff = RA.fixedFields();
            ff = FB;
            Values &uf = RA.userFields();
            uf = *VALUES_DATA[2];
            const Obj &CRA = RA;
            LOOP_ASSERT(CRA.fixedFields(), CRA.fixedFields() == FB);
            LOOP_ASSERT(CRA.userFields(),
                        CRA.userFields() == *VALUES_DATA[2]);
            if (verbose) {
                cout << "tested const versions of 'fixedFields'"
                     << "and 'userFields'"
                     << endl;
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
