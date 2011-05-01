// baetzo_localtimeperiod.t.cpp                                       -*-C++-*-
#include <baetzo_localtimeperiod.h>

#include <baetzo_localtimedescriptor.h>

#include <bdet_datetime.h>

#include <bdex_byteinstream.h>           // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a value-semantic aggregate type whose state is
// identical to its value.  Given the absence of any complex operation, our
// testing concerns are safely limited to the mechanical functioning of the
// various methods and free operators, and exception neutrality during 'bdex'
// streaming.
//
// The component interface represents a description of a local time period
// value as in its underlying implementation: the combination of a local time
// type value of type 'baetzo_LocalTimeDescriptor', two utc time values of type
// 'bdet_Datetime' representing the start end end of the period.  The primary
// manipulators for 'baetzo_LocalTimePeriod' are the default constructor,
// 'setDescriptor', 'setUtcStartTime' and 'setUtcEndTime'.  The basic
// accessors are 'descriptor', 'utcStartTime' and 'utcEndTime'.
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//=============================================================================
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Converting a UTC Time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use a local time period to convert a
// of UTC time to the corresponding local time in some time zone.
//
// First, we define a function that performs a conversion from UTC time to
// local time:
int convertUtcToLocalTime(bdet_Datetime                 *result,
                          const bdet_Datetime&           utcTime,
                          const baetzo_LocalTimePeriod&  period)
    // Load into the specified 'result' the date-time value corresponding to
    // the specified 'utcTime' in the local time described by the specified
    // 'period'.  Return 0 on success, and a non-zero value if
    // 'utcTime < period.utcStartTime()' or 'utcTime >= period.utcEndTime()'.
{

    if (utcTime < period.utcStartTime() || utcTime >= period.utcEndTime()) {
        return 1;                                                     // RETURN
    }

    *result = utcTime;
    result->addSeconds(period.descriptor().utcOffsetInSeconds());

    return 0;
}

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------
typedef baetzo_LocalTimePeriod Obj;
typedef bdex_TestInStream      In;
typedef bdex_TestOutStream     Out;

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        //   comment characters, and replace 'BSLS_ASSERT' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

//..
// First, we define a 'baetzo_LocalTimePeriod' object that characterizes
// the local time in effect for New York Daylight-Saving Time in 2010:
//..
    enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds

    baetzo_LocalTimeDescriptor edt(NEW_YORK_DST_OFFSET, true, "EDT");

    baetzo_LocalTimePeriod edt2010(edt,
                                   bdet_Datetime(2010,  3, 14, 7),
                                   bdet_Datetime(2010, 11,  7, 6));

    ASSERT(bdet_Datetime(2010,  3, 14, 7) == edt2010.utcStartTime());
    ASSERT(bdet_Datetime(2010, 11,  7, 6) == edt2010.utcEndTime());
    ASSERT("EDT" == edt2010.descriptor().description());
    ASSERT(true == edt2010.descriptor().dstInEffectFlag());
    ASSERT(NEW_YORK_DST_OFFSET == edt2010.descriptor().utcOffsetInSeconds());
//..
// Then, we create a 'bdet_Datetime' representing the UTC time
// "Jul 20, 2010 11:00":
//..
    bdet_Datetime utcDatetime(2010, 7, 20, 11, 0, 0);
//..
// Now we use the 'convertUtcToLocalTime' function we defined earlier to
// convert 'utcDatetime' into its local time in Eastern Daylight Time, as
// described by 'edt2010' (defined above):
//..
    bdet_Datetime localDatetime;
    int status = convertUtcToLocalTime(&localDatetime, utcDatetime, edt2010);
    if (0 != status) {
        // The conversion failed so return an error code.

        return 1;                                                     // RETURN
    }
//..
// Finally, we verify that the result corresponds to the expected local time
// in New York, "Jul 20, 2010 7:00":
//..
    P(localDatetime);
    ASSERT(bdet_Datetime(2010, 7, 20, 7) == localDatetime);
