// btls5_detailedstatus.t.cpp                                         -*-C++-*-
#include <btls5_detailedstatus.h>

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
// The value-semantic struct 'btls5::DetailedStatus' provides information about
// the status of a SOCKS5 negotiation.  There are two attributes in the object:
// the human-readable 'description', and possibly empty 'address'.  The
// attribute values can be set at construction or subsequently using the two
// setter methods.
//
// The setters and getters are used to verify the functionality of this type,
// and 'bslma::TestAllocator' is used to check proper allocator use.
//
//-----------------------------------------------------------------------------
// CREATORS
// [2] btls5::DetailedStatus(Allocator *a = 0);
// [2] btls5::DetailedStatus(StringRef& des, Allocator *a = 0);
// [2] btls5::DetailedStatus(StringRef& des, btlso::Endpoint& addr, *a = 0);
// [2] btls5::DetailedStatus(btls5::DetailedStatus& original, *a = 0);
// [2] // ~btls5::DetailedStatus() = default;
//
// MANIPULATORS
// [2] btls5::DetailedStatus& operator=(btls5::DetailedStatus& rhs);
// [2] void setDescription(StringRef& value);
// [2] void setAddress(btlso::Endpoint& value);
//
// ACCESSORS
// [2] string& description() const;
// [2] btlso::Endpoint& address() const;
//
// FREE OPERATORS
// [2] bool operator==(lhs, rhs);
// [2] bool operator!=(lhs, rhs);
// [2] ostream& operator<<(ostream& o, btls5::DetailedStatus& error);
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

///Example 1: Assign Error Information
///- - - - - - - - - - - - - - - - - -
// Suppose we encounter an authentication failure during a SOCKS5 negotiation.
// We would like to encode the information about this error before signaling
// the client code of the error.
//
// First, we construct an empty 'btls5::DetailedStatus' object with the failure
// description:
//..
    btls5::DetailedStatus error("authentication failure");
//..
//  Now, we set the address of the proxy host that reported the error:
//..
    btlso::Endpoint proxy("proxy1.corp.com", 1080);
    error.setAddress(proxy);
