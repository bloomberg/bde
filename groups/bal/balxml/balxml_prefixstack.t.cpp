// balxml_prefixstack.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_prefixstack.h>

#include <bslim_testutil.h>

#include <balxml_namespaceregistry.h>

#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing is divided into following two independent parts.
// (1) Testing 'lookup' in [02].
//
// (2) Testing 'pushPrefix', 'lookupByPrefix' and 'popPrefix'.
//   It is further divided into following parts.
//   First the primary manipulator 'pushPrefix' is tested.
//
//   The primary manipulator is used to test accessor 'lookupByPrefix'.
//
//   The primary manipulator is used to test other manipulator
//   'popPrefix'.
// ----------------------------------------------------------------------------
// CREATORS
// [01] balxml::NamespaceRegistry(bslma::Allocator *allocator = 0);
// [01] ~balxml::NamespaceRegistry();
//
// MANIPULATORS
// [02] int lookup(const bsl::string& namespaceUri);
// [03] int pushPrefix(const bsl::string& prefix,
//                         const bsl::string& namespaceUri);
// [05] void popPrefix(const bsl::string& prefix);
//
// ACCESSORS
// [04] int lookupByPrefix(const bsl::string& prefix) const;
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

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

// ============================================================================
//         GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef balxml::NamespaceRegistry Registry;

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

    switch (test) { case 0:  // Zero is always the leading case.

      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE:
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and execute as shown.
        //
        // Plan:
        //   Incorporate the usage example from the header file into the test
        //   driver.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING USAGE EXAMPLE"
                               << "\n=====================" << bsl::endl;

        bslma::Allocator *allocator = 0;
        balxml::NamespaceRegistry namespaceRegistry(allocator);
        balxml::PrefixStack z(&namespaceRegistry,allocator);

        bsl::string namespaceUri1 = "http://www.google.com";
        bsl::string namespaceUri2 = "http://www.yahoo.com";
        bsl::string namespaceUri3 = "http://www.hotmail.com";

        int namespaceId1 = namespaceRegistry.lookupOrRegister(namespaceUri1);
        int namespaceId2 = namespaceRegistry.lookupOrRegister(namespaceUri2);
        int namespaceId3 = namespaceRegistry.lookupOrRegister(namespaceUri3);

        if (verbose ) {
            bsl::cout << namespaceUri1 << ":" << namespaceId1 << bsl::endl;
            bsl::cout << namespaceUri2 << ":" << namespaceId2 << bsl::endl;
            bsl::cout << namespaceUri3 << ":" << namespaceId3 << bsl::endl;
        }
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'lookupByPrefix':
        //   Verify 'lookupByPrefix'.
        //
        // Concerns:
        //   That 'lookupByPrefix' works correctly in various white-box
        //   states.
        //
        // Plan:
        //   Create 3 prefixes P, P1 and P2.
        //
        //   Lookup 'P' in the registry when registry is empty and then verify
        //   the result.
        //
        //   Lookup 'P' in the registry when registry has [P] in it and
        //   then verify the result.
        //
        //   Lookup 'P' in the registry when registry has [P1] in it
        //   and then verify the result.
        //
        //   Lookup 'P' in the registry when registry has [P1, P2] in it
        //   and then verify the result.
        //
        //   Lookup 'P' in the registry when registry has [P, P2] in it
        //   and then verify the result.
        //
        //   Lookup 'P' in the registry when registry has [P1, P] in it
        //   and then verify the result.
        //
        // Testing:
        //   int lookupByPrefix(const bsl::string& prefix) const;
        // --------------------------------------------------------------------
        if (verbose) bsl::cout << "\nTESTING 'lookupByPrefix'"
                               << "\n========================" << bsl::endl;

        const bsl::string N  = "http://google.com/xml";
        const bsl::string N1 = "http://google.com/xml1";
        const bsl::string N2 = "http://google.com/xml2";

        const bsl::string P  = "pref";
        const bsl::string P1 = "pref1";
        const bsl::string P2 = "pref2";

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));

          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P2, N2));

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(1 == z.pushPrefix(P2, N2));

          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P, N));

          ASSERT(1 == z.lookupNamespaceId(P));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'pushPrefix':
        //   Verify 'pushPrefix'.
        //
        // Concerns:
        //   That 'pushPrefix' works correctly in various white-box
        //   states.
        //
        // Plan:
        //   invoke 'pushPrefix(P, N)' when neither P nor N exists in the
        //   registry and then verify the result.
        //
        //   invoke 'pushPrefix(P, N)' when both P and N exist in the
        //   registry and P is associated with N and then verify the result.
        //
        //   invoke 'pushPrefix(P, N)' when P exist in the registry but
        //   is associated with N1 and then verify the result.
        //
        //   invoke 'pushPrefix(P, N)' when P exist in the registry but
        //   is associated with N1 and then verify the result.
        //
        // Testing:
        //   int pushPrefix(const bslstl::StringRef& prefix,
        //                  const bslstl::StringRef& namespaceUri);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'pushPrefix'"
                               << "\n====================" << bsl::endl;

        const bsl::string N  = "http://google.com/xml";
        const bsl::string N1 = "http://google.com/xml1";
        const bsl::string N2 = "http://google.com/xml2";

        const bsl::string P  = "pref";
        const bsl::string P1 = "pref1";
        const bsl::string P2 = "pref2";

        // invoke 'pushPrefix(P, N)' when neither P nor N exists in the
        // registry
        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        // invoke 'pushPrefix(P, N)' when both P and N exist in the registry
        // and P is associated with N
        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(1 == z.pushPrefix(P1, N1));

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P, N));

          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(1 == z.lookupNamespaceId(P));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(2 == z.pushPrefix(P2, N2));

          ASSERT(1 == z.pushPrefix(P, N));
        }

        // invoke 'pushPrefix(P, N)' when P exist in the registry but is
        // associated with N1
        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N1));

          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(1 == z.lookupNamespaceId(P));
        }

        // invoke 'pushPrefix(P, N)' when N exist in the registry but
        // N does not exist
        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          balxml::PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N));

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'lookupNamespaceId':
        //   Verify 'lookupNamespaceId'.
        //
        // Concerns:
        //   That 'lookupNamespaceId' works correctly in various white-box
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
        //   int lookupNamespaceId(const bslstl::StringRef& namespaceUri);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'lookupNamespaceId'"
                               << "\n===========================" << bsl::endl;

        const bsl::string N  = "http://google.com/xml";
        const bsl::string N1 = "http://google.com/xml1";
        const bsl::string N2 = "http://google.com/xml2";

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));

          ASSERT(1 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(2 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma::TestAllocator ta(veryVeryVerbose);
          balxml::NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));
          ASSERT(1 == x.lookupOrRegister(N));

          ASSERT(1 == x.lookupOrRegister(N));
        }

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise the basic functionality.
        //
        // Concerns:
        //   That basic essential functionality is operational
        //   for one thread.
        //
        // Plan:
        //   Create 3 namespaces N1, N2, N3, 3 prefixes P1, P2, P3 and a
        //   registry x.
        //
        //   Invoke  'lookup' (3 times) on registry passing N1, N2 and N3 and
        //   verify the result.  Invoke 'lookup' again (3 times) on
        //   registry passing N1, N2 and N3 and verify the result.
        //
        //   Invoke 'z.pushPrefix(P1, N1)' and verify the result.
        //   Invoke 'z.pushPrefix(P2, N2)' and verify the result.
        //   Invoke 'z.pushPrefix(P3, N3)' and verify the result.
        //
        //   Invoke 'z.lookupNamespaceId(P1, N1)' and verify the result.
        //   Invoke 'z.lookupNamespaceId(P2, N2)' and verify the result.
        //   Invoke 'z.lookupNamespaceId(P3, N3)' and verify the result.
        //
        //   Now unregister P1, P2 and P3.  Invoke 'z.lookupNamespaceId(P1,
        //   N1)' and verify the result.  Invoke 'z.lookupNamespaceId(P2, N2)'
        //   and verify the result.  Invoke 'z.lookupNamespaceId(P3, N3)' and
        //   verify the result.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        const bsl::string  N1 = "http://google.com/xml1";
        const char        *N2 = "http://google.com/xml2";
        const bsl::string  N3 = "http://google.com/xml3";

        const char        *P1 = "pref1";
        const bsl::string  P2 = "pref2";
        const bsl::string  P3 = "pref3";

        bslma::TestAllocator ta(veryVeryVerbose);
        balxml::NamespaceRegistry x(&ta);
        balxml::PrefixStack  y(&x, &ta);

        int i = -1;
        i = x.lookupOrRegister(N1); ASSERT(0 == i);
        i = x.lookupOrRegister(N2); ASSERT(1 == i);

        ASSERT(0 == y.numPrefixes());

        ASSERT(Registry::e_XML == y.lookupNamespaceId("xml"));
        ASSERT(Registry::e_XMLNS == y.lookupNamespaceId("xmlns"));
        ASSERT(Registry::e_XMLSCHEMA_INSTANCE ==
                                                   y.lookupNamespaceId("xsi"));

        ASSERT(0 == strcmp("xml", y.lookupNamespacePrefix("xml")));
        ASSERT(0 == strcmp("xmlns", y.lookupNamespacePrefix("xmlns")));
        ASSERT(0 == strcmp("xsi", y.lookupNamespacePrefix("xsi")));

        const char *X1 = "http://www.w3.org/XML/1998/namespace";
        const char *X2 = "http://www.w3.org/2000/xmlns/";
        const char *X3 = "http://www.w3.org/2001/XMLSchema-instance";

        ASSERT(0 == strcmp(X1, y.lookupNamespaceUri("xml")));
        ASSERT(0 == strcmp(X2, y.lookupNamespaceUri("xmlns")));
        ASSERT(0 == strcmp(X3, y.lookupNamespaceUri("xsi")));

        ASSERT(0 == strcmp(X1, y.lookupNamespaceUri(Registry::e_XML)));
        ASSERT(0 == strcmp(X2, y.lookupNamespaceUri(Registry::e_XMLNS)));
        ASSERT(0 == strcmp(X3,
                   y.lookupNamespaceUri(Registry::e_XMLSCHEMA_INSTANCE)));

        ASSERT(0 == strcmp("xml", y.lookupNamespacePrefix("xml")));
        ASSERT(0 == strcmp("xmlns", y.lookupNamespacePrefix("xmlns")));
        ASSERT(0 == strcmp("xsi", y.lookupNamespacePrefix("xsi")));

        ASSERT(-1 == y.lookupNamespaceId(P1));
        ASSERT(-1 == y.lookupNamespaceId(P2));
        ASSERT(-1 == y.lookupNamespaceId(P3));

        ASSERT(0 == y.pushPrefix(P1, N1));
        ASSERT(1 == y.pushPrefix(P2, N2));
        ASSERT(2 == y.pushPrefix(P3, N3));

        ASSERT(0 == y.lookupNamespaceId(P1));
        ASSERT(1 == y.lookupNamespaceId(P2));
        ASSERT(2 == y.lookupNamespaceId(P3));

        y.popPrefixes(3);

        ASSERT(-1 == y.lookupNamespaceId(P1));
        ASSERT(-1 == y.lookupNamespaceId(P2));
        ASSERT(-1 == y.lookupNamespaceId(P3));

      } break;

      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
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
