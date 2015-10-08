// ball_loggermanagerconfiguration.t.cpp                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_loggermanagerconfiguration.h>

#include <ball_userfields.h>                                // for testing only
#include <ball_userfieldsschema.h>

#include <bslim_testutil.h>
#include <bsls_assert.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()
#include <bsl_functional.h>


using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test plan follows the standard approach for components implementing
//
// Abbreviations
// -------------
// For the sake of brevity in the Test Plan below, the following abbreviations
// are used as needed:
//..
//    ball::LMC   ball::LoggerManagerConfiguration
//    ball::LMD   ball::LoggerManagerDefaults
//    Descriptors ball::UserFieldsSchema
//    Populator   bsl::function<void(UserFields*,const Descriptors&)>
//    CNF         bsl::function<void(bsl::string *, const char *)>
//    DTC         bsl::function<void(int *, int *, int *, int *, const char*)>
//..
//-----------------------------------------------------------------------------
// [ 1] ball::LoggerManagerConfiguration();
// [ 1] ball::LoggerManagerConfiguration(const LoggerManagerConfiguration&)
// [ 1] ~ball::LoggerManagerConfiguration();
//
// [ 1] ball::LMC& operator=(const ball::LoggerManagerConfiguration& rhs);
// [ 1] void setDefaultValues(const ball::LMD& defaults);
// [ 5] void setLogOrder(LogOrder value);
// [ 6] void setTriggerMarkers(TriggerMarkers value);
// [ 1] void setUserFieldsSchema(const Schema& , const Populator& );
// [ 1] void setCategoryNameFilterCallback(const CNF& nameFilter);
// [ 1] void setDefaultThresholdLevelsCallback(const DTC& );
// [ 2] void setDefaultThresholdLevelsIfValid(int)
// [ 1] const ball::LMD& defaults() const;
// [ 1] const ball::UserFieldsSchema& userFieldsSchema() const;
// [ 5] const LogOrder logOrder() const;
// [ 6] const TriggerMarkers triggerMarkers() const;
// [ 1] const Populator& userFieldsPopulatorCallback() const;
// [ 1] const CNF& categoryNameFilterCallback() const;
// [ 1] const DTC& defaultThresholdLevelsCallback() const;
// [ 1] bsl::ostream& print(bsl::ostream& stream, int level, int spacesPerLev)
//
// [ 1] bool operator==(const ball::LMC& lhs, const ball::LMC& rhs);
// [ 1] bool operator!=(const ball::LMC& lhs, const ball::LMC& rhs);
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const ball::LMC);
//-----------------------------------------------------------------------------
// [ 7] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

typedef ball::LoggerManagerConfiguration Obj;
typedef ball::LoggerManagerDefaults      Defs;
typedef ball::UserFieldsSchema           Descriptors;


// Functor typedefs
typedef bsl::function<void(ball::UserFields *, const ball::UserFieldsSchema&)>
                                                                         PopCb;
typedef bsl::function<void(bsl::string *, const char *)>                 CnfCb;
typedef bsl::function<void(int *, int *, int *, int *, const char*)>     DtCb;


//=============================================================================
//                             USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

///Usage
///-----
// The following snippets of code illustrate how to use a
// 'ball::LoggerManagerConfiguration' object.
//
// First we define a simple function that will serve as a
// 'UserFieldsPopulatorCallback', a callback that will be invoked for each
// logged message to populate user defined fields for the log record:
//..
    void exampleCallback(ball::UserFields              *fields,
                         const ball::UserFieldsSchema&  schema)
    {
      // Verify the schema matches this callbacks expectations.
//
      BSLS_ASSERT(1                             == schema.length());
      BSLS_ASSERT(ball::UserFieldType::e_STRING == schema.type(0));
      BSLS_ASSERT("example"                     == schema.name(0));
//
      fields->appendString("example user field value");
    }
