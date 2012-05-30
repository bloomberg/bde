// baetzo_timezoneidutil.t.cpp                                        -*-C++-*-
#include <baetzo_timezoneidutil.h>

#include <baetzo_timezoneutil.h>
#include <baetzo_dstpolicy.h>

#include <baet_localdatetime.h>
#include <bdet_date.h>
#include <bdet_datetime.h>

#include <bsl_iostream.h>
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provide a utility for mapping time zone identifiers
// between Olson and Windows naming systems.  The methods are implemented using
// standard alorithms on two sorted static tables: one mapping Windows timezone
// identifiers to Olson timezone identifiers; the other mapping Olson timezone
// identifiers to Windows timezone identifiers.  These tables were
// semi-mechanically generated from a mapping table obtained from
// 'unicode.org'.  As a check on that transformation, the tests here use as a
// reference, a separate copy of the 'unicode.org' table, that has been
// minimally transformed into a C-compatible table, 'DEFAULT_DATA'.  Each row
// and column has the same data, in the same order as the original
// 'unicode.org'.  The only changes has be the conversion of 'html' tags to
// double quotes, commas, etc.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] getTimeZoneIdFromWindowsTimeZoneId(const char **, const char *);
// [ 3] getWindowsTimeZoneIdFromTimeZoneId(const char **, const char *);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
//
// ============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_  cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baetzo_TimeZoneIdUtil      Obj;

typedef struct DefaultDataRow {
    int         d_line;
    const char *d_windowsId;
    const char *d_region;
    const char *d_olsonId;
} DefaultDataRow;