//..
    } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 5], and
        //   assignment operator without [7, 8] and with [9] aliasing.  Use the
        //   basic accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while 'U'
        //   denotes the valid, but "unknown", default object value.
        //
        // 1. Create an object x1 (init. to VA).    { x1:VA }
        // 2. Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3. Set x1 to VB.                         { x1:VB x2:VA }
        // 4. Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        // 5. Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        // 6. Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        // 7. Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        // 8. Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:U }
        // 9. Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing
        const int OA = -4 * 60 * 60;  // offset for VA
        const int OB =  1 * 60 * 60;  // offset for VB
        const int OC =  0;            // offset for VC

        const bool IDA = true ;       // isDst for VA
        const bool IDB = false;       // isDst for VB
        const bool IDC = false;       // isDst for VC

        const char *DESCA = "EDT";    // description for VA
        const char *DESCB = "GMT+1";  // description for VB
        const char *DESCC = "UTC";    // description for VC

        const baetzo_LocalTimeDescriptor LTTA(OA, IDA, DESCA);  // desc for VA
        const baetzo_LocalTimeDescriptor LTTB(OB, IDB, DESCB);  // desc for VB
        const baetzo_LocalTimeDescriptor LTTC(OC, IDC, DESCC);  // desc for VC

        const bdet_Datetime SDA(2010,  3, 14,  2,  0,  0);  // start time VA
        const bdet_Datetime EDA(2010, 11,  7,  2,  0,  0);  // end time VA
        const bdet_Datetime SDB(2010,  3, 28,  2,  0,  0);  // start time VB
        const bdet_Datetime EDB(2010, 10, 30,  2,  0,  0);  // end time VB
        const bdet_Datetime SDC(   1,  1,  2,  0,  0,  0);  // start time VC
        const bdet_Datetime EDC(9999, 12, 31, 23, 59, 59);  // end time VC

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(LTTA, SDA, EDA);  const Obj& X1 = mX1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(LTTA == X1.descriptor());
        ASSERT(SDA  == X1.utcStartTime());
        ASSERT(EDA  == X1.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(LTTA == X2.descriptor());
        ASSERT(SDA  == X2.utcStartTime());
        ASSERT(EDA  == X2.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDescriptor(LTTB);
        mX1.setUtcStartAndEndTime(SDB, EDB);
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(LTTB == X1.descriptor());
        ASSERT(SDB  == X1.utcStartTime());
        ASSERT(EDB  == X1.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;

        {
            const baetzo_LocalTimeDescriptor defaultTimeType;
            const bdet_Datetime              defaultDatetime;

            ASSERT(defaultTimeType == X3.descriptor());
            ASSERT(defaultDatetime == X3.utcStartTime());
            ASSERT(defaultDatetime == X3.utcEndTime());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { T_ P(X4) }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        {
            const baetzo_LocalTimeDescriptor defaultTimeType;
            const bdet_Datetime              defaultDatetime;

            ASSERT(defaultTimeType == X4.descriptor());
            ASSERT(defaultDatetime == X4.utcStartTime());
            ASSERT(defaultDatetime == X4.utcEndTime());
        }

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setDescriptor(LTTC);
        mX3.setUtcStartAndEndTime(SDC, EDC);
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(LTTC == X3.descriptor());
        ASSERT(SDC  == X3.utcStartTime());
        ASSERT(EDC  == X3.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(LTTB == X2.descriptor());
        ASSERT(SDB  == X2.utcStartTime());
        ASSERT(EDB  == X2.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(LTTC == X2.descriptor());
        ASSERT(SDC  == X2.utcStartTime());
        ASSERT(EDC  == X2.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(LTTB == X1.descriptor());
        ASSERT(SDB  == X1.utcStartTime());
        ASSERT(EDB  == X1.utcEndTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
