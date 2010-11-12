// bdetu_null.t.cpp          -*-C++-*-

#include <bdetu_null.h>

#include <cstdlib>          // atoi()
#include <cstring>          // strcmp(), memcmp(), memcpy()
#include <iostream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements utility functions for the null value for a set
// of 10 scalar types.  A test case is provided for each of these types in
// which the 3 utility functions are tested for that type.
//-----------------------------------------------------------------------------
// [ 1] isNull<char>(const char&)
// [ 1] nullValue<char>()
// [ 1] setNull<char>(char*)
// [ 2] isNull<short>(const short&)
// [ 2] nullValue<short>()
// [ 2] setNull<short>(short*)
// [ 3] isNull<int>(const int&)
// [ 3] nullValue<int>()
// [ 3] setNull<int>(int*)
// [ 4] isNull<bdes_PlatformUtil::Int64>(const bdes_PlatformUtil::Int64&)
// [ 4] nullValue<bdes_PlatformUtil::Int64>()
// [ 4] setNull<bdes_PlatformUtil::Int64>(bdes_PlatformUtil::Int64*)
// [ 5] isNull<float>(const float&)
// [ 5] nullValue<float>()
// [ 5] setNull<float>(float*)
// [ 6] isNull<double>(const double&)
// [ 6] nullValue<double>()
// [ 6] setNull<double>(double*)
// [ 7] isNull<std::string>(const std::string&)
// [ 7] nullValue<std::string>()
// [ 7] setNull<std::string>(std::string*)
// [ 8] isNull<bdet_Date>(const bdet_Date&)
// [ 8] nullValue<bdet_Date>()
// [ 8] setNull<bdet_Date>(bdet_Date*)
// [ 9] isNull<bdet_Time>(const bdet_Time&)
// [ 9] nullValue<bdet_Time>()
// [ 9] setNull<bdet_Time>(bdet_Time*)
// [10] isNull<bdet_Datetime>(const bdet_Datetime&)
// [10] nullValue<bdet_Datetime>()
// [10] setNull<bdet_Datetime>(bdet_Datetime*)
// [11] isNull<bool>(const bool&)
// [11] nullValue<bool>()
// [11] setNull<bool>(bool*)
// [12] isNull<bdet_DateTz>(const bdet_DateTz&)
// [12] nullValue<bdet_DateTz>()
// [12] setNull<bdet_DateTz>(bdet_DateTz*)
// [13] isNull<bdet_TimeTz>(const bdet_TimeTz&)
// [13] nullValue<bdet_TimeTz>()
// [13] setNull<bdet_TimeTz>(bdet_TimeTz*)
// [14] isNull<bdet_DatetimeTz>(const bdet_DatetimeTz&)
// [14] nullValue<bdet_DatetimeTz>()
// [14] setNull<bdet_DatetimeTz>(bdet_DatetimeTz*)
//-----------------------------------------------------------------------------
// [15] USAGE EXAMPLE

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }


