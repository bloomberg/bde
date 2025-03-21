// ball_fmt.t.cpp                                                     -*-C++-*-
#include <ball_fmt.h>

#include <ball_administration.h>
#include <ball_log.h>
#include <ball_testobserver.h>
#include <ball_streamobserver.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <bsl_vector.h>  // for testing only

#include <bsl_cstddef.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>    // atoi()
#include <bsl_cstring.h>    // strlen(), strcmp(), memset(), memcpy(), memcmp()

// Warning: the following `using` declarations interfere with the testing of
// the macros defined in this component.  Please do not un-comment them.
//
// using namespace BloombergLP;
// using namespace bsl;
//
// Also note that such a `using` in the unnamed namespace applies to all code
// that follows the closing of the namespace.

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a few preprocessor macros.
//
// The utility functions are merely facades for methods defined in the
// `ball_loggermanager` component.  It is sufficient to test that each "facade"
// method correctly forwards its arguments to the corresponding method of
// `ball_loggermanager`, and that the correct value is returned.
//
// The preprocessor macros are largely implemented in terms of the utility
// functions.  Each macro is individually tested to ensure that the macro's
// arguments are correctly forwarded and that the side-effects of the macro
// match the expected behavior.
// ----------------------------------------------------------------------------
// [ 1] BALL_FMT
// [ 2] USAGE EXAMPLES

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

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

using BloombergLP::bslma::TestAllocator;

static bool verbose;
static bool veryVerbose;
static bool veryVeryVerbose;
static bool veryVeryVeryVerbose;

static const int k_MULTILINE_OFFSET =
    // `k_MULTILINE_OFFSET` is `1` if the `__LINE__` macro is substituted by
    // the line number of the last line of a macro invocation split on several
    // lines; and it is `0` if the first line is reported.
#if defined(BSLS_COMPILERFEATURES_PP_LINE_IS_ON_FIRST)
    0;
#else
    1;
#endif

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

