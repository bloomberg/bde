// bteso_endpoint.t.cpp                                               -*-C++-*-

#include <bteso_endpoint.h>

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
// This component provides a value-semantic class that represents a TCP or UDP
// endpoint as a hostname and port.  It's tested using the class setters and
// getters.  'bslma::TestAllocator' is used to test proper allocator usage.
//-----------------------------------------------------------------------------
// CREATORS
// [2] bteso_Endpoint(*allocator = 0);
// [2] bteso_Endpoint(bteso_Endpoint& original, *allocator = 0);
// [ ] bteso_Endpoint(hostname, port, *allocator = 0);
// [2] ~bteso_Endpoint();
//
// MANIPULATORS
// [ ] bteso_Endpoint& operator=(const bteso_Endpoint& rhs) = default;
// [2] void set(hostname, int port);
// [ ] void reset();
//
// ACCESSORS
// [2] bool isSet() const;
// [2] const bsl::string& hostname() const;
// [2] int port() const;
//
// FREE OPERATORS
// [ ] bool operator==(lhs, rhs);
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

///Example 1: Set Hostname and Port
/// - - - - - - - - - - - - - - - -
// Let us encode a TCP address as a hostname and port.
//
// First, we declare an empty (unset) 'bteso_Endpoint':
//..
    bteso_Endpoint address;
//..
// Now, we set hostname and port:
//..
    address.set("www.bloomberg.com", 80);
//..
// Finally, we have an object that describes the HTTP server at Bloomberg LP:
//..
    ASSERT(address.hostname() == "www.bloomberg.com");
    ASSERT(address.port() == 80);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PUBLIC INTERFACE
        //   Verify the public interface works as documented.
        //
        // Concerns:
        //: 1 Public interfaces function per contract.
        //: 2 If supplied, the allocator is used for all allocations.
        //
        // Plan:
        //: 1 Perform a test of the constructors, modifiers, accessors and free
        //:   functions.
        //:
        //: 2 Use 'bslma::TestAllocator' to check for improper default use.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUBLIC INTERFACE" << endl
                          << "================" << endl;

        bteso_Endpoint address1;
        ASSERT(!address1.isSet());

        address1.set("localhost", 8194);
        ASSERT(address1.isSet());
        ASSERT(address1.hostname() == "localhost");
        ASSERT(address1.port() == 8194);
        if (verbose) {
            cout << "address1=" << address1 << endl;
        }

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator ea("explicitAllocator", veryVeryVerbose);
        {
            bteso_Endpoint address2(address1, &ea);
            ASSERT(address2.isSet());
            ASSERT(address2.hostname() == "localhost");
            ASSERT(address2.port() == 8194);
            ASSERT(address1 == address2);
            if (verbose) {
                cout << "address2=" << address2 << endl;
            }

            address2.set("bloomberg.com", 80);
            ASSERT(address1 != address2);
            if (verbose) {
                cout << "address1=" << address1
                     << " address2=" << address2
                     << endl;
            }
            address1 = address2;
            ASSERT(address1.isSet());
            ASSERT(address1.hostname() == "bloomberg.com");
            ASSERT(address1.port() == 80);

            ASSERT(address1.isSet());
            address1.reset();
            ASSERT(!address1.isSet());
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        bteso_Endpoint address1;
        ASSERT(!address1.isSet());

        address1.set("localhost", 8194);
        ASSERT(address1.isSet());
        ASSERT(address1.hostname() == "localhost");
        ASSERT(address1.port() == 8194);
        if (verbose) {
            cout << "address1=" << address1 << endl;
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
