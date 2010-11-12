// bael_loggermanagerconfiguration.t.cpp  -*-C++-*-

#include <bael_loggermanagerconfiguration.h>

#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_byteinstream.h>                  // for testing only

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen()

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
//    bael_LMC   bael_LoggerManagerConfiguration
//    bael_LMD   bael_LoggerManagerDefaults
//    Schema     bdem_Schema
//    Populator  bdef_Function<void (*)(bdem_List *, bdem_Schema)>
//    CNF        bdef_Function<void (*)(bsl::string *, const char *)>
//    DTC        bdef_Function<
//                           void (*)(int *, int *, int *, int *, const char*)>
//..
//-----------------------------------------------------------------------------
// [ 1] bael_LoggerManagerConfiguration();
// [ 1] bael_LoggerManagerConfiguration(const bael_LoggerManagerConfiguration&)
// [ 1] ~bael_LoggerManagerConfiguration();
//
// [ 1] bael_LMC& operator=(const bael_LoggerManagerConfiguration& rhs);
// [ 1] void setDefaultValues(const bael_LMD& defaults);
// [ 4] void setLogOrder(LogOrder value);
// [ 5] void setTriggerMarkers(TriggerMarkers value);
// [ 1] void setUserFields(const Schema& schema, const Populator& populator);
// [ 1] void setCategoryNameFilterCallback(const CNF& nameFilter);
// [ 1] void setDefaultThresholdLevelsCallback(
//                                  const DTC& defaultThresholdLevelsCallback);
//
// [ 1] const bael_LMD& defaults() const;
// [ 1] const bdem_Schema& userSchema() const;
// [ 4] const LogOrder logOrder() const;
// [ 5] const TriggerMarkers triggerMarkers() const;
// [ 1] const Populator& userPopulatorCallback() const;
// [ 1] const CNF& categoryNameFilterCallback() const;
// [ 1] const DTC& defaultThresholdLevelsCallback() const;
// [ 1] bsl::ostream& print(bsl::ostream& stream, int level, int spacesPerLev)
//
// [ 1] bool operator==(const bael_LMC& lhs, const bael_LMC& rhs);
// [ 1] bool operator!=(const bael_LMC& lhs, const bael_LMC& rhs);
// [ 1] bsl::ostream& operator<<(bsl::ostream&, const bael_LMC);
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
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

typedef bael_LoggerManagerConfiguration Obj;
typedef bael_LoggerManagerDefaults      Defs;
typedef bdem_Schema                     Schema;

typedef bdex_TestInStream               In;
typedef bdex_TestOutStream              Out;

// Functor typedefs
typedef bdef_Function<void (*)(bdem_List *, bdem_Schema)>                PopCb;
typedef bdef_Function<void (*)(bsl::string *, const char *)>             CnfCb;
typedef bdef_Function<void (*)(int *, int *, int *, int *, const char*)> DtCb;

//-----------------------------------------------------------------------------
//          Dummy functions to populate each of the three functors
//-----------------------------------------------------------------------------
void pop(bdem_List *list, bdem_Schema schema)
{
}

void cnf(bsl::string *a, const char *c)
{
}

void dtc(int *i, int *j, int *k, int *l, const char *c)
{
}

//-----------------------------------------------------------------------------
//          Some 'bael_LoggerManagerDefaults' field values
//-----------------------------------------------------------------------------

const int NUM_VALUES = 7;

// The 0th element in each array contains the implementation-defined value used
// by the default constructor for 'bael_LoggerManagerDefaults'.

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

    // Build up a non-default 'bael_LoggerManagerDefaults' value.
    Defs d1;
    d1.setDefaultRecordBufferSizeIfValid(RECORD_BUFFER_SIZE[1]);
    d1.setDefaultLoggerBufferSizeIfValid(LOGGER_BUFFER_SIZE[1]);
    d1.setDefaultThresholdLevelsIfValid(RECORD_LEVEL[1], PASS_LEVEL[1],
                                        TRIGGER_LEVEL[1],TRIGGER_ALL_LEVEL[1]);

    // Build up a non-default 'bdem_Schema'.
    Schema s1;
    s1.createRecord("A");
    s1.createRecord("B");

    // Populate the three functors (from functions defined above)
    PopCb pCb1(&pop);

    CnfCb cnfCb1(&cnf);

    DtCb dtCb1(&dtc);

    // Create a default ('X0') and a non-default ('X1') attribute object
    const Defs   D0;
    const Defs   D1(d1);

    const Schema S0;
    const Schema S1(s1);

    const PopCb  PCB0;
    const PopCb  PCB1(pCb1);

    const CnfCb  CNFCB0;
    const CnfCb  CNFCB1(cnfCb1);

    const DtCb   DTCB0;
    const DtCb   DTCB1(dtCb1);

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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