/// Return `true` if the last record published to the specified `observer`
/// includes the name of the specified `category` and the specified
/// `severity`, `fileName`, `lineNumber`, and `message`, and `false`
/// otherwise.
static bool isRecordOkay(
           const bsl::shared_ptr<BloombergLP::ball::TestObserver>&  observer,
           const BloombergLP::ball::Category                       *category,
           int                                                      severity,
           const char                                              *fileName,
           int                                                      lineNumber,
           const char                                              *message)
{
    const BloombergLP::ball::RecordAttributes& attributes =
                                 observer->lastPublishedRecord().fixedFields();

    bool status = 0   == bsl::strcmp(category->categoryName(),
                                     attributes.category())
        && severity   == attributes.severity()
        && 0          == bsl::strcmp(fileName, attributes.fileName())
        && lineNumber == attributes.lineNumber()
        && 0          == bsl::strcmp(message, attributes.message());

    if (!status) {
        P_(category->categoryName());  P(attributes.category());
        P_(severity);                  P(attributes.severity());
        P_(fileName);                  P(attributes.fileName());
        P_(lineNumber);                P(attributes.lineNumber());
        P(message);                    P(attributes.message());
    }

    return status;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace BloombergLP {

}  // close enterprise namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    TestAllocator ta("test", veryVeryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns:
        // 1. The usage example provided in the component header file must
        //    compile, link, and run on all platforms as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into driver, remove leading
        //    comment characters, and replace `assert` with `ASSERT`.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLES"
                               << "\n==============" << bsl::endl;

        using namespace BloombergLP;    // OK here

        ball::LoggerManagerConfiguration lmConfig;
        lmConfig.setDefaultThresholdLevelsIfValid(ball::Severity::e_TRACE);
        ball::LoggerManagerScopedGuard lmGuard(lmConfig);

        bsl::shared_ptr<ball::StreamObserver> observer =
                            bsl::make_shared<ball::StreamObserver>(&bsl::cout);

        ball::LoggerManager::singleton().registerObserver(observer, "default");

///Example 1: A Basic Logging Example
/// - - - - - - - - - - - - - - - - -
// The following trivial example shows how to use the logging macros to log
// messages at various levels of severity.
//
// First, we initialize the log category within the context of this function.
// The logging macros such as `BALL_FMT_ERROR` will not compile unless a
// category has been specified in the current lexical scope:
// ```
   BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
// ```
// Then, we record messages at various levels of severity.  These messages will
// be conditionally written to the log depending on the current logging
// threshold of the category (configured using the `ball::LoggerManager`
// singleton):
// ```
   BALL_FMT_FATAL("Write this message to the log if the log threshold "
                  "is above 'ball::Severity::e_FATAL' (i.e., {}).", 32);

   BALL_FMT_TRACE("Write this message to the log if the log threshold "
                  "is above 'ball::Severity::e_TRACE' (i.e., {}).", 192);
// ```
// Next, we demonstrate how to use proprietary code within logging macros.
// Suppose you want to add the content of a vector to the log trace:
// ```
   bsl::vector<int> myVector(4, 328);
   BALL_LOG_TRACE_BLOCK {
       BALL_FMT("{}", "myVector = [ ");
       unsigned int position = 0;
       for (bsl::vector<int>::const_iterator it  = myVector.begin(),
                                             end = myVector.end();
           it != end;
           ++it, ++position) {
           BALL_FMT("{}:{} ", position, *it);
       }
       BALL_FMT("{}", ']');
   }
// ```
// Note that the code block will be conditionally executed depending on the
// current logging threshold of the category.  The code within the block must
// not produce any side effects, because its execution depends on the current
// logging configuration.  The special macro `BALL_FMT` provides the means to
// write to the log record from within the block.
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING OSTREAM AND FMT MACROS
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   OSTREAM AND FMT MACROS (WITHOUT CALLBACK)
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING OSTREAM AND FMT MACROS"
                               << "\n==============================\n";

        const char *MESSAGE = "message:1:2:3";
        const char  SEP     = ':';
        const int   ARGS[]  = { 1, 2, 3 };

        BloombergLP::ball::LoggerManagerConfiguration lmc;
        BloombergLP::ball::LoggerManagerScopedGuard   lmg(lmc, &ta);

        bsl::shared_ptr<BloombergLP::ball::TestObserver> observer(
               new (ta) BloombergLP::ball::TestObserver(&bsl::cout, &ta), &ta);

        BloombergLP::ball::LoggerManager& manager =
                                 BloombergLP::ball::LoggerManager::singleton();

        ASSERT(0 == manager.registerObserver(observer, "test"));

        // Re "sieve" category: (1) if recorded, then also published;
        // (2) never triggered.

        BloombergLP::ball::Administration::addCategory(
                                "sieve",
                                BloombergLP::ball::Severity::e_TRACE,
                                BloombergLP::ball::Severity::e_TRACE,
                                0,
                                0);
        BALL_LOG_SET_CATEGORY("sieve")

        BALL_LOG_TRACE << "This will load the category";

        const BloombergLP::ball::Category *CAT  = BALL_LOG_CATEGORY;
        const char                        *FILE = __FILE__;

        const int TRACE = BloombergLP::ball::Severity::e_TRACE;
        const int DEBUG = BloombergLP::ball::Severity::e_DEBUG;
        const int INFO  = BloombergLP::ball::Severity::e_INFO;
        const int WARN  = BloombergLP::ball::Severity::e_WARN;
        const int ERROR = BloombergLP::ball::Severity::e_ERROR;
        const int FATAL = BloombergLP::ball::Severity::e_FATAL;

        BloombergLP::ball::Administration::addCategory(
                           "noTRACE",
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1,
                           BloombergLP::ball::Severity::e_TRACE - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_TRACE`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noTRACE")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_TRACE("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_TRACE("message{0}{1}{0}{2}{0}{3}",
                           SEP,
                           ARGS[0],
                           ARGS[1],
                           ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_TRACE_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, TRACE, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                              "noDEBUG",
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1,
                              BloombergLP::ball::Severity::e_DEBUG - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_DEBUG`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noDEBUG")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_DEBUG("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_DEBUG("message{0}{1}{0}{2}{0}{3}",
                           SEP,
                           ARGS[0],
                           ARGS[1],
                           ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_DEBUG_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, DEBUG, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                               "noINFO",
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1,
                               BloombergLP::ball::Severity::e_INFO - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_INFO`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noINFO")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_INFO("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_INFO("message{0}{1}{0}{2}{0}{3}",
                          SEP,
                          ARGS[0],
                          ARGS[1],
                          ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_INFO_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, INFO, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                  "noWARN",
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1,
                                  BloombergLP::ball::Severity::e_WARN - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_WARN`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noWARN")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_WARN("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_WARN("message{0}{1}{0}{2}{0}{3}",
                          SEP,
                          ARGS[0],
                          ARGS[1],
                          ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_WARN_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, WARN, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                "noERROR",
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1,
                                BloombergLP::ball::Severity::e_ERROR - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_ERROR`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noERROR")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_ERROR("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_ERROR("message{0}{1}{0}{2}{0}{3}",
                           SEP,
                           ARGS[0],
                           ARGS[1],
                           ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_ERROR_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, ERROR, FILE, LINE, MESSAGE));
        }

        BloombergLP::ball::Administration::addCategory(
                                   "noFATAL",
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1,
                                   BloombergLP::ball::Severity::e_FATAL - 1);

        if (veryVerbose)
            bsl::cout << "\tTesting `BALL_FMT_FATAL`" << bsl::endl;
        {
            {
                BALL_LOG_SET_CATEGORY("noFATAL")
                const int numPublishedRecords = observer->numPublishedRecords();

                BALL_FMT_FATAL("{}", "message");
                ASSERT(numPublishedRecords == observer->numPublishedRecords());
            }

            int LINE = L_ + 1 + k_MULTILINE_OFFSET * 4;
            BALL_FMT_FATAL("message{0}{1}{0}{2}{0}{3}",
                           SEP,
                           ARGS[0],
                           ARGS[1],
                           ARGS[2]);
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MESSAGE));

            LINE = L_ + 1;
            BALL_LOG_FATAL_BLOCK {
                BALL_FMT("message{0}{1}{0}{2}{0}{3}",
                         SEP,
                         ARGS[0],
                         ARGS[1],
                         ARGS[2]);
            }
            ASSERT(u::isRecordOkay(observer, CAT, FATAL, FILE, LINE, MESSAGE));
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
