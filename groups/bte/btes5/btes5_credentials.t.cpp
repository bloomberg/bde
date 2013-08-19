// btes5_credentials.t.cpp                                        -*-C++-*-

#include <btes5_credentials.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
// The component under test implements a value-semantic attribute class.  There
// are two attributes, and the primary manipulators modify both at once.  The
// accessors access each individually.  The test exercises each public
// interface, using the accessors to verify the state set by the manipulators.
//
// BDEX streaming is not yet implemented for this component.
//
//-----------------------------------------------------------------------------
// CREATORS
// [2] explicit btes5_Credentials(bslma::Allocator *allocator = 0);
// [2] btes5_Credentials(const StringRef& u, const StringRef& p, *a = 0);
// [2] btes5_Credentials(const btes5_Credentials& original, *a = 0);
// [2] ~btes5_Credentials() = default;
//
// MANIPULATORS
// [2] set(const bslstl::StringRef& u, const bslstl::StringRef& p);
// [2] reset();
//
// ACCESSORS
// [2] isSet() const;
// [2] bsl::string& username() const;
// [2] bsl::string& password() const;
//
// FREE OPERATORS
// [2] operator==(btes5_Credentials& lhs, btes5_Credentials& rhs);
// [2] operator!=(btes5_Credentials& lhs, btes5_Credentials& rhs);
// [2] operator<<(bsl::ostream& output, const btes5_Credentials& object);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE
// [2] CONCERN: All memory allocation is from the object's allocator.
// [2] CONCERN: Precondition violations are detected when enabled.

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
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
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
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
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

///Example 1: Assign User Name and Password
///- - - - - -
// Suppose that we want to specify user name and password for authenticating
// with a SOCKS5 host (see RFC 1929).  We can use a 'btes5_Credentials' object
// to represent the credentials.
//
// First, we construct an empty object:
//..
    btes5_Credentials credentials1;
    ASSERT(!credentials1.isSet());
//..
// Now, we set the user name and password passing in literals:
    credentials1.set("john.smith", "pass1");
//..
// Finally, we verify that 'credentials1' is no longer empty, and its username
// and password values are as expected:
//..
    ASSERT(credentials1.isSet());
    ASSERT(credentials1.username() == "john.smith");
    ASSERT(credentials1.password() == "pass1");

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BTES5_CREDENTIALS
        //  Test the public interface of 'btes5_Credentials'
        //
        // Concerns:
        //: 1 Objects can be constructed, manipulated and accessed.
        //: 2 If supplied, the memory allocator is propagated.
        //: 3 Preconditions are properly verified (in non-optimized builds).
        //
        // Plan:
        //: 1 Use several ad-hoc values, compare output to expected results.
        //: 2 Compare member allocators to construction-supplied allocator.
        //: 3 Use 'bsls_asserttest' to try construction with invalid values.
        //: 3 Use 'bsls_asserttest' to try to 'set' with invalid values.
        //
        // Testing:
        //   explicit btes5_Credentials(bslma::Allocator *allocator = 0);
        //   btes5_Credentials(const StringRef& u, const StringRef& p, *a = 0);
        //   btes5_Credentials(const btes5_Credentials& original, *a = 0);
        //   ~btes5_Credentials() = default;
        //
        // MANIPULATORS
        //   set(const bslstl::StringRef& u, const bslstl::StringRef& p);
        //   reset();
        //   isSet() const;
        //   bsl::string& username() const;
        //   bsl::string& password() const;
        //   operator==(btes5_Credentials& lhs, btes5_Credentials& rhs);
        //   operator!=(btes5_Credentials& lhs, btes5_Credentials& rhs);
        //   operator<<(bsl::ostream& output, const btes5_Credentials& object);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: Precondition violations are detected when enabled.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BTES5_CREDENTIALS" << endl
                          << "=================" << endl;

        btes5_Credentials credentials1;
        ASSERT(!credentials1.isSet());

        credentials1.set("john.smith", "pass1");
        ASSERT(credentials1.isSet());
        ASSERT(credentials1.username() == "john.smith");
        ASSERT(credentials1.password() == "pass1");
        if (verbose) {
            cout << "credentials1=" << credentials1 << endl;
        }

        btes5_Credentials credentials2(credentials1);
        ASSERT(credentials2.isSet());
        ASSERT(credentials2.username() == "john.smith");
        ASSERT(credentials2.password() == "pass1");
        ASSERT(credentials1 == credentials2);

        credentials2.set("jane.doe", "pass2");
        if (verbose) {
            cout << "credentials1=" << credentials1
                 << " credentials2=" << credentials2
                 << endl;
        }
        ASSERT(credentials1 != credentials2);
        credentials1 = credentials2;
        ASSERT(credentials1.isSet());
        ASSERT(credentials1.username() == "jane.doe");
        ASSERT(credentials1.password() == "pass2");

        ASSERT(credentials1.isSet());
        credentials1.reset();
        ASSERT(!credentials1.isSet());

        if (verbose) cout << " allocator propagation" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        bslma::TestAllocator ta1("defaultConstructor", veryVeryVerbose);
        btes5_Credentials c1(&ta1);
        ASSERT(c1.username().allocator() == &ta1);
        ASSERT(c1.password().allocator() == &ta1);

        bslma::TestAllocator ta2("initializer", veryVeryVerbose);
        btes5_Credentials c2("User", "Password", &ta2);
        ASSERT(c2.username().allocator() == &ta2);

        bslma::TestAllocator ta3("copyConstructor", veryVeryVerbose);
        btes5_Credentials c3(c1, &ta3);
        ASSERT(c3.username().allocator() == &ta3);
        ASSERT(c3.password().allocator() == &ta3);

        // CONCERN: All memory allocation is from the object's allocator.

        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());

        if (verbose) cout << " negative test" << endl;
#ifdef BDE_BUILD_TARGET_EXC
        {
            bsls::AssertFailureHandlerGuard
                g(bsls::AssertTest::failTestDriver);
            string empty;              // too short
            string legal(255, 'a');    // 255 character-string is okay
            string toolong(256, 'a');  // 256 character-string is too long
            BSLS_ASSERTTEST_ASSERT_PASS(btes5_Credentials(legal, legal));

            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(empty, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(legal, empty));
            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(empty, empty));

            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(toolong, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(legal, toolong));
            BSLS_ASSERTTEST_ASSERT_FAIL(btes5_Credentials(toolong, toolong));

            btes5_Credentials c;
            BSLS_ASSERTTEST_ASSERT_PASS(c.set(legal, legal));

            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(empty, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(legal, empty));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(empty, empty));

            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(toolong, legal));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(legal, toolong));
            BSLS_ASSERTTEST_ASSERT_FAIL(c.set(toolong, toolong));

        }
#else
        if (verbose) " not enabled in non-exception builds" << endl;
#endif

        if (verbose) cout << " operator<<" << endl;
        ostringstream s;
        s << btes5_Credentials("Username", "Password");
        const char *expect = "Username:Password";
        LOOP2_ASSERT(s.str(), expect, expect == s.str());
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        btes5_Credentials credentials1;
        ASSERT(!credentials1.isSet());

        credentials1.set("john.smith", "pass1");
        ASSERT(credentials1.isSet());
        ASSERT(credentials1.username() == "john.smith");
        ASSERT(credentials1.password() == "pass1");
        if (verbose) {
            cout << "credentials1=" << credentials1 << endl;
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
