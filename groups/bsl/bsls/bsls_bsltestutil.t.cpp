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
    struct bsls_BslTestUtil_FortyTwo {
        // This utility class provides sample functionality to demonstrate
        // how a test driver might be written validating its only method.

        static int value();
            // Return the integer value '42'.
    };

    inline
    int bsls_BslTestUtil_FortyTwo::value()
    {
        return 42;
    }
//..
// Then, we can write a test driver for this component.  We start by providing
// the standard BDE assert test macro.
//..
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
// Next, we define the standard print and LOOP_ASSERT macros, as aliases to the
// macros defined by this component.
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
// Finally, we write the test case for the 'value' function of this component,
// using the (standard) abbreviated macro names we have just defined.
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

        const int value = bsls_BslTestUtil_FortyTwo::value();
        if (verbose) P(value);
        LOOP_ASSERT(value, 42 == value);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   Want to observe the basic operation of this component.
        //
        // Plan:
        //   Invoke each of the "print" macros, protected by a check for
        //   'verbose' mode, and manually inspect the output to the console.
        //   Invoke each 'LOOPx_ASSERT' macro with a test that passes, and
        //   again with a test that fails only in verbose mode.  Then reset the
        //   'testStatus' count to reflect the expected number of failed
        //   'LOOPx_ASSERT's.
        //
        // Testing:
        //   BREATHING TEST
        //   BSLS_BSLTESTUTIL_P()
        //   BSLS_BSLTESTUTIL_P_()
        //   BSLS_BSLTESTUTIL_Q()
        //   BSLS_BSLTESTUTIL_T_
        //   BSLS_BSLTESTUTIL_L_
        //   BSLS_BSLTESTUTIL_LOOP_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP2_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP3_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP4_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP5_ASSERT
        //   BSLS_BSLTESTUTIL_LOOP6_ASSERT
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
        const long sl = -7654321l;
        const unsigned long ul = 7654321ul;
        const long long sll = -5123467890ll;
        const unsigned long long ull = 9876543210ull;
        const float f = 2.78f;
        const double d = 3.14159268;
        const long double ld = 1.608e300L;
        char hello[] = { 'h', 'e', 'l', 'l', 'o', '\0' };
        char *s = hello;
        const char world[] = { 'w', 'o', 'r', 'l', 'd', '\0' };
        const char *cs = world;
        void *pv = hello;
        const void *pcv = &si;

        // Check verbose so that we do not unexpectedly write to the console
        if (verbose) {
            P(b)   P(c) P(sc) P(uc) P(ss) P(us) P(si) P(ui) P(sl) P(ul) P(sll)
            P(ull) P(f) P(d)  P(ld) P(s)  P(cs) P(pv) P(pcv)

            P_(b)  P_(c)   P_(sc)  P_(uc) P_(ss) P_(us) P_(si) P_(ui) P_(sl)
            P_(ul) P_(sll) P_(ull) P_(f)  P_(d)  P_(ld) P_(s)  P_(cs) P_(pv)
            P_(pcv)

            Q(L_) T_ P(L_)
        }

        LOOP_ASSERT(b, true == b);
        LOOP2_ASSERT(c, sc, 'c' == c && -42 == sc);
        LOOP3_ASSERT(uc, ss, us,
                       142 == uc &&
                     -1234 == ss &&
                      1234 == us);
        LOOP4_ASSERT(si, ui, sl, ul,
                      -123456 == si &&
                       123456 == ui &&
                     -7654321 == sl &&
                      7654321 == ul);
        LOOP5_ASSERT(si, ui, sl, ul, sll,
                         -123456 == si &&
                          123456 == ui &&
                        -7654321 == sl &&
                         7654321 == ul &&
                     -5123467890 == sll);
        LOOP6_ASSERT(si, ui, sl, ul, sll, ull,
                         -123456 == si  &&
                          123456 == ui  &&
                        -7654321 == sl  &&
                         7654321 == ul  &&
                     -5123467890 == sll &&
                      9876543210 == ull);

        if (verbose) {
            LOOP_ASSERT(verbose, !verbose);
            LOOP2_ASSERT(verbose, c, !verbose);
            LOOP3_ASSERT(verbose, uc, sc, !verbose);
            LOOP4_ASSERT(verbose, f, d, ld, !verbose);
            LOOP5_ASSERT(verbose, s, cs, pv, pcv, !verbose);
            LOOP6_ASSERT(verbose, ss, us, sll, ull, si, !verbose);

            testStatus -= 6;
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
