// ball_administration.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_administration.h>

#include <ball_loggermanagerconfiguration.h> // for testing only
#include <ball_loggermanager.h>              // for testing only
#include <ball_severity.h>                   // for testing only
#include <ball_testobserver.h>               // for testing only
#include <ball_defaultobserver.h>            // for testing only

#include <bslim_testutil.h>

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
// the 'ball_loggermanager' and 'ball_loggercategoryutil' components.  It is
// sufficient to test that each "facade" method correctly forwards its
// arguments to the corresponding methods of 'ball_loggermanager' and
// 'ball_loggercategoryutil', and that the correct value is returned.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 1] static int addCategory(const char *name, int, int, int, int);
// [ 1] static int setThresholdLevels(const char *re, int, int, int, int);
// [ 1] static int setDefaultCategoryThresholds(int, int, int, int);
// [ 1] static void resetDefaultCategoryThresholds();
// [ 1] static int setDefaultThresholdLevels(int, int, int, int);
// [ 3] static int setAllThresholdLevels(int, int, int, int);
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

typedef ball::Administration Obj;

// Note: the following *must* be kept in sync with 'ball_loggermanager'.

const char *DEFAULT_CATEGORY_NAME                 = "";
const int   DEFAULT_CATEGORY_RECORD_THRESHOLD     = 0;
const int   DEFAULT_CATEGORY_PASS_THRESHOLD       = ball::Severity::e_ERROR;
const int   DEFAULT_CATEGORY_TRIGGER_THRESHOLD    = 0;
const int   DEFAULT_CATEGORY_TRIGGERALL_THRESHOLD = 0;

const int   DEFAULT_RECORD_THRESHOLD_LEVEL        = 0;
const int   DEFAULT_PASS_THRESHOLD_LEVEL          = ball::Severity::e_ERROR;
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

    ball::TestObserver  testObserver(cout);
    ball::TestObserver *TO = &testObserver;

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
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
            bsl::ostrstream cout;

///USAGE
///-----
// The code fragments in this example demonstrate several administration
// utilities that are used to create categories, and to set and access their
// threshold levels.
//
// First we initialize the logger manager (for the purposes of this example,
// we use a minimal configuration):
//..
     ball::DefaultObserver observer(&cout);
     ball::LoggerManagerConfiguration configuration;
     ball::LoggerManagerScopedGuard guard(&observer, configuration);
//..
// Next define some hypothetical category names:
//..
     const char *equityCategories[] = {
         "EQUITY.MARKET.NYSE",
         "EQUITY.MARKET.NASDAQ",
         "EQUITY.GRAPHICS.MATH.FACTORIAL",
         "EQUITY.GRAPHICS.MATH.ACKERMANN"
     };
     const int NUM_CATEGORIES = sizeof equityCategories
                              / sizeof equityCategories[0];
//..
// Category naming is by convention only.  In this example, we have chosen a
// hierarchical naming convention that uses '.' to separate the constituents
// of category names.
//
// In the following, the 'addCategory' method is used to define a category for
// each of the category names in 'equityCategories'.  The threshold levels
// for each of the categories are set to slightly different values to help
// distinguish them when they are printed later.  The 'addCategory' method
// returns the address of the new category:
//..
     for (int i = 0; i < NUM_CATEGORIES; ++i) {
         int retValue = ball::Administration::addCategory(
                                              equityCategories[i],
                                              ball::Severity::e_TRACE + i,
                                              ball::Severity::e_WARN  + i,
                                              ball::Severity::e_ERROR + i,
                                              ball::Severity::e_FATAL + i);
         ASSERT(0 == retValue);  // added new category
     }
//..
// In the following, each of the new categories is accessed from the registry
// and its name and threshold levels are printed to 'bsl::cout':
//..
     for (int i = 0; i < NUM_CATEGORIES; ++i) {
         const char* name    = equityCategories[i];
         int recordLevel     = ball::Administration::recordLevel(name);
         int passLevel       = ball::Administration::passLevel(name);
         int triggerLevel    = ball::Administration::triggerLevel(name);
         int triggerAllLevel = ball::Administration::triggerAllLevel(name);
//
         using namespace bsl;
         cout << "Category name: "       << name            << endl;
         cout << "\tRecord level:      " << recordLevel     << endl;
         cout << "\tPass level:        " << passLevel       << endl;
         cout << "\tTrigger level:     " << triggerLevel    << endl;
         cout << "\tTrigger-all level: " << triggerAllLevel << endl
              << endl;
     }