//..
// Next, we define a function 'inititialize' in which we will create and
// configure a 'ball::LoggerManagerConfiguration' object (see
// {'ball_loggermanager'} for an example of how to create the logger-manager
// singleton object):
//..
    void initializeConfiguration(bool verbose)
    {
      ball::LoggerManagerConfiguration config;
//
//..
// Here, we configure the default record buffer size, logger buffer size, and
// the various logging thresholds (see {'ball_loggermanager'} for more
// information on the various threshold levels):
//..
      if (0 != config.setDefaultRecordBufferSizeIfValid(32768) ||
          0 != config.setDefaultLoggerBufferSizeIfValid(1024)  ||
          0 != config.setDefaultThresholdLevelsIfValid(0, 64, 0, 0)) {
         bsl::cerr << "Failed set log configuration defaults." << bsl::endl;
         bsl::exit(-1);
      }
//
      ASSERT(32768 == config.defaultRecordBufferSize());
      ASSERT( 1024 == config.defaultLoggerBufferSize());
      ASSERT(    0 == config.defaultRecordLevel());
      ASSERT(   64 == config.defaultPassLevel());
      ASSERT(    0 == config.defaultTriggerLevel());
      ASSERT(    0 == config.defaultTriggerAllLevel());
//..
// Next, we create a user field schema, that will be used with the user field
// populator callback 'exampleCallback':
//..
      ball::UserFieldsSchema schema;
      schema.appendFieldDescription("example", ball::UserFieldType::e_STRING);
//..
// Now, we set populate the configuration options in our schema (note that the
// following methods cannot fail and return 'void'):
//..
      config.setUserFieldsSchema(schema, &exampleCallback);
      config.setLogOrder(ball::LoggerManagerConfiguration::e_FIFO);
      config.setTriggerMarkers(
                            ball::LoggerManagerConfiguration::e_NO_MARKERS);
//..
// Then, we verify the options are configured correctly:
//..
      ASSERT(schema == config.userFieldsSchema());
      ASSERT(ball::LoggerManagerConfiguration::e_FIFO == config.logOrder());
      ASSERT(ball::LoggerManagerConfiguration::e_NO_MARKERS
                                                   == config.triggerMarkers());
//..
// Finally, we print the configuration value to 'stdout' and return:
//..
      if (verbose) {
        bsl::cout << config << bsl::endl;
      }
    }
//..
// This produces the following (multi-line) output:
//..
//  [
//      Defaults:
//      [
//          recordBufferSize : 32768
//          loggerBufferSize : 1024
//          recordLevel      : 0
//          passLevel        : 64
//          triggerLevel     : 0
//          triggerAllLevel  : 0
//      ]
//      User Fields Schema:
//      [
//          example = STRING
//      ]
//      User Fields Populator functor is not null
//      Category Name Filter functor is null
//      Default Threshold Callback functor is null
//      Logging order is FIFO
//      Trigger markers are NO_MARKERS
//  ]
//..

//-----------------------------------------------------------------------------
//          Dummy functions to populate each of the three functors
//-----------------------------------------------------------------------------
void pop(ball::UserFields *, ball::UserFieldsSchema )
{
}

void cnf(bsl::string *, const char *)
{
}

void dtc(int *, int *, int *, int *, const char *)
{
}

//-----------------------------------------------------------------------------
//          Some 'ball::LoggerManagerDefaults' field values
//-----------------------------------------------------------------------------

const int NUM_VALUES = 7;

// The 0th element in each array contains the implementation-defined value used
// by the default constructor for 'ball::LoggerManagerDefaults'.

const int _DRBS = Defs::defaultDefaultRecordBufferSize();
const int _DLBS = Defs::defaultDefaultLoggerBufferSize();
const int _DRL  = Defs::defaultDefaultRecordLevel();
const int _DPL  = Defs::defaultDefaultPassLevel();
const int _DTL  = Defs::defaultDefaultTriggerAllLevel();
const int _DTAL = Defs::defaultDefaultTriggerAllLevel();

