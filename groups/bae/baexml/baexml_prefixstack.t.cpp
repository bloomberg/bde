// baexml_prefixstack.t.cpp      -*-C++-*-
#include <baexml_prefixstack.h>
#include <baexml_namespaceregistry.h>

#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

//=============================================================================
//                                   TEST PLAN
//-----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// Testing is divided into following two independent parts.
// (1) Testing 'lookup' in [02].
//
// (2) Testing 'pushPrefix', 'lookupByPrefix' and 'popPrefix'.
// It is further divided into following parts.
//   First the primary manipulator 'pushPrefix' is tested.
//
//   The primary manipulator is used to test accessor 'lookupByPrefix'.
//
//   The primary manipulator is used to test other manipulator
//   'popPrefix'.
//-----------------------------------------------------------------------------
// CREATORS
// [01] baexml_NamespaceRegistry(bslma_Allocator* allocator = 0);
// [01] ~baexml_NamespaceRegistry();
//
// MANIPULATORS
// [02] int lookup(const bsl::string& namespaceUri);
// [03] int pushPrefix(const bsl::string& prefix,
//                         const bsl::string& namespaceUri);
// [05] void popPrefix(const bsl::string& prefix);
//
// ACCESSORS
// [04] int lookupByPrefix(const bsl::string& prefix) const;
//-----------------------------------------------------------------------------
// [01] BREATHING TEST
// [06] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
                          << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\n";           \
                aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J    \
                         << "\t" << #K << ": " << K << "\t" << #L << ": " \
                         << L << "\t" << #M << ": " << M << "\n";         \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J     \
                         << "\t" << #K << ": " << K << "\t" << #L << ": "  \
                         << L << "\t" << #M << ": " << M << "\t" << #N     \
                         << ": " << N << "\n"; aSsErT(1, #X, __LINE__); } }

// Allow compilation of individual test-cases (for test drivers that take a
// very long time to compile).  Specify '-DSINGLE_TEST=<testcase>' to compile
// only the '<testcase>' test case.
#define TEST_IS_ENABLED(num) (! defined(SINGLE_TEST) || SINGLE_TEST == (num))

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl; // Print ID and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;  // Quote ID literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << flush; // P(X) w/o '\n'
#define L_ __LINE__                                // current Line number
#define T_ bsl::cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//          GLOBAL TYPEDEFS/CONSTANTS/VARIABLES/FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef baexml_NamespaceRegistry Registry;

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
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

        bslma_Allocator *allocator = 0;
        baexml_NamespaceRegistry namespaceRegistry(allocator);
        baexml_PrefixStack z(&namespaceRegistry,allocator);

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
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));

          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P2, N2));

          ASSERT(-1 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(1 == z.pushPrefix(P2, N2));

          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
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
        //   int pushPrefix(const bdeut_StringRef& prefix,
        //                  const bdeut_StringRef& namespaceUri);
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
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        // invoke 'pushPrefix(P, N)' when both P and N exist in the
        // registry and P is associated with N
        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(1 == z.pushPrefix(P1, N1));

          ASSERT(0 == z.pushPrefix(P, N));
          ASSERT(0 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P, N));

          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(1 == z.lookupNamespaceId(P));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P1, N1));
          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(2 == z.pushPrefix(P2, N2));

          ASSERT(1 == z.pushPrefix(P, N));
        }

        // invoke 'pushPrefix(P, N)' when P exist in the registry but
        // is associated with N1
        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
          ASSERT(0 == z.pushPrefix(P, N1));

          ASSERT(1 == z.pushPrefix(P, N));
          ASSERT(1 == z.lookupNamespaceId(P));
        }

        // invoke 'pushPrefix(P, N)' when N exist in the registry but
        // N does not exist
        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          baexml_PrefixStack z(&x,&ta);
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
        //   int lookupNamespaceId(const bdeut_StringRef& namespaceUri);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING 'lookupNamespaceId'"
                               << "\n===========================" << bsl::endl;

        const bsl::string N  = "http://google.com/xml";
        const bsl::string N1 = "http://google.com/xml1";
        const bsl::string N2 = "http://google.com/xml2";

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(0 == x.lookupOrRegister(N));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));

          ASSERT(1 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N1));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(2 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
          ASSERT(0 == x.lookupOrRegister(N));
          ASSERT(1 == x.lookupOrRegister(N2));

          ASSERT(0 == x.lookupOrRegister(N));
        }

        {
          bslma_TestAllocator ta(veryVeryVerbose);
          baexml_NamespaceRegistry x(&ta);
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

        bslma_TestAllocator ta(veryVeryVerbose);
        baexml_NamespaceRegistry x(&ta);
        baexml_PrefixStack  y(&x, &ta);

        int i = -1;
        i = x.lookupOrRegister(N1); ASSERT(0 == i);
        i = x.lookupOrRegister(N2); ASSERT(1 == i);

        ASSERT(0 == y.numPrefixes());

        ASSERT(Registry::BAEXML_XML == y.lookupNamespaceId("xml"));
        ASSERT(Registry::BAEXML_XMLNS == y.lookupNamespaceId("xmlns"));
        ASSERT(Registry::BAEXML_XMLSCHEMA_INSTANCE ==
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

        ASSERT(0 == strcmp(X1, y.lookupNamespaceUri(Registry::BAEXML_XML)));
        ASSERT(0 == strcmp(X2, y.lookupNamespaceUri(Registry::BAEXML_XMLNS)));
        ASSERT(0 == strcmp(X3,
                   y.lookupNamespaceUri(Registry::BAEXML_XMLSCHEMA_INSTANCE)));

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
