// btls5_proxydescription.t.cpp                                       -*-C++-*-

#include <btls5_proxydescription.h>

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
// This component provides a value-semantic class 'btls5::ProxyDescription'
// that encodes information about a SOCKS5 proxy.  Basic setters and getters
// are used to verify that proxy description accurately reflects data as set.
//-----------------------------------------------------------------------------
// CREATORS
// [2] btls5::ProxyDescription(address, *a = 0);
// [ ] btls5::ProxyDescription(address, credentials, *a = 0);
// [ ] btls5::ProxyDescription(original, *a = 0);
// [ ] ~btls5::ProxyDescription();
//
// MANIPULATORS
// [2] operator=(rhs) = default;
// [2] void setAddress(const btlso::Endpoint& value);
// [2] void setCredentials(const btls5::Credentials& value);
//
// ACCESSORS
// [2] const btlso::Endpoint& address() const;
// [2] const btls5::Credentials& credentials() const;
//
// FREE OPERATORS
// [2] bool operator==(lhs, rhs);
// [2] bool operator!=(lhs, rhs);
// [ ] bsl::ostream& operator<<(stream, object);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE
// [2] CONCERN: All memory allocation is from the object's allocator.

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

///Example 1: Describe a Proxy with Credentials
/// - - - - - - - - - - - - - - - - - - - - - -
// We would like to describe a proxy host that requires a username and password
// for authentication.
//
// First, we define a description with just the address (hostname and port):
//..
    btls5::ProxyDescription proxy(btlso::Endpoint("proxy.corp.com", 1080));
    ASSERT(proxy.address().hostname() == "proxy.corp.com");
    ASSERT(proxy.address().port()     == 1080);
//..
// Now, we associate a set of credentials to authenticate connections with this
// proxy:
//..
    proxy.setCredentials(btls5::Credentials("joe.user", "password123"));
    ASSERT(proxy.credentials().username() == "joe.user");
    ASSERT(proxy.credentials().password() == "password123");
//..
// Finally, we have a fully defined proxy description.
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PUBLIC INTERFACE
        //   Test public constructors, modifiers and accessors.
        //
        // Concerns:
        //: 1 Public interfaces function per contract.
        //
        // Plan:
        //: 1 Perform a test of the constructors, modifiers, accessors and free
        //:   functions.
        //:
        //: 2 Use 'bslma::TestAllocator' to check for improper default use.
        //
        // Testing:
        //   btls5::ProxyDescription(address, *a = 0);
        //   operator=(rhs) = default;
        //   void setAddress(const btlso::Endpoint& value);
        //   void setCredentials(const btls5::Credentials& value);
        //   const btlso::Endpoint& address() const;
        //   const btls5::Credentials& credentials() const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUBLIC INTERFACE" << endl
                          << "================" << endl;

        btlso::Endpoint address1("localhost", 8194);
        btls5::ProxyDescription proxy1(address1);
        ASSERT(proxy1.address() == address1);
        ASSERT(proxy1.credentials().username().empty());
        verbose && cout << "proxy1=" << proxy1 << endl;

        btlso::Endpoint address2("example.com", 80);
        btls5::Credentials credentials2("john.smith", "pass2");
        btls5::ProxyDescription proxy2(address2, credentials2);
        ASSERT(proxy2.address() == address2);
        ASSERT(proxy2.credentials().username().length());
        ASSERT(proxy1 != proxy2);
        verbose && cout << "proxy2=" << proxy2 << endl;

        btls5::ProxyDescription proxy3(proxy2);
        ASSERT(proxy3.address() == address2);
        ASSERT(proxy3.credentials().username().length());
        ASSERT(proxy3 == proxy2);
        verbose && cout << "proxy3=" << proxy3 << endl;

        proxy1 = proxy2;
        ASSERT(proxy1.address() == address2);
        ASSERT(proxy1.credentials().username().length());
        verbose && cout << "proxy1=" << proxy3 << endl;

        proxy2.setAddress(address1);
        ASSERT(proxy2.address() == address1);
        verbose && cout << "proxy2=" << proxy2 << endl;

        if (verbose) cout << " setCredentials, allocator use" << endl;

        string username("A fairly long name"
                        " to avoid short-string optimization"
                        " and cause a likely memory allocation");
        string password("A fairly long password"
                        " to avoid short-string optimization"
                        " and cause a likely memory allocation");
        btls5::Credentials credentials3(username, password);

        // Install a 'TestAllocator' as default to check for incorrect usage`,
        // and specify another 'TestAllocator' explicitly to check proper
        // propagation of the allocator

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator ea("explicitAllocator", veryVeryVerbose);
        {
            btls5::ProxyDescription proxy3(address1, &ea);
            proxy3.setCredentials(credentials3);

            if (veryVerbose) { T_ P(proxy3) }

            ASSERT(proxy3.credentials() == credentials3);
        }

        // verify that the default allocator was not used

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

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
        //: 1 Ad-hoc test of the constructors, modifiers, accessors and free
        //:   operators.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        btlso::Endpoint address1("localhost", 8194);
        btls5::ProxyDescription proxy1(address1);
        ASSERT(proxy1.address() == address1);
        ASSERT(!proxy1.credentials().username().length());
        verbose && cout << "proxy1=" << proxy1 << endl;

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