//..
// Finally, we have an encoded 'error' which provides detailed information
// about the failure.
//..
    ASSERT("authentication failure" == error.description());
    ASSERT(proxy                    == error.address());
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PUBLIC INTERFACE
        //   Test constructors, public modifiers and accessors.
        //
        // Concerns:
        //: 1 Objects can be constructed, manipulated and accessed.
        //: 2 If supplied, the memory allocator is propagated.
        //: 3 If not supplied, the default allocator will be used.
        //: 4 If 'basicAllocator == 0', the default allocator will be used.
        //: 5 Defeault-constructed object containes empty attributes
        //
        // Plan:
        //: 1 Use several ad-hoc values, compare output to expected results.
        //: 2 Compare member allocators to construction-supplied allocator.
        //: 3 Use 'TestAllocator' to verify destruction frees allocated memory.
        //
        // Testing:
        //   btls5::DetailedStatus(Allocator *a = 0);
        //   btls5::DetailedStatus(StringRef& des, Allocator *a = 0);
        //   btls5::DetailedStatus(StringRef& des, btlso::Endpoint& addr, *a = 0);
        //   btls5::DetailedStatus(btls5::DetailedStatus& original, *a = 0);
        //   // ~btls5::DetailedStatus() = default;
        //   btls5::DetailedStatus& operator=(btls5::DetailedStatus& rhs);
        //   void setDescription(StringRef& value);
        //   void setAddress(btlso::Endpoint& value);
        //   string& description() const;
        //   btlso::Endpoint& address() const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        //   ostream& operator<<(ostream& o, btls5::DetailedStatus& error);
        //   CONCERN: All memory allocation is from the object's allocator.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUBLIC INTERFACE" << endl
                          << "================" << endl;

        if (verbose) cout << "  c-tors, setters, getters" << endl;

        if (verbose) cout << "  default constructor";
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard guard(&da);
            bslma::TestAllocator ta("test", veryVeryVeryVerbose);

            // Concern 5: deafult-constructed attributes are empty

            btls5::DetailedStatus eNone;
            ASSERT(eNone.description().empty());
            ASSERT(eNone.address() == btlso::Endpoint());

            // Concern 3: no allocator supplied

            ASSERT(eNone.allocator() == &da);
            ASSERT(eNone.description().get_allocator().mechanism() == &da);
            ASSERT(eNone.address().allocator() == &da);

            // Concern 4: no allocator supplied

            btls5::DetailedStatus eZero(0);
            ASSERT(eZero.description().empty());         // C-5
            ASSERT(eZero.address() == btlso::Endpoint());  // C-5
            ASSERT(eZero.allocator() == &da);
            ASSERT(eZero.description().get_allocator().mechanism() == &da);
            ASSERT(eZero.address().allocator() == &da);

            // Concern 2: explicit allocator is injected properly

            btls5::DetailedStatus eExplicit(&ta);
            ASSERT(eExplicit.description().empty());         // C-5
            ASSERT(eExplicit.address() == btlso::Endpoint());  // C-5
            ASSERT(eExplicit.allocator() == &ta);
            ASSERT(eExplicit.description().get_allocator().mechanism() == &ta);
            ASSERT(eExplicit.address().allocator() == &ta);
        }

        btls5::DetailedStatus error1("Error 1");
        ASSERT(error1.description() == "Error 1");
        ASSERT(!error1.address().port());
        verbose && cout << "error1=" << error1 << endl;

        btlso::Endpoint address2("example.com", 80);
        btls5::DetailedStatus error2("Error 2", address2);
        ASSERT(error2.description() == "Error 2");
        ASSERT(error2.address() == address2);
        ASSERT(error1 != error2);
        verbose && cout << "error2=" << error2 << endl;

        btls5::DetailedStatus error3(error2);
        ASSERT(error3.description() == error2.description());
        ASSERT(error3.address().port());
        ASSERT(error3 == error2);
        verbose && cout << "error3=" << error3 << endl;

        error1 = error2;
        ASSERT(error1.description() == "Error 2");
        ASSERT(error1.address().port());
        verbose && cout << "error1=" << error3 << endl;

        error2.setDescription("Error 1");
        ASSERT(error2.description() == "Error 1");
        verbose && cout << "error2=" << error2 << endl;

        btlso::Endpoint address3("localhost", 8194);
        error1.setAddress(address3);
        ASSERT(error1.address() == address3);
        verbose && cout << "error1=" << error1 << endl;

        if (verbose) cout << "  allocator test" << endl;

        bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&defaultAllocator);

        bslma::TestAllocator ta1("description", veryVeryVerbose);
        btls5::DetailedStatus e1("Error", &ta1);
        ASSERT(e1.description().allocator() == &ta1);
        ASSERT(e1.address().hostname().allocator() == &ta1);

        bslma::TestAllocator ta2("description + address", veryVeryVerbose);
        btls5::DetailedStatus e2("Error w/address", address2, &ta2);
        ASSERT(e2.description().allocator() == &ta2);

        bslma::TestAllocator ta3("copy constructor", veryVeryVerbose);
        btls5::DetailedStatus e3(e1, &ta3);
        ASSERT(e3.description().allocator() == &ta3);

        // CONCERN: All memory allocation is from the object's allocator.

        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());

        // Test allocators 'ta1', 'ta2' and 'ta3' will verify deallocation

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

        btls5::DetailedStatus error1("Error 1");
        ASSERT(error1.description() == "Error 1");
        ASSERT(!error1.address().port());
        verbose && cout << "error1=" << error1 << endl;

        btlso::Endpoint address2("example.com", 80);
        btls5::DetailedStatus error2("Error 2", address2);
        ASSERT(error2.description() == "Error 2");
        ASSERT(error2.address() == address2);
        ASSERT(error1 != error2);
        verbose && cout << "error2=" << error2 << endl;

        btls5::DetailedStatus error3(error2);
        ASSERT(error3.description() == error2.description());
        ASSERT(error3.address().port());
        ASSERT(error3 == error2);
        verbose && cout << "error3=" << error3 << endl;

        error1 = error2;
        ASSERT(error1.description() == "Error 2");
        ASSERT(error1.address().port());
        verbose && cout << "error1=" << error3 << endl;

        error2.setDescription("Error 1");
        ASSERT(error2.description() == "Error 1");
        verbose && cout << "error2=" << error2 << endl;

        btlso::Endpoint address3("localhost", 8194);
        error1.setAddress(address3);
        ASSERT(error1.address() == address3);
        verbose && cout << "error1=" << error1 << endl;

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
