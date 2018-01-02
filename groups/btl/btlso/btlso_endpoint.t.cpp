// btlso_endpoint.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_endpoint.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_newdeleteallocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iomanip.h>
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
// CLASS METHODS
// [ 3] bool isValid(const bslstl::StringRef& hostname, int port);
//
// CREATORS
// [ 2] btlso::Endpoint(*basicAllocator = 0);
// [ 2] btlso::Endpoint(btlso::Endpoint& original, *basicAllocator = 0);
// [ 2] btlso::Endpoint(hostname, port, *basicAllocator = 0);
// [ 2] ~btlso::Endpoint();
//
// MANIPULATORS
// [ 2] btlso::Endpoint& operator=(const btlso::Endpoint& rhs) = default;
// [ 2] void set(hostname, int port);
// [ 3] int setIfValid(hostname, int port);
// [  ] void swap(bltso::Endpoint& other);
//
// ACCESSORS
// [ 2] const bsl::string& hostname() const;
// [ 2] int port() const;
// [ 2] Allocator *allocator() const;
// [  ] ostream& print(ostream& stream, level, spacesPerLevel) const;
//
// FREE OPERATORS
// [ 2] bool operator==(lhs, rhs);
// [ 2] bool operator!=(lhs, rhs);
// [ 2] bsl::ostream& operator<<(stream, object);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
// [ 2] CONCERN: All memory allocation is from the object's allocator.

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

    // Test Apparatus allocator

    bslma::Allocator *ta = &bslma::NewDeleteAllocator::singleton();

    switch (test) { case 0:
      case 4: {
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
// First, we declare an empty 'btlso::Endpoint':
//..
    btlso::Endpoint address;
//..
// Now, we set hostname and port:
//..
    address.set("www.bloomberg.com", 80);
//..
// Finally, we have an object that describes the HTTP server at Bloomberg LP:
//..
    ASSERT(address.hostname() == "www.bloomberg.com");
    ASSERT(address.port()     == 80);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ISVALID
        //   Verify 'isValid' and 'setIfValid' work correctly.
        //
        // Concerns:
        //: 1 isValid accepts hostnames between 1 and 254 characters.
        //: 2 isValid accepts port numbers from 1 to 65636.
        //: 3 isValid rejects empty strings, and strings longer than 255 bytes.
        //: 4 isValid rejects port numbers less than 1 or greater than 65535.
        //: 5 setIfValid sets the attributes iff its parameters are valid.
        //: 6 setIfValid returns 0 iff its parameters are valid
        //
        // Plan:
        //: 1 Use a data driven approach to check is the values are valid:
        //:   1 Check against expected return of 'isValid'.
        //:   2 Call 'setIfValid' and check the return code.
        //:   3 Check if the object value was changed based on validity.
        //
        // Testing:
        //   bool isValid(const bslstl::StringRef& hostname, int port);
        //   int setIfValid(hostname, int port);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ISVALID" << endl
                          << "=======" << endl;

        ostringstream s(ta);

        s << setw(255) << '.';
        const string longHostname(s.str(), ta);
        ASSERT(255 == longHostname.length());

        s << '.';
        const string tooLongHostname(s.str(), ta);
        ASSERT(256 == tooLongHostname.length());

        static const struct {
            int         d_line;  // source line number
            const char *d_hostname;
            int         d_port;
            bool        d_valid;
        } DATA[] = {

        //LINE HOSTNAME                  PORT  VALID
        //---- ----------------         -----  -----
        { L_,  "",                          0, true },
        { L_,  "",                          1, false },
        { L_,  "h",                         1, true  },
        { L_,  longHostname.c_str(),        1, true  },
        { L_,  tooLongHostname.c_str(),     1, false },

        { L_,  "h",                        -1, false },
        { L_,  "h",                         0, false },
        { L_,  "h",                         1, true  },
        { L_,  "h",                     65535, true  },
        { L_,  "h",                     65536, false },
        { L_,  "h",                     65537, false },

        { L_,  longHostname.c_str(),    65535, true  },
        { L_,  tooLongHostname.c_str(), 65536, false },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const btlso::Endpoint obj0("initial.host.name", 12345);

        for (int ti = 0; ti < NUM_DATA; ti++) {
            const int   LINE     = DATA[ti].d_line;
            const char *HOSTNAME = DATA[ti].d_hostname;
            const int   PORT     = DATA[ti].d_port;
            const bool  VALID    = DATA[ti].d_valid;

            if (veryVerbose) { T_ P_(LINE) P_(HOSTNAME) P_(PORT) P(VALID) }

            LOOP4_ASSERT(LINE, HOSTNAME, PORT, VALID,
                         VALID == btlso::Endpoint::isValid(HOSTNAME, PORT));

            btlso::Endpoint obj(obj0);
            int rc = obj.setIfValid(HOSTNAME, PORT);
            LOOP4_ASSERT(LINE, HOSTNAME, PORT, VALID, VALID == (0 == rc));
            LOOP4_ASSERT(LINE, HOSTNAME, PORT, VALID, VALID == (obj0 != obj));
        }

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
        //   btlso::Endpoint(*basicAllocator = 0);
        //   btlso::Endpoint(btlso::Endpoint& original, *basicAllocator = 0);
        //   ~btlso::Endpoint();
        //   void set(hostname, int port);
        //   const bsl::string& hostname() const;
        //   int port() const;
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PUBLIC INTERFACE" << endl
                          << "================" << endl;

        btlso::Endpoint address1;
        ASSERT(!address1.port());

        address1.set("localhost", 8194);
        ASSERT(address1.hostname() == "localhost");
        ASSERT(address1.port() == 8194);
        if (verbose) {
            cout << "address1 = " << address1 << endl;
        }

        bslma::TestAllocator da("defaultAllocator", veryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&da);

        bslma::TestAllocator ea("explicitAllocator", veryVeryVerbose);
        {
            btlso::Endpoint address2(address1, &ea);
            ASSERT(address2.hostname() == "localhost");
            ASSERT(address2.port() == 8194);
            ASSERT(address2.allocator() == &ea);
            ASSERT(address1 == address2);
            if (verbose) {
                cout << "address2 = " << address2 << endl;
            }

            address2.set("bloomberg.com", 80);
            ASSERT(address1 != address2);
            if (verbose) {
                cout << "address1 = " << address1
                     << " address2 = " << address2
                     << endl;
            }

            address1 = address2;
            ASSERT(address1.hostname() == "bloomberg.com");
            ASSERT(address1.port() == 80);
        }

        btlso::Endpoint address3("localhost", 8194);
        ASSERT(address3.hostname() == "localhost");
        ASSERT(address3.port() == 8194);
        ASSERT(address1 != address3);

        {
            btlso::Endpoint address4;
            ASSERT(!address4.port());
            ASSERT(address1 != address4);

            address4 = address1;
            ASSERT(address1 == address4);
        }

        {
            btlso::Endpoint address3, address4;
            address3.set("localhost", 8194);
            address4.set("bloomberg.com", 80);

            ASSERT(address3.hostname() == "localhost");
            ASSERT(address3.port() == 8194);
            ASSERT(address4.hostname() == "bloomberg.com");
            ASSERT(address4.port() == 80);

            address3.swap(address4);

            ASSERT(address3.hostname() == "bloomberg.com");
            ASSERT(address3.port() == 80);
            ASSERT(address4.hostname() == "localhost");
            ASSERT(address4.port() == 8194);
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
        btlso::Endpoint address1;
        ASSERT(!address1.port());
        ASSERT(address1.hostname().empty());

        address1.set("localhost", 8194);
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