///Usage
///-----
// The following snippets of code illustrate how to use a
// 'bael_LoggerManagerConfiguration' object.  First, we create a configuration
// object named 'config'.  Note that it is necessarily initialized to valid but
// unpublished defaults:
//..
    bael_LoggerManagerConfiguration config;
//
    ASSERT(    0 == config.setDefaultRecordBufferSizeIfValid(32768));
    ASSERT(    0 == config.setDefaultLoggerBufferSizeIfValid(1024));
    ASSERT(    0 == config.setDefaultThresholdLevelsIfValid(192, 64, 48, 32));
//
    ASSERT(32768 == config.defaultRecordBufferSize());
    ASSERT( 1024 == config.defaultLoggerBufferSize());
    ASSERT(  192 == config.defaultRecordLevel());
    ASSERT(   64 == config.defaultPassLevel());
    ASSERT(   48 == config.defaultTriggerLevel());
    ASSERT(   32 == config.defaultTriggerAllLevel());
//
    ASSERT(bael_LoggerManagerConfiguration::BAEL_LIFO == config.logOrder());
//..
// Next, set each attribute.  Note that the user schema and the corresponding
// populator functor must be set atomically (i.e., with a single two-argument
// "set" method).  The user is responsible for the logical sensibility of the
// functor attributes, and especially the coherence of the schema and its
// populator.
//
// We will need to define a few objects before we can call the "set" methods.
// Also, to illustrate a non-null functor, we will create a trivial function
// payload 'pop' for the populator functor attribute:
//..
//    void pop(bdem_List *list, bdem_Schema schema);
//    {
//    }
//..
// Now we can proceed to define the contents of our 'config' object.  We will
// need a schema and three functors, two of which will be default-constructed
// and not populated, resulting in "null" functors:
//..
       bdem_Schema schema;
       schema.createRecord("A");
//
       bdef_Function<void (*)(bdem_List *, bdem_Schema)>    populator(&pop);
       bdef_Function<void (*)(bsl::string *, const char *)> nameFilter;
       bdef_Function<void (*)(int *, int *, int *, int *, const char*)>
                                                            defaultThresholds;
//..
// Note that the 'bdef_Function' class takes an optional allocator.  The same
// allocator that is used to initialize the logger manager singleton (which
// is the global allocator if one is not explicitly supplied) should also be
// passed to 'bdef_Function'.  In this simple example, we allow the default
// allocator to be used.
//
// We are now ready to populate our 'config' object, which is our goal.  Note
// that the "set" methods called in this example cannot fail, so they return
// 'void':
//..
       config.setUserFields(schema, populator);
       config.setCategoryNameFilterCallback(nameFilter);
       config.setDefaultThresholdLevelsCallback(defaultThresholds);
       config.setLogOrder(bael_LoggerManagerConfiguration::BAEL_FIFO);
       config.setTriggerMarkers(
                      bael_LoggerManagerConfiguration::BAEL_BEGIN_END_MARKERS);
//
       ASSERT(           schema == config.userSchema());
       ASSERT(        populator == config.userPopulatorCallback());
       ASSERT(       nameFilter == config.categoryNameFilterCallback());
       ASSERT(defaultThresholds == config.defaultThresholdLevelsCallback());
       ASSERT(bael_LoggerManagerConfiguration::BAEL_FIFO == config.logOrder());
       ASSERT(bael_LoggerManagerConfiguration::BAEL_BEGIN_END_MARKERS
                                                   == config.triggerMarkers());
//..
// The configuration object is now validly configured with our choice of
// parameters.  We can now print the configuration value to 'stdout':
//..
if (veryVerbose)
      bsl::cout << config << bsl::endl;
