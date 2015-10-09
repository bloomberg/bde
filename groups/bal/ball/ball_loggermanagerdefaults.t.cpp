// ball_loggermanagerdefaults.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_loggermanagerdefaults.h>

#include <bsl_string.h>
#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing a
// constrained-attribute type.
//
// In the test plan documentation below, 'ball::LMD' is occasionally used for
// 'ball::LoggerManagerDefaults' for brevity.
//
//-----------------------------------------------------------------------------
// [ 1] ball::LoggerManagerDefaults();
// [ 1] ball::LoggerManagerDefaults(const ball::LoggerManagerDefaults&);
// [ 1] ~ball::LoggerManagerDefaults();
// [ 1] ball::LMD& operator=(const ball::LoggerManagerDefaults& rhs);
// [ 2] int setDefaultRecordBufferSizeIfValid(int numBytes);
// [ 2] int setDefaultLoggerBufferSizeIfValid(int numBytes);
// [ 2] int setDefaultThresholdLevelsIfValid(int, int, int, int);
// [ 3] int setDefaultThresholdLevelsIfValid(4);
// [ 1] int defaultRecordBufferSize() const;
// [ 1] int defaultLoggerBufferSize() const;
// [ 1] int defaultRecordLevel() const;
// [ 1] int defaultPassLevel() const;
// [ 1] int defaultTriggerLevel() const;
// [ 1] int defaultTriggerAllLevel() const;
//
// [ 1] bool operator==(const ball::LMD& lhs, const ball::LMD& rhs);
// [ 1] bool operator!=(const ball::LMD& lhs, const ball::LMD& rhs);
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const ball::LMD& rhs);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::LoggerManagerDefaults Obj;

const int NUM_VALUES = 7;

// The 0th element in each array contains the value set by the default
// constructor for the corresponding attribute.

// NOTE: In this test driver only, these values must be written as explicit
// literals to provide an independent representation.  These values MUST NOT
// be copied to other test drivers.  Use the 'Obj::defaultDefault...' class
// methods in other test drivers.

const int RECORD_BUFFER_SIZE[NUM_VALUES] = { 32768, 10, 20, 30, 40, 50, 60 };
const int LOGGER_BUFFER_SIZE[NUM_VALUES] = {  8192, 11, 21, 31, 41, 51, 61 };
const int RECORD_LEVEL[NUM_VALUES]       = {     0, 12, 22, 32, 42, 52, 62 };
const int PASS_LEVEL[NUM_VALUES]         = {    64, 13, 23, 33, 43, 53, 63 };
const int TRIGGER_LEVEL[NUM_VALUES]      = {     0, 14, 24, 34, 44, 54, 64 };
const int TRIGGER_ALL_LEVEL[NUM_VALUES]  = {     0, 15, 25, 35, 45, 55, 65 };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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

        ball::LoggerManagerDefaults lmd;

        ASSERT(    0 == lmd.setDefaultRecordBufferSizeIfValid(32768));
        ASSERT(32768 == lmd.defaultRecordBufferSize());

        ASSERT(   0 == lmd.setDefaultLoggerBufferSizeIfValid(2048));
        ASSERT(2048 == lmd.defaultLoggerBufferSize());

        ASSERT(  0 == lmd.setDefaultThresholdLevelsIfValid(192, 64, 48, 32));
        ASSERT(192 == lmd.defaultRecordLevel());
        ASSERT( 64 == lmd.defaultPassLevel());
        ASSERT( 48 == lmd.defaultTriggerLevel());
        ASSERT( 32 == lmd.defaultTriggerAllLevel());
//..
// The configuration object is now validly configured with our choice of
// parameters.  If, however, we attempt to set an invalid configuration, the
// "set" method will fail (with a non-zero return status), and the
// configuration will be left unchanged.
//..
        ASSERT(  0 != lmd.setDefaultThresholdLevelsIfValid(256, 90, 60, 30));
        ASSERT(192 == lmd.defaultRecordLevel());
        ASSERT( 64 == lmd.defaultPassLevel());
        ASSERT( 48 == lmd.defaultTriggerLevel());
        ASSERT( 32 == lmd.defaultTriggerAllLevel());
//..
// Finally, we can print the configuration value to 'stdout'.
//..
        if (verbose) {
            cout << "Executing: bsl::cout << lmd << bsl::endl;" << endl;
            bsl::cout << lmd << bsl::endl;
        }

