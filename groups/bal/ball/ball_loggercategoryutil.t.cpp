// ball_loggercategoryutil.t.cpp                                      -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <ball_loggercategoryutil.h>

#include <ball_loggermanager.h>
#include <ball_testobserver.h>                  // for testing only

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
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a utility consisting of three functions,
// 'addCategoryHierarchically', 'setThresholdLevelsHierarchically', and
// 'setThresholdLevels' (deprecated).  These methods are tested using a
// table-based approach where the following test data consisting of valid and
// invalid threshold level values, category names, and regular expressions are
// populated.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJUST".
//-----------------------------------------------------------------------------
// [ 1] static ball::Category *addCategoryHierarchically(*lm, *name);
// [ 2] static int setTLHierarchically(*lm, *name, int, int, int, int);
// [ 3] static int setThresholdLevels(*lm, *re, int, int, int, int);
//-----------------------------------------------------------------------------
// [ 4] USAGE EXAMPLE
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

typedef ball::LoggerCategoryUtil Obj;
typedef ball::Category           Cat;

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

    BloombergLP::ball::TestObserver  testObserver(bsl::cout);
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
        //   representative threshold level values (both valid and invalid)
        //   and regular expressions.
        //
        // Plan:
        //   Initialize the logger manager singleton and populate its category
        //   registry from a tabulated set of names.  First verify that
        //   'setThresholdLevels' has no effect on existing categories when
        //   invalid threshold level values are passed as arguments.  Next
        //   test 'setThresholdLevels' with the cross product of a tabulated
        //   set of regular expressions and a tabulated set of valid threshold
        //   level values.  Verify using appropriate asserts that the
        //   categories that match a given regular expression are indeed
        //   modified by 'setThresholdLevels' whereas the categories that fail
        //   to match the regular expression are unaffected.
        //
        // Testing:
        //   static int setThresholdLevels(*lm, *re, int, int, int, int);
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
            const char *RE[] = {                                      // ADJUST
                "",
                "x",
                "*",
                "x*",
            };
            const int NUM_RE = sizeof RE / sizeof *RE;

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
                    T_(); T_();
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int re = 0; re < NUM_RE; ++re) {
                    const int r =
                      Obj::setThresholdLevels(LM,
                                              RE[re],
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
                const char *d_re;             // regular expression
                int         d_numMatch;       // expected number of matches
                int         d_matchMask[SZ];  // d_matchMask[i] == 1 if
                                              // NAME[i] matches, otherwise 0
            } REDATA[] = {
                // line    regular
                // no.     expression     # matches    match mask
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
            const int NUM_REDATA = sizeof REDATA / sizeof *REDATA;

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
                    T_(); T_();
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int re = 0; re < NUM_REDATA; ++re) {
                    const int   RELINE  = REDATA[re].d_line;
                    const char *RE      = REDATA[re].d_re;
                    const int   MATCHES = REDATA[re].d_numMatch;
                    const int  *MASK    = REDATA[re].d_matchMask;

                    if (veryVeryVerbose) {
                        T_(); T_(); P_(RELINE); P_(RE); P(MATCHES);
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
                                                          RE,
                                                          RECORD_LEVEL,
                                                          PASS_LEVEL,
                                                          TRIGGER_LEVEL,
                                                          TRIGGERALL_LEVEL);
                    LOOP_ASSERT(RELINE, MATCHES == r);

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
                    T_(); T_();
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
                    T_(); T_();
                    P_(LINE); P_(RECORD_LEVEL); P_(PASS_LEVEL);
                    P_(TRIGGER_LEVEL); P(TRIGGERALL_LEVEL);
                }

                for (int cn = 0; cn < NUM_CNDATA; ++cn) {
                    const int   CNLINE  = CNDATA[cn].d_line;
                    const char *CN      = CNDATA[cn].d_cn;
                    const int  *MASK    = CNDATA[cn].d_matchMask;

                    if (veryVeryVerbose) { T_(); T_(); P_(CNLINE); P_(CN); }

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

        // initialize the logger manager
        ball::LoggerManager::initSingleton(TO);
        ball::LoggerManager *LM = &ball::LoggerManager::singleton();

        const Cat& defaultCat = LM->defaultCategory();
        ASSERT(0 == strcmp(DEFAULT_CATEGORY_NAME, defaultCat.categoryName()));
        const int DRL  = defaultCat.recordLevel();
        const int DPL  = defaultCat.passLevel();
        const int DTL  = defaultCat.triggerLevel();
        const int DTAL = defaultCat.triggerAllLevel();

        static const struct {
            int         d_line;             // line number
            const char *d_name;             // category name
            int         d_recordLevel;      // record level
            int         d_passLevel;        // pass level
            int         d_triggerLevel;     // trigger level
            int         d_triggerAllLevel;  // trigger all level
            int         d_populated;        // used to populate the registry
        } DATA[] = {                                                 // ADJUST
                // line  cat      record pass  trigger tAll
                // no.   name     level  level  level  level populated
                // ----  -----    ------ ------ ------ ----- ---------
                {  L_,      "",   DRL,   DPL,   DTL,   DTAL,       1 },
                {  L_,     "x",     5,     6,     7,     8,        1 },
                {  L_,    "x*",     9,    10,    11,    12,        1 },
                {  L_,    "xy",    13,    14,    15,    16,        1 },
                {  L_,     "y",    17,    18,    19,    20,        1 },
                {  L_,    "yx",    21,    22,    23,    24,        1 },
                {  L_,   "yyy",    25,    26,    27,    28,        1 },
                // Entries above will be used to populate the registry.
                // entries below will be added using addCategoryHierarchically.
                {  L_,   "xyz",    13,    14,    15,    16,        0 },
                {  L_,    "yy",    17,    18,    19,    20,        0 },
                {  L_,     "z",   DRL,   DPL,   DTL,   DTAL,       0 },
                {  L_,  "xyz*",    13,    14,    15,    16,        0 },
            };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // populate the category registry in the logger manager
        for (int n = 1; n < NUM_DATA && DATA[n].d_populated; ++n) {
            const Cat *p = LM->addCategory(DATA[n].d_name,
                                           DATA[n].d_recordLevel,
                                           DATA[n].d_passLevel,
                                           DATA[n].d_triggerLevel,
                                           DATA[n].d_triggerAllLevel);
            LOOP_ASSERT(DATA[n].d_line, p);

            LM->lookupCategory("xy");
        }

        if (verbose)
            cout << "Testing with existing category names." << endl;
        {
            for (int n = 0; n < NUM_DATA && DATA[n].d_populated; ++n) {
                const Cat *p = Obj::addCategoryHierarchically(LM,
                                                              DATA[n].d_name);
                LOOP_ASSERT(DATA[n].d_line, 0 == p);
            }
        }

        if (verbose)
            cout << "Testing with new category names." << endl;
        {
            int n;

            // find the first entry whose 'd_populated' flag isn't set
            for (n = 0; n < NUM_DATA && DATA[n].d_populated; ++n) {
            }

            for (; n < NUM_DATA; ++n) {
                const Cat *p = Obj::addCategoryHierarchically(LM,
                                                              DATA[n].d_name);
                LOOP_ASSERT(DATA[n].d_line, p);

                for (int i = 0; i <= n; ++i) {
                    const Cat *cat = LM->lookupCategory(DATA[i].d_name);
                    LOOP2_ASSERT(DATA[n].d_line, DATA[i].d_line, cat);
                    LOOP2_ASSERT(DATA[n].d_line,
                                 DATA[i].d_line,
                                 DATA[i].d_recordLevel == cat->recordLevel());
                    LOOP2_ASSERT(DATA[n].d_line,
                                 DATA[i].d_line,
                                 DATA[i].d_passLevel == cat->passLevel());
                    LOOP2_ASSERT(DATA[n].d_line,
                                 DATA[i].d_line,
                                 DATA[i].d_triggerLevel ==
                                     cat->triggerLevel());
                    LOOP2_ASSERT(DATA[n].d_line,
                                 DATA[i].d_line,
                                 DATA[i].d_triggerAllLevel ==
                                     cat->triggerAllLevel());
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
