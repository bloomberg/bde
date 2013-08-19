// btes5_credentialsprovider.t.cpp                                    -*-C++-*-

#include <btes5_credentialsprovider.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_protocoltest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This class defines a protocol for a mechanism that supplies SOCKS5
// credentials for a proxy host. The class protocol definition is tested, as
// well as the usage example.
//-----------------------------------------------------------------------------
// [1] PROTOCOL TEST
// [2] USAGE EXAMPLE

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
struct ProviderImp : bsls::ProtocolTestImp<btes5_CredentialsProvider> {
    virtual void acquireCredentials(const bteso_Endpoint&, Callback) {
        markDone();
    };
    virtual void cancelAcquiringCredentials() { markDone(); }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------
namespace {

///Example 1: Acquire Username and Passowrd
/// - - - - - - - - - - - - - - - - - - - -
// During the process of SOCKS5 negotiation, a proxy host may require the
// username and password to authenticate the SOCKS5 client.  In this example we
// acquire the credentials from secure storage and pass them to the client code
// through a client-supplied callback.
//
// First, we define a class that implements the 'btes5_CredentialsProvider'
// protocol.
//..
    class MyCredentialsProvider : public btes5_CredentialsProvider {
      public:
        // Construction and destruction elided.

    virtual void acquireCredentials(const bteso_Endpoint& proxy,
                                    Callback              callback);
        // Acquire credentials and invoke the specified 'callback' with
        // username and password to authenticate the SOCKS5 client with the
        // specified 'proxy'.

    virtual void cancelAcquiringCredentials();
        // Cancel acquiring credentials.
    };
//..
// Then, we define the 'acquireCredentials' method.  Since here we look up the
// credentials in secure storage which is a non-blocking operation, we can
// invoke the callback directly.
//..
    void MyCredentialsProvider::acquireCredentials(
        const bteso_Endpoint& proxy,
        Callback              callback)
    {
        bsl::string username("Defaultuser");
        bsl::string password("Defaultpassword");
        // Look up the credentials for the specified 'proxy' ...
        callback(0, username, password);
    }
//..
// Now, we define the callback function which would make use of the acquired
// credentials by sending them to the SOCKS5 server.
//..
    void credentialsCb(int                      status,
                       const bslstl::StringRef& username,
                       const bslstl::StringRef& password)
    {
        if (0 == status) {
            ASSERT("Defaultuser" == username);
            ASSERT("Defaultpassword" == password);
            // send username and password to the SOCKS5 host ...
        } else {
            // credentials not available, report error and stop negotiation ...
        }
    }
//..
// Finally, we define the function which will be able to authenticate the
// connection using credentials supplied by a 'MyCredentialsProvider'.
//..
    void socks5Authenticate()
    {
        MyCredentialsProvider provider;
        bteso_Endpoint proxy("proxy1.corp.com", 1080);
        provider.acquireCredentials(proxy, &credentialsCb);
        // next stage of negotiation will be done by 'credentialsCb'
    }
//..

void MyCredentialsProvider::cancelAcquiringCredentials() {
}

}  // close unnamed namespace

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

        socks5Authenticate();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   Ensure this class is a properly defined protocol.
        //
        // Concerns:
        //: 1 The protocol is abstract: no objects of it can be created.
        //:
        //: 2 The protocol has no data members.
        //:
        //: 3 The protocol has a virtual destructor.
        //:
        //: 4 All methods of the protocol are pure virtual.
        //:
        //: 5 All methods of the protocol are publicly accessible.
        //
        // Plan:
        //: 1 Define a concrete derived implementation, 'ProviderImp',
        //:   of the protocol.
        //:
        //: 2 Create an object of the 'bsls::ProtocolTest' class template
        //:   parameterized by 'ProviderImp', and use it to verify
        //:   that:
        //:
        //:   1 The protocol is abstract. (C-1)
        //:
        //:   2 The protocol has no data members. (C-2)
        //:
        //:   3 The protocol has a virtual destructor. (C-3)
        //:
        //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
        //:   non-creator methods of the protocol are:
        //:
        //:   1 virtual, (C-4)
        //:
        //:   2 publicly accessible. (C-5)
        //
        // Testing:
        //   virtual ~btes5_CredentialsProvider();
        //   virtual void acquireCredentials(const bteso_Endpoint& proxy,
        //                                   Callback callback) = 0;
        //   virtual void cancelAcquiringCredentials() = 0;
        // ------------------------------------------------------------------------

        if (verbose) printf("\nPROTOCOL TEST"
                            "\n=============\n");
        bsls::ProtocolTest<ProviderImp> testObj(veryVerbose);
        if (verbose) printf("\nVerify that the protocol is abstract.\n");

          ASSERT(testObj.testAbstract());

          if (verbose) printf("\nVerify that there are no data members.\n");

          ASSERT(testObj.testNoDataMembers());

          if (verbose) printf("\nVerify that the destructor is virtual.\n");

          ASSERT(testObj.testVirtualDestructor());
        if (verbose) printf("\nVerify that methods are public and virtual.\n");

          btes5_CredentialsProvider::Callback cb;
          bteso_Endpoint proxy("localhost", 8194);
          BSLS_PROTOCOLTEST_ASSERT(testObj, acquireCredentials(proxy, cb));
          BSLS_PROTOCOLTEST_ASSERT(testObj, cancelAcquiringCredentials());
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