//..
// The following is printed to 'stdout':
//..
//   Category name: EQUITY.MARKET.NYSE
//           Record level:      192
//           Pass level:        96
//           Trigger level:     64
//           Trigger-all level: 32
//
//   Category name: EQUITY.MARKET.NASDAQ
//           Record level:      193
//           Pass level:        97
//           Trigger level:     65
//           Trigger-all level: 33
//
//   Category name: EQUITY.GRAPHICS.MATH.FACTORIAL
//           Record level:      194
//           Pass level:        98
//           Trigger level:     66
//           Trigger-all level: 34
//
//   Category name: EQUITY.GRAPHICS.MATH.ACKERMANN
//           Record level:      195
//           Pass level:        99
//           Trigger level:     67
//           Trigger-all level: 35
//..
// The following is similar to the first for-loop above, but this time the
// 'setThresholdLevels' method is used to modify the threshold levels of
// existing categories.  The 'setThresholdLevels' method returns 1 to indicate
// the number of existing categories that were affected by the call:
//..
     for (int i = 0; i < NUM_CATEGORIES; ++i) {
         const int returnValue =
                   ball::Administration::setThresholdLevels(
                                              equityCategories[i],
                                              ball::Severity::e_TRACE - i,
                                              ball::Severity::e_WARN  - i,
                                              ball::Severity::e_ERROR - i,
                                              ball::Severity::e_FATAL - i);
         ASSERT(1 == returnValue);  // modified one category
     }
//..
// When the 'NUM_CATEGORIES' categories are accessed from the registry a second
// time and printed, the following is output to 'stdout' showing the new
// threshold levels of the categories:
//..
//   Category name: EQUITY.MARKET.NYSE
//           Record level:      192
//           Pass level:        96
//           Trigger level:     64
//           Trigger-all level: 32
//
//   Category name: EQUITY.MARKET.NASDAQ
//           Record level:      191
//           Pass level:        95
//           Trigger level:     63
//           Trigger-all level: 31
//
//   Category name: EQUITY.GRAPHICS.MATH.FACTORIAL
//           Record level:      190
//           Pass level:        94
//           Trigger level:     62
//           Trigger-all level: 30
//
//   Category name: EQUITY.GRAPHICS.MATH.ACKERMANN
//           Record level:      189
//           Pass level:        93
//           Trigger level:     61
//           Trigger-all level: 29
//..
// Finally, the category registry is closed to further additions by setting its
// maximum capacity to (the original) 'NUM_CATEGORIES':
//..
     ball::Administration::setMaxNumCategories(NUM_CATEGORIES);