//==========================================================================
//                  STANDARD BDEX EXCEPTION TEST MACROS
//--------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    {                                                                       \
        static int firstTime = 1;                                           \
        if (veryVerbose && firstTime)  cout <<                              \
            "\t\t BDEX EXCEPTION TEST -- (ENABLED) --" << endl;             \
        firstTime = 0;                                                      \
    }                                                                       \
    if (veryVeryVerbose) cout <<                                            \
        "\t\t Begin bdex exception test. --" << endl;                       \
    int bdexExceptionCounter = 0;                                           \
    testInStream.setInputLimit(bdexExceptionCounter);                       \
    do {                                                                    \
        try {

#define END_BDEX_EXCEPTION_TEST                                             \
        } catch (bdex_TestInStreamException& e) {                           \
            if (veryVeryVerbose) cout << endl << "\t*** BDEX_EXCEPTION: "   \
            << "input limit = " << bdexExceptionCounter << ", "             \
            << "last data type = " << e.dataType() << " ***" << endl;       \
            testInStream.setInputLimit(++bdexExceptionCounter);             \
            continue;                                                       \
        }                                                                   \
        testInStream.setInputLimit(-1);                                     \
        break;                                                              \
    } while (1);                                                            \
    if (veryVeryVerbose) cout <<                                            \
        "\t\t End bdex exception test. --" << endl;                         \
}
#else
#define BEGIN_BDEX_EXCEPTION_TEST                                           \
{                                                                           \
    static int firstTime = 1;                                               \
    if (verbose && firstTime) { cout <<                                     \
        "\t\t BDEX EXCEPTION TEST -- (NOT ENABLED) --" << endl;             \
        firstTime = 0;                                                      \
    }                                                                       \
}
#define END_BDEX_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        {
            double nullValue = bdetu_Null<double>::nullValue();
            double value     = 0.0;

            ASSERT(bdetu_Null<double>::isNull(value) == 0);

            bdetu_Null<double>::setNull(&value);

            ASSERT(bdetu_Null<double>::isNull(value) == 1);
            ASSERT(value == nullValue);
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETIMETZ
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_DatetimeTz> function.
        //   2. Assert the value for nullValue<bdet_DatetimeTz> is as expected.
        //   3. For a variable of type bdet_DatetimeTz with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_DatetimeTz>(const bdet_DatetimeTz&)
        //   nullValue<bdet_DatetimeTz>()
        //   setNull<bdet_DatetimeTz>(bdet_DatetimeTz*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for bdet_DatetimeTz" << endl
                         << "=====================================" << endl;

        typedef bdet_DatetimeTz Obj;

        const Obj NULLVALUE;
        const bdet_Datetime NONNULLDATETIME(10, 10, 10, 10, 10, 10, 10);
        const Obj NONNULLVALUE(NONNULLDATETIME, -5);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_TIMETZ
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_TimeTz> function.
        //   2. Assert the value for nullValue<bdet_TimeTz> is as expected.
        //   3. For a variable of type bdet_TimeTz with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_TimeTz>(const bdet_TimeTz&)
        //   nullValue<bdet_TimeTz>()
        //   setNull<bdet_TimeTz>(bdet_TimeTz*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdet_TimeTz" << endl
                                  << "=================================" << endl;

        typedef bdet_TimeTz Obj;

        const Obj NULLVALUE;
        const bdet_Time NONNULLTIME(10, 10, 10, 10);
        const Obj NONNULLVALUE(NONNULLTIME, -5);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETZ
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_DateTz> function.
        //   2. Assert the value for nullValue<bdet_DateTz> is as expected.
        //   3. For a variable of type bdet_DateTz with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_DateTz>(const bdet_DateTz&)
        //   nullValue<bdet_DateTz>()
        //   setNull<bdet_DateTz>(bdet_DateTz*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdet_DateTz" << endl
                                  << "=================================" << endl;

        typedef bdet_DateTz Obj;

        const Obj NULLVALUE;
        const bdet_Date NONNULLDATE(10, 10, 10);
        const Obj NONNULLVALUE(NONNULLDATE, -5);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BOOL
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //   from the isNull<bool> function.
        //   2. Assert the value for nullValue<bool> is as expected.
        //   3. For a variable of type bool with a non null value, use setNull
        //      and verify that it becomes null.
        //
        // Testing:
        //   isNull<bool>(const bool&)
        //   nullValue<bool>()
        //   setNull<bool>(bool*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bool" << endl
                                  << "==========================" << endl;

        typedef bool Obj;

        const Obj DATA[]  = { true, false };
        const Obj NULLVALUE = false;
        const Obj NONNULLVALUE = true;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_((int) DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P((int) bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATETIME
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_Datetime> function.
        //   2. Assert the value for nullValue<bdet_Datetime> is as expected.
        //   3. For a variable of type bdet_Datetime with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_Datetime>(const bdet_Datetime&)
        //   nullValue<bdet_Datetime>()
        //   setNull<bdet_Datetime>(bdet_Datetime*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for bdet_Datetime" << endl
                         << "===================================" << endl;

        typedef bdet_Datetime Obj;

        const Obj NULLVALUE;
        const bdet_Date NONNULLDATE(10, 10, 10);
        const Obj NONNULLVALUE(NONNULLDATE);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_TIME
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_Time> function.
        //   2. Assert the value for nullValue<bdet_Time> is as expected.
        //   3. For a variable of type bdet_Time with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_Time>(const bdet_Time&)
        //   nullValue<bdet_Time>()
        //   setNull<bdet_Time>(bdet_Time*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdet_Time" << endl
                                  << "===============================" << endl;

        typedef bdet_Time Obj;

        const Obj NULLVALUE;
        const Obj NONNULLVALUE(10, 10, 10);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE BDET_DATE
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdet_Date> function.
        //   2. Assert the value for nullValue<bdet_Date> is as expected.
        //   3. For a variable of type bdet_Date with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdet_Date>(const bdet_Date&)
        //   nullValue<bdet_Date>()
        //   setNull<bdet_Date>(bdet_Date*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for bdet_Date" << endl
                                  << "===============================" << endl;

        typedef bdet_Date Obj;

        const Obj NULLVALUE;
        const Obj NONNULLVALUE(10, 10, 10);

        ASSERT( bdetu_Null<Obj>::isNull(NULLVALUE) == 1);

        ASSERT( bdetu_Null<Obj>::isNull(NONNULLVALUE) == 0);

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE std::string
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<std::string> function.
        //   2. Assert the value for nullValue<std::string> is as expected.
        //   3. For a variable of type std::string with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<std::string>(const std::string&)
        //   nullValue<std::string>()
        //   setNull<std::string>(std::string*)
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing functions for std::string" << endl
                         << "=================================" << endl;

        typedef std::string Obj;

        const Obj NULLVALUE;
        const Obj DATA[]  = { NULLVALUE, " ", "abc" };
        const Obj NONNULLVALUE = "x";

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE DOUBLE
        // Concern:
        //   The value chosen for the null value must never round and must
        //   never equal zero.
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<double> function.
        //   2. Assert the value for nullValue<double> is as expected.
        //   3. For a variable of type double with a non null value, use
        //      'setNull' and verify that it becomes null.
        //
        // Testing:
        //   isNull<double>(const double&)
        //   nullValue<double>()
        //   setNull<double>(double*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for double" << endl
                                  << "============================" << endl;

        typedef double Obj;

        const Obj NULLVALUE = -27953.0 / (1 << 30) / (1 << 30);
        const Obj DATA[]  = { NULLVALUE, -1, 1, 0, 127, -127 };
        const Obj NONNULLVALUE = 10;

        ASSERT((NULLVALUE * (1 << 30) * (1 << 30)) == -27953.0);

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        if(veryVerbose) P(NULLVALUE);

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE FLOAT
        // Concern:
        //   The value chosen for the null value must never round and must
        //   never equal zero.
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<float> function.
        //   2. Assert the value for nullValue<float> is as expected.
        //   3. For a variable of type float with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<float>(const float&)
        //   nullValue<float>()
        //   setNull<float>(float*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for float" << endl
                                  << "===========================" << endl;

        typedef float Obj;

        const Obj NULLVALUE = -7568.0/(1<<30)/(1<<30);
        const Obj DATA[]  = { NULLVALUE, -1, 1, 0, 127, -127 };
        const Obj NONNULLVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        ASSERT((NULLVALUE * (1<<30) * (1<<30)) == -7568.0);

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE INT64
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<bdes_PlatformUtil::Int64> function.
        //   2. Assert the value for nullValue<bdes_PlatformUtil::Int64> is as
        //      expected.
        //   3. For a variable of type bdes_PlatformUtil::Int64 with a non null
        //      value, use setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<bdes_PlatformUtil::Int64>(const bdes_PlatformUtil::Int64&)
        //   nullValue<bdes_PlatformUtil::Int64>()
        //   setNull<bdes_PlatformUtil::Int64>(bdes_PlatformUtil::Int64*)
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl << "Testing functions for bdes_PlatformUtil::Int64" << endl
            << "==============================================" << endl;

        typedef bdes_PlatformUtil::Int64 Obj;

        const Obj NULLVALUE = -(bdes_PlatformUtil::Int64)
                              (((bdes_PlatformUtil::Uint64) 1 << 63) - 1) - 1;
        const Obj DATA[]  = { NULLVALUE, -1, 1, 0, 127, -127 };
        const Obj NONNULLVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE INT
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //      from the isNull<int> function.
        //   2. Assert the value for nullValue<int> is as expected.
        //   3. For a variable of type int with a non null value, use setNull
        //      and verify that it becomes null.
        //
        // Testing:
        //   isNull<int>(const int&)
        //   nullValue<int>()
        //   setNull<int>(int*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for int" << endl
                                  << "==========================" << endl;

        typedef int Obj;

        const Obj DATA[]  = { -2147483647 - 1, -1, 1, 0, 127, -127 };
        const Obj NULLVALUE = -2147483647 - 1;
        const Obj NONNULLVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE SHORT
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //   from the isNull<short> function.
        //   2. Assert the value for nullValue<short> is as expected.
        //   3. For a variable of type short with a non null value, use
        //      setNull and verify that it becomes null.
        //
        // Testing:
        //   isNull<short>(const short&)
        //   nullValue<short>()
        //   setNull<short>(short*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for short" << endl
                                  << "==========================" << endl;

        typedef short Obj;

        const Obj DATA[]  = { -32768, -1, 1, 0, 127, -127 };
        const Obj NULLVALUE = -32768;
        const Obj NONNULLVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_(DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P(bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING FUNCTIONS FOR TYPE CHAR
        //
        // Plan:
        //   1. For a set of values (null and not null) test the return value
        //   from the isNull<char> function.
        //   2. Assert the value for nullValue<char> is as expected.
        //   3. For a variable of type char with a non null value, use setNull
        //      and verify that it becomes null.
        //
        // Testing:
        //   isNull<char>(const char&)
        //   nullValue<char>()
        //   setNull<char>(char*)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing functions for char" << endl
                                  << "==========================" << endl;

        typedef char Obj;

        const Obj DATA[]  = { 0, -1, 1, 127, -127 };
        const Obj NULLVALUE = 0;
        const Obj NONNULLVALUE = 10;

        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        for (int i = 0; i < NUM_DATA; ++i) {
            int hasNull = DATA[i] == NULLVALUE;
            if (veryVerbose) { P_((int) DATA[i]); P(hasNull); }
            LOOP_ASSERT(i, bdetu_Null<Obj>::isNull(DATA[i]) == hasNull);
        }

        if (veryVerbose) P((int) bdetu_Null<Obj>::nullValue());

        ASSERT(bdetu_Null<Obj>::nullValue() == NULLVALUE);

        Obj U = NONNULLVALUE;
        ASSERT( bdetu_Null<Obj>::isNull(U) == 0);

        bdetu_Null<Obj>::setNull(&U);
        ASSERT( bdetu_Null<Obj>::isNull(U) == 1);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
    }
    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
