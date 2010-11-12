// bael_severityutil.t.cpp                                            -*-C++-*-

#include <bael_severityutil.h>

#include <bael_severity.h>

#include <bsl_iostream.h>
#include <bsl_new.h>          // placement 'new' syntax
#include <bsl_strstream.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
// This component currently has two procedures: The 'fromAsciiCaseless'
// procedure is an inverse operation on 'bael_Severity::toAscii'.  The
// 'isValidNameCaseless' procedure is a helper to 'fromAsciiCaseless'.  The
// test strategy is to first validate that 'fromAsciiCaseless' correctly
// interprets the small set of C-style strings that represent the enumerated
// values of 'bael_Severity::Level' and then confirm that it rejects as
// invalid strings that are small perturbations from the members of the valid
// set.  Once some confidence in 'fromAsciiCaseless' is established, it is
// checked for consistency with 'isValidNameCaseless' and then checked for
// consistency with 'bael_Severity::toAscii'.
//-----------------------------------------------------------------------------
// [ 1] static int fromAsciiCaseless(bael_Severity::Level *, const char *);
// [ 5] static int fromAscii(bael_Severity::Level *, const char *);
// [ 2] static bool isValidNameCaseless(const char *);
// [ 4] static bool isValidName(const char *);
//-----------------------------------------------------------------------------
// [ 3] TESTING: consistency with 'bael_Severity::toAscii'
// [ 6] USAGE EXAMPLE
//-----------------------------------------------------------------------------
//=============================================================================
// STANDARD BDE LOOP-ASSERT TEST MACROS
//=============================================================================
// STANDARD BDE ASSERT TEST MACRO
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

//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
// SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
// GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bael_Severity     Class;
typedef Class::Level      Enum;
typedef bael_SeverityUtil Util;
const Enum initialValue = Class::BAEL_OFF;

//=============================================================================
// GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
// MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
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

        if (verbose) cout << "USAGE EXAMPLE"
                          << "=============" << endl;

///Usage
///-----
// In this example, we show how to validate that a set of C-style strings
// correspond to 'bael_Severity::Level' enumerators, and then use those strings
// to generate enumerator values that, in turn, may be used to administer a
// logger manager.  Here, for convenience, we define our strings in an array,
// much as how we might receive them from a command line:
//..
      const char *argv[] = {
          "INFO",   // record
          "WARN",   // pass
          "ERROR",  // trigger
          "FATAL"   // trigger-all
      };
//
      ASSERT(bael_SeverityUtil::isValidNameCaseless(argv[0]));
      ASSERT(bael_SeverityUtil::isValidNameCaseless(argv[1]));
      ASSERT(bael_SeverityUtil::isValidNameCaseless(argv[2]));
      ASSERT(bael_SeverityUtil::isValidNameCaseless(argv[3]));
//
      bael_Severity::Level record;
      bael_Severity::Level pass;
      bael_Severity::Level trigger;
      bael_Severity::Level triggerAll;
//
      ASSERT(0 == bael_SeverityUtil::fromAsciiCaseless(&record,     argv[0]));
      ASSERT(0 == bael_SeverityUtil::fromAsciiCaseless(&pass,       argv[1]));
      ASSERT(0 == bael_SeverityUtil::fromAsciiCaseless(&trigger,    argv[2]));
      ASSERT(0 == bael_SeverityUtil::fromAsciiCaseless(&triggerAll, argv[3]));
//
      ASSERT(bael_Severity::BAEL_INFO  == record);
      ASSERT(bael_Severity::BAEL_WARN  == pass);
      ASSERT(bael_Severity::BAEL_ERROR == trigger);
      ASSERT(bael_Severity::BAEL_FATAL == triggerAll);