const int RECORD_BUFFER_SIZE[NUM_VALUES] = { _DRBS, 10, 20, 30, 40, 50, 60 };
const int LOGGER_BUFFER_SIZE[NUM_VALUES] = { _DLBS, 11, 21, 31, 41, 51, 61 };
const int RECORD_LEVEL[NUM_VALUES]       = { _DRL,  12, 22, 32, 42, 52, 62 };
const int PASS_LEVEL[NUM_VALUES]         = { _DPL,  13, 23, 33, 43, 53, 63 };
const int TRIGGER_LEVEL[NUM_VALUES]      = { _DTL,  14, 24, 34, 44, 54, 64 };
const int TRIGGER_ALL_LEVEL[NUM_VALUES]  = { _DTAL, 15, 25, 35, 45, 55, 65 };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    //    int veryVeryVerbose = argc > 4; // not used

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    //-------------------------------------------------------------------------
    //                           Useful Test Data
    //-------------------------------------------------------------------------

    // Build up a non-default 'ball::LoggerManagerDefaults' value.
    Defs d1;
    d1.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[1]);
    d1.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[1]);
    d1.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[1], PASS_LEVEL[1],
                                        TRIGGER_LEVEL[1],TRIGGER_ALL_LEVEL[1]);

    // Build up a non-default 'ball::UserFieldsSchema'.
    Descriptors s1;

    // Populate the three functors (from functions defined above)
    PopCb pCb1(&pop);

    CnfCb cnfCb1(&cnf);

    DtCb dtCb1(&dtc);

    // Create a default ('X0') and a non-default ('X1') attribute object
    const Defs   D0;
    const Defs   D1(d1);

    const Descriptors S0;
    const Descriptors S1(s1);

    const PopCb  PCB0;
    const PopCb  PCB1(pCb1);

    const CnfCb  CNFCB0;
    const CnfCb  CNFCB1(cnfCb1);

    const DtCb   DTCB0;
    const DtCb   DTCB1(dtCb1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
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

        initializeConfiguration(verbose);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING  'setTriggerMarkers' AND 'triggerMarkers':
        //   Verify 'setTriggerMarkers' and 'triggerMarkers'.
        //
        // Concern:
        //   That 'setTriggerMarkers' and 'triggerMarkers' works correctly.
        //
        // Plan:
        //   1. Create a logger manager and verify 'logOrder'.
        //   2. Invoke 'setTriggerMarkers' with 'e_NO_MARKERS' and verify
        //      'triggerMarkers'.
        //   3. Invoke 'setTriggerMarkers' with 'e_BEGIN_END_MARKERS' and
        //      verify 'triggerMarkers'.
        //
        // Testing:
        //   void setTriggerMarkers(TriggerMarkers value);
        //   const TriggerMarkers triggerMarkers() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING  'setTriggerMarkers' AND 'triggerMarkers'"
                 << "\n=================================================\n";

        Obj lmc;
        ASSERT(lmc.triggerMarkers() == Obj::e_BEGIN_END_MARKERS);

        lmc.setTriggerMarkers(Obj::e_NO_MARKERS);
        ASSERT(lmc.triggerMarkers() == Obj::e_NO_MARKERS);

        lmc.setTriggerMarkers(Obj::e_BEGIN_END_MARKERS);
        ASSERT(lmc.triggerMarkers() == Obj::e_BEGIN_END_MARKERS);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING  'setLogOrder' AND 'logOrder':
        //   Verify 'setLogOrder' and 'logOrder'.
        //
        // Concern:
        //   That 'setLogOrder' and 'logOrder' works correctly.
        //
        // Plan:
        //   Create a logger manager and verify 'logOrder'.
        //   Invoke 'setLogOrder' with 'e_FIFO' and verify 'logOrder'.
        //   Invoke 'setLogOrder' with 'e_LIFO' and verify 'logOrder'.
        //
        // Testing:
        //   void setLogOrder(LogOrder value);
        //   const LogOrder logOrder() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING  'setLogOrder' AND 'logOrder'"
                          << "\n====================================" << endl;

        Obj lmc;
        ASSERT(lmc.logOrder() == Obj::e_LIFO);

        lmc.setLogOrder(Obj::e_FIFO);
        ASSERT(lmc.logOrder() == Obj::e_FIFO);

        lmc.setLogOrder(Obj::e_LIFO);
        ASSERT(lmc.logOrder() == Obj::e_LIFO);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // This component does not support 'bdex' streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        if (verbose)
            cout << "\nThis component does not support 'bdex' streaming."
                 << endl;

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
        //:   configuration is not changed and the method is not 0.  (C-3)
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
        // Constraints Test:
        //
        // Concerns:
        //   This component imposes exactly the constraints on its contained
        //   'ball::LoggerManagerDefaults' object as that object imposes on
        //   itself; the constrained 'set' manipulators and "get" accessors
        //   forward their arguments to the corresponding 'Defaults' methods.
        //   It is sufficient to repeat the test from the 'Defaults' test
        //   driver
        //
        //
        // Plan:
        //    1. Set a "good" function in 'assertH' and 'assertCpp'
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

        if (verbose)
            cout << "\nTesting constraints on contained 'Defaults' object."
                 << endl;
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

        if (verbose) cout << "\nCheck default ctor. " << endl;

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S0 == X1.userFieldsSchema());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\tSetting default values explicitly." <<endl;

        mX1.setDefaultValues(D0);
        mX1.setUserFieldsSchema(S0, PCB0);
        mX1.setCategoryNameFilterCallback(CNFCB0);
        mX1.setDefaultThresholdLevelsCallback(DTCB0);

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S0 == X1.userFieldsSchema());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t\tChange attribute 0." << endl;

        mX1.setDefaultValues(D1);

        if (veryVerbose) { cout << "\t\tX1 = ";  X1.print(cout, -5, 4); }

        ASSERT(    D1 == X1.defaults());
        ASSERT(    S0 == X1.userFieldsSchema());
        ASSERT(!X1.userFieldsPopulatorCallback());
        ASSERT(!X1.categoryNameFilterCallback());
        ASSERT(!X1.defaultThresholdLevelsCallback());

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

        mX1.setDefaultValues(D0);
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 1." << endl;

        mX1.setUserFieldsSchema(S1, PCB1);

        if (veryVerbose) { cout << "\t\tX1 = ";  X1.print(cout, -5, 4); }

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S1 == X1.userFieldsSchema());

        ASSERT(X1.userFieldsPopulatorCallback());
        ASSERT(!X1.categoryNameFilterCallback());
        ASSERT(!X1.defaultThresholdLevelsCallback());

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

        mX1.setUserFieldsSchema(S0, PCB0);

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 2." << endl;

        mX1.setCategoryNameFilterCallback(CNFCB1);

        if (veryVerbose) { cout << "\t\tX1 = ";  X1.print(cout, -5, 4); }

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S0 == X1.userFieldsSchema());

        ASSERT(!X1.userFieldsPopulatorCallback());
        ASSERT(X1.categoryNameFilterCallback());
        ASSERT(!X1.defaultThresholdLevelsCallback());

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

        mX1.setCategoryNameFilterCallback(CNFCB0);

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 3." << endl;

        mX1.setDefaultThresholdLevelsCallback(DTCB1);

        if (veryVerbose) { cout << "\t\tX1 = ";  X1.print(cout, -5, 4); }

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S0 == X1.userFieldsSchema());

        ASSERT(!X1.userFieldsPopulatorCallback());
        ASSERT(!X1.categoryNameFilterCallback());
        ASSERT(X1.defaultThresholdLevelsCallback());

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

        mX1.setDefaultThresholdLevelsCallback(DTCB0);

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "Testing output operator<<." << endl;

        mY1.setDefaultValues(D1);
        mY1.setUserFieldsSchema(S1, PCB1);
        mY1.setCategoryNameFilterCallback(CNFCB1);
        mY1.setDefaultThresholdLevelsCallback(DTCB1);


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
