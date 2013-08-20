// btes5_networkdescription.t.cpp                                     -*-C++-*-

#include <btes5_networkdescription.h>

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
// This component implements a value-semantic description of a network of
// SOCKS5 proxies.  The component is tested by exercising its setters and using
// the getters to compare to expected values.
//-----------------------------------------------------------------------------
// CREATORS
// [2] btes5_NetworkDescription(bslma::Allocator *allocator = 0);
// [ ] btes5_NetworkDescription(original, bslma::Allocator *allocator = 0);
// [ ] ~btes5_NetworkDescription();
//
// MANIPULATORS
// [2] btes5_NetworkDescription& operator=(rhs);
// [ ] bsl::size_t addProxy(level, addr, credentials);
// [2] bsl::size_t addProxy(level, addr);
// [ ] void setCredentials(level, order, credentials);
//
// ACCESSORS
// [2] bsl::size_t levelCount() const;
// [ ] bsl::size_t numProxies(bsl::size_t level) const;
// [ ] ProxyIterator beginLevel(bsl::size_t level) const;
// [ ] ProxyIterator endLevel(bsl::size_t level) const;
//
// UTILITIES (struct btes5_NetworkDescriptionUtil)
// [ ] static void setLevelCredentials(proxyNetwork, level, credentials);
// [ ] static void setAllCredentials(proxyNetwork, credentials);
//
// FREE OPERATORS
// [2] bool operator==(lhs, rhs);
// [2] bool operator!=(lhs, rhs);
// [2] bsl::ostream& operator<<(stream, object);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] USAGE EXAMPLE
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

///Example 1: Describe a Two-level Proxy Network
///- - - - - - - - - - - - - - - - - - - - - - -
// Let's define a network of proxies necessary to reach the Internet from a
// corporate Intranet.  The Internet can be reached through 2 levels: a
// corporate proxy, which then has a connection to a regional proxy, which
// finally has direct access to the Internet addresses of interest.  Each proxy
// level has two proxies for redundancy.
//
// First, we declare an empty network:
//..
    btes5_NetworkDescription network;
    ASSERT(0 == network.levelCount());
//..
// Then, we add the addresses of corporate proxies as level 0 (directly
// reachable from our host):
//..
    bteso_Endpoint corp1("proxy1.corp.com", 1081);
    bteso_Endpoint corp2("proxy2.corp.com", 1082);
    network.addProxy(0, corp1);
    network.addProxy(0, corp2);
    ASSERT(1 == network.levelCount());
//..
// Now, we add the regional proxies, we can only connect to through the
// corporate proxies.  There are two regional proxies, for redundancy.
//..
    bteso_Endpoint region1("proxy2.region.com", 1091);
    bteso_Endpoint region2("proxy2.region.com", 1092);
    network.addProxy(1, region1);
    network.addProxy(1, region2);
    ASSERT(2 == network.levelCount());
//..
// Finally, we have a fully defined network which we can use for connection
// using 'btes_networkDescriptor'.
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // SETTERS AND GETTERS
        //   Test the basic setters and getters.
        //
        // Concerns:
        //: 1 The values set are accurately retrieved through the getters.
        //
        // Plan:
        //: 1 Perform ad-hoc test of the primary modifiers and accessors.
        //: 2 Use 'bslma::TestAllocator' to check for improper default use.
        //
        // Testing:
        //   btes5_NetworkDescription(bslma::Allocator *allocator = 0);
        //   btes5_NetworkDescription& operator=(rhs);
        //   bsl::size_t addProxy(level, addr);
        //   bsl::size_t levelCount() const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        //   bsl::ostream& operator<<(stream, object);
        //   USAGE EXAMPLE
        //   CONCERN: All memory allocation is from the object's allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SETTERS AND GETTERS" << endl
                          << "===================" << endl;

        veryVerbose && cout << "C-tor and levelCount()" << endl;
        btes5_NetworkDescription network;
        ASSERT(0 == network.levelCount());

        veryVerbose && cout << "addProxy()" << endl;
        bteso_Endpoint corp1("proxy1.corp.com", 1081);
        bteso_Endpoint corp2("proxy2.corp.com", 1082);
        bteso_Endpoint region1("proxy2.region.com", 1091);
        bteso_Endpoint region2("proxy2.region.com", 1092);
        network.addProxy(0, corp1);
        network.addProxy(0, corp2);
        ASSERT(1 == network.levelCount());
        network.addProxy(1, region1);
        network.addProxy(1, region2);
        ASSERT(2 == network.levelCount());

        // Install a 'TestAllocator' as default to check for incorrect usage`,
        // and specify another 'TestAllocator' explicitly to check proper
        // propagation of the allocator

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator ea("explicitAllocator", veryVeryVerbose);

        veryVerbose &&
            cout << "operator=(), operator!=() and operator==()" << endl;
        {
            btes5_NetworkDescription network2(&ea);
            ASSERT(network != network2);
            network2 = network;
            ASSERT(network == network2);
        }

        // verify that the default allocator was not used

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        ostringstream s;
        s << network;
        string expected =
            "Proxy level 0: proxy1.corp.com:1081 proxy2.corp.com:1082\n"
            "Proxy level 1: proxy2.region.com:1091 proxy2.region.com:1092\n";
        LOOP2_ASSERT(s.str(), expected, expected == s.str());

        // TODO Credentials tests, incl. utility functions
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

        veryVerbose && cout << "C-tor and levelCount()" << endl;
        btes5_NetworkDescription network;
        ASSERT(0 == network.levelCount());

        bteso_Endpoint corp1("proxy1.corp.com", 1081);
        bteso_Endpoint corp2("proxy2.corp.com", 1082);
        bteso_Endpoint region1("proxy2.region.com", 1091);
        bteso_Endpoint region2("proxy2.region.com", 1092);
        network.addProxy(0, corp1);
        network.addProxy(0, corp2);
        ASSERT(1 == network.levelCount());
        network.addProxy(1, region1);
        network.addProxy(1, region2);
        ASSERT(2 == network.levelCount());

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
