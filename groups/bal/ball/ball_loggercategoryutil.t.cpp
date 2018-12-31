// ball_loggercategoryutil.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_loggercategoryutil.h>

#include <ball_loggermanager.h>
#include <ball_severity.h>
#include <ball_testobserver.h>                  // for testing only

#include <bslim_testutil.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

#include <bsl_new.h>         // placement 'new' syntax
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility consisting of three functions,
// 'addCategoryHierarchically', 'setThresholdLevelsHierarchically', and
// 'setThresholdLevels' (deprecated).  These methods are tested using a
// table-based approach where the following test data consisting of valid and
// invalid threshold level values, category names, and patterns are populated.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJUST".
//-----------------------------------------------------------------------------
// [ 1] static ball::Category *addCategoryHierarchically(*lm, *name);
// [ 2] static int setTLHierarchically(*lm, *name, int, int, int, int);
// [ 3] static int setThresholdLevels(*lm, *pat, int, int, int, int);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::LoggerCategoryUtil Obj;
typedef ball::Category           Cat;
typedef ball::Severity           Sev;
typedef Sev::Level               Level;

const char *DEFAULT_CATEGORY_NAME = "";

//=============================================================================
//                  HELPER FUNCTIONS FOR USAGE EXAMPLE
//-----------------------------------------------------------------------------

static void printCategory(const ball::Category *category)
{
    bsl::cout << "\t[ " << category->categoryName()
              << ", " << category->recordLevel()
              << ", " << category->passLevel()
              << ", " << category->triggerLevel()
              << ", " << category->triggerAllLevel()
              << " ]" << bsl::endl;
}
void printAllCategories()
{

    ball::LoggerManager& lm = ball::LoggerManager::singleton();
    using namespace bdlf::PlaceHolders;
    lm.visitCategories(bdlf::BindUtil::bind(printCategory,  _1));
}

// The function 'dtlCallbackRaw' below sets the values returned through the
// first four arguments of its parameter list to the values of the four
// fields of this 'struct', respectively.

ball::ThresholdAggregate callbackLevels;

void dtlCallbackRaw(int        *recordLevel,
                    int        *passLevel,
                    int        *triggerLevel,
                    int        *triggerAllLevel,
                    const char *categoryName)
    // Callback to be used by the logger manager to obtain new logging
    // threshold levels for a new category that is about to be created.  For
    // the purposes of this test, return values through the specified
    // 'recordLevel', 'passLevel', 'triggerLevel', and 'triggerAllLevel'.
    // Verify that no category with the specified 'categoryName' exists.  The
    // behavior is undefined if this method is called when the logger manager
    // singleton is not initialized.
{
    ASSERT(0 == ball::LoggerManager::singleton().lookupCategory(categoryName));

    *recordLevel     = callbackLevels.recordLevel();
    *passLevel       = callbackLevels.passLevel();
    *triggerLevel    = callbackLevels.triggerLevel();
    *triggerAllLevel = callbackLevels.triggerAllLevel();
}

