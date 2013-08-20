// btes5_proxydescription.t.cpp                                       -*-C++-*-

#include <btes5_proxydescription.h>

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
// This component provides a value-semantic class 'btes5_ProxyDescription' that
// encodes information about a SOCKS5 proxy.  Basic setters and getters are
// used to verify that proxy description accurately reflects data as set.
//-----------------------------------------------------------------------------
// CREATORS
// [2] btes5_ProxyDescription(address, *a = 0);
// [ ] btes5_ProxyDescription(address, credentials, *a = 0);
// [ ] btes5_ProxyDescription(original, *a = 0);
// [ ] ~btes5_ProxyDescription();
//
// MANIPULATORS
// [2] operator=(rhs) = default;
// [2] void setAddress(const bteso_Endpoint& value);
// [2] void setCredentials(const btes5_Credentials& value);
//
// ACCESSORS
// [2] const bteso_Endpoint& address() const;
// [2] const btes5_Credentials& credentials() const;
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

///Example 1: Desrcibe a Proxy with User Credentials
///- - - - - - - - - - - - - - - - - - - - - - - - -
// We would like to describe a proxy host that requires a username and password
// authentication.
//
// First, we define a descirption with just the address (hostname and port):
//..
    btes5_ProxyDescription proxy(bteso_Endpoint("proxy.corp.com", 1080));
    ASSERT(proxy.address().hostname() == "proxy.corp.com");
    ASSERT(proxy.address().port() == 1080);
//..
// Now, we associate a set of credentials to authenticate connections with this
// proxy:
//..
    proxy.setCredentials(btes5_Credentials("joe.user", "password123"));
    ASSERT(proxy.credentials().isSet());
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
        //   btes5_ProxyDescription(address, *a = 0);
        //   operator=(rhs) = default;
        //   void setAddress(const bteso_Endpoint& value);
        //   void setCredentials(const btes5_Credentials& value);
        //   const bteso_Endpoint& address() const;
        //   const btes5_Credentials& credentials() const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUBLIC INTERFACE" << endl
                          << "================" << endl;

        bteso_Endpoint address1("localhost", 8194);
        btes5_ProxyDescription proxy1(address1);
        ASSERT(proxy1.address() == address1);
        ASSERT(!proxy1.credentials().isSet());
        verbose && cout << "proxy1=" << proxy1 << endl;

        bteso_Endpoint address2("example.com", 80);
        btes5_Credentials credentials2("john.smith", "pass2");
        btes5_ProxyDescription proxy2(address2, credentials2);
        ASSERT(proxy2.address() == address2);
        ASSERT(proxy2.credentials().isSet());
        ASSERT(proxy1 != proxy2);
        verbose && cout << "proxy2=" << proxy2 << endl;

        btes5_ProxyDescription proxy3(proxy2);
        ASSERT(proxy3.address() == address2);
        ASSERT(proxy3.credentials().isSet());
        ASSERT(proxy3 == proxy2);
        verbose && cout << "proxy3=" << proxy3 << endl;

        proxy1 = proxy2;
        ASSERT(proxy1.address() == address2);
        ASSERT(proxy1.credentials().isSet());
        verbose && cout << "proxy1=" << proxy3 << endl;

        proxy2.setAddress(address1);
        ASSERT(proxy2.address() == address1);
        verbose && cout << "proxy2=" << proxy2 << endl;

        // Install a 'TestAllocator' as default to check for incorrect usage`,
        // and specify another 'TestAllocator' explicitly to check proper
        // propagation of the allocator

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator ea("explicitAllocator", veryVeryVerbose);
        {
            btes5_Credentials credentials3("jane.dow", "pass3", &ea);
            proxy1.setCredentials(credentials3);
            ASSERT(proxy1.credentials() == credentials3);
            verbose && cout << "proxy1=" << proxy1 << endl;
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

        bteso_Endpoint address1("localhost", 8194);
        btes5_ProxyDescription proxy1(address1);
        ASSERT(proxy1.address() == address1);
        ASSERT(!proxy1.credentials().isSet());
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
