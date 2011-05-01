// baet_localdatetime.t.cpp                                           -*-C++-*-

#include <baet_localdatetime.h>

#include <bdet_datetime.h>
#include <bdeut_stringref.h>

#include <bdex_byteinstream.h>           // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, unconstrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore respectively the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setDatetimeTz'
//: o 'setTimeZoneId'
//
// Basic Accessors:
//: o 'datetimeTz'
//: o 'timeZoneId'
//
// We will therefore follow our standard 10-step approach to testing
// value-semantic types, with certain test cases omitted:
//: o [ 3] -- No custom test apparatus is needed.
//
// Global Concerns:
//: o ACCESSOR methods are declared 'const'.
//: o Getter methods do not allocate memory.
//: o CREATOR & MANIPULATOR pointer/reference arguments are declared 'const'.
//: o Memory is allocated from the appropriate allocator.
//: o Memory is *never* allocated from the global allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
// ----------------------------------------------------------------------------
// CLASS METHODS
// [  ] static int maxSupportedBdexVersion();
//
// CREATORS
// [  ] baet_LocalDatetime(bslma_Allocator *bA = 0);
// [  ] baet_LocalDatetime(DatetimeTz& d, const char *t, *bA = 0);
// [  ] baet_LocalDatetime(const baet_LocalDatetime& o, *bA = 0);
//
// MANIPULATORS
// [  ] baet_LocalDatetime& operator=(const baet_LocalDatetime& rhs);
// [  ] setDatetimeTz(const bdet_DatetimeTz& value);
// [  ] setTimeZoneId(const char *value);
// [  ] swap(baet_LocalDatetime& other);
// [  ] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [  ] const bdet_DatetimeTz& datetimeTz() const;
// [  ] const bsl::string& timeZoneId() const;
// [  ] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
// [  ] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [  ] bool operator==(const baet_LocalDatetime& lhs, rhs);
// [  ] bool operator!=(const baet_LocalDatetime& lhs, rhs);
// [  ] operator<<(ostream& s, const baet_LocalDatetime& d);
// [  ] swap(baet_LocalDatetime& a, b);
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE
// [  ] CONCERN: All memory allocation is from the object's allocator
// [  ] CONCERN: There is no temporary allocation from any allocator.
// [  ] CONCERN: Memory allocation is exception neutral.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

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

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef baet_LocalDatetime  Obj;

typedef bslma_TestAllocator TestAllocator;
typedef bdex_TestInStream   In;
typedef bdex_TestOutStream  Out;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    TestAllocator globalAllocator(veryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

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
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;
// In this usage example we illustrate how to create and use a
// 'baet_LocalDatetime' object:
//
// First, we create a default uninitialized 'localDatetime':
//..
      baet_LocalDatetime localDatetime;
//..
// Now, we update the time referred to by 'localDatetime', to the New York time
// "Dec. 25, 2009, 11:00", and we set the time zone identifier of
// 'localDatetime' to "America/New_York":
//..
      bdet_Datetime     datetime(2009, 12, 25, 11, 00, 00);
      bdet_DatetimeTz   datetimeTz(datetime, -500);

      bsl::string       timeZoneId("America/New_York");
      localDatetime.setDatetimeTz(datetimeTz);
      localDatetime.setTimeZoneId(timeZoneId.c_str());

      ASSERT(timeZoneId == localDatetime.timeZoneId());
      ASSERT(datetimeTz == localDatetime.datetimeTz());
//..
// Next, we change the time zone id to another string, for example
// "Europe/Berlin", and verify it is valid:
//..
    bsl::string anotherTimeZoneId("Europe/Berlin");
    localDatetime.setDatetimeTz(datetimeTz);
    localDatetime.setTimeZoneId(anotherTimeZoneId.c_str());

    ASSERT(anotherTimeZoneId == localDatetime.timeZoneId());
    ASSERT(datetimeTz        == localDatetime.datetimeTz());
//..
// Finally, we stream the value of 'dt2' to 'stdout':
//..
    bsl::cout << localDatetime << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  [ 25DEC2009_11:00:00.000-0820, Europe/Berlin ]
//..
      }; break;
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
        const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for VA
        const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for VB
        const int YRC = 7, MOC = 8, DAC = 9;           // y, m, d for VC

        const int HRA = 1, MIA = 2, SCA = 3, MSA = 4;  // h, m, s, ms for VA
        const int HRB = 5, MIB = 6, SCB = 7, MSB = 8;  // h, m, s, ms for VB
        const int HRC = 9, MIC = 9, SCC = 9, MSC = 9;  // h, m, s, ms for VC

        const bdet_Date DA(YRA, MOA, DAA),
                        DB(YRB, MOB, DAB),
                        DC(YRC, MOC, DAC);

        const bdet_Time TA(HRA, MIA, SCA, MSA),
                        TB(HRB, MIB, SCB, MSB),
                        TC(HRC, MIC, SCC, MSC);

        const bdet_Datetime DTA(DA, TA),
                            DTB(DB, TB),
                            DTC(DC, TC);

        const int OA = 60, OB = -300, OC = 270;

        const bdet_DatetimeTz DTZA(DTA, OA),
                              DTZB(DTB, OB),
                              DTZC(DTC, OC);

        const bsl::string TZIA = "Europe/Berlin",
                          TZIB = "America/New_York",
                          TZIC = "Pacific/Kawajelein";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DTZA, TZIA.c_str());  const Obj& X1 = mX1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(DTZA == X1.datetimeTz());
        ASSERT(TZIA == X1.timeZoneId());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(DTZA == X2.datetimeTz());
        ASSERT(TZIA == X2.timeZoneId());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDatetimeTz(DTZB);
        mX1.setTimeZoneId(TZIB.c_str());
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(DTZB == X1.datetimeTz());
        ASSERT(TZIB == X1.timeZoneId());

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
            const bdet_DatetimeTz defaultDatetimeTz;
            const bsl::string     defaultString;

            ASSERT(defaultDatetimeTz == X3.datetimeTz());
            ASSERT(defaultString     == X3.timeZoneId());
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
            const bdet_DatetimeTz defaultDatetimeTz;
            const bsl::string     defaultString;

            ASSERT(defaultDatetimeTz == X4.datetimeTz());
            ASSERT(defaultString     == X4.timeZoneId());
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
        mX3.setDatetimeTz(DTZC);
        mX3.setTimeZoneId(TZIC.c_str());
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(DTZC == X3.datetimeTz());
        ASSERT(TZIC == X3.timeZoneId());

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
        ASSERT(DTZB == X2.datetimeTz());
        ASSERT(TZIB == X2.timeZoneId());

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
        ASSERT(DTZC == X2.datetimeTz());
        ASSERT(TZIC == X2.timeZoneId());

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
        ASSERT(DTZB == X1.datetimeTz());
        ASSERT(TZIB == X1.timeZoneId());

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
