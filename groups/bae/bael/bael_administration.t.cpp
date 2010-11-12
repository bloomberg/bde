// bael_administration.t.cpp         -*-C++-*-

#include <bael_administration.h>

#include <bael_loggermanager.h>      // for testing only
#include <bael_severity.h>           // for testing only
#include <bael_testobserver.h>       // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is merely a facade for several methods defined in
// the 'bael_loggermanager' and 'bael_loggercategoryutil' components.  It is
// sufficient to test that each "facade" method correctly forwards its
// arguments to the corresponding methods of 'bael_loggermanager' and
// 'bael_loggercategoryutil', and that the correct value is returned.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 1] static int addCategory(const char *name, int, int, int, int);
// [ 1] static int setThresholdLevels(const char *re, int, int, int, int);
// [ 1] static int setDefaultCategoryThresholds(int, int, int, int);
// [ 1] static void resetDefaultCategoryThresholds();
// [ 1] static int setDefaultThresholdLevels(int, int, int, int);
// [ 1] static void resetDefaultThresholdLevels();
// [ 1] static int recordLevel(const char *name);
// [ 1] static int passLevel(const char *name);
// [ 1] static int triggerLevel(const char *name);
// [ 1] static int triggerAllLevel(const char *name);
// [ 1] static int defaultRecordThresholdLevel();
// [ 1] static int defaultPassThresholdLevel();
// [ 1] static int defaultTriggerThresholdLevel();
// [ 1] static int defaultTriggerAllThresholdLevel();
// [ 2] static int maxNumCategories();
// [ 1] static int numCategories();
// [ 2] static void setMaxNumCategories(int length);
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

void aSsErT(int c, const char *s, int i)
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

typedef bael_Administration Obj;

// Note: the following *must* be kept in sync with 'bael_loggermanager'.

const char *DEFAULT_CATEGORY_NAME                 = "";
const int   DEFAULT_CATEGORY_RECORD_THRESHOLD     = 0;
const int   DEFAULT_CATEGORY_PASS_THRESHOLD       = bael_Severity::BAEL_ERROR;
const int   DEFAULT_CATEGORY_TRIGGER_THRESHOLD    = 0;
const int   DEFAULT_CATEGORY_TRIGGERALL_THRESHOLD = 0;