static const DefaultDataRow DEFAULT_DATA[] = {
      //  +--------------------------------+--------+---------------------+
      //  |Windows Time Zone Identifer     | Region | Olson Identifier    |
      //  +--------------------------------+--------+---------------------+
    { L_,       "AUS Central Standard Time",  "001", "Australia/Darwin"    },
    { L_,       "AUS Eastern Standard Time",  "001", "Australia/Sydney"    },
    { L_,       "Afghanistan Standard Time",  "001", "Asia/Kabul"          },
    { L_,           "Alaskan Standard Time",  "001", "America/Anchorage"   },
    { L_,              "Arab Standard Time",  "001", "Asia/Riyadh"         },
    { L_,           "Arabian Standard Time",  "001", "Asia/Dubai"          },
    { L_,            "Arabic Standard Time",  "001", "Asia/Baghdad"        },
    { L_,         "Argentina Standard Time",  "001", "America/Buenos_Aires"},
    { L_,          "Atlantic Standard Time",  "001", "America/Halifax"     },
    { L_,        "Azerbaijan Standard Time",  "001", "Asia/Baku"           },
    { L_,            "Azores Standard Time",  "001", "Atlantic/Azores"     },
    { L_,             "Bahia Standard Time",  "001", "America/Bahia"       },
    { L_,        "Bangladesh Standard Time",  "001", "Asia/Dhaka"          },
    { L_,    "Canada Central Standard Time",  "001", "America/Regina"      },
    { L_,        "Cape Verde Standard Time",  "001", "Atlantic/Cape_Verde" },
    { L_,          "Caucasus Standard Time",  "001", "Asia/Yerevan"        },
    { L_,    "Cen. Australia Standard Time",  "001", "Australia/Adelaide"  },
    { L_,   "Central America Standard Time",  "001", "America/Guatemala"   },
    { L_,      "Central Asia Standard Time",  "001", "Asia/Almaty"         },
    { L_, "Central Brazilian Standard Time",  "001", "America/Cuiaba"      },
    { L_,    "Central Europe Standard Time",  "001", "Europe/Budapest"     },
    { L_,  "Central European Standard Time",  "001", "Europe/Warsaw"       },
    { L_,   "Central Pacific Standard Time",  "001", "Pacific/Guadalcanal" },
    { L_,           "Central Standard Time",  "001", "America/Chicago"     },
    { L_,  "Central Standard Time (Mexico)",  "001", "America/Mexico_City" },
    { L_,             "China Standard Time",  "001", "Asia/Shanghai"       },
    { L_,          "Dateline Standard Time",  "001", "Etc/GMT+12"          },
    { L_,         "E. Africa Standard Time",  "001", "Africa/Nairobi"      },
    { L_,      "E. Australia Standard Time",  "001", "Australia/Brisbane"  },
    { L_,         "E. Europe Standard Time",  "001", "Asia/Nicosia"        },
    { L_,  "E. South America Standard Time",  "001", "America/Sao_Paulo"   },
    { L_,           "Eastern Standard Time",  "001", "America/New_York"    },
    { L_,             "Egypt Standard Time",  "001", "Africa/Cairo"        },
    { L_,      "Ekaterinburg Standard Time",  "001", "Asia/Yekaterinburg"  },
    { L_,               "FLE Standard Time",  "001", "Europe/Kiev"         },
    { L_,              "Fiji Standard Time",  "001", "Pacific/Fiji"        },
    { L_,               "GMT Standard Time",  "001", "Europe/London"       },
    { L_,               "GTB Standard Time",  "001", "Europe/Bucharest"    },
    { L_,          "Georgian Standard Time",  "001", "Asia/Tbilisi"        },
    { L_,         "Greenland Standard Time",  "001", "America/Godthab"     },
    { L_,         "Greenwich Standard Time",  "001", "Atlantic/Reykjavik"  },
    { L_,          "Hawaiian Standard Time",  "001", "Pacific/Honolulu"    },
    { L_,             "India Standard Time",  "001", "Asia/Calcutta"       },
    { L_,              "Iran Standard Time",  "001", "Asia/Tehran"         },
    { L_,            "Israel Standard Time",  "001", "Asia/Jerusalem"      },
    { L_,            "Jordan Standard Time",  "001", "Asia/Amman"          },
    { L_,       "Kaliningrad Standard Time",  "001", "Europe/Kaliningrad"  },
    { L_,             "Korea Standard Time",  "001", "Asia/Seoul"          },
    { L_,           "Magadan Standard Time",  "001", "Asia/Magadan"        },
    { L_,         "Mauritius Standard Time",  "001", "Indian/Mauritius"    },
    { L_,       "Middle East Standard Time",  "001", "Asia/Beirut"         },
    { L_,        "Montevideo Standard Time",  "001", "America/Montevideo"  },
    { L_,           "Morocco Standard Time",  "001", "Africa/Casablanca"   },
    { L_,          "Mountain Standard Time",  "001", "America/Denver"      },
    { L_, "Mountain Standard Time (Mexico)",  "001", "America/Chihuahua"   },
    { L_,           "Myanmar Standard Time",  "001", "Asia/Rangoon"        },
    { L_,   "N. Central Asia Standard Time",  "001", "Asia/Novosibirsk"    },
    { L_,           "Namibia Standard Time",  "001", "Africa/Windhoek"     },
    { L_,             "Nepal Standard Time",  "001", "Asia/Katmandu"       },
    { L_,       "New Zealand Standard Time",  "001", "Pacific/Auckland"    },
    { L_,      "Newfoundland Standard Time",  "001", "America/St_Johns"    },
    { L_,   "North Asia East Standard Time",  "001", "Asia/Irkutsk"        },
    { L_,        "North Asia Standard Time",  "001", "Asia/Krasnoyarsk"    },
    { L_,        "Pacific SA Standard Time",  "001", "America/Santiago"    },
    { L_,           "Pacific Standard Time",  "001", "America/Los_Angeles" },
    { L_,  "Pacific Standard Time (Mexico)",  "001", "America/Santa_Isabel"},
    { L_,          "Pakistan Standard Time",  "001", "Asia/Karachi"        },
    { L_,          "Paraguay Standard Time",  "001", "America/Asuncion"    },
    { L_,           "Romance Standard Time",  "001", "Europe/Paris"        },
    { L_,           "Russian Standard Time",  "001", "Europe/Moscow"       },
    { L_,        "SA Eastern Standard Time",  "001", "America/Cayenne"     },
    { L_,        "SA Pacific Standard Time",  "001", "America/Bogota"      },
    { L_,        "SA Western Standard Time",  "001", "America/La_Paz"      },
    { L_,           "SE Asia Standard Time",  "001", "Asia/Bangkok"        },
    { L_,             "Samoa Standard Time",  "001", "Pacific/Apia"        },
    { L_,         "Singapore Standard Time",  "001", "Asia/Singapore"      },
    { L_,      "South Africa Standard Time",  "001", "Africa/Johannesburg" },
    { L_,         "Sri Lanka Standard Time",  "001", "Asia/Colombo"        },
    { L_,             "Syria Standard Time",  "001", "Asia/Damascus"       },
    { L_,            "Taipei Standard Time",  "001", "Asia/Taipei"         },
    { L_,          "Tasmania Standard Time",  "001", "Australia/Hobart"    },
    { L_,             "Tokyo Standard Time",  "001", "Asia/Tokyo"          },
    { L_,             "Tonga Standard Time",  "001", "Pacific/Tongatapu"   },
    { L_,            "Turkey Standard Time",  "001", "Europe/Istanbul"     },
    { L_,        "US Eastern Standard Time",  "001", "America/Indianapolis"},
    { L_,       "US Mountain Standard Time",  "001", "America/Phoenix"     },
    { L_,                             "UTC",  "001", "Etc/GMT"             },
    { L_,                          "UTC+12",  "001", "Etc/GMT-12"          },
    { L_,                          "UTC-02",  "001", "Etc/GMT+2"           },
    { L_,                          "UTC-11",  "001", "Etc/GMT+11"          },
    { L_,       "Ulaanbaatar Standard Time",  "001", "Asia/Ulaanbaatar"    },
    { L_,         "Venezuela Standard Time",  "001", "America/Caracas"     },
    { L_,       "Vladivostok Standard Time",  "001", "Asia/Vladivostok"    },
    { L_,      "W. Australia Standard Time",  "001", "Australia/Perth"     },
    { L_, "W. Central Africa Standard Time",  "001", "Africa/Lagos"        },
    { L_,         "W. Europe Standard Time",  "001", "Europe/Berlin"       },
    { L_,         "West Asia Standard Time",  "001", "Asia/Tashkent"       },
    { L_,      "West Pacific Standard Time",  "001", "Pacific/Port_Moresby"},
    { L_,           "Yakutsk Standard Time",  "001", "Asia/Yakutsk"        }
      //  +--------------------------------+--------+---------------------+
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

//=============================================================================
//                      USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Converting From Windows to Olson Time Zone Idenitifers
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'getTimeZoneIdFromWindowsTimeZoneId' method allows Windows clients to
// use their configured time zone information (from the registry) to create
// the vocabulary types required by BDE libraries.
//
// First, we define our function interface.
//..
    int myLoadLocalDatetime(baet_LocalDatetime   *resultPtr,
                            const char           *timezoneIdFromRegistry,
                            const bdet_Datetime&  localtime);
        // Load, into the specified 'result', the local date-time value (in the
        // (Windows) time zone indicated by the specified
        // 'timezoneIdFromRegistry') corresponding the specifie 'utcTime'.
        // Return 0 on success, and a non-zero value otherwise.
//..
//  Then, we implment our function.
//..
    int myLoadLocalDatetime(baet_LocalDatetime   *resultPtr,
                            const char           *timezoneIdFromRegistry,
                            const bdet_Datetime&  localtime)
    {
        ASSERT(resultPtr);
        ASSERT(timezoneIdFromRegistry);

        const char *timeZoneId;

        int rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
                                                       &timeZoneId,
                                                       timezoneIdFromRegistry);
        if (0 != rc) {
            return -1;                                                // RETURN
        }

#if 1
        return 0;
#else
        return baetzo_TimeZoneUtil::initLocalTime(
                                            resultPtr,
                                            localtime,
                                            timeZoneId,
                                            baetzo_DstPolicy::BAETZO_STANDARD);
#endif
    }
