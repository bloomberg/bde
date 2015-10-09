// btls5_networkdescriptionutil.t.cpp                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btls5_networkdescriptionutil.h>

#include <btls5_networkdescription.h>

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
//                              --------
//
//-----------------------------------------------------------------------------
// [1] static bool isWellFormed(const NetworkDescription& socks5Servers);
// [1] static void setLevelCredentials(proxyNetwork, level, credentials);
// [1] static void setAllCredentials(proxyNetwork, credentials);
//
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] USAGE TEST

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
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
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
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
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btls5::NetworkDescriptionUtil Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose, (void) veryVeryVerbose, (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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

///Example 1: Set Credentials and Check for Well-Formedness
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to configure a 'btls5::NetworkDescription' object,
// first with uniform credentials, then with variable credentials, and finally
// check if the description is well-formed.
//
// First, we define a 'btls5::NetworkDescription' object with two proxies in
// level 0 ("corporate proxies") and two proxies in level 1 ("regional
// proxies"):
//..
    btls5::NetworkDescription network;
    network.addProxy(0, btlso::Endpoint("proxy1.bloomberg.com", 1080));
    network.addProxy(0, btlso::Endpoint("proxy2.bloomberg.com", 1080));
    network.addProxy(1, btlso::Endpoint("proxy1.ny.bloomberg.com", 1080));
    network.addProxy(1, btlso::Endpoint("proxy2.ny.bloomberg.com", 1080));
//..
// Then, we set all proxies to use the same username/password credentials for
// authentication:
//..
    const btls5::Credentials c1("Jane", "Pass1");
    btls5::NetworkDescriptionUtil::setAllCredentials(&network, c1);
    ASSERT(network.beginLevel(0)[0].credentials() == c1);
    ASSERT(network.beginLevel(0)[1].credentials() == c1);
    ASSERT(network.beginLevel(1)[0].credentials() == c1);
    ASSERT(network.beginLevel(1)[1].credentials() == c1);
//..
// Now, we change the credentials for only level 1 proxies to another
// username/password:
//..
    const btls5::Credentials c2("John", "Pass2");
    btls5::NetworkDescriptionUtil::setLevelCredentials(&network, 1, c2);
    ASSERT(network.beginLevel(0)[0].credentials() == c1);
    ASSERT(network.beginLevel(0)[1].credentials() == c1);
    ASSERT(network.beginLevel(1)[0].credentials() == c2);
    ASSERT(network.beginLevel(1)[1].credentials() == c2);
//..
// Finally, we verify that the network description is well-formed:
//..
    ASSERT(btls5::NetworkDescriptionUtil::isWellFormed(network));
//..
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
        //   static void setLevelCredentials(proxyNetwork, level, credentials);
        //   static void setAllCredentials(proxyNetwork, credentials);
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        btls5::NetworkDescription n1;
        n1.addProxy(0, btlso::Endpoint("proxy1.bloomberg.com", 1080));
        n1.addProxy(0, btlso::Endpoint("proxy2.bloomberg.com", 1080));
        n1.addProxy(1, btlso::Endpoint("proxy1.ny.bloomberg.com", 1080));
        n1.addProxy(1, btlso::Endpoint("proxy2.ny.bloomberg.com", 1080));

        // credentials should all be empty now:

        ASSERT(n1.beginLevel(0)[0].credentials() == btls5::Credentials());
        ASSERT(n1.beginLevel(0)[1].credentials() == btls5::Credentials());
        ASSERT(n1.beginLevel(1)[0].credentials() == btls5::Credentials());
        ASSERT(n1.beginLevel(1)[1].credentials() == btls5::Credentials());

        btls5::Credentials c1("John", "Pass1");
        Obj::setLevelCredentials(&n1, 0, c1);

        // level 0 credentials should be equal to 'c1'

        ASSERT(n1.beginLevel(0)[0].credentials() == c1);
        ASSERT(n1.beginLevel(0)[1].credentials() == c1);
        ASSERT(n1.beginLevel(1)[0].credentials() == btls5::Credentials());
        ASSERT(n1.beginLevel(1)[1].credentials() == btls5::Credentials());

        btls5::Credentials c2("Janne", "Pass2");
        Obj::setAllCredentials(&n1, c2);

        // all credentials should be equal to 'c2'

        ASSERT(n1.beginLevel(0)[0].credentials() == c2);
        ASSERT(n1.beginLevel(0)[1].credentials() == c2);
        ASSERT(n1.beginLevel(1)[0].credentials() == c2);
        ASSERT(n1.beginLevel(1)[1].credentials() == c2);

        btls5::NetworkDescription n2;
        ASSERT(true  == Obj::isWellFormed(n1))
        ASSERT(false == Obj::isWellFormed(n2))

        n1.addProxy(3, btlso::Endpoint("proxy1.ny.bloomberg.com", 1080));
        n2.addProxy(0, btlso::Endpoint("proxy1.ny.bloomberg.com", 1080));

        ASSERT(false == Obj::isWellFormed(n1))
        ASSERT(true  == Obj::isWellFormed(n2))
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
