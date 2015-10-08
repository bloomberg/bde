// balxml_namespaceregistry.t.cpp                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_namespaceregistry.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>  // for bsl::strcpy
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// CREATORS
// [01] balxml::NamespaceRegistry(bslma::Allocator *allocator = 0);
// [01] ~balxml::NamespaceRegistry();
//
// MANIPULATORS
// [02] int lookupOrRegister(const bsl::string& namespaceUri);
//
// ACCESSORS
// [03] int lookup (const bsl::string& namespaceUri);
// [04] const bsl::string& lookup (int id);
// ----------------------------------------------------------------------------
// [01] BREATHING TEST
// [06] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef balxml::NamespaceRegistry          Obj;

// ============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    bslma::TestAllocator da;
    bslma::DefaultAllocatorGuard defaultGuard(&da);
    bslma::TestAllocator ta(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.

      case 6: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver, changing 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING USAGE EXAMPLE"
                               << "\n=====================" << bsl::endl;

// Typically, a program will register namespaces as it encounters them in an
// XML document.  Alternatively, namespaces that are important to the program
// are registered in advance, as in the following code:
//..
    const char googleUri[] = "http://www.google.com/schemas/results.xsd";
    const char yahooUri[]  = "http://www.yahoo.com/xsd/searchResults.xsd";

    balxml::NamespaceRegistry namespaceRegistry;
    int googleId = namespaceRegistry.lookupOrRegister(googleUri);
    ASSERT(googleId >= 0);
    int yahooId = namespaceRegistry.lookupOrRegister(yahooUri);
    ASSERT(yahooId >= 0);
    ASSERT(yahooId != googleId);
//..
// Later, IDs can be looked up without concern for whether they have already
// been registered.  Any new namespaces are simply given a new ID:
//..
    char input[100];

    // First input is a new namespace URI.
    bsl::strcpy(input, "http://www.bloomberg.com/schemas/example.xsd");
    int id1 = namespaceRegistry.lookupOrRegister(input);
    ASSERT(id1 >= 0);
    ASSERT(id1 != googleId);
    ASSERT(id1 != yahooId);

    // Next input happens to be the same as yahoo.
    bsl::strcpy(input, "http://www.yahoo.com/xsd/searchResults.xsd");
    int id2 = namespaceRegistry.lookupOrRegister(input);
    ASSERT(id2 == yahooId);
//..
// If one of the preregistered namespaces is presented, it's predefined ID is
// returned, even though it was never explicitly registered:
//..
    bsl::strcpy(input, "http://www.w3.org/2001/XMLSchema");
    int id3 = namespaceRegistry.lookupOrRegister(input);
    ASSERT(id3 == balxml::NamespaceRegistry::e_XMLSCHEMA);
//..
// Using the 'lookup' method, a namespace ID can be looked up without
// registering it.  In this case, an unregistered namespace will result in an
// ID of -1:
//..
    ASSERT(googleId  == namespaceRegistry.lookup(googleUri));
    ASSERT(balxml::NamespaceRegistry::e_BDEM ==
           namespaceRegistry.lookup("http://bloomberg.com/schemas/bdem"));
    ASSERT(-1 == namespaceRegistry.lookup("urn:1234"));
//..
// There is also a 'lookup' method for performing the reverse mapping -- from
// ID to URI:
//..
    const char *uri = namespaceRegistry.lookup(googleId);
    ASSERT(0 == bsl::strcmp(uri, googleUri));
//..
      } break;

      case 5:
      case 4:
      case 3:
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'lookupOrRegister':
        //   Verify 'lookupOrRegister'.
        //
        // Concerns:
        //   That 'lookupOrRegister' works correctly in various white-box
        //   states.
        //
        // Plan:
        //   Create 3 namespace N, N1 and N2.
        //
        //   Lookup 'N' in the registry when registry is empty and then verify
        //   the result.
        //
        //   Lookup 'N' in the registry when registry has [N] in it and
        //   then verify the result.
        //
        //   Lookup 'N' in the registry when registry has [N1] in it
        //   and then verify the result.
        //
        //   Lookup 'N' in the registry when registry has [N1, N2] in it
        //   and then verify the result.
        //
        //   Lookup 'N' in the registry when registry has [N, N2] in it
        //   and then verify the result.
        //
        //   Lookup 'N' in the registry when registry has [N1, N] in it
        //   and then verify the result.
        //
        // Testing:
        //   int lookupOrRegister(const bsl::string& namespaceUri);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'lookupOrRegister'" << endl
                          << "================" << endl;

        static const char N[]  = "http://google.com/xml";
        static const char N1[] = "http://google.com/xml1";
        static const char N2[] = "http://google.com/xml2";

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));

          ASSERT(1 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(2 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          Obj x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));
          ASSERT(1 == x.lookupOrRegister(N));

          ASSERT(1 == x.lookupOrRegister(N));
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   That basic essential functionality is operational.
        //
        // Plan:
        //   Invoke each of the basic functions of 'balxml::NamespaceRegistry'
        //   under a combination of the following conditions:
        //   - An empty registry
        //   - A non-empty registry
        //   - A new namespace
        //   - A previously-registered namespace
        //   - A preregistered namespace
        //   - A URI string that is empty
        //   - An ID integer that is -1
        //   Assert that the results are as expected in each case.
        //
        // Testing:
        //     BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        const char N1[] = "http://google.com/xml1";
        const char N2[] = "http://google.com/xml2";
        const char XX[] = "urn:xx";

        bslma::TestAllocator tb;
        bslma::TestAllocator tc;

        {
            Obj x(&ta); const Obj& X = x;
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == ta.numBlocksInUse());

            int i = -1;

            i = X.lookup(N1); ASSERT(-1 == i);
            i = X.lookup(N2); ASSERT(-1 == i);
            i = X.lookup(XX); ASSERT(-1 == i);
            i = X.lookup(""); ASSERT(-1 == i);

            i = x.lookupOrRegister(N1); ASSERT(0 == i);
            i = x.lookupOrRegister(N2); ASSERT(1 == i);
            i = x.lookupOrRegister(""); ASSERT(-1 == i);

            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 <  ta.numBlocksInUse());

            i = X.lookup(N1); ASSERT(0 == i);
            i = X.lookup(N2); ASSERT(1 == i);
            i = X.lookup(XX); ASSERT(-1 == i);
            i = X.lookup(""); ASSERT(-1 == i);

            i = x.lookupOrRegister(N1); ASSERT(0 == i);
            i = x.lookupOrRegister(N2); ASSERT(1 == i);
            i = x.lookupOrRegister(""); ASSERT(-1 == i);

            i = X.lookup(N1); ASSERT(0 == i);
            i = X.lookup(N2); ASSERT(1 == i);
            i = X.lookup(XX); ASSERT(-1 == i);
            i = X.lookup(""); ASSERT(-1 == i);

            ASSERT(0 == bsl::strcmp(N1, X.lookup(0)));
            ASSERT(0 == bsl::strcmp(N2, X.lookup(1)));
            ASSERT(0 == bsl::strcmp("", X.lookup(3)));
            ASSERT(0 == bsl::strcmp("", X.lookup(-1)));

            // Look up preregistered namespaces
            i = X.lookup("http://www.w3.org/XML/1998/namespace");
            ASSERT(Obj::e_XML == i);
            const char *s = X.lookup(i);
            ASSERT(0 == bsl::strcmp("http://www.w3.org/XML/1998/namespace",s));

            i = X.lookup("http://bloomberg.com/schemas/bdem");
            ASSERT(Obj::e_BDEM == i);
            s = X.lookup(i);
            ASSERT(0 == bsl::strcmp("http://bloomberg.com/schemas/bdem", s));

            // Copy construct and compare for equality
            const int aAllocs = ta.numBlocksTotal();
            Obj y(x, &tb); const Obj& Y = y;
            ASSERT(aAllocs == ta.numBlocksTotal());
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 <  tb.numBlocksInUse());
            i = Y.lookup(N1); ASSERT(0 == i);
            i = Y.lookup(N2); ASSERT(1 == i);
            i = Y.lookup(XX); ASSERT(-1 == i);
            i = Y.lookup(""); ASSERT(-1 == i);
            ASSERT(Y == X);

            // Assignment
            Obj z(&tc); const Obj& Z = z;
            ASSERT(Y != Z);
            z = x;
            ASSERT(aAllocs == ta.numBlocksTotal());
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 <  tc.numBlocksInUse());
            i = Z.lookup(N1); ASSERT(0 == i);
            i = Z.lookup(N2); ASSERT(1 == i);
            i = Z.lookup(XX); ASSERT(-1 == i);
            i = Z.lookup(""); ASSERT(-1 == i);
            ASSERT(Y == Z);

            // Test reset
            x.reset();
            i = X.lookup(N1); ASSERT(-1 == i);
            i = X.lookup(N2); ASSERT(-1 == i);
            i = X.lookup("http://www.w3.org/XML/1998/namespace");
            ASSERT(Obj::e_XML == i);
        }
        ASSERT(0 == da.numBlocksInUse());
        ASSERT(0 == ta.numBlocksInUse());
        ASSERT(0 == tb.numBlocksInUse());

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