const int   DEFAULT_RECORD_THRESHOLD_LEVEL        = 0;
const int   DEFAULT_PASS_THRESHOLD_LEVEL          = bael_Severity::BAEL_ERROR;
const int   DEFAULT_TRIGGER_THRESHOLD_LEVEL       = 0;
const int   DEFAULT_TRIGGERALL_THRESHOLD_LEVEL    = 0;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

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

    bael_TestObserver  testObserver(cout);
    bael_TestObserver *TO = &testObserver;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
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

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            char buf[2048];
            ostrstream out(buf, sizeof buf);

            bael_LoggerManager::initSingleton(TO); // initialize logger manager

            const char *equityCategories[] = {
                "EQUITY.MARKET.NYSE",
                "EQUITY.MARKET.NASDAQ",
                "EQUITY.GRAPHICS.MATH.FACTORIAL",
                "EQUITY.GRAPHICS.MATH.ACKERMANN"
            };
            const int NUM_CATEGORIES = sizeof equityCategories
                                     / sizeof equityCategories[0];

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                int retValue = bael_Administration::addCategory(
                                             equityCategories[i],
                                             bael_Severity::BAEL_TRACE + i,
                                             bael_Severity::BAEL_WARN  + i,
                                             bael_Severity::BAEL_ERROR + i,
                                             bael_Severity::BAEL_FATAL + i);
                ASSERT(0 == retValue);  // added new category
            }

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char* name    = equityCategories[i];
                int recordLevel     = bael_Administration::recordLevel(name);
                int passLevel       = bael_Administration::passLevel(name);
                int triggerLevel    = bael_Administration::triggerLevel(name);
                int triggerAllLevel =
                                   bael_Administration::triggerAllLevel(name);

                out << "Category name: "       << name            << endl;
                out << "\tRecord level:      " << recordLevel     << endl;
                out << "\tPass level:        " << passLevel       << endl;
                out << "\tTrigger level:     " << triggerLevel    << endl;
                out << "\tTrigger-all level: " << triggerAllLevel << endl
                    << endl;
            }

            if (veryVerbose) { out << ends; cout << buf << endl; }

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const int returnValue =
                     bael_Administration::setThresholdLevels(
                                             equityCategories[i],
                                             bael_Severity::BAEL_TRACE - i,
                                             bael_Severity::BAEL_WARN  - i,
                                             bael_Severity::BAEL_ERROR - i,
                                             bael_Severity::BAEL_FATAL - i);
                ASSERT(1 == returnValue);  // modified one category
            }

            out.seekp(0);  // reset ostrstream

            for (int i = 0; i < NUM_CATEGORIES; ++i) {
                const char* name    = equityCategories[i];
                int recordLevel     = bael_Administration::recordLevel(name);
                int passLevel       = bael_Administration::passLevel(name);
                int triggerLevel    = bael_Administration::triggerLevel(name);
                int triggerAllLevel =
                                   bael_Administration::triggerAllLevel(name);

                out << "Category name: "       << name            << endl;
                out << "\tRecord level:      " << recordLevel     << endl;
                out << "\tPass level:        " << passLevel       << endl;
                out << "\tTrigger level:     " << triggerLevel    << endl;
                out << "\tTrigger-all level: " << triggerAllLevel << endl
                    << endl;
            }

            if (veryVerbose) { out << ends; cout << buf << endl; }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING REGISTRY CAPACITY METHODS
        //
        // Concerns:
        //   The 'setMaxNumCategories' method should have the expected effect
        //   on the registry capacity and the 'maxNumCategories' method should
        //   return the current registry capacity.
        //
        // Plan:
        //   After the logger manager singleton has been initialized, the
        //   registry holds the *Default* *Category* (only) and the capacity
        //   is unlimited.
        //     (1) Verify that the registry capacity immediately after
        //         initialization is unlimited.  Then set the capacity to 1 and
        //         verify that no new categories can be added to the registry.
        //     (2) Set the capacity to m > 1; then verify that only m - 1 new
        //         categories can be added to the registry.
        //     (3) Set the capacity to be unlimited.  Then for n > m, add
        //         n - m new categories to the registry.  Finally, set the
        //         capacity to n and verify that 'addCategory' fails.
        //
        // Testing:
        //   static void setMaxNumCategories(int length);
        //   static int maxNumCategories();
        // --------------------------------------------------------------------

        if (verbose) cout << "Testing Registry Capacity Methods" << endl
                          << "=================================" << endl;

        bael_LoggerManager::initSingleton(TO);  // initialize logger manager

        ASSERT(0 == Obj::maxNumCategories());

        Obj::setMaxNumCategories(1);  ASSERT(1 == Obj::maxNumCategories());
        ASSERT(0 != Obj::addCategory("abc", 0, 0, 0, 0));

        const int M = 4;
        const int N = 8;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_name;     // category name
        } DATA[] = {
            // line   name     // ADJUST
            // ----   ----
            {  L_,    ""              },  // *Default* *Category*

            {  L_,    "A"             },  // "A" - "C" added in 1st 'for' loop
            {  L_,    "B"             },
            {  L_,    "C"             },

            {  L_,    "D"             },  // "D" - "G" added in 2nd 'for' loop
            {  L_,    "E"             },
            {  L_,    "F"             },
            {  L_,    "G"             },

            {  L_,    "H"             },
        };

        Obj::setMaxNumCategories(M);  ASSERT(M == Obj::maxNumCategories());

        for (int ti = 1; ti < M; ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *NAME = DATA[ti].d_name;

            if (veryVeryVerbose) { P_(LINE); P(NAME); }

            ASSERT(0 == Obj::addCategory(NAME, 16, 32, 64, 128));
        }
        ASSERT(0 != Obj::addCategory(DATA[M].d_name, 16, 32, 64, 128));

        Obj::setMaxNumCategories(0);  ASSERT(0 == Obj::maxNumCategories());

        for (int ti = M; ti < N; ++ti) {
            const int   LINE = DATA[ti].d_lineNum;
            const char *NAME = DATA[ti].d_name;

            if (veryVeryVerbose) { P_(LINE); P(NAME); }

            ASSERT(0 == Obj::addCategory(NAME, 16, 32, 64, 128));
        }

        Obj::setMaxNumCategories(N);  ASSERT(N == Obj::maxNumCategories());
        ASSERT(0 != Obj::addCategory(DATA[N].d_name, 16, 32, 64, 128));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING REGISTRY MANIPULATORS AND ACCESSORS
        //
        // Concerns:
        //   (1) Each method under test must correctly forward its arguments
        //       to the corresponding method of 'bael_loggermanager' or
        //       'bael_loggercategoryutil'.
        //   (2) Methods not defined to return 'void' must return the expected
        //       value.
        //
        // Plan:
        //   Exercise the methods with various valid and invalid argument
        //   values and assert the expected results.  Test the value returned
        //   by all methods not defined to return 'void'.
        //
        // Testing:
        //   static int addCategory(const char *name, int, int, int, int);
        //   static int setThresholdLevels(const char *re, int, int, int, int);
        //   static int setDefaultCategoryThresholds(int, int, int, int);
        //   static void resetDefaultCategoryThresholds();
        //   static int setDefaultThresholdLevels(int, int, int, int);
        //   static void resetDefaultThresholdLevels();
        //   static int recordLevel(const char *name);
        //   static int passLevel(const char *name);
        //   static int triggerLevel(const char *name);
        //   static int triggerAllLevel(const char *name);
        //   static int defaultCategoryRecordThreshold();
        //   static int defaultCategoryPassThreshold();
        //   static int defaultCategoryTriggerThreshold();
        //   static int defaultCategoryTriggerAllThreshold();
        //   static int defaultRecordThresholdLevel();
        //   static int defaultPassThresholdLevel();
        //   static int defaultTriggerThresholdLevel();
        //   static int defaultTriggerAllThresholdLevel();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "Testing Registry Manipulators and Accessors" << endl
                 << "===========================================" << endl;
        }

        bael_LoggerManager::initSingleton(TO);  // initialize logger manager

        static const struct {
            int         d_lineNum;          // source line number
            int         d_recordLevel;      // record level
            int         d_passLevel;        // pass level
            int         d_triggerLevel;     // trigger level
            int         d_triggerAllLevel;  // trigger-all level
            const char *d_name;             // category name
        } DATA[] = {
            // line   record   pass   trigger   trigger-all   name    // ADJUST
            // ----   ------   ----   -------   -----------   ----
            {  L_,      0,       0,     0,        0,          "E.M.P"        },
            {  L_,      2,       4,     6,        8,          "E.M.Q"        },
            {  L_,     11,       9,     7,        5,          "E.G.M.A"      },
            {  L_,    255,     255,   255,      255,          "E.G.M.B"      },
        };
        const int NUM_DATA = sizeof DATA / sizeof DATA[0];

        const char *XNAME = "X";  // control name for 'BAD_DATA'

        static const struct {
            int         d_lineNum;          // source line number
            int         d_recordLevel;      // record level
            int         d_passLevel;        // pass level
            int         d_triggerLevel;     // trigger level
            int         d_triggerAllLevel;  // trigger-all level
            const char *d_name;             // category name
        } BAD_DATA[] = {
            // line   record   pass   trigger   trigger-all   name    // ADJUST
            // ----   ------   ----   -------   -----------   ----
            {  L_,      -1,      0,     0,        0,          XNAME          },
            {  L_,       0,     -1,     0,        0,          XNAME          },
            {  L_,       0,      0,    -1,        0,          XNAME          },
            {  L_,       0,      0,     0,       -1,          XNAME          },
            {  L_,     256,      0,     0,        0,          XNAME          },
            {  L_,       0,    256,     0,        0,          XNAME          },
            {  L_,       0,      0,   256,        0,          XNAME          },
            {  L_,       0,      0,     0,      256,          XNAME          },
        };
        const int NUM_BAD_DATA = sizeof BAD_DATA / sizeof BAD_DATA[0];
        LOOP_ASSERT(Obj::numCategories(), 1 == Obj::numCategories());

        if (veryVerbose)
            cout << "Testing \"default threshold levels\" methods." << endl;

         ASSERT(DEFAULT_RECORD_THRESHOLD_LEVEL
                                    == Obj::defaultRecordThresholdLevel());
         ASSERT(DEFAULT_PASS_THRESHOLD_LEVEL
                                    == Obj::defaultPassThresholdLevel());
         ASSERT(DEFAULT_TRIGGER_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerThresholdLevel());
         ASSERT(DEFAULT_TRIGGERALL_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerAllThresholdLevel());

        if (veryVerbose) cout << "\tTesting valid threshold levels." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE       = DATA[ti].d_lineNum;
            const int RECORD     = DATA[ti].d_recordLevel;
            const int PASS       = DATA[ti].d_passLevel;
            const int TRIGGER    = DATA[ti].d_triggerLevel;
            const int TRIGGERALL = DATA[ti].d_triggerAllLevel;

            if (veryVeryVerbose) {
                P_(LINE); P_(RECORD); P_(PASS); P_(TRIGGER); P(TRIGGERALL);
            }

            const int S = Obj::setDefaultThresholdLevels(RECORD,
                                                         PASS,
                                                         TRIGGER,
                                                         TRIGGERALL);
            ASSERT(0          == S);
            ASSERT(RECORD     == Obj::defaultRecordThresholdLevel());
            ASSERT(PASS       == Obj::defaultPassThresholdLevel());
            ASSERT(TRIGGER    == Obj::defaultTriggerThresholdLevel());
            ASSERT(TRIGGERALL == Obj::defaultTriggerAllThresholdLevel());
        }

        Obj::resetDefaultThresholdLevels();

        ASSERT(DEFAULT_RECORD_THRESHOLD_LEVEL
                                    == Obj::defaultRecordThresholdLevel());
        ASSERT(DEFAULT_PASS_THRESHOLD_LEVEL
                                    == Obj::defaultPassThresholdLevel());
        ASSERT(DEFAULT_TRIGGER_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerThresholdLevel());
        ASSERT(DEFAULT_TRIGGERALL_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerAllThresholdLevel());

        if (veryVerbose) cout << "\tTesting invalid threshold levels." << endl;

        for (int ti = 0; ti < NUM_BAD_DATA; ++ti) {
            const int LINE       = BAD_DATA[ti].d_lineNum;
            const int RECORD     = BAD_DATA[ti].d_recordLevel;
            const int PASS       = BAD_DATA[ti].d_passLevel;
            const int TRIGGER    = BAD_DATA[ti].d_triggerLevel;
            const int TRIGGERALL = BAD_DATA[ti].d_triggerAllLevel;

            if (veryVeryVerbose) {
                P_(LINE); P_(RECORD); P_(PASS); P_(TRIGGER); P(TRIGGERALL);
            }

            const int S = Obj::setDefaultThresholdLevels(RECORD,
                                                         PASS,
                                                         TRIGGER,
                                                         TRIGGERALL);
            ASSERT(0 != S);
            ASSERT(DEFAULT_RECORD_THRESHOLD_LEVEL
                                    == Obj::defaultRecordThresholdLevel());
            ASSERT(DEFAULT_PASS_THRESHOLD_LEVEL
                                    == Obj::defaultPassThresholdLevel());
            ASSERT(DEFAULT_TRIGGER_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerThresholdLevel());
            ASSERT(DEFAULT_TRIGGERALL_THRESHOLD_LEVEL
                                    == Obj::defaultTriggerAllThresholdLevel());
        }

        if (veryVerbose) cout << "Testing \"by-name\" methods." << endl;

        ASSERT(DEFAULT_CATEGORY_RECORD_THRESHOLD
                               == Obj::recordLevel(DEFAULT_CATEGORY_NAME));
        ASSERT(DEFAULT_CATEGORY_PASS_THRESHOLD
                               == Obj::passLevel(DEFAULT_CATEGORY_NAME));
        ASSERT(DEFAULT_CATEGORY_TRIGGER_THRESHOLD
                               == Obj::triggerLevel(DEFAULT_CATEGORY_NAME));
        ASSERT(DEFAULT_CATEGORY_TRIGGERALL_THRESHOLD
                               == Obj::triggerAllLevel(DEFAULT_CATEGORY_NAME));

        if (veryVerbose) cout << "\tTesting valid threshold levels." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE       = DATA[ti].d_lineNum;
            const int   RECORD     = DATA[ti].d_recordLevel;
            const int   PASS       = DATA[ti].d_passLevel;
            const int   TRIGGER    = DATA[ti].d_triggerLevel;
            const int   TRIGGERALL = DATA[ti].d_triggerAllLevel;
            const char *NAME       = DATA[ti].d_name;

            if (veryVeryVerbose) {
                P_(LINE); P_(NAME); P_(RECORD); P_(PASS);
                P_(TRIGGER); P(TRIGGERALL);
            }

            {
                const int S = Obj::setThresholdLevels(NAME,
                                                      RECORD,
                                                      PASS,
                                                      TRIGGER,
                                                      TRIGGERALL);
                ASSERT(0 == S);
                ASSERT(0 >  Obj::recordLevel(NAME));
                ASSERT(0 >  Obj::passLevel(NAME));
                ASSERT(0 >  Obj::triggerLevel(NAME));
                ASSERT(0 >  Obj::triggerAllLevel(NAME));
            }

            {
                int oldNumCategories = Obj::numCategories();
                const int S = Obj::addCategory(NAME,
                                               RECORD,
                                               PASS,
                                               TRIGGER,
                                               TRIGGERALL);
                int newNumCategories = Obj::numCategories();
                ASSERT(oldNumCategories + 1 == newNumCategories);
                ASSERT(0          == S);
                ASSERT(RECORD     == Obj::recordLevel(NAME));
                ASSERT(PASS       == Obj::passLevel(NAME));
                ASSERT(TRIGGER    == Obj::triggerLevel(NAME));
                ASSERT(TRIGGERALL == Obj::triggerAllLevel(NAME));
            }

            for (int tk = 0; tk < NUM_DATA; ++tk) {
//              const int K_LINE       = DATA[tk].d_lineNum; // unused
                const int K_RECORD     = DATA[tk].d_recordLevel;
                const int K_PASS       = DATA[tk].d_passLevel;
                const int K_TRIGGER    = DATA[tk].d_triggerLevel;
                const int K_TRIGGERALL = DATA[tk].d_triggerAllLevel;

                {
                    const int S = Obj::setThresholdLevels(NAME,  // ti's name
                                                          K_RECORD,
                                                          K_PASS,
                                                          K_TRIGGER,
                                                          K_TRIGGERALL);
                    ASSERT(1            == S);
                    ASSERT(K_RECORD     == Obj::recordLevel(NAME));
                    ASSERT(K_PASS       == Obj::passLevel(NAME));
                    ASSERT(K_TRIGGER    == Obj::triggerLevel(NAME));
                    ASSERT(K_TRIGGERALL == Obj::triggerAllLevel(NAME));
                }
            }

            // restore pre-tk levels
            {
                const int S = Obj::setThresholdLevels(NAME,
                                                      RECORD,
                                                      PASS,
                                                      TRIGGER,
                                                      TRIGGERALL);
                ASSERT(1          == S);
                ASSERT(RECORD     == Obj::recordLevel(NAME));
                ASSERT(PASS       == Obj::passLevel(NAME));
                ASSERT(TRIGGER    == Obj::triggerLevel(NAME));
                ASSERT(TRIGGERALL == Obj::triggerAllLevel(NAME));
            }

            for (int tk = 0; tk < NUM_DATA; ++tk) {
//                const int K_LINE       = DATA[tk].d_lineNum; // unused
                const int K_RECORD     = DATA[tk].d_recordLevel;
                const int K_PASS       = DATA[tk].d_passLevel;
                const int K_TRIGGER    = DATA[tk].d_triggerLevel;
                const int K_TRIGGERALL = DATA[tk].d_triggerAllLevel;

                {
                    int oldNumCategories = Obj::numCategories();
                    const int S = Obj::addCategory(NAME,  // ti's name
                                                   K_RECORD,
                                                   K_PASS,
                                                   K_TRIGGER,
                                                   K_TRIGGERALL);
                    int newNumCategories = Obj::numCategories();
                    ASSERT(oldNumCategories == newNumCategories);

                    ASSERT(0          != S);
                    ASSERT(RECORD     == Obj::recordLevel(NAME));
                    ASSERT(PASS       == Obj::passLevel(NAME));
                    ASSERT(TRIGGER    == Obj::triggerLevel(NAME));
                    ASSERT(TRIGGERALL == Obj::triggerAllLevel(NAME));
                }
            }
        }

        if (veryVerbose) cout << "\tTesting invalid threshold levels." << endl;

        for (int ti = 0; ti < NUM_BAD_DATA; ++ti) {
            const int   LINE       = BAD_DATA[ti].d_lineNum;
            const int   RECORD     = BAD_DATA[ti].d_recordLevel;
            const int   PASS       = BAD_DATA[ti].d_passLevel;
            const int   TRIGGER    = BAD_DATA[ti].d_triggerLevel;
            const int   TRIGGERALL = BAD_DATA[ti].d_triggerAllLevel;
            const char *NAME       = BAD_DATA[ti].d_name;

            if (veryVeryVerbose) {
                P_(LINE); P_(NAME); P_(RECORD); P_(PASS);
                P_(TRIGGER); P(TRIGGERALL);
            }

            int oldNumCategories = Obj::numCategories();

            const int S = Obj::addCategory(NAME,
                                           RECORD,
                                           PASS,
                                           TRIGGER,
                                           TRIGGERALL);
            int newNumCategories = Obj::numCategories();
            ASSERT(oldNumCategories == newNumCategories);

            ASSERT(0 != S);
            ASSERT(0 >  Obj::recordLevel(NAME));
            ASSERT(0 >  Obj::passLevel(NAME));
            ASSERT(0 >  Obj::triggerLevel(NAME));
            ASSERT(0 >  Obj::triggerAllLevel(NAME));
        }

        {
            int oldNumCategories = Obj::numCategories();
            const int S = Obj::addCategory(XNAME,
                                           DEFAULT_RECORD_THRESHOLD_LEVEL,
                                           DEFAULT_PASS_THRESHOLD_LEVEL,
                                           DEFAULT_TRIGGER_THRESHOLD_LEVEL,
                                           DEFAULT_TRIGGERALL_THRESHOLD_LEVEL);
            int newNumCategories = Obj::numCategories();
            ASSERT(oldNumCategories + 1 == newNumCategories);

            ASSERT(0 == S);
            ASSERT(DEFAULT_RECORD_THRESHOLD_LEVEL
                                               == Obj::recordLevel(XNAME));
            ASSERT(DEFAULT_PASS_THRESHOLD_LEVEL
                                               == Obj::passLevel(XNAME));
            ASSERT(DEFAULT_TRIGGER_THRESHOLD_LEVEL
                                               == Obj::triggerLevel(XNAME));
            ASSERT(DEFAULT_TRIGGERALL_THRESHOLD_LEVEL
                                               == Obj::triggerAllLevel(XNAME));
        }

        for (int ti = 0; ti < NUM_BAD_DATA; ++ti) {
            const int LINE       = BAD_DATA[ti].d_lineNum;
            const int RECORD     = BAD_DATA[ti].d_recordLevel;
            const int PASS       = BAD_DATA[ti].d_passLevel;
            const int TRIGGER    = BAD_DATA[ti].d_triggerLevel;
            const int TRIGGERALL = BAD_DATA[ti].d_triggerAllLevel;

            if (veryVeryVerbose) {
                P_(LINE); P_(RECORD); P_(PASS); P_(TRIGGER); P(TRIGGERALL);
            }

            const int S = Obj::setThresholdLevels(XNAME,
                                                  RECORD,
                                                  PASS,
                                                  TRIGGER,
                                                  TRIGGERALL);
            ASSERT(0 > S);
            ASSERT(DEFAULT_RECORD_THRESHOLD_LEVEL
                                               == Obj::recordLevel(XNAME));
            ASSERT(DEFAULT_PASS_THRESHOLD_LEVEL
                                               == Obj::passLevel(XNAME));
            ASSERT(DEFAULT_TRIGGER_THRESHOLD_LEVEL
                                               == Obj::triggerLevel(XNAME));
            ASSERT(DEFAULT_TRIGGERALL_THRESHOLD_LEVEL
                                               == Obj::triggerAllLevel(XNAME));
        }

        if (veryVerbose) cout << "\tTesting regular expressions." << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_re;       // regular expression
            int         d_numSet;   // # of categories set
        } RE_DATA[] = {
            // line   r.e.            # set                           // ADJUST
            // ----   ----------      ----
            {  L_,    "E.G.M.Ax",      0                                     },
            {  L_,    "E.G.M.A",       1                                     },
            {  L_,    "",              1                                     },
            {  L_,    "E.G.M.*",       2                                     },
            {  L_,    "E.G.*",         2                                     },
            {  L_,    "E.*",           4                                     },
            {  L_,    "*",             6                                     },
        };
        const int NUM_RE_DATA = sizeof RE_DATA / sizeof RE_DATA[0];

        for (int ti = 0; ti < NUM_RE_DATA; ++ti) {
            const int   LINE    = RE_DATA[ti].d_lineNum;
            const char *RE      = RE_DATA[ti].d_re;
            const int   NUM_SET = RE_DATA[ti].d_numSet;

            if (veryVeryVerbose) { P_(LINE); P_(RE); P(NUM_SET); }

            ASSERT(NUM_SET == Obj::setThresholdLevels(RE, 16, 32, 64, 128));
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
