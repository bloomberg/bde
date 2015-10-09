// ball_loggerfunctorpayloads.t.cpp                                   -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_loggerfunctorpayloads.h>

#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>    // for testing only
#include <ball_testobserver.h>                  // for testing only

#include <bdlf_bind.h>                          // for testing only
#include <bdlf_placeholder.h>                   // for testing only

#include <bslim_testutil.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility consisting of one pure procedure,
// 'setThresholdLevels'.  That lone method is tested using a table-based
// approach where the following test data is tabulated: valid and invalid
// threshold level values; category names; regular expressions.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJ".
//-----------------------------------------------------------------------------
// [ 1] static int setThresholdLevels(*lm, *re, int, int, int, int);
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

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

typedef ball::LoggerFunctorPayloads Util;
typedef ball::Category              Cat;

const char *DEFAULT_CATEGORY_NAME = "";


//=============================================================================
//                               USAGE EXAMPLE 1
//-----------------------------------------------------------------------------

namespace BALL_USAGE_EXAMPLE_1 {

///Usage
///-----
// The following code snippets illustrate how to use this component's
// 'loadParentCategoryThresholdValues' method to allow a newly-created "child"
// category to inherit the logging threshold levels from its most proximate
// parent category (if such a category already exists).  Note that the category
// "hierarchy" is by naming convention only, but that the callback makes it
// simple for the *user* to impose hierarchical meaning on names that are, from
// the *logger's* perspective, peers.  In this example, we will choose the dot
// character ('.') as the hierarchy delimiter; to the logger itself, '.' is not
// special.
//
// To keep this example transparent, we will create and inspect several
// categories within 'main' directly; some categories will be "declared" to be
// "parent" categories, and we will set the threshold levels explicitly, while
// other categories will act as "children", which is to say that they will
// obtain their threshold levels through the callback mechanism.  In a more
// realistic example, there would be no explicit distinction between "parent"
// and "child" categories, but rather as categories are dynamically
// administered by the user, newly created categories would pick up the changes
// made to existing parents.  As a practical matter, beginning of the function
// 'main' constitute the "usage" that the user must master to *install* the
// callback; the rest of this example merely illustrates the *consequences* of
// installing the callback.
//
// Before we can begin logging, we must first create an observer with which to
// initialize the logger manager.  For this example, a 'ball::TestObserver'
// initialized to write to 'stdout' will do just fine.
//..
     // myapp.cpp
     int main()
     {
         ball::TestObserver testObserver(bsl::cout);

//..
// Now, we load the logger manager 'configuration' with the desired "payload"
// function, 'ball::LoggerFunctorPayloads::loadParentCategoryThresholdValues',
// and use the trailing 'char' argument 'delimiter', set to the value '.',
// which will be bound into the functor and supplied back to the payload on
// each invocation.
//..
         using namespace bdlf::PlaceHolders;

         ball::LoggerManagerConfiguration configuration;
         char delimiter = '.';
         configuration.setDefaultThresholdLevelsCallback(
             bdlf::BindUtil::bind(
               &ball::LoggerFunctorPayloads::loadParentCategoryThresholdValues,
               _1,
               _2,
               _3,
               _4,
               _5,
               delimiter));
//..
// We are now ready to initialize the logger manager, using the observer and
// the callback defined above.
//..
         ball::LoggerManagerScopedGuard guard(&testObserver, configuration);
//..
// The above code is all that the user needs to do to customize the logger to
// "inherit" thresholds from parents.  The rest of this example illustrates the
// consequences of having installed 'myCallback'.  For convenience in what
// follows, we define a reference, 'manager', to the singleton logger manager.
//..
         ball::LoggerManager& manager = ball::LoggerManager::singleton();
//..
// We now create two "parent" categories named "EQUITY.MARKET" and
// "EQUITY.GRAPHICS", and give them arbitrary but distinct threshold levels.
// We also set the default levels to distinct values in order to be able to
// verify exactly where "child" levels have come from later on.
//..
         manager.setDefaultThresholdLevels(128, 96, 64, 32);
//
         manager.addCategory("EQUITY.MARKET", 127, 95, 63, 31);
         manager.addCategory("EQUITY.GRAPHICS", 129, 97, 65, 33);
//..
// Note that the call to 'addCategory', which takes the four 'int' threshold
// arguments, does not invoke the callback at all, but rather -- assuming that
// the named category does not yet exist -- sets the thresholds to the
// specified values directly.
//
// We can use the logger manager interface to verify that the levels have been
// set.  First, we use the 'lookupCategory' method to obtain the two parent
// categories (here assigned 'p1' and 'p2').
//..
         const ball::Category *p1 = manager.lookupCategory("EQUITY.MARKET");
         const ball::Category *p2 = manager.lookupCategory("EQUITY.GRAPHICS");
//..
// Next, we can use the appropriate 'ball::Category' accessor methods to
// 'ASSERT' the expected results.  Recall that the ordered sequence of levels
// is "Record", "Pass", "Trigger", and "TriggerAll".
//..
             ASSERT(127 == p1->recordLevel());
             ASSERT( 95 == p1->passLevel());
             ASSERT( 63 == p1->triggerLevel());
             ASSERT( 31 == p1->triggerAllLevel());
//
             ASSERT(129 == p2->recordLevel());
             ASSERT( 97 == p2->passLevel());
             ASSERT( 65 == p2->triggerLevel());
             ASSERT( 33 == p2->triggerAllLevel());
//..
// Now, we will add several "child" categories using the 'setCategory' method
// taking a single argument, the 'char*' category name.  This method uses the
// callback in determining the "default" threshold levels to use.  The six
// statements are numbered for subsequent discussion.
//..
         manager.setCategory("EQUITY.MARKET.NYSE");                      // (1)
         manager.setCategory("EQUITY.MARKET.NASDAQ");                    // (2)
         manager.setCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");          // (3)
         manager.setCategory("EQUITY.GRAPHICS.MATH.ACKERMANN");          // (4)
         manager.setCategory("EQUITY.GRAPHICS.MATH");                    // (5)
         manager.setCategory("EQUITY");                                  // (6)
//..
// Note that all six calls to 'setCategory' will succeed in adding new
// categories to the registry.  Calls (1)-(5) will "find" their parent's names
// and "inherit" the parent's levels.  Call (6), however, will not find a
// parent category, and so will receive the default threshold levels, just as
// if there were no callback installed.
//
// Note also that, although in this "static" (i.e., unadministered) example
// there is no significance to the order in which the above categories are
// created, in general (e.g., when categories are being dynamically
// administered) the order of creation *does* matter.  If line (5) were
// executed before line (4) then the call on line (4) would find the
// "EQUITY.GRAPHICS.MATH" category as its "parent" and inherit those threshold
// levels.  If, before line (4) executed, the thresholds of
// "EQUITY.GRAPHICS.MATH" were changed, then "EQUITY.GRAPHICS.MATH.FACTORIAL"
// and "EQUITY.GRAPHICS.MATH.ACKERMANN" would have different threshold levels
// despite their equivalent standing in the category hierarchy.
//
// Let us now verify some of the 24 threshold levels that have been set by the
// above calls.  We will verify the results of lines (1), (3), and (6) above.
//..
         const ball::Category *c1, *c3, *c6;
//
         c1 =  manager.lookupCategory("EQUITY.MARKET.NYSE");
             ASSERT(127 == c1->recordLevel());
             ASSERT( 95 == c1->passLevel());
             ASSERT( 63 == c1->triggerLevel());
             ASSERT( 31 == c1->triggerAllLevel());
//
         c3 =  manager.lookupCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");
             ASSERT(129 == c3->recordLevel());
             ASSERT( 97 == c3->passLevel());
             ASSERT( 65 == c3->triggerLevel());
             ASSERT( 33 == c3->triggerAllLevel());
//
         c6 =  manager.lookupCategory("EQUITY");
             ASSERT(128 == c6->recordLevel());
             ASSERT( 96 == c6->passLevel());
             ASSERT( 64 == c6->triggerLevel());
             ASSERT( 32 == c6->triggerAllLevel());
//
         return 0;
    }
//..

}  // close namespace BALL_USAGE_EXAMPLE_1
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

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        BALL_USAGE_EXAMPLE_1::main();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'loadParentCategoryThresholdValues'
        //
        // Concerns:
        //   The method under test relies on the services of the 'ball' logger,
        //   but does not modify the state of the logger.  The concerns are
        //   that, given known "parent" categories already registered in the
        //   logger, that the method can "find" the proximate parent as
        //   determined by the position(s) of a specific delimiter character
        //   and correctly load the parent's four threshold values, or else
        //   load the default values if no parent exists.
        //
        //   We are specifically *not* concerned with bsl::string's abilities
        //   to find a specific character among other characters, and so we can
        //   safely and effectively limit parent and child names to simple,
        //   subtly different sequences without compromising this test.  We do
        //   vary name lengths *slightly* to confirm that we correctly
        //   calculate indices in our own code, and we do use different
        //   delimiters.
        //
        //   Since the "factory" default threshold values cannot be relied on,
        //   we must explicitly set known defaults, which will be needed when
        //   a candidate "child" has no parent.
        //
        // Plan:
        //   Initialize the logger manager singleton, explicitly set its
        //   default thresholds, and populate its category registry with
        //   "parent" categories and "known" thresholds from a table.  The
        //   parent names use one of two delimiters, '.' and '/', and have
        //   either one or two non-delimiter characters at each name level.
        //
        //   Construct a table of candidate child category names, delimiters,
        //   and expected thresholds (either from a parent or from the default)
        //   and call 'loadParentCategoryThresholdValues' with names and
        //   delimiters from the table, confirming that the four 'int*'
        //   arguments were correctly assigned to.
        //
        // Testing:
        //   loadParentCategoryThresholdValues(4x(int*), const char *, char);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing 'loadParentCategoryThresholdValues'" << endl
                 << "===========================================" << endl;

        // initialize logger manager
        BloombergLP::ball::TestObserver testObserver(bsl::cout);
        ball::LoggerManager::initSingleton(&testObserver);
        ball::LoggerManager& manager = ball::LoggerManager::singleton();

        manager.setDefaultThresholdLevels(10, 11, 12, 13);

        static const struct {
            const char *d_name;
            int         d_record;
            int         d_pass;
            int         d_trigger;
            int         d_triggerAll;
        } PARENT[] = {
            // parent name                       rec.  pass  trig.  trgAll.
            // --------------------------        ----  ----  -----  -------
            {  "A",                               20,   21,   22,    23 },
            {  "AA",                              30,   31,   32,    33 },
            {  "A.A",                             40,   41,   42,    43 },
            {  "A/A",                             50,   51,   52,    53 },
            {  "AA.A",                            60,   61,   62,    63 },
            {  "AA/A",                            70,   71,   72,    73 },
            {  "A.AA",                            80,   81,   82,    83 },
            {  "A/AA",                            90,   91,   92,    93 }
        };
        const int NUM_PARENTS = sizeof PARENT / sizeof *PARENT;

        if (verbose) cout << "\nPopulating parent categories." << endl;
        for (int i = 0; i < NUM_PARENTS; ++i) {
            const Cat *p = manager.addCategory(PARENT[i].d_name,
                                               PARENT[i].d_record,
                                               PARENT[i].d_pass,
                                               PARENT[i].d_trigger,
                                               PARENT[i].d_triggerAll);

            if (veryVerbose) {
                const Cat *category = manager.lookupCategory(PARENT[i].d_name);
                cout << "\t[ " << PARENT[i].d_name << '\t'
                     << ", " << category->recordLevel()
                     << ", " << category->passLevel()
                     << ", " << category->triggerLevel()
                     << ", " << category->triggerAllLevel()
                     << " ]" << endl;
            }

            ASSERT(p);
        }

        static const struct {
            int         d_line;
            const char *d_name;
            char        d_delimiter;
            int         d_expRecord;
            int         d_expPass;
            int         d_expTrigger;
            int         d_expTriggerAll;
        } DATA[] = {
            // line  child name             delim.  rec.  pass  trig.  trgAll.
            // ----  --------------------   -----   ----  ----  -----  -------
            {   L_,  "Z",                    '.',    10,   11,   12,    13 },
            {   L_,  "Z.A",                  '.',    10,   11,   12,    13 },
            {   L_,  "ZZ.A",                 '.',    10,   11,   12,    13 },
            {   L_,  "Z.AA",                 '.',    10,   11,   12,    13 },
            {   L_,  "Z/A",                  '/',    10,   11,   12,    13 },
            {   L_,  "ZZ/A",                 '/',    10,   11,   12,    13 },
            {   L_,  "Z/AA",                 '/',    10,   11,   12,    13 },

            {   L_,  "A.A.A",                '/',    10,   11,   12,    13 },

            {   L_,  "A.B",                  '.',    20,   21,   22,    23 },
            {   L_,  "A%B",                  '%',    20,   21,   22,    23 },
            {   L_,  "AA.B",                 '.',    30,   31,   32,    33 },
            {   L_,  "A.A/A",                '/',    40,   41,   42,    43 },

            {   L_,  "A/A/B",                '/',    50,   51,   52,    53 },
            {   L_,  "A/A/BB",               '/',    50,   51,   52,    53 },
            {   L_,  "A/A/B/C/D",            '/',    50,   51,   52,    53 },

            {   L_,  "AA.A.B",               '.',    60,   61,   62,    63 },
            {   L_,  "AA.A.BBB",             '.',    60,   61,   62,    63 },
            {   L_,  "AA.A.B.CC.DDD",        '.',    60,   61,   62,    63 },
            {   L_,  "AA.A/X/YY/ZZZ",        '/',    60,   61,   62,    63 },

            {   L_,  "AA/A/BB/CC/DD",        '/',    70,   71,   72,    73 },
            {   L_,  "A.AA.BB.CC.DD",        '.',    80,   81,   82,    83 },
            {   L_,  "A/AA/BB/CC/DD",        '/',    90,   91,   92,    93 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\nTesting child categories." << endl;

        int recordLevel, passLevel, triggerLevel, triggerAllLevel;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE           = DATA[ti].d_line;
            const char *NAME           = DATA[ti].d_name;
            const char  DELIM          = DATA[ti].d_delimiter;
            const int   EXP_RECORD     = DATA[ti].d_expRecord;
            const int   EXP_PASS       = DATA[ti].d_expPass;
            const int   EXP_TRIGGER    = DATA[ti].d_expTrigger;
            const int   EXP_TRIGGERALL = DATA[ti].d_expTriggerAll;

            if (veryVerbose) {
                T_;               P_(NAME);
                T_;               P_(DELIM);
                P_(EXP_RECORD);   P_(EXP_PASS);
                P_(EXP_TRIGGER);  P(EXP_TRIGGERALL);
            }

            Util::loadParentCategoryThresholdValues(&recordLevel,
                                                    &passLevel,
                                                    &triggerLevel,
                                                    &triggerAllLevel,
                                                    NAME,
                                                    DELIM);

            if (veryVeryVerbose) {
                T_;                P_(NAME);
                T_;                P_(DELIM);
                P_(recordLevel);   P_(passLevel);
                P_(triggerLevel);  P(triggerAllLevel);
            }

            LOOP_ASSERT(LINE, EXP_RECORD     == recordLevel);
            LOOP_ASSERT(LINE, EXP_PASS       == passLevel);
            LOOP_ASSERT(LINE, EXP_TRIGGER    == triggerLevel);
            LOOP_ASSERT(LINE, EXP_TRIGGERALL == triggerAllLevel);

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
