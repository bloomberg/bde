// bsls_bsltestutil.t.cpp                                             -*-C++-*-

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
//-----------------------------------------------------------------------------
// [ ]
// [ ]
// [ ]
//-----------------------------------------------------------------------------
// [ 1] Breathing test
// [ 2] Usage example
//-----------------------------------------------------------------------------
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Writing a test driver
/// - - - - - - - - - - - - - - - -
// First we write a component to test, which might hold the following utility
// class.
//..
//..
// Then we can write a test driver for this component.
//=============================================================================
//                       STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//..
// Next...
//..
//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number
//..
// Then...
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    setbuf(stdout, 0);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
       case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING USAGE EXAMPLE"
                            "\n---------------------\n");

  
       } break;
       case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        const bool b = true;
        const char c = 'c';
        const signed char sc = -42;
        const unsigned char uc = 142;
        const short ss = -1234;
        const unsigned short us = 1234;
        const int si = -123456;
        const unsigned int ui = 123456;
        const long sl = -7654321;
        const unsigned long ul = 7654321;
        const long long sll = -5123467890;
        const unsigned long long ull = 9876543210;
        const float f = 2.78f;
        const double d = 3.14159268;
        const long double ld = 1.608e300L;
        char hello[] = { 'h', 'e', 'l', 'l', 'o', '\0' };
        char *s = hello;
        const char world[] = { 'w', 'o', 'r', 'l', 'd', '\0' };
        const char *cs = world;
        void *pv = hello;
        const void *pcv = &si;

        // Check verbose so that we do not write to the console
        if (verbose) {
            P(b)   P(c) P(sc) P(uc) P(ss) P(us) P(si) P(ui) P(sl) P(ul) P(sll)
            P(ull) P(f) P(d)  P(ld) P(s)  P(cs) P(pv) P(pcv)

            P_(b)  P_(c)   P_(sc)  P_(uc) P_(ss) P_(us) P_(si) P_(ui) P_(sl)
            P_(ul) P_(sll) P_(ull) P_(f)  P_(d)  P_(ld) P_(s)  P_(cs) P_(pv)
            P_(pcv)
        }

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