ball::LoggerManager::DefaultThresholdLevelsCallback dtlCallback(
                                                              &dtlCallbackRaw);
    // The method 'setDefaultThresholdLevelsCallback' won't take just a
    // function ptr, it needs a pointer to this 'bsl::function' type.

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

    BloombergLP::ball::TestObserver  testObserver(&bsl::cout);
    BloombergLP::ball::TestObserver *TO = &testObserver;

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
        //   comment characters, and adjust the line lengths.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example" << endl
                                  << "=====================" << endl;

        {
            ball::LoggerManager::initSingleton(TO);
            ball::LoggerManager& lm = ball::LoggerManager::singleton() ;
            lm.setDefaultThresholdLevels(191, 95, 63, 31);

            lm.addCategory("EQ", 192, 96, 64, 32);
            lm.addCategory("EQ.MARKET", 193, 97, 65, 33);
            if (veryVerbose) printAllCategories();

            ball::LoggerCategoryUtil
                ::addCategoryHierarchically(&lm, "EQ.MARKET.NYSE");
            if (veryVerbose) printAllCategories();

            ball::LoggerCategoryUtil
                ::setThresholdLevelsHierarchically(&lm,
                                                   "EQ.MARKET",
                                                   194,
                                                   98,
                                                   66,
                                                   34);
            if (veryVerbose) printAllCategories();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING SETTHRESHOLDLEVELS
        //
        // Concerns:
        //   Our concern is that 'setThresholdLevels' behaves correctly with
        //   representative threshold level values (both valid and invalid) and
        //   patterns.
        //
        // Plan:
        //   Initialize the logger manager singleton and populate its category
        //   registry from a tabulated set of names.  First verify that
        //   'setThresholdLevels' has no effect on existing categories when
        //   invalid threshold level values are passed as arguments.  Next
        //   test 'setThresholdLevels' with the cross product of a tabulated
        //   set of patterns and a tabulated set of valid threshold level
        //   values.  Verify using appropriate asserts that the categories that
        //   match a given pattern are indeed modified by 'setThresholdLevels'
        //   whereas the categories that fail to match the pattern are
        //   unaffected.
        //
        // Testing:
        //   static int setThresholdLevels(*lm, *pat, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'setThresholdLevels'" << endl
                                  << "============================" << endl;

        // initialize logger manager
        ball::LoggerManager::initSingleton(TO);
        ball::LoggerManager *LM = &ball::LoggerManager::singleton();

        const Cat& defaultCat = LM->defaultCategory();
        ASSERT(0 == strcmp(DEFAULT_CATEGORY_NAME, defaultCat.categoryName()));
        const int DEFAULT_RECORD_LEVEL     = defaultCat.recordLevel();
        const int DEFAULT_PASS_LEVEL       = defaultCat.passLevel();
        const int DEFAULT_TRIGGER_LEVEL    = defaultCat.triggerLevel();
        const int DEFAULT_TRIGGERALL_LEVEL = defaultCat.triggerAllLevel();

        const char *NAME[] = {                                        // ADJUST
            DEFAULT_CATEGORY_NAME,
            "x",
            "*",
            "xy",
        };
        const int NUM_NAME = sizeof NAME / sizeof *NAME;

        for (int n = 1; n < NUM_NAME; ++n) {  // skip *Default* *Category*
            const Cat *p = LM->addCategory(NAME[n],
                                           DEFAULT_RECORD_LEVEL,
                                           DEFAULT_PASS_LEVEL,
                                           DEFAULT_TRIGGER_LEVEL,
                                           DEFAULT_TRIGGERALL_LEVEL);
            ASSERT(p);
        }

        if (verbose)
            cout << "Testing with invalid threshold levels." << endl;
        {
            const char *PAT[] = {                                     // ADJUST
                "",
                "x",
                "*",
                "x*",
            };
            const int NUM_PAT = sizeof PAT / sizeof *PAT;

            static const struct {
                int d_line;             // line number
                int d_recordLevel;      // record level
                int d_passLevel;        // pass level
                int d_triggerLevel;     // trigger level
                int d_triggerAllLevel;  // trigger all level
            } DATA[] = {
                // line    record    pass     trigger  triggerAll
                // no.     level     level     level     level
                // ----    ------    ------    ------    -----        // ADJUST
                {  L_,     256,      0,        0,        0,                  },
                {  L_,     0,        256,      0,        0,                  },
                {  L_,     0,        0,        256,      0,                  },
                {  L_,     0,        0,        0,        256,                },
                {  L_,     256,      256,      256,      256,                },
                {  L_,     INT_MAX,  0,        0,        0,                  },
                {  L_,     0,        INT_MAX,  0,        0,                  },
                {  L_,     0,        0,        INT_MAX,  0,                  },
                {  L_,     0,        0,        0,        INT_MAX,            },
                {  L_,     INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX,            },
                {  L_,     -1,       0,        0,        0,                  },
                {  L_,     0,        -1,       0,        0,                  },
                {  L_,     0,        0,        -1,       0,                  },
                {  L_,     0,        0,        0,        -1,                 },
                {  L_,     -1,       -1,       -1,       -1,                 },
                {  L_,     INT_MIN,  0,        0,        0,                  },
                {  L_,     0,        INT_MIN,  0,        0,                  },
                {  L_,     0,        0,        INT_MIN,  0,                  },
                {  L_,     0,        0,        0,        INT_MIN,            },
                {  L_,     INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE             = DATA[ti].d_line;
                const int RECORD_LEVEL     = DATA[ti].d_recordLevel;
                const int PASS_LEVEL       = DATA[ti].d_passLevel;
                const int TRIGGER_LEVEL    = DATA[ti].d_triggerLevel;
                const int TRIGGERALL_LEVEL = DATA[ti].d_triggerAllLevel;

                if (veryVeryVerbose) {
                    T_; T_;
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int pat = 0; pat < NUM_PAT; ++pat) {
                    const int r =
                      Obj::setThresholdLevels(LM,
                                              PAT[pat],
                                              RECORD_LEVEL,
                                              PASS_LEVEL,
                                              TRIGGER_LEVEL,
                                              TRIGGERALL_LEVEL);
                    LOOP_ASSERT(LINE, 0 > r);
                }

                for (int n = 0; n < NUM_NAME; ++n) {
                     const Cat *p = LM->lookupCategory(NAME[n]);
                     ASSERT(DEFAULT_RECORD_LEVEL     == p->recordLevel());
                     ASSERT(DEFAULT_PASS_LEVEL       == p->passLevel());
                     ASSERT(DEFAULT_TRIGGER_LEVEL    == p->triggerLevel());
                     ASSERT(DEFAULT_TRIGGERALL_LEVEL == p->triggerAllLevel());
                }
            }
        }

        if (verbose)
            cout << "Testing with valid threshold levels." << endl;
        {
            const int SZ = NUM_NAME;

            static const struct {
                int         d_line;           // line number
                const char *d_pat_p;          // pattern
                int         d_numMatch;       // expected number of matches
                int         d_matchMask[SZ];  // d_matchMask[i] == 1 if
                                              // NAME[i] matches, otherwise 0
            } PATDATA[] = {
                // line    pattern        # matches    match mask
                // ----    ----------     ---------    -------------- // ADJUST
                {  L_,     "",               1,        { 1, 0, 0, 0 }        },
                {  L_,     "x",              1,        { 0, 1, 0, 0 }        },
                {  L_,     "*",              4,        { 1, 1, 1, 1 }        },
                {  L_,     "*x",             0,        { 0, 0, 0, 0 }        },
                {  L_,     "x*",             2,        { 0, 1, 0, 1 }        },
                {  L_,     "z*",             0,        { 0, 0, 0, 0 }        },
                {  L_,     "x*y",            0,        { 0, 0, 0, 0 }        },
                {  L_,     "xy*",            1,        { 0, 0, 0, 1 }        },
            };
            const int NUM_PATDATA = sizeof PATDATA / sizeof *PATDATA;

            static const struct {
                int d_line;             // line number
                int d_recordLevel;      // record level
                int d_passLevel;        // pass level
                int d_triggerLevel;     // trigger level
                int d_triggerAllLevel;  // trigger all level
            } DATA[] = {
                // line    record    pass     trigger  triggerAll
                // no.     level     level     level     level
                // ----    ------    ------    ------    -----        // ADJUST
                {  L_,     0,        0,        0,        0                   },
                {  L_,     1,        0,        0,        0                   },
                {  L_,     0,        1,        0,        0                   },
                {  L_,     0,        0,        1,        0                   },
                {  L_,     0,        0,        0,        1                   },
                {  L_,     16,       32,       48,       64                  },
                {  L_,     64,       48,       32,       16                  },
                {  L_,     16,       32,       64,       48                  },
                {  L_,     16,       48,       32,       64                  },
                {  L_,     32,       16,       48,       64                  },
                {  L_,     255,      0,        0,        0                   },
                {  L_,     0,        255,      0,        0                   },
                {  L_,     0,        0,        255,      0                   },
                {  L_,     0,        0,        0,        255                 },
                {  L_,     255,      255,      255,      255                 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE             = DATA[ti].d_line;
                const int RECORD_LEVEL     = DATA[ti].d_recordLevel;
                const int PASS_LEVEL       = DATA[ti].d_passLevel;
                const int TRIGGER_LEVEL    = DATA[ti].d_triggerLevel;
                const int TRIGGERALL_LEVEL = DATA[ti].d_triggerAllLevel;

                if (veryVeryVerbose) {
                    T_; T_;
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int pat = 0; pat < NUM_PATDATA; ++pat) {
                    const int   PATLINE = PATDATA[pat].d_line;
                    const char *PAT     = PATDATA[pat].d_pat_p;
                    const int   MATCHES = PATDATA[pat].d_numMatch;
                    const int  *MASK    = PATDATA[pat].d_matchMask;

                    if (veryVeryVerbose) {
                        T_; T_; P_(PATLINE); P_(PAT); P(MATCHES);
                    }

                    for (int n = 0; n < NUM_NAME; ++n) {
                         Cat *p = LM->lookupCategory(NAME[n]);
                         const int r = p->setLevels(DEFAULT_RECORD_LEVEL,
                                                    DEFAULT_PASS_LEVEL,
                                                    DEFAULT_TRIGGER_LEVEL,
                                                    DEFAULT_TRIGGERALL_LEVEL);
                         ASSERT(0 == r);
                    }

                    const int r = Obj::setThresholdLevels(LM,
                                                          PAT,
                                                          RECORD_LEVEL,
                                                          PASS_LEVEL,
                                                          TRIGGER_LEVEL,
                                                          TRIGGERALL_LEVEL);
                    LOOP_ASSERT(PATLINE, MATCHES == r);

                    for (int n = 0; n < NUM_NAME; ++n) {
                         const Cat *p = LM->lookupCategory(NAME[n]);
                         if (MASK[n]) {
                             ASSERT(RECORD_LEVEL     == p->recordLevel());
                             ASSERT(PASS_LEVEL       == p->passLevel());
                             ASSERT(TRIGGER_LEVEL    == p->triggerLevel());
                             ASSERT(TRIGGERALL_LEVEL == p->triggerAllLevel());
                         }
                         else {
                             ASSERT(DEFAULT_RECORD_LEVEL
                                                     == p->recordLevel());
                             ASSERT(DEFAULT_PASS_LEVEL
                                                     == p->passLevel());
                             ASSERT(DEFAULT_TRIGGER_LEVEL
                                                     == p->triggerLevel());
                             ASSERT(DEFAULT_TRIGGERALL_LEVEL
                                                     == p->triggerAllLevel());
                         }
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING SETTHRESHOLDLEVELSHIERARCHICALLY
        //
        // Concerns:
        //   Our concern is that 'setThresholdLevelsHierarchically' behaves
        //   correctly with representative threshold level values (both valid
        //   and invalid) and should adjust threshold levels only for those
        //   prefix-matching categories without affecting others.
        //
        // Plan:
        //   Initialize the logger manager singleton and populate its category
        //   registry from a tabulated set of names.  First verify that
        //   'setThresholdLevelsHierarchically' has no effect on existing
        //   categories when invalid threshold level values are passed as
        //   arguments.  Next test 'setThresholdLevelsHierarchically' with the
        //   cross product of a tabulated set of category names and a
        //   tabulated set of valid threshold level values.  Verify using
        //   appropriate asserts that the categories that match a given name
        //   prefix are indeed modified by 'setThresholdLevels' whereas the
        //   categories that fail to match the name prefix are unaffected.
        //
        // Testing:
        //   static int setTLHierarchically(*lm, *cn, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'setThresholdLevelsHierarchically'"
                          << endl
                          << "=========================================="
                          << endl;

        // initialize logger manager
        ball::LoggerManager::initSingleton(TO);
        ball::LoggerManager *LM = &ball::LoggerManager::singleton();

        const Cat& defaultCat = LM->defaultCategory();
        ASSERT(0 == strcmp(DEFAULT_CATEGORY_NAME, defaultCat.categoryName()));
        const int DEFAULT_RECORD_LEVEL     = defaultCat.recordLevel();
        const int DEFAULT_PASS_LEVEL       = defaultCat.passLevel();
        const int DEFAULT_TRIGGER_LEVEL    = defaultCat.triggerLevel();
        const int DEFAULT_TRIGGERALL_LEVEL = defaultCat.triggerAllLevel();

        const char *NAME[] = {                                        // ADJUST
            DEFAULT_CATEGORY_NAME,
            "x",
            "xy",
            "x*",
            "y",
            "yx",
        };
        const int NUM_NAME = sizeof NAME / sizeof *NAME;

        for (int n = 1; n < NUM_NAME; ++n) {  // skip *Default* *Category*
            const Cat *p = LM->addCategory(NAME[n],
                                           DEFAULT_RECORD_LEVEL,
                                           DEFAULT_PASS_LEVEL,
                                           DEFAULT_TRIGGER_LEVEL,
                                           DEFAULT_TRIGGERALL_LEVEL);
            ASSERT(p);
        }

        if (verbose)
            cout << "Testing with invalid threshold levels." << endl;
        {
            const char *CN[] = {                                      // ADJUST
                "",
                "x",
                "*",
                "x*",
                "xy",
                "y",
            };
            const int NUM_CN = sizeof CN / sizeof *CN;

            static const struct {
                int d_line;             // line number
                int d_recordLevel;      // record level
                int d_passLevel;        // pass level
                int d_triggerLevel;     // trigger level
                int d_triggerAllLevel;  // trigger all level
            } DATA[] = {
                // line    record    pass     trigger  triggerAll
                // no.     level     level     level     level
                // ----    ------    ------    ------    -----        // ADJUST
                {  L_,     256,      0,        0,        0,                  },
                {  L_,     0,        256,      0,        0,                  },
                {  L_,     0,        0,        256,      0,                  },
                {  L_,     0,        0,        0,        256,                },
                {  L_,     256,      256,      256,      256,                },
                {  L_,     INT_MAX,  0,        0,        0,                  },
                {  L_,     0,        INT_MAX,  0,        0,                  },
                {  L_,     0,        0,        INT_MAX,  0,                  },
                {  L_,     0,        0,        0,        INT_MAX,            },
                {  L_,     INT_MAX,  INT_MAX,  INT_MAX,  INT_MAX,            },
                {  L_,     -1,       0,        0,        0,                  },
                {  L_,     0,        -1,       0,        0,                  },
                {  L_,     0,        0,        -1,       0,                  },
                {  L_,     0,        0,        0,        -1,                 },
                {  L_,     -1,       -1,       -1,       -1,                 },
                {  L_,     INT_MIN,  0,        0,        0,                  },
                {  L_,     0,        INT_MIN,  0,        0,                  },
                {  L_,     0,        0,        INT_MIN,  0,                  },
                {  L_,     0,        0,        0,        INT_MIN,            },
                {  L_,     INT_MIN,  INT_MIN,  INT_MIN,  INT_MIN,            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE             = DATA[ti].d_line;
                const int RECORD_LEVEL     = DATA[ti].d_recordLevel;
                const int PASS_LEVEL       = DATA[ti].d_passLevel;
                const int TRIGGER_LEVEL    = DATA[ti].d_triggerLevel;
                const int TRIGGERALL_LEVEL = DATA[ti].d_triggerAllLevel;

                if (veryVeryVerbose) {
                    T_; T_;
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int cn = 0; cn < NUM_CN; ++cn) {
                    const int r = Obj
                        ::setThresholdLevelsHierarchically(LM,
                                                           CN[cn],
                                                           RECORD_LEVEL,
                                                           PASS_LEVEL,
                                                           TRIGGER_LEVEL,
                                                           TRIGGERALL_LEVEL);
                    LOOP_ASSERT(LINE, 0 > r);
                }

                for (int n = 0; n < NUM_NAME; ++n) {
                     const Cat *p = LM->lookupCategory(NAME[n]);
                     ASSERT(DEFAULT_RECORD_LEVEL     == p->recordLevel());
                     ASSERT(DEFAULT_PASS_LEVEL       == p->passLevel());
                     ASSERT(DEFAULT_TRIGGER_LEVEL    == p->triggerLevel());
                     ASSERT(DEFAULT_TRIGGERALL_LEVEL == p->triggerAllLevel());
                }

                ASSERT(NUM_NAME == LM->numCategories());
            }
        }

        if (verbose)
            cout << "Testing with valid threshold levels." << endl;
        {
            const int SZ = NUM_NAME;

            static const struct {
                int         d_line;           // line number
                const char *d_cn;             // category name prefix
                int         d_matchMask[SZ];  // d_matchMask[i] == 1 if
                                              // NAME[i] matches, otherwise 0
            } CNDATA[] = {
                // line    category
                // no.     name prefix      match mask
                // ----    -----------    --------------------- // ADJUST
                {  L_,     "",           { 1, 1, 1, 1, 1, 1 }        },
                {  L_,     "x",          { 0, 1, 1, 1, 0, 0 }        },
                {  L_,     "xy",         { 0, 0, 1, 0, 0, 0 }        },
                {  L_,     "xyz",        { 0, 0, 0, 0, 0, 0 }        },
                {  L_,     "x*",         { 0, 0, 0, 1, 0, 0 }        },
                {  L_,     "*",          { 0, 0, 0, 0, 0, 0 }        },
                {  L_,     "y",          { 0, 0, 0, 0, 1, 1 }        },
                {  L_,     "yx",         { 0, 0, 0, 0, 0, 1 }        },
                {  L_,     "y*",         { 0, 0, 0, 0, 0, 0 }        },
            };
            const int NUM_CNDATA = sizeof CNDATA / sizeof *CNDATA;

            static const struct {
                int d_line;             // line number
                int d_recordLevel;      // record level
                int d_passLevel;        // pass level
                int d_triggerLevel;     // trigger level
                int d_triggerAllLevel;  // trigger all level
            } DATA[] = {
                // line    record    pass     trigger  triggerAll
                // no.     level     level     level     level
                // ----    ------    ------    ------    -----        // ADJUST
                {  L_,     0,        0,        0,        0                   },
                {  L_,     1,        0,        0,        0                   },
                {  L_,     0,        1,        0,        0                   },
                {  L_,     0,        0,        1,        0                   },
                {  L_,     0,        0,        0,        1                   },
                {  L_,     16,       32,       48,       64                  },
                {  L_,     64,       48,       32,       16                  },
                {  L_,     16,       32,       64,       48                  },
                {  L_,     16,       48,       32,       64                  },
                {  L_,     32,       16,       48,       64                  },
                {  L_,     255,      0,        0,        0                   },
                {  L_,     0,        255,      0,        0                   },
                {  L_,     0,        0,        255,      0                   },
                {  L_,     0,        0,        0,        255                 },
                {  L_,     255,      255,      255,      255                 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE             = DATA[ti].d_line;
                const int RECORD_LEVEL     = DATA[ti].d_recordLevel;
                const int PASS_LEVEL       = DATA[ti].d_passLevel;
                const int TRIGGER_LEVEL    = DATA[ti].d_triggerLevel;
                const int TRIGGERALL_LEVEL = DATA[ti].d_triggerAllLevel;

                if (veryVeryVerbose) {
                    T_; T_;
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int cn = 0; cn < NUM_CNDATA; ++cn) {
                    const int   CNLINE  = CNDATA[cn].d_line;
                    const char *CN      = CNDATA[cn].d_cn;
                    const int  *MASK    = CNDATA[cn].d_matchMask;

                    if (veryVeryVerbose) { T_; T_; P_(CNLINE); P_(CN); }

                    for (int n = 0; n < NUM_NAME; ++n) {
                         Cat *p = LM->lookupCategory(NAME[n]);
                         const int r = p->setLevels(DEFAULT_RECORD_LEVEL,
                                                    DEFAULT_PASS_LEVEL,
                                                    DEFAULT_TRIGGER_LEVEL,
                                                    DEFAULT_TRIGGERALL_LEVEL);
                         ASSERT(0 == r);
                    }

                    const int r =
                      Obj::setThresholdLevelsHierarchically(LM,
                                                            CN,
                                                            RECORD_LEVEL,
                                                            PASS_LEVEL,
                                                            TRIGGER_LEVEL,
                                                            TRIGGERALL_LEVEL);
                    int MATCHES = 0;
                    for (int n = 0; n < NUM_NAME; ++n) {
                         const Cat *p = LM->lookupCategory(NAME[n]);
                         if (MASK[n]) {
                             ASSERT(RECORD_LEVEL     == p->recordLevel());
                             ASSERT(PASS_LEVEL       == p->passLevel());
                             ASSERT(TRIGGER_LEVEL    == p->triggerLevel());
                             ASSERT(TRIGGERALL_LEVEL == p->triggerAllLevel());
                             MATCHES ++;
                         }
                         else {
                             ASSERT(DEFAULT_RECORD_LEVEL
                                                     == p->recordLevel());
                             ASSERT(DEFAULT_PASS_LEVEL
                                                     == p->passLevel());
                             ASSERT(DEFAULT_TRIGGER_LEVEL
                                                     == p->triggerLevel());
                             ASSERT(DEFAULT_TRIGGERALL_LEVEL
                                                     == p->triggerAllLevel());
                         }
                    }
                    LOOP_ASSERT(CNLINE, MATCHES == r);
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING ADDCATEGORYHIERARCHICALLY
        //
        // Concerns:
        //   Our concern is that 'addCategoryHierarchically' behaves correctly
        //   with representative threshold level values (both valid and
        //   invalid), adds the category with the correct threshold levels or
        //   has no effect if the specified category already exists.
        //
        // Plan:
        // Initialize the logger manager singleton and populate its category
        // registry with the cross product of a tabulated set of category
        // names and a tabulated set of different but valid threshold levels.
        // First verify that 'addCategoryHierarchically' has no effect on
        // existing categories when the category with the specified name
        // already exists.  Next test 'addCategoryHierarchically' by adding one
        // by one a tabulated set of category names that are not present in
        // the logger manager.  Verify using appropriate asserts that the
        // categories being added always inherit threshold levels from the
        // longest prefix categories and that when doing so no existing
        // categories are affected.
        //
        // Testing:
        //   static int addCategoryHierarchically(*lm, *cn);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'addCategoryHierarchically'"
                          << endl
                          << "==================================="
                          << endl;

        for (int ti = 0; ti < 2; ++ti) {
            const bool USE_CALLBACK = 0 == ti;

            int DRL  = 10, CBDRL  = DRL;
            int DPL  = 11, CBDPL  = DPL;
            int DTL  = 12, CBDTL  = DTL;
            int DTAL = 13, CBDTAL = DTAL;

            // initialize the logger manager
            ball::LoggerManagerConfiguration mLMC;
            ASSERT(0 == mLMC.setDefaultThresholdLevelsIfValid(DRL,
                                                              DPL,
                                                              DTL,
                                                              DTAL));
            ball::LoggerManagerScopedGuard lmGuard(mLMC);
            ball::LoggerManager *LM = &ball::LoggerManager::singleton();

            if (USE_CALLBACK) {
                CBDRL  += 10;
                CBDPL  += 10;
                CBDTL  += 10;
                CBDTAL += 10;

                callbackLevels.setLevels(CBDRL, CBDPL, CBDTL, CBDTAL);

                LM->setDefaultThresholdLevelsCallback(&dtlCallback);
            }

            if (veryVerbose) {
                cout << endl;    P_(USE_CALLBACK);    P_(DPL);    P(CBDPL);
            }

            const Cat& defaultCat = LM->defaultCategory();
            ASSERT(0 == strcmp(DEFAULT_CATEGORY_NAME,
                               defaultCat.categoryName()));

            const struct Data {  // Not static
                int         d_line;             // line number
                const char *d_name;             // category name
                int         d_recordLevel;      // record level
                int         d_passLevel;        // pass level
                int         d_triggerLevel;     // trigger level
                int         d_triggerAllLevel;  // trigger all level
                int         d_populated;        // used to populate the
                                                // registry
                int         d_baseIdx;          // index of base category (-1
                                                // for none)
            } DATA[] = {                                             // ADJUST
                // line  cat      record pass  trigger    tAll
                // no.   name     level  level  level    level pop base
                // ----  -----    ------ ------ ------   ----- --- ------
                {  L_,      "",    DRL,    DPL,    DTL,   DTAL,  1,  -1 },// 0
                {  L_,     "x",      5,      6,      7,      8,  1,  -1 },// 1
                {  L_,    "x*",      9,     10,     11,     12,  1,  -1 },// 2
                {  L_,    "xy",     13,     14,     15,     16,  1,  -1 },// 3

                {  L_,     "y",     17,     18,     19,     20,  1,  -1 },// 4
                {  L_,  "meow",  CBDRL,  CBDPL,  CBDTL, CBDTAL,  1,  -1 },// 5
                {  L_,    "yx",     21,     22,     23,     24,  1,  -1 },// 6
                {  L_,   "yyy",     25,     26,     27,     28,  1,  -1 },// 7
                // Entries above will be used to populate the registry.

                // Entries below will be added using
                // 'addCategoryHierarchically'.
                {  L_,   "xyz",     -1,     -1,     -1,     -1,  0,   3 },// 8
                {  L_,    "yy",     -1,     -1,     -1,     -1,  0,   4 },// 9
                {  L_,     "z",  CBDRL,  CBDPL,  CBDTL, CBDTAL,  0,  -1 },//10
                {  L_,  "woof",  CBDRL,  CBDPL,  CBDTL, CBDTAL,  0,  -1 },//11
                {  L_,  "yxaa",     -1,     -1,     -1,     -1,  0,   6 },//12
                {  L_,  "x*__",     -1,     -1,     -1,     -1,  0,   2 },//13
                {  L_,"meower",     -1,     -1,     -1,     -1,  0,   5 },//14
                {  L_,  "xyz*",     -1,     -1,     -1,     -1,  0,   8 },//15
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // populate the category registry in the logger manager
            for (int n = 0; n < NUM_DATA && DATA[n].d_populated; ++n) {
                bool isDefault = !strcmp("", DATA[n].d_name);

                const Cat *p = LM->addCategory(DATA[n].d_name,
                                               DATA[n].d_recordLevel,
                                               DATA[n].d_passLevel,
                                               DATA[n].d_triggerLevel,
                                               DATA[n].d_triggerAllLevel);
                LOOP_ASSERT(DATA[n].d_line, isDefault ? !p : !!p);
            }

            if (verbose) cout << "Testing with mixed category names.\n";
            {
                for (int nn = 0; nn < NUM_DATA; ++nn) {
                    const Data& ndata = DATA[nn];
                    const bool  npop  = ndata.d_populated;
                    const char *nname = ndata.d_name;

                    if (veryVerbose) P(nname);

                    const Cat *np = Obj::addCategoryHierarchically(LM, nname);

                    ASSERTV(nname, npop, !np, npop == !np);

                    if (npop) {
                        continue;
                    }

                    for (int ii = 0; ii <= nn; ++ii) {
                        const Data&  idata    = DATA[ii];
                        const char  *iname    = idata.d_name;
                        const int    ibaseIdx = idata.d_baseIdx;
                        const char  *bname    = (0 <= ibaseIdx)
                                              ? DATA[ibaseIdx].d_name
                                              : "(null)";
                        int ebaseIdx = ii, next;
                        while (0 <= (next = DATA[ebaseIdx].d_baseIdx)) {
                             ebaseIdx = next;
                        }
                        const Data&  edata    = DATA[ebaseIdx];
                        const char  *ename    = edata.d_name;

                        if (veryVeryVerbose) { P_(iname); P_(bname); P(ename);}

                        const Cat *icat = LM->lookupCategory(iname);

                        ASSERTV(ii, (0 == ii) == (&defaultCat == icat));
                        ASSERTV(ii, np, (ii == nn) == (np == icat));

                        ASSERTV(iname, bname, ename, icat);
                        ASSERTV(iname, bname, ename, edata.d_recordLevel ==
                                                      icat->recordLevel());
                        ASSERTV(iname, bname, ename, edata.d_passLevel ==
                                                      icat->passLevel());
                        ASSERTV(iname, bname, ename, edata.d_triggerLevel ==
                                                      icat->triggerLevel());
                        ASSERTV(iname, bname, ename, edata.d_triggerAllLevel ==
                                                      icat->triggerAllLevel());
                    }
                }
            }
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