//..
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BASIC TEST 'isValidName'
        //   Test that 'isValidName' correctly classifies input
        //   strings.
        //
        // Concerns:
        //   - Are 'fromAscii' and 'isvalidName' in agreement
        //     in their interpretation of input?
        //
        // Plan:
        //   - Replicate the basic test of 'fromAscii' to test
        //     'isValidName'.  Where the return status of
        //     'fromAscii' is 0 the return value of
        //     'isValidName' should be 'true', and 'false' otherwise.
        //
        // Testing:
        //   bool isValidName(const char *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'isValidName'" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            bool        d_result;       // expected return value
         } DATA[] = {
           ///d_linenum  d_severityName   d_result
           ///---------  --------------   --------
            { L_,        "OFF",           true,    },
            { L_,        "FATAL",         true,    },
            { L_,        "ERROR",         true,    },
            { L_,        "WARN",          true,    },
            { L_,        "INFO",          true,    },
            { L_,        "DEBUG",         true,    },
            { L_,        "TRACE",         true,    },
            { L_,        "NONE",          true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },
            { L_,        "nOnE",          true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },
            { L_,        "nOnE",          true,    },

            { L_,        "OF",            false,   },
            { L_,        "FATA",          false,   },
            { L_,        "ERRO",          false,   },
            { L_,        "WAR",           false,   },
            { L_,        "INF",           false,   },
            { L_,        "DEBU",          false,   },
            { L_,        "TRAC",          false,   },
            { L_,        "NON",           false,   },

            { L_,        "OFFa",          false,   },
            { L_,        "FATALa",        false,   },
            { L_,        "ERRORa",        false,   },
            { L_,        "WARNa",         false,   },
            { L_,        "INFOa",         false,   },
            { L_,        "DEBUGa",        false,   },
            { L_,        "TRACEa",        false,   },
            { L_,        "NONEa",         false,   },

            { L_,        "xxyyzz",        false,   },
            { L_,        "xFATAL",        false,   },
            { L_,        "FATALy",        false,   },
         };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *SEVERITYNAME = DATA[i].d_severityName;
                const bool  RESULT       = DATA[i].d_result;
                bool result = Util::isValidName(SEVERITYNAME);
                LOOP_ASSERT(LINE, result == RESULT);
            }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC TEST 'fromAscii'
        //    Test that 'fromAscii' correctly returns the enumerated
        //    integer value that is named by its argument.
        //
        // Concerns:
        //   - Does 'fromAscii' work correctly for each of the valid
        //     inputs?
        //   - Does 'fromAscii' correctly return an error condition
        //     when given incorrect input?
        //
        // Plan:
        //   - Confirm that 'fromAscii' returns success (0), and loads
        //     the correct severity value for each each of the valid
        //     input strings.
        //   - Confirm that 'fromAscii' correctly returns invalid
        //     status (non-zero) and leaves unchanged the severity value at
        //     the load address when given invalid input.  A set of invalid
        //     input is created by "ad hoc" perturbations on the set of
        //     valid inputs.
        //
        // Testing:
        //   int fromAscii(bael_Severity::Level *result,
        //                 const char           *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'fromAscii'" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            int         d_status;       // expected status
            Enum        d_severity;     // expected severity
         } DATA[] = {
           ///d_linenum  d_severityName   d_status   d_severity
           ///---------  --------------   ---------  ----------
            { L_,        "OFF",             0,       Class::BAEL_OFF   },
            { L_,        "FATAL",           0,       Class::BAEL_FATAL },
            { L_,        "ERROR",           0,       Class::BAEL_ERROR },
            { L_,        "WARN",            0,       Class::BAEL_WARN  },
            { L_,        "INFO",            0,       Class::BAEL_INFO  },
            { L_,        "DEBUG",           0,       Class::BAEL_DEBUG },
            { L_,        "TRACE",           0,       Class::BAEL_TRACE },
            { L_,        "NONE",            0,       Class::BAEL_NONE  },

            { L_,        "oFf",             0,       Class::BAEL_OFF   },
            { L_,        "fAtAl",           0,       Class::BAEL_FATAL },
            { L_,        "eRrOr",           0,       Class::BAEL_ERROR },
            { L_,        "wArN",            0,       Class::BAEL_WARN  },
            { L_,        "iNfO",            0,       Class::BAEL_INFO  },
            { L_,        "dEbUg",           0,       Class::BAEL_DEBUG },
            { L_,        "tRaCe",           0,       Class::BAEL_TRACE },
            { L_,        "nOnE",            0,       Class::BAEL_NONE  },

            { L_,        "OfF",             0,       Class::BAEL_OFF   },
            { L_,        "FaTaL",           0,       Class::BAEL_FATAL },
            { L_,        "ErRoR",           0,       Class::BAEL_ERROR },
            { L_,        "WaRn",            0,       Class::BAEL_WARN  },
            { L_,        "InFo",            0,       Class::BAEL_INFO  },
            { L_,        "DeBuG",           0,       Class::BAEL_DEBUG },
            { L_,        "TrAcE",           0,       Class::BAEL_TRACE },
            { L_,        "NoNe",            0,       Class::BAEL_NONE  },

            { L_,        "OF",             -1,       initialValue },
            { L_,        "FATA",           -1,       initialValue },
            { L_,        "ERRO",           -1,       initialValue },
            { L_,        "WAR",            -1,       initialValue },
            { L_,        "INF",            -1,       initialValue },
            { L_,        "DEBU",           -1,       initialValue },
            { L_,        "TRAC",           -1,       initialValue },
            { L_,        "NON",            -1,       initialValue },

            { L_,        "OFFa",           -1,       initialValue },
            { L_,        "FATALa",         -1,       initialValue },
            { L_,        "ERRORa",         -1,       initialValue },
            { L_,        "WARNa",          -1,       initialValue },
            { L_,        "INFOa",          -1,       initialValue },
            { L_,        "DEBUGa",         -1,       initialValue },
            { L_,        "TRACEa",         -1,       initialValue },
            { L_,        "NONEa",          -1,       initialValue },

            { L_,        "xxyyzz",         -1,       initialValue },
            { L_,        "xFATAL",         -1,       initialValue },
            { L_,        "FATALy",         -1,       initialValue },
         };

         const int NUM_DATA = sizeof DATA / sizeof *DATA;

         for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE         = DATA[i].d_lineNum;
            const char *SEVERITYNAME = DATA[i].d_severityName;
            const int   STATUS       = DATA[i].d_status;
            const Enum  RESULT       = DATA[i].d_severity;
            Enum        result = initialValue;
            const int   status = Util::fromAscii(&result, SEVERITYNAME);
            LOOP_ASSERT(LINE, status == STATUS && result == RESULT);
         }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: consistency with 'bael_Severity::toAscii'
        //    The 'fromAsciiCaseless' procedure should be a functional
        //    inverse to 'bael_Severity::ToAscii'.
        //
        // Concerns:
        //    - The implementations of 'bael_Severity' and 'bael_SeverityUtil'
        //      may become out-of-sync.  This test is provided to aid
        //      maintenance.
        //
        //  Plan:
        //    - Test the conversion from severity level to ascii and
        //      from ascii back to severity level
        //      for the entire set of enumerated severity values.
        //
        // Testing:
        //    int fromAsciiCaseless(bael_Severity::Level *result,
        //                  const char           *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "CONSISTENCY WITH 'ToAscii'" << endl
                          << "========================="  << endl;
            Enum VALUES[] = {
               Class::BAEL_OFF,
               Class::BAEL_FATAL,
               Class::BAEL_ERROR,
               Class::BAEL_WARN,
               Class::BAEL_INFO,
               Class::BAEL_DEBUG,
               Class::BAEL_TRACE,
               Class::BAEL_NONE
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Enum  SEVERITY = VALUES[i];
                Enum result = initialValue;
                int  status = Util::fromAsciiCaseless(&result,
                                                     Class::toAscii(SEVERITY));
                LOOP_ASSERT(i, 0 == status && SEVERITY == result);
            }
        } break;
      case 2: {
        // --------------------------------------------------------------------
        // BASIC TEST 'isValidNameCaseless'
        //   Test that 'isValidNameCaseless' correctly classifies input
        //   strings.
        //
        // Concerns:
        //   - Are 'fromAsciiCaseless' and 'isvalidName' in agreement
        //     in their interpretation of input?
        //
        // Plan:
        //   - Replicate the basic test of 'fromAsciiCaseless' to test
        //     'isValidNameCaseless'.  Where the return status of
        //     'fromAsciiCaseless' is 0 the return value of
        //     'isValidNameCaseless' should be 'true', and 'false' otherwise.
        //
        // Testing:
        //   bool isValidNameCaseless(const char *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'isValidNameCaseless'" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            bool        d_result;       // expected return value
         } DATA[] = {
           ///d_linenum  d_severityName   d_result
           ///---------  --------------   --------
            { L_,        "OFF",           true,    },
            { L_,        "FATAL",         true,    },
            { L_,        "ERROR",         true,    },
            { L_,        "WARN",          true,    },
            { L_,        "INFO",          true,    },
            { L_,        "DEBUG",         true,    },
            { L_,        "TRACE",         true,    },
            { L_,        "NONE",          true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },
            { L_,        "nOnE",          true,    },

            { L_,        "oFf",           true,    },
            { L_,        "fAtAl",         true,    },
            { L_,        "eRrOr",         true,    },
            { L_,        "wArN",          true,    },
            { L_,        "iNfO",          true,    },
            { L_,        "dEbUg",         true,    },
            { L_,        "tRaCe",         true,    },
            { L_,        "nOnE",          true,    },

            { L_,        "OF",            false,   },
            { L_,        "FATA",          false,   },
            { L_,        "ERRO",          false,   },
            { L_,        "WAR",           false,   },
            { L_,        "INF",           false,   },
            { L_,        "DEBU",          false,   },
            { L_,        "TRAC",          false,   },
            { L_,        "NON",           false,   },

            { L_,        "OFFa",          false,   },
            { L_,        "FATALa",        false,   },
            { L_,        "ERRORa",        false,   },
            { L_,        "WARNa",         false,   },
            { L_,        "INFOa",         false,   },
            { L_,        "DEBUGa",        false,   },
            { L_,        "TRACEa",        false,   },
            { L_,        "NONEa",         false,   },

            { L_,        "xxyyzz",        false,   },
            { L_,        "xFATAL",        false,   },
            { L_,        "FATALy",        false,   },
         };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE         = DATA[i].d_lineNum;
                const char *SEVERITYNAME = DATA[i].d_severityName;
                const bool  RESULT       = DATA[i].d_result;
                bool result = Util::isValidNameCaseless(SEVERITYNAME);
                LOOP_ASSERT(LINE, result == RESULT);
            }
        } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST 'fromAsciiCaseless'
        //    Test that 'fromAsciiCaseless' correctly returns the enumerated
        //    integer value that is named by its argument.
        //
        // Concerns:
        //   - Does 'fromAsciiCaseless' work correctly for each of the valid
        //     inputs?
        //   - Does 'fromAsciiCaseless' correctly return an error condition
        //     when given incorrect input?
        //
        // Plan:
        //   - Confirm that 'fromAsciiCaseless' returns success (0), and loads
        //     the correct severity value for each each of the valid
        //     input strings.
        //   - Confirm that 'fromAsciiCaseless' correctly returns invalid
        //     status (non-zero) and leaves unchanged the severity value at
        //     the load address when given invalid input.  A set of invalid
        //     input is created by "ad hoc" perturbations on the set of
        //     valid inputs.
        //
        // Testing:
        //   int fromAsciiCaseless(bael_Severity::Level *result,
        //                 const char           *severityName);
        // --------------------------------------------------------------------

        if (verbose) cout << "BASIC TEST 'fromAsciiCaseless'" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;      // line number
            const char *d_severityName; // severity name
            int         d_status;       // expected status
            Enum        d_severity;     // expected severity
         } DATA[] = {
           ///d_linenum  d_severityName   d_status   d_severity
           ///---------  --------------   ---------  ----------
            { L_,        "OFF",             0,       Class::BAEL_OFF   },
            { L_,        "FATAL",           0,       Class::BAEL_FATAL },
            { L_,        "ERROR",           0,       Class::BAEL_ERROR },
            { L_,        "WARN",            0,       Class::BAEL_WARN  },
            { L_,        "INFO",            0,       Class::BAEL_INFO  },
            { L_,        "DEBUG",           0,       Class::BAEL_DEBUG },
            { L_,        "TRACE",           0,       Class::BAEL_TRACE },
            { L_,        "NONE",            0,       Class::BAEL_NONE  },

            { L_,        "oFf",             0,       Class::BAEL_OFF   },
            { L_,        "fAtAl",           0,       Class::BAEL_FATAL },
            { L_,        "eRrOr",           0,       Class::BAEL_ERROR },
            { L_,        "wArN",            0,       Class::BAEL_WARN  },
            { L_,        "iNfO",            0,       Class::BAEL_INFO  },
            { L_,        "dEbUg",           0,       Class::BAEL_DEBUG },
            { L_,        "tRaCe",           0,       Class::BAEL_TRACE },
            { L_,        "nOnE",            0,       Class::BAEL_NONE  },

            { L_,        "OfF",             0,       Class::BAEL_OFF   },
            { L_,        "FaTaL",           0,       Class::BAEL_FATAL },
            { L_,        "ErRoR",           0,       Class::BAEL_ERROR },
            { L_,        "WaRn",            0,       Class::BAEL_WARN  },
            { L_,        "InFo",            0,       Class::BAEL_INFO  },
            { L_,        "DeBuG",           0,       Class::BAEL_DEBUG },
            { L_,        "TrAcE",           0,       Class::BAEL_TRACE },
            { L_,        "NoNe",            0,       Class::BAEL_NONE  },

            { L_,        "OF",             -1,       initialValue },
            { L_,        "FATA",           -1,       initialValue },
            { L_,        "ERRO",           -1,       initialValue },
            { L_,        "WAR",            -1,       initialValue },
            { L_,        "INF",            -1,       initialValue },
            { L_,        "DEBU",           -1,       initialValue },
            { L_,        "TRAC",           -1,       initialValue },
            { L_,        "NON",            -1,       initialValue },

            { L_,        "OFFa",           -1,       initialValue },
            { L_,        "FATALa",         -1,       initialValue },
            { L_,        "ERRORa",         -1,       initialValue },
            { L_,        "WARNa",          -1,       initialValue },
            { L_,        "INFOa",          -1,       initialValue },
            { L_,        "DEBUGa",         -1,       initialValue },
            { L_,        "TRACEa",         -1,       initialValue },
            { L_,        "NONEa",          -1,       initialValue },

            { L_,        "xxyyzz",         -1,       initialValue },
            { L_,        "xFATAL",         -1,       initialValue },
            { L_,        "FATALy",         -1,       initialValue },
         };

         const int NUM_DATA = sizeof DATA / sizeof *DATA;

         for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE         = DATA[i].d_lineNum;
            const char *SEVERITYNAME = DATA[i].d_severityName;
            const int   STATUS       = DATA[i].d_status;
            const Enum  RESULT       = DATA[i].d_severity;
            Enum        result = initialValue;
            const int   status = Util::fromAsciiCaseless(&result,
                                                         SEVERITYNAME);
            LOOP_ASSERT(LINE, status == STATUS && result == RESULT);
         }
        } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
