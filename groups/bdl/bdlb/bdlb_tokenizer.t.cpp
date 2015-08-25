// bdlb_tokenizer.t.cpp                                               -*-C++-*-
#include <bdlb_tokenizer.h>

#include <bdls_testutil.h>

#include <bdlb_strtokenrefiter.h>         // for performance compare only

#include <bsl_string.h>

#include <bsl_cstdlib.h>                  // 'bsl::atoi()'
#include <bsl_cstring.h>                  // 'bsl::memcpy', 'bsl::strcmp'
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Primary Manipulators:
//: o
//: o
//
/// Basic Accessors:
//: o
//: o
//
//-----------------------------------------------------------------------------
// CREATORS
// [ 2]
// [ 2] ~Tokenizer();
//
// MANIPULATORS
// [ 2] ;
//
// ACCESSORS
// [ 4]
// [ 4]
// [ 4]
// [12]
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3]
// [13] USAGE EXAMPLE

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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlb::Tokenizer    Obj;
typedef bslstl::StringRef  StringRef;

const char INPUT[] = {  "aaaaaaaaaaaaaaaaaaaaaaaaaaaa "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
                        "bbbbbbbbbbbbbbbbbbbbbbbbbbbb "
};

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 13: {
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
        //:   leading comment characters and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;
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
        //: 1 Developer test sandbox. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' with const char*." << endl;
        {
            Obj tokenizer("Hello, world,,,", " ,");

            while (tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.delimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef(",,Hello, world,,,"),
                          StringRef(" "),
                          StringRef(","));

            while (tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.delimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
                ++tokenizer;
            }
        }

        if (verbose) cout << "\n'bdlb::Tokenizer' with StringRef."  << endl;
        {
            Obj tokenizer(StringRef("   I've : been a : :bad   boy!"),
                          StringRef(" "),
                          StringRef(":/"));

            for (; tokenizer; ++tokenizer) {
                cout << "|\t"
                     << '"' << tokenizer.token() << '"'
                     << "\t"
                     << '"' << tokenizer.delimiter() << '"'
                     << "\tSoft?: " << (tokenizer.hasSoft() ? "T":"F")
                     << "\tPSoft?: " << (tokenizer.hasPreviousSoft() ? "T":"F")
                     << "\tHard?: " << (tokenizer.isHard() ? "T":"F")
                     << "\tPHard?: " << (tokenizer.isPreviousHard() ? "T":"F")
                     << endl;
            }
        }

        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj tokenizer(StringRef("   I've : been a : :  bad   boy!   "),
                          StringRef(" "),
                          StringRef(":/"));

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }
            ++tokenizer;
            ++tokenizer;

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }
        }

        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj tokenizer("   I've : been a : :  bad   boy!   ",
                          StringRef(" "),
                          StringRef(":/"));

            for (Obj::iterator it=tokenizer.begin();
                               it != tokenizer.end();
                               ++it) {
                cout << "|\t"
                     << '"' << *it << '"'
                     << endl;

            }

            Obj::iterator t1=tokenizer.begin();
            Obj::iterator t2=tokenizer.begin();

            ASSERT(t1 == t2);
            ++t1; ++t2;
            ASSERT(t1 == t2);
        }


      } break;

      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::TokenizerIterator' test." << endl;
        {
            Obj      tokenizer(INPUT, StringRef(" "));
            for (int i = 0; i < 1000000; ++i) {
                Obj::iterator end = tokenizer.end();
                for (Obj::iterator it = tokenizer.begin();
                                   it != end;
                                   ++it) {
                     *it;
                }
            }
        }
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (Obj it(INPUT, StringRef(" "));
                         it;
                         ++it) {
                     it.token();
                }
            }
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::Tokenizer' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (Obj it(StringRef(INPUT), StringRef(" "));
                         it;
                         ++it) {
                     it.token();
                }
            }
        }
      } break;
      case -4: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // Testing:
        //   PERFORMANCE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE TEST" << endl
                          << "================" << endl;


        if (verbose) cout << "\n'bdlb::StrTokenRefIter' test." << endl;
        {
            for (int i = 0; i < 1000000; ++i) {
                for (bdlb::StrTokenRefIter it(StringRef(INPUT), " ");
                                   it;
                                   ++it) {
                     it();
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
