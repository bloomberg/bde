// bael_loggerfunctorpayloads.t.cpp            -*-C++-*-

#include <bael_loggerfunctorpayloads.h>

#include <bael_loggermanager.h>                 // for testing only
#include <bael_testobserver.h>                  // for testing only

#include <bsls_platformutil.h>                  // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bdef_function.h>                      // for testing only
#include <bdef_bind.h>                          // for testing only
#include <bdef_placeholder.h>                   // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>
#include <bsl_strstream.h>

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

typedef bael_LoggerFunctorPayloads Util;
typedef bael_Category              Cat;

const char *DEFAULT_CATEGORY_NAME = "";

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

        {
//   // myapp.cpp
//   int main()
//   {
           BloombergLP::bael_TestObserver testObserver(bsl::cout);

           char delimiter = '.';

           using namespace bdef_PlaceHolders;
           bael_LoggerManager::DefaultThresholdLevelsCallback myCallback =
               bdef_BindUtil::bind( &bael_LoggerFunctorPayloads
                                        ::loadParentCategoryThresholdValues
                                  , _1
                                  , _2
                                  , _3
                                  , _4
                                  , _5
                                  , delimiter);

           bael_LoggerManager::initSingleton(&testObserver, myCallback);

           bael_LoggerManager& manager = bael_LoggerManager::singleton();

           manager.setDefaultThresholdLevels(128, 96, 64, 32);

           manager.addCategory("EQUITY.MARKET", 127, 95, 63, 31);
           manager.addCategory("EQUITY.GRAPHICS", 129, 97, 65, 33);

           const bael_Category *p1 = manager.lookupCategory("EQUITY.MARKET");
           const bael_Category *p2 = manager.lookupCategory("EQUITY.GRAPHICS");

               ASSERT(127 == p1->recordLevel());
               ASSERT( 95 == p1->passLevel());
               ASSERT( 63 == p1->triggerLevel());
               ASSERT( 31 == p1->triggerAllLevel());

               ASSERT(129 == p2->recordLevel());
               ASSERT( 97 == p2->passLevel());
               ASSERT( 65 == p2->triggerLevel());
               ASSERT( 33 == p2->triggerAllLevel());

           manager.setCategory("EQUITY.MARKET.NYSE");                    // (1)
           manager.setCategory("EQUITY.MARKET.NASDAQ");                  // (2)
           manager.setCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");        // (3)
           manager.setCategory("EQUITY.GRAPHICS.MATH.ACKERMANN");        // (4)
           manager.setCategory("EQUITY.GRAPHICS.MATH");                  // (5)
           manager.setCategory("EQUITY");                                // (6)

           const bael_Category *c1, *c3, *c6;

           c1 =  manager.lookupCategory("EQUITY.MARKET.NYSE");
               ASSERT(127 == c1->recordLevel());
               ASSERT( 95 == c1->passLevel());
               ASSERT( 63 == c1->triggerLevel());
               ASSERT( 31 == c1->triggerAllLevel());

           c3 =  manager.lookupCategory("EQUITY.GRAPHICS.MATH.FACTORIAL");
               ASSERT(129 == c3->recordLevel());
               ASSERT( 97 == c3->passLevel());
               ASSERT( 65 == c3->triggerLevel());
               ASSERT( 33 == c3->triggerAllLevel());

           c6 =  manager.lookupCategory("EQUITY");
               ASSERT(128 == c6->recordLevel());
               ASSERT( 96 == c6->passLevel());
               ASSERT( 64 == c6->triggerLevel());
               ASSERT( 32 == c6->triggerAllLevel());

//         return;
//      }

        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'loadParentCategoryThresholdValues'
        //
        // Concerns:
        //   The method under test relies on the services of the 'bael' logger,
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
        BloombergLP::bael_TestObserver testObserver(bsl::cout);
        bael_LoggerManager::initSingleton(&testObserver);
        bael_LoggerManager& manager = bael_LoggerManager::singleton();

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
                T_();             P_(NAME);
                T_();             P_(DELIM);
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
                T_();              P_(NAME);
                T_();              P_(DELIM);
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
