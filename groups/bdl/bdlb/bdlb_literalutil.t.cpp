// bdlb_literalutil.t.cpp                                             -*-C++-*-
#include <bdlb_literalutil.h>

#include <bsl_iostream.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>            // to verify that we do not
#include <bslma_testallocatormonitor.h>     // allocate any memory
#include <bsls_asserttest.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// TBD
//
// Primary Manipulators:
//: o TBD
//
// Basic Accessors:
//: o TBD
//
// Global Concerns:
//: o No memory is allocated.
//: o TBD
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o TBD
// ----------------------------------------------------------------------------
// CLASS METHODS
// [  ] TBD
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
// [ *] CONCERN: DOES NOT ALLOCATE MEMORY
// [  ] TEST APPARATUS: TBD
// [ 5] PRINTING: TBD

// ============================================================================
//                        STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && 100 >= testStatus) {
            ++testStatus;
        }
    }
}
# define ASSERT(expX) { aSsErT(!(expX), #expX, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(expI, expX) {                                             \
    if (!(expX)) { aSsErT(1, #expX, __LINE__);                                \
                cout << "\t"#expI": " << expI << "\n";}}

#define LOOP2_ASSERT(expI, expJ, expX) {                                      \
    if (!(expX)) { aSsErT(1, #expX, __LINE__);                                \
                   cout <<   "\t"#expI": " << expI <<                         \
                           "\n\t"#expJ": " << expJ << "\n";}}

#define LOOP3_ASSERT(expI, expJ, expK, expX) {                                \
    if (!(expX)) { aSsErT(1, #expX, __LINE__);                                \
                   cout <<   "\t"#expI": " << expI <<                         \
                           "\n\t"#expJ": " << expJ <<                         \
                           "\n\t"#expK": " << expK << "\n"; } }

#define LOOP4_ASSERT(expI, expJ, expK, expL, expX) {                          \
    if (!(expX)) { aSsErT(1, #expX, __LINE__);                                \
                   cout <<   "\t"#expI": " << expI <<                         \
                           "\n\t"#expJ": " << expJ <<                         \
                           "\n\t"#expK": " << expK <<                         \
                           "\n\t"#expL": " << expL << "\n"; } }

#define LOOP5_ASSERT(expI, expJ, expK, expL, expM, expX) {                    \
    if (!(expX)) { aSsErT(1, #expX, __LINE__);                                \
                   cout <<   "\t"#expI": " << expI <<                         \
                           "\n\t"#expJ": " << expJ <<                         \
                           "\n\t"#expK": " << expK <<                         \
                           "\n\t"#expL": " << expL <<                         \
                           "\n\t"#expM": " << expM << "\n"; } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

// ============================================================================
//                   STANDARD BDE VARIADIC ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(expX) cout << #expX " = " << (expX) << endl;
    // Print expression and value.

#define Q(expX) cout << "<| " #expX " |>" << endl;
    // Quote expression literally.

#define P_(expX) cout << #expX " = " << (expX) << ", " << flush;
    // 'P(expX)' without '\n'

#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                      NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

// ============================================================================
//                                USEFUL MACROS
// ----------------------------------------------------------------------------

// The following macros may be used to print an expression 'X' at different
// levels of verbosity.  Note that 'X' is not surrounded with parentheses so
// that expressions containing output stream operations can be supported.

#define PV(X)   if         (verbose) cout << endl << X << endl;
#define PVV(X)  if     (veryVerbose) cout << endl << X << endl;
#define PVVV(X) if (veryVeryVerbose) cout << endl << X << endl;

// ============================================================================
//                    GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                                TEST APPARATUS
// ----------------------------------------------------------------------------

// ============================================================================
//                                 MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: DOES NOT ALLOCATE MEMORY

    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&da);

    bslma::TestAllocatorMonitor gam(&ga), dam(&da);

    switch (test) { case 0:
      case 1: {
        //---------------------------------------------------------------------
        // BREATHING TEST:
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases
        //
        // Plan:
        //:
        //: 1 TBD
        //
        // Testing:
        //   BREATHING TEST
        //---------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        bslma::TestAllocator ta("test", veryVeryVeryVerbose);

        static const char inArray[] = "0123456789"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "~`@#$%^&*()_-+={[}]|\\;:'\"<>,.?/"
                                      "\0'\"\a\b\f\n\r\t\v\x7f\xe1";

        bslstl::StringRef in(inArray, sizeof(inArray) - 1);

        bsl::string       result(&ta);

        bdlb::LiteralUtil::createQuotedEscapedCString(&result, in);

        static const char oracleArray[] =
            "\"0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "~`@#$%^&*()_-+={[}]|\\\\;:'\\\"<>,.?/"
            "\\000'\\\"\\a\\b\\f\\n\\r\\t\\v\\177\\341\"";

        bslstl::StringRef oracle(oracleArray, sizeof(oracleArray) - 1);

        LOOP2_ASSERT(result, oracle, result == oracle);

      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: DOES NOT ALLOCATE MEMORY

    ASSERTV(gam.isTotalSame());
    ASSERTV(dam.isTotalSame());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2016
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