//..
// Following this call to 'setMaxNumCategories', subsequent calls to
// 'addCategory' will fail (until such time as 'setMaxNumCategories' is called
// again with an argument value of either 0 or one that is greater than
// 'NUM_CATEGORIES').

            if (veryVerbose) {
                bsl::cout << cout.str() << bsl::endl;
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: setAllThresholdLevels
        //
        // Concerns:
        //: 1 'setAllThresholdLevels' sets the default threshold level to
        //:   the supplied thresholds.
        //:
        //: 2 'setAllThresholdLevels' sets supplies the correct record, pass,
        //:    trigger, and trigger all levels to both the default threshold
        //:    level and the existing categories threshold levels.
        //:
        //: 3 'setAllThresholdLevels' sets any existing category threshold
        //:    levels to the supplied thresholds.
        //:
        //: 4  'setAllThresholdLevels' returns 0 on success.
        //:
        //: 5 If the supplied pass-through level is not in the ranage [0..255]
        //:   the 'setAllThresholdLevels' returns a non-zero value
        //
        // Plan:
        //: 1 Manually call method with valid values and verify the return is
        //:   0 and the default threshold levels are set (C-1, C-2, C-4)
        //:
        //: 2 Create a couple categories and call method with valid values and
        //:   verify the return is 0 and the default threshold levels as well
        //:   the created threshold levels are set (C-2..4))
        //:
        //: 2 Call the method several times, supplying an invalid level
        //:   for a different threshold each time, verify the return value is
        //:   not 0, the default threshold level isn't set, and the existing
        //:   categories threshold levels are not modified. (C-5)
        //
        // Testing:
        //   int setDefaultThresholdLevels(int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING: setAllThresholdLevels" << endl
                          << "==============================" << endl;

        ball::LoggerManager::initSingleton(TO);  // initialize logger manager


        if (verbose) cout
            << "\nTest 'setAllThresholdLevels' sets the default threshold"
            << endl;
        {
            ASSERT(0 == Obj::setDefaultThresholdLevels(0, 0, 0, 0));

            ASSERT(0 == Obj::setAllThresholdLevels(1, 2, 3, 4));
            ASSERT(1 == Obj::defaultRecordThresholdLevel());
            ASSERT(2 == Obj::defaultPassThresholdLevel());
            ASSERT(3 == Obj::defaultTriggerThresholdLevel());
            ASSERT(4 == Obj::defaultTriggerAllThresholdLevel());
        }

        if (verbose) cout
            << "\nTest 'setAllThresholdLevels' updates existing categories"
            << endl;
        {
            ASSERT(0 == Obj::setDefaultThresholdLevels(0, 0, 0, 0));
            ASSERT(0 == Obj::addCategory("A", 1, 1, 1, 1));
            ASSERT(0 == Obj::addCategory("B", 1, 1, 1, 1));

            ASSERT(0 == Obj::setAllThresholdLevels(5, 6, 7, 8));
            ASSERT(5 == Obj::defaultRecordThresholdLevel());
            ASSERT(6 == Obj::defaultPassThresholdLevel());
            ASSERT(7 == Obj::defaultTriggerThresholdLevel());
            ASSERT(8 == Obj::defaultTriggerAllThresholdLevel());

            ASSERT(5 == Obj::recordLevel("A"));
            ASSERT(6 == Obj::passLevel("A"));
            ASSERT(7 == Obj::triggerLevel("A"));
            ASSERT(8 == Obj::triggerAllLevel("A"));
            ASSERT(5 == Obj::recordLevel("B"));
            ASSERT(6 == Obj::passLevel("B"));
            ASSERT(7 == Obj::triggerLevel("B"));
            ASSERT(8 == Obj::triggerAllLevel("B"));
        }
        if (verbose) cout
            << "\nTest 'setAllThresholdLevels' with invalid thresholds"
            << endl;
        {
            ASSERT(0 == Obj::setDefaultThresholdLevels(0, 0, 0, 0));
            ASSERT(1 == Obj::setThresholdLevels("A", 1, 1, 1, 1));
            ASSERT(1 == Obj::setThresholdLevels("B", 1, 1, 1, 1));


            ASSERT(0 != Obj::setAllThresholdLevels(-1, 1, 1, 1));

            ASSERT(0 == Obj::defaultRecordThresholdLevel());
            ASSERT(0 == Obj::defaultPassThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerAllThresholdLevel());
            ASSERT(1 == Obj::recordLevel("A"));
            ASSERT(1 == Obj::passLevel("A"));
            ASSERT(1 == Obj::triggerLevel("A"));
            ASSERT(1 == Obj::triggerAllLevel("A"));
            ASSERT(1 == Obj::recordLevel("B"));
            ASSERT(1 == Obj::passLevel("B"));
            ASSERT(1 == Obj::triggerLevel("B"));
            ASSERT(1 == Obj::triggerAllLevel("B"));

            ASSERT(0 != Obj::setAllThresholdLevels(1, -1, 1, 1));

            ASSERT(0 == Obj::defaultRecordThresholdLevel());
            ASSERT(0 == Obj::defaultPassThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerAllThresholdLevel());
            ASSERT(1 == Obj::recordLevel("A"));
            ASSERT(1 == Obj::passLevel("A"));
            ASSERT(1 == Obj::triggerLevel("A"));
            ASSERT(1 == Obj::triggerAllLevel("A"));
            ASSERT(1 == Obj::recordLevel("B"));
            ASSERT(1 == Obj::passLevel("B"));
            ASSERT(1 == Obj::triggerLevel("B"));
            ASSERT(1 == Obj::triggerAllLevel("B"));

            ASSERT(0 != Obj::setAllThresholdLevels(1, 1, -1, 1));

            ASSERT(0 == Obj::defaultRecordThresholdLevel());
            ASSERT(0 == Obj::defaultPassThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerAllThresholdLevel());
            ASSERT(1 == Obj::recordLevel("A"));
            ASSERT(1 == Obj::passLevel("A"));
            ASSERT(1 == Obj::triggerLevel("A"));
            ASSERT(1 == Obj::triggerAllLevel("A"));
            ASSERT(1 == Obj::recordLevel("B"));
            ASSERT(1 == Obj::passLevel("B"));
            ASSERT(1 == Obj::triggerLevel("B"));
            ASSERT(1 == Obj::triggerAllLevel("B"));

            ASSERT(0 != Obj::setAllThresholdLevels(1, 1, 1, -1));

            ASSERT(0 == Obj::defaultRecordThresholdLevel());
            ASSERT(0 == Obj::defaultPassThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerThresholdLevel());
            ASSERT(0 == Obj::defaultTriggerAllThresholdLevel());
            ASSERT(1 == Obj::recordLevel("A"));
            ASSERT(1 == Obj::passLevel("A"));
            ASSERT(1 == Obj::triggerLevel("A"));
            ASSERT(1 == Obj::triggerAllLevel("A"));
            ASSERT(1 == Obj::recordLevel("B"));
            ASSERT(1 == Obj::passLevel("B"));
            ASSERT(1 == Obj::triggerLevel("B"));
            ASSERT(1 == Obj::triggerAllLevel("B"));
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

        ball::LoggerManager::initSingleton(TO);  // initialize logger manager

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
        //       to the corresponding method of 'ball_loggermanager' or
        //       'ball_loggercategoryutil'.
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

        ball::LoggerManager::initSingleton(TO);  // initialize logger manager

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