//..
// This produces the following (multi-line) output:
//..
// [
//     recordBufferSize : 32768
//     loggerBufferSize : 2048
//     recordLevel      : 192
//     passLevel        : 64
//     triggerLevel     : 48
//     triggerAllLevel  : 32
// ]
        // Programmatically verify the output format
        char buf[10000];
        {
            ostrstream o(buf, sizeof buf);
            o << lmd << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 32768" NL
                "    loggerBufferSize : 2048"  NL
                "    recordLevel      : 192"   NL
                "    passLevel        : 64"    NL
                "    triggerLevel     : 48"    NL
                "    triggerAllLevel  : 32"    NL
                "]" NL
                ;
            ASSERT(buf == s);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: setDefaultThresholdLevels
        //
        // Concerns:
        //: 1 'setDefaultThresholdLevels' sets the pass-through level to the
        //:    supplied value, and all the other thresholds to 0.
        //:
        //: 2 'setDefaultThresholdLevels' returns 0 on success
        //:
        //: 3 If the supplied pass-through level is not in the ranage [0..255]
        //:   the 'setDefaultThresholdLevels' returns a non-zero value
        //
        // Plan:
        //: 1 Manually call method with valid values and verify the expected
        //:   configuration and the return value is 0.  (C-1, C-2)
        //:
        //: 2 Manually call method with invalid values and verify the
        //:   configuraiton is not changed and the method is not 0.  (C-3)
        //
        // Testing:
        //   int setDefaultThresholdLevels(int);
        // --------------------------------------------------------------------

        if (verbose) cout
            << endl
            << "TESTING: setDefaultThresholdLevelsIfValid" << endl
            << "=========================================" << endl;


        if (verbose) cout
            << "\nTest successful call to 'setDefaultThresholdLevelsIfValid'."
            << endl;
        {
            Obj mX; const Obj& X = mX;
            ASSERT(0 == mX.setDefaultThresholdLevelsIfValid(5));
            ASSERT(0 == X.defaultRecordLevel());
            ASSERT(5 == X.defaultPassLevel());
            ASSERT(0 == X.defaultTriggerLevel());
            ASSERT(0 == X.defaultTriggerAllLevel());

            ASSERT(0   == mX.setDefaultThresholdLevelsIfValid(100));
            ASSERT(0   == X.defaultRecordLevel());
            ASSERT(100 == X.defaultPassLevel());
            ASSERT(0   == X.defaultTriggerLevel());
            ASSERT(0   == X.defaultTriggerAllLevel());
        }
        if (verbose) cout
            << "\nTest incorrect call to 'setDefaultThresholdLevelsIfValid'."
            << endl;

        {
            Obj mX; const Obj& X = mX;

            ASSERT(0 != mX.setDefaultThresholdLevelsIfValid(-1));
            ASSERT(     RECORD_LEVEL[0] == X.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X.defaultTriggerAllLevel());

            ASSERT(0 != mX.setDefaultThresholdLevelsIfValid(256));
            ASSERT(     RECORD_LEVEL[0] == X.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X.defaultTriggerAllLevel());

        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONSTRAINTS TEST
        //
        // Concerns:
        //
        //
        //
        // Plan:
        //
        //
        //
        // Testing:
        //    bool isValidDefaultRecordBufferSize(int numBytes);
        //    bool isValidDefaultLoggerBufferSize(int numBytes);
        //    bool areValidDefaultThresholdLevels(int, int, int, int);
        //    int setDefaultRecordBufferSizeIfValid(int numBytes);
        //    int setDefaultLoggerBufferSizeIfValid(int numBytes);
        //    int setDefaultThresholdLevels(int, int, int, int);
        //    int defaultRecordBufferSize();
        //    int defaultLoggerBufferSize();
        //    int defaultRecordLevel();
        //    int defaultPassLevel();
        //    int defaultTriggerLevel();
        //    int defaultTriggerAllLevel();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Constraints Test" << endl
                          << "================" << endl;

        Obj mX1; const Obj& X1 = mX1;

        if (verbose)
            cout << "\t Check defaultRecordBufferSize contraint. " << endl;
        {
            ASSERT(0 == Obj::isValidDefaultRecordBufferSize(0));
            ASSERT(1 == Obj::isValidDefaultRecordBufferSize(1));
            ASSERT(1 == Obj::isValidDefaultRecordBufferSize(2));

            ASSERT(0 != mX1.setDefaultRecordBufferSizeIfValid(0));
            ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
            ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(1));
            ASSERT(1 == X1.defaultRecordBufferSize());
            ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(2));
            ASSERT(2 == X1.defaultRecordBufferSize());
        }
        if (verbose)
            cout << "\t Check defaultLoggerBufferSize contraint. " << endl;
        {
            ASSERT(0 == Obj::isValidDefaultLoggerBufferSize(0));
            ASSERT(1 == Obj::isValidDefaultLoggerBufferSize(1));
            ASSERT(1 == Obj::isValidDefaultLoggerBufferSize(2));

            ASSERT(0 != mX1.setDefaultLoggerBufferSizeIfValid(0));
            ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
            ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(1));
            ASSERT(1 == X1.defaultLoggerBufferSize());
            ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(2));
            ASSERT(2 == X1.defaultLoggerBufferSize());
        }
        if (verbose)
            cout << "\t Check defaultThresholdLevels contraint. " << endl;
        {
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(-1, 2, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, -1, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, -1, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 3, -1));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(256, 2, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 256, 3, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 256, 4));
            ASSERT(0 == Obj::areValidDefaultThresholdLevels(1, 2, 3, 256));
            ASSERT(1 == Obj::areValidDefaultThresholdLevels(1, 2, 3, 4));

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(-1, 2, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, -1, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, -1, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, -1));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(256, 2, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 256, 3, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 256, 4));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 != mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, 256));
            ASSERT(     RECORD_LEVEL[0] == X1.defaultRecordLevel());
            ASSERT(       PASS_LEVEL[0] == X1.defaultPassLevel());
            ASSERT(    TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
            ASSERT(TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

            ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(1, 2, 3, 4));
            ASSERT(1 == X1.defaultRecordLevel());
            ASSERT(2 == X1.defaultPassLevel());
            ASSERT(3 == X1.defaultTriggerLevel());
            ASSERT(4 == X1.defaultTriggerAllLevel());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "=====================" << endl;

        Obj mX1;  const Obj& X1 = mX1;
        Obj mY1;  const Obj& Y1 = mY1;
        Obj mZ1;  const Obj& Z1 = mZ1; // Z1 is the control
        if (verbose) { cout << "X1 = ";  X1.print(cout, -1, 4); }

        if (verbose) cout << "\nCheck imp.-defined default values " << endl;

        ASSERT(RECORD_BUFFER_SIZE[0] == Obj::defaultDefaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == Obj::defaultDefaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == Obj::defaultDefaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == Obj::defaultDefaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == Obj::defaultDefaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == Obj::defaultDefaultTriggerAllLevel());

        if (verbose) cout << "\nCheck default ctor. " << endl;

        ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\tSetting default values explicitly." <<endl;

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[0]));
        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[0]));
        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[0],
                                                        PASS_LEVEL[0],
                                                        TRIGGER_LEVEL[0],
                                                        TRIGGER_ALL_LEVEL[0]));

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t\tChange attribute 0." << endl;

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[1]));

        ASSERT(RECORD_BUFFER_SIZE[1] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 1." << endl;

        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[1]));

        ASSERT(RECORD_BUFFER_SIZE[0] == X1.defaultRecordBufferSize());
        ASSERT(LOGGER_BUFFER_SIZE[1] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[0] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[0] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[0] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[0] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 2." << endl;

        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[1],
                                                  PASS_LEVEL[1],
                                                  TRIGGER_LEVEL[1],
                                                  TRIGGER_ALL_LEVEL[1]));

        ASSERT(LOGGER_BUFFER_SIZE[0] == X1.defaultLoggerBufferSize());
        ASSERT(      RECORD_LEVEL[1] == X1.defaultRecordLevel());
        ASSERT(        PASS_LEVEL[1] == X1.defaultPassLevel());
        ASSERT(     TRIGGER_LEVEL[1] == X1.defaultTriggerLevel());
        ASSERT( TRIGGER_ALL_LEVEL[1] == X1.defaultTriggerAllLevel());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        ASSERT(0 == mX1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[0],
                                                        PASS_LEVEL[0],
                                                        TRIGGER_LEVEL[0],
                                                        TRIGGER_ALL_LEVEL[0]));
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing output operator<<." << endl;

        ASSERT(0 == mY1.setDefaultRecordBufferSizeIfValid(
                                                       RECORD_BUFFER_SIZE[1]));
        ASSERT(0 == mY1.setDefaultLoggerBufferSizeIfValid(
                                                       LOGGER_BUFFER_SIZE[1]));
        ASSERT(0 == mY1.setDefaultThresholdLevelsIfValid(
                                                        RECORD_LEVEL[1],
                                                        PASS_LEVEL[1],
                                                        TRIGGER_LEVEL[1],
                                                        TRIGGER_ALL_LEVEL[1]));

        char buf[10000];

        if (verbose) cout << "\tOn a default object:" << endl;
        {
            ostrstream o(buf, sizeof buf);
            o << X1 << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 32768" NL
                "    loggerBufferSize : 8192"  NL
                "    recordLevel      : 0"     NL
                "    passLevel        : 64"    NL
                "    triggerLevel     : 0"     NL
                "    triggerAllLevel  : 0"     NL
                "]" NL
                ;
            ASSERT(buf == s);
            if (veryVerbose) cout << "buf:\n" << buf << "\ns:\n" << s << endl;
        }

        if (verbose) cout << "\tOn a non-default object:" << endl;
        {
            ostrstream o(buf, sizeof buf);
            o << Y1 << ends;
            bsl::string s =
                "[" NL
                "    recordBufferSize : 10" NL
                "    loggerBufferSize : 11" NL
                "    recordLevel      : 12" NL
                "    passLevel        : 13" NL
                "    triggerLevel     : 14" NL
                "    triggerAllLevel  : 15" NL
                "]" NL
                ;
            ASSERT(buf == s);
            if (veryVerbose) cout << "buf:\n" << buf << "\ns:\n" << s << endl;
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