//..
// Notice that the 'dstPolicy' parameters has been set to
// 'baetzo_DstPolicy::BAETZO_STANDARD', because each of the supported Windows
// timezone identifiers is a "Standard Time".

//=============================================================================
//                      GLOBAL CONSTANTS
//-----------------------------------------------------------------------------

// ============================================================================
//                  GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

//=============================================================================
//                                 HELPER FUNCTIONS
//-----------------------------------------------------------------------------

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Finally, the resulting 'bdet_Date' object has wide utility.
//..
    baet_LocalDatetime localDatetime;
    bdet_Datetime      utcDatetime(2012, 5, 28, 22);
    int                rc  = myLoadLocalDatetime(&localDatetime,
                                                 "Ekaterinburg Standard Time",
                                                 utcDatetime);
    ASSERT(0 == rc);
    ASSERT(0 == bsl::strcmp("Asia/Yekaterinburg",
                            localDatetime.timeZoneId().c_str()));

    bdet_Date localDate = localDatetime.datetimeTz().dateTz().localDate();
    bdet_Date yesterday = localDate - 1;
    bdet_Date tomorrow  = localDate + 1;
    // ...
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getWindowsTimeZoneIdFromTimeZoneId'
        //
        // Concerns:
        //: 1 The sorted static table mapping Olson timezone identifiers to
        //:   Windows timezone identifiers is sorted, has a single record for
        //:   each Olson timezone identifier for each entry of the
        //:   'unicode.org' table, and that entry has the correct,
        //:   corresponding Windows timezone identifier.
        //:
        //: 2 The table has no entries other than those described in C-1.
        //:
        //: 3 When given an invalid Olson timezone identifier, the method
        //:   returns a value indicating non-success, and has no other effect.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each Olson timezone identifier the 'DEFAULT_DATA', confirm
        //:   that the method returns success and loads the corresponding
        //:   Windows timezone identifier.  (C-1)
        //:
        //: 2 Compile-time asserts in the implementation check that the number
        //:   of entries of the internal table matches that of 'DEFAULT_DATA'.
        //:   (C-2)
        //:
        //: 3 Use a table-driven test to confirm that for several classes of
        //:   invalid Olson timezone identifiers, the method returns a value
        //:   indicating failure, and that the contents of the given load
        //:   address for the Windows timezone identifier is unchanged.  (C-3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   getWindowsTimeZoneIdFromTimeZoneId(const char **, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "CLASS METHOD 'getWindowsTimeZoneIdFromTimeZoneId'" << endl
                << "=================================================" << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const char *GIVEN_ID    = DATA[ti].d_olsonId;
            const char *EXPECTED_ID = DATA[ti].d_windowsId;

            if (veryVerbose) { T_ P_(LINE) P_(GIVEN_ID) P(EXPECTED_ID) }

            int         rc;
            const char *winId;
            rc = Obj::getWindowsTimeZoneIdFromTimeZoneId(&winId, GIVEN_ID);
            ASSERT(0 == rc);
            ASSERT(0 == bsl::strcmp(EXPECTED_ID, winId));
        }

        if (verbose) cout << "\nFailure Testing." << endl;
        {
             const struct {
                 int         d_line;
                 const char *d_badOlsonId;
             } DATA [] = {
                 // LINE  BAD OLSON ID
                 // ----  ----------------------------------------
                  { L_,   "aBelowLowerBoundofValidInputs"          },
                  { L_,   "America/Mexico_City amidValidInputs"    },
                  { L_,   "ZZZZZZZZZZAboveUpperBoundofValidInputs" }
             };
             const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

             for (int ti = 0; ti < NUM_DATA; ++ti) {
                 const int   LINE     = DATA[ti].d_line;
                 const char *BAD_ID   = DATA[ti].d_badOlsonId;
                 const char *EXP_ADDR = (const char *)&ti;

                 if (veryVerbose) { T_ P_(LINE)
                                       P_(BAD_ID)
                                       P((void *)EXP_ADDR) }

                 const char *winId = EXP_ADDR;
                 int         rc    = Obj::getTimeZoneIdFromWindowsTimeZoneId(
                                                                       &winId,
                                                                       BAD_ID);
                 LOOP2_ASSERT(LINE, BAD_ID,           0        != rc);
                 LOOP2_ASSERT(LINE, (void *)EXP_ADDR, EXP_ADDR == winId);
             }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            const char  *tzId = "America/Mexico_City";
            const char *winId;

            ASSERT_SAFE_PASS(Obj::getWindowsTimeZoneIdFromTimeZoneId(&winId,
                                                                     tzId));
            ASSERT_SAFE_FAIL(Obj::getWindowsTimeZoneIdFromTimeZoneId(0,
                                                                     tzId));
            ASSERT_SAFE_FAIL(Obj::getWindowsTimeZoneIdFromTimeZoneId(&winId,
                                                                     0));
            ASSERT_SAFE_FAIL(Obj::getWindowsTimeZoneIdFromTimeZoneId(0,
                                                                     0));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'getTimeZoneIdFromWindowsTimeZoneId'
        //
        // Concerns:
        //: 1 The sorted static table mapping Windows timezone identifiers to
        //:   Olson timezone identifiers is sorted, has a single record for
        //:   each Windows timezone identifier for each entry of the
        //:   'unicode.org' table, and that entry has the correct,
        //:   corresponding Olson timezone identifier.
        //:
        //: 2 The table has no entries other than those described in C-1.
        //:
        //: 3 When given an invalid Windows timezone identifier, the method
        //:   returns a value indicating non-success, and has no other effect.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each Windows timezone identifier the 'DEFAULT_DATA', confirm
        //:   that the method returns success and loads the corresponding Olson
        //:   timezone identifier.  (C-1)
        //:
        //: 2 Compile-time asserts in the implementation check that the number
        //:   of entries of the internal table matches that of 'DEFAULT_DATA'.
        //:   (C-2)
        //:
        //: 3 Use a table-driven test to confirm that for several classes of
        //:   invalid Windows timezone identifiers, the method returns a value
        //:   indicating failure, and that the contents of the given load
        //:   address for the Olson timezone identifier is unchanged.  (C-3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid argument values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-4)
        //
        // Testing:
        //   getTimeZoneIdFromWindowsTimeZoneId(const char **, const char *);
        // --------------------------------------------------------------------

        if (verbose) cout
                << endl
                << "CLASS METHOD 'getTimeZoneIdFromWindowsTimeZoneId'" << endl
                << "=================================================" << endl;

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const char *GIVEN_ID    = DATA[ti].d_windowsId;
            const char *EXPECTED_ID = DATA[ti].d_olsonId;

            if (veryVerbose) { T_ P_(LINE) P_(GIVEN_ID) P(EXPECTED_ID) }

            int         rc;
            const char *tzId;
            rc = Obj::getTimeZoneIdFromWindowsTimeZoneId(&tzId, GIVEN_ID);
            ASSERT(0 == rc);
            ASSERT(0 == bsl::strcmp(EXPECTED_ID, tzId));
        }

        if (verbose) cout << "\nFailure Testing." << endl;
        {
             const struct {
                 int         d_line;
                 const char *d_badWinId;
             } DATA [] = {
                 // LINE  BAD WINDOWS ID
                 // ----  ------------------------------------------------
                  { L_,   "aBelowLowerBoundofValidInputs"                  },
                  { L_,   "Central Standard Time (Mexico) amidValidInputs" },
                  { L_,   "ZZZZZZZAboveUpperBoundofValidInputs"            }
             };
             const int NUM_DATA = sizeof(DATA)/sizeof(*DATA);

             for (int ti = 0; ti < NUM_DATA; ++ti) {
                 const int   LINE     = DATA[ti].d_line;
                 const char *BAD_ID   = DATA[ti].d_badWinId;
                 const char *EXP_ADDR = (const char *)&ti;

                 if (veryVerbose) { T_ P_(LINE)
                                       P_(BAD_ID)
                                       P((void *)EXP_ADDR) }

                 const char *tzId = EXP_ADDR;
                 int         rc   = Obj::getTimeZoneIdFromWindowsTimeZoneId(
                                                                       &tzId,
                                                                       BAD_ID);
                 LOOP2_ASSERT(LINE, BAD_ID,           0        != rc);
                 LOOP2_ASSERT(LINE, (void *)EXP_ADDR, EXP_ADDR == tzId);
             }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);

            const char *winId = "Central Standard Time (Mexico)";
            const char *tzId;

            ASSERT_SAFE_PASS(Obj::getTimeZoneIdFromWindowsTimeZoneId(&tzId,
                                                                     winId));
            ASSERT_SAFE_FAIL(Obj::getTimeZoneIdFromWindowsTimeZoneId(0,
                                                                     winId));
            ASSERT_SAFE_FAIL(Obj::getTimeZoneIdFromWindowsTimeZoneId(&tzId,
                                                                     0));
            ASSERT_SAFE_FAIL(Obj::getTimeZoneIdFromWindowsTimeZoneId(0,
                                                                     0));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 For each method, check basic functionality by confirming that
        //:   given a valid input, a return code indicating success is returned
        //:   and the expected time identifier is set.
        //:
        //: 2 For each method, check basic functionality by confirming that
        //:   given an invalid input, a return code indicating failure returned
        //:   no time identifer is set.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        int         rc;
        const char *timeZoneId;
        const char *windowsTimeZoneId;

        rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
                                             &timeZoneId,
                                             "Central Standard Time (Mexico)");
        ASSERT(0 == rc);
        ASSERT(0 == bsl::strcmp("America/Mexico_City", timeZoneId));

        rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
                                                        &windowsTimeZoneId,
                                                        "America/Mexico_City");
        ASSERT(0 == rc);
        ASSERT(0 == bsl::strcmp("Central Standard Time (Mexico)",
                                 windowsTimeZoneId));

        timeZoneId = (const char *)0xdeadbeef;
        rc = baetzo_TimeZoneIdUtil::getTimeZoneIdFromWindowsTimeZoneId(
                                                                   &timeZoneId,
                                                                   "ABCZ");
        ASSERT(0                        != rc);
        ASSERT(0xdeadbeef == (unsigned)timeZoneId);

        windowsTimeZoneId = (const char*)0xcafef00d;
        rc = baetzo_TimeZoneIdUtil::getWindowsTimeZoneIdFromTimeZoneId(
                                                            &windowsTimeZoneId,
                                                            "XYZA");
        ASSERT(0          != rc);
        ASSERT(0xcafef00d == (unsigned)windowsTimeZoneId);

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