//..
// This produces the following (multi-line) output:
//..
//  [
//      Defaults:
//      [
//          recordBufferSize : 32768
//          loggerBufferSize : 1024
//          recordLevel      : 192
//          passLevel        : 64
//          triggerLevel     : 48
//          triggerAllLevel  : 32
//      ]
//      User Fields Schema:
//      {
//          SEQUENCE RECORD "A" {
//          }
//      }
//      User Fields Populator functor is not null
//      Category Name Filter functor is null
//      Default Threshold Callback functor is null
//      Logging order is FIFO
//  ]
//..

        // Programmatically verify the output format
        {
            bsl::ostringstream o;
            o << config;

            static const char EXPECTED[] =
                "["                                              NL
                "    Defaults:"                                  NL
                "    ["                                          NL
                "        recordBufferSize : 32768"               NL
                "        loggerBufferSize : 1024"                NL
                "        recordLevel      : 192"                 NL
                "        passLevel        : 64"                  NL
                "        triggerLevel     : 48"                  NL
                "        triggerAllLevel  : 32"                  NL
                "    ]"                                          NL
                "    User Fields Schema:"                        NL
                "    {"                                          NL
                "        SEQUENCE RECORD \"A\" {"                NL
                "        }"                                      NL
                "    }"                                          NL
                "    User Fields Populator functor is not null"  NL
                "    Category Name Filter functor is null"       NL
                "    Default Threshold Callback functor is null" NL
                "    Logging order is FIFO"                      NL
                "    Trigger markers are BEGIN_END_MARKERS"      NL
                "]"                                              NL
                ;

            if (veryVerbose) { P(EXPECTED);  P(o.str()); }

            ASSERT(EXPECTED == o.str());
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING  'setTriggerMarkers' AND 'triggerMarkers':
        //   Verify 'setTriggerMarkers' and 'triggerMarkers'.
        //
        // Concern:
        //   That 'setTriggerMarkers' and 'triggerMarkers' works correctly.
        //
        // Plan:
        //   1. Create a logger manager and verify 'logOrder'.
        //   2. Invoke 'setTriggerMarkers' with 'BAEL_NO_MARKERS' and verify
        //      'triggerMarkers'.
        //   3. Invoke 'setTriggerMarkers' with 'BAEL_BEGIN_END_MARKERS' and
        //      verify 'triggerMarkers'.
        //
        // Testing:
        //   void setTriggerMarkers(TriggerMarkers value);
        //   const TriggerMarkers triggerMarkers() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING  'setTriggerMarkers'"
                             "AND 'triggerMarkers'"
                          << "\n============================"
                             "=====================" << endl;

        Obj lmc;
        ASSERT(lmc.triggerMarkers() == Obj::BAEL_NO_MARKERS);

        lmc.setTriggerMarkers(Obj::BAEL_BEGIN_END_MARKERS);
        ASSERT(lmc.triggerMarkers() == Obj::BAEL_BEGIN_END_MARKERS);

        lmc.setTriggerMarkers(Obj::BAEL_NO_MARKERS);
        ASSERT(lmc.triggerMarkers() == Obj::BAEL_NO_MARKERS);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING  'setLogOrder' AND 'logOrder':
        //   Verify 'setLogOrder' and 'logOrder'.
        //
        // Concern:
        //   That 'setLogOrder' and 'logOrder' works correctly.
        //
        // Plan:
        //   Create a logger manager and verify 'logOrder'.
        //   Invoke 'setLogOrder' with 'BAEL_FIFO' and verify 'logOrder'.
        //   Invoke 'setLogOrder' with 'BAEL_LIFO' and verify 'logOrder'.
        //
        // Testing:
        //   void setLogOrder(LogOrder value);
        //   const LogOrder logOrder() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING  'setLogOrder' AND 'logOrder'"
                          << "\n====================================" << endl;

        Obj lmc;
        ASSERT(lmc.logOrder() == Obj::BAEL_LIFO);

        lmc.setLogOrder(Obj::BAEL_FIFO);
        ASSERT(lmc.logOrder() == Obj::BAEL_FIFO);

        lmc.setLogOrder(Obj::BAEL_LIFO);
        ASSERT(lmc.logOrder() == Obj::BAEL_LIFO);

      } break;
      case 3: {
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
      case 2: {
        // --------------------------------------------------------------------
        // Constraints Test:
        //
        // Concerns:
        //   This component imposes exactly the constraints on its contained
        //   'bael_LoggerManagerDefaults' object as that object imposes on
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
        ASSERT(    S0 == X1.userSchema());
        ASSERT(  PCB0 == X1.userPopulatorCallback());
        ASSERT(CNFCB0 == X1.categoryNameFilterCallback());
        ASSERT( DTCB0 == X1.defaultThresholdLevelsCallback());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\tSetting default values explicitly." <<endl;

        mX1.setDefaultValues(D0);
        mX1.setUserFields(S0, PCB0);
        mX1.setCategoryNameFilterCallback(CNFCB0);
        mX1.setDefaultThresholdLevelsCallback(DTCB0);

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S0 == X1.userSchema());
        ASSERT(  PCB0 == X1.userPopulatorCallback());
        ASSERT(CNFCB0 == X1.categoryNameFilterCallback());
        ASSERT( DTCB0 == X1.defaultThresholdLevelsCallback());

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
        ASSERT(    S0 == X1.userSchema());
        ASSERT(  PCB0 == X1.userPopulatorCallback());
        ASSERT(CNFCB0 == X1.categoryNameFilterCallback());
        ASSERT( DTCB0 == X1.defaultThresholdLevelsCallback());

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

        mX1.setUserFields(S1, PCB1);

        if (veryVerbose) { cout << "\t\tX1 = ";  X1.print(cout, -5, 4); }

        ASSERT(    D0 == X1.defaults());
        ASSERT(    S1 == X1.userSchema());
        ASSERT(  PCB1 == X1.userPopulatorCallback());
        ASSERT(CNFCB0 == X1.categoryNameFilterCallback());
        ASSERT( DTCB0 == X1.defaultThresholdLevelsCallback());

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

        mX1.setUserFields(S0, PCB0);

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
        ASSERT(    S0 == X1.userSchema());
        ASSERT(  PCB0 == X1.userPopulatorCallback());
        ASSERT(CNFCB1 == X1.categoryNameFilterCallback());
        ASSERT( DTCB0 == X1.defaultThresholdLevelsCallback());

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
        ASSERT(    S0 == X1.userSchema());
        ASSERT(  PCB0 == X1.userPopulatorCallback());
        ASSERT(CNFCB0 == X1.categoryNameFilterCallback());
        ASSERT( DTCB1 == X1.defaultThresholdLevelsCallback());

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
        mY1.setUserFields(S1, PCB1);
        mY1.setCategoryNameFilterCallback(CNFCB1);
        mY1.setDefaultThresholdLevelsCallback(DTCB1);

        if (verbose) cout << "\tOn a default object:" << endl;
        {
            ostringstream o;
            o << X1;

            static const char EXPECTED[] =
                "["                                NL
                "    Defaults:"                    NL
                "    ["                            NL
                "        recordBufferSize : 32768" NL
                "        loggerBufferSize : 8192"  NL
                "        recordLevel      : 0"     NL
                "        passLevel        : 64"    NL
                "        triggerLevel     : 0"     NL
                "        triggerAllLevel  : 0"     NL
                "    ]"                            NL
                "    User Fields Schema:"          NL
                "    {"                            NL
                "    }"                            NL
                "    User Fields Populator functor is null"      NL
                "    Category Name Filter functor is null"       NL
                "    Default Threshold Callback functor is null" NL
                "    Logging order is LIFO"                      NL
                "    Trigger markers are NO_MARKERS"             NL
                "]"                                              NL
                ;

            ASSERT(EXPECTED == o.str());
            if (veryVerbose) cout << "EXPECTED:\n" << EXPECTED
                                  << "\noutput:\n" << o.str() << endl;
        }

        if (verbose) cout << "\tOn a non-default object:" << endl;
        {
            ostringstream o;
            o << Y1;

            static const char EXPECTED[] =
                "["                               NL
                "    Defaults:"                   NL
                "    ["                           NL
                "        recordBufferSize : 10"   NL
                "        loggerBufferSize : 11"   NL
                "        recordLevel      : 12"   NL
                "        passLevel        : 13"   NL
                "        triggerLevel     : 14"   NL
                "        triggerAllLevel  : 15"   NL
                "    ]"                           NL
                "    User Fields Schema:"         NL
                "    {"                           NL
                "        SEQUENCE RECORD \"A\" {" NL
                "        }"                       NL
                "        SEQUENCE RECORD \"B\" {" NL
                "        }"                       NL
                "    }"                           NL
                "    User Fields Populator functor is not null"      NL
                "    Category Name Filter functor is not null"       NL
                "    Default Threshold Callback functor is not null" NL
                "    Logging order is LIFO"                          NL
                "    Trigger markers are NO_MARKERS"                 NL
                "]"                                                  NL
                ;
            ASSERT(EXPECTED == o.str());
            if (veryVerbose) cout << "EXPECTED:\n" << EXPECTED
                                  << "\noutput:\n" << o.str() << endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
