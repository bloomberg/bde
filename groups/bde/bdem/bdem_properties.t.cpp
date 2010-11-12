// bdem_properties.t.cpp                  -*-C++-*-

#include <bdem_properties.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// TBD
//-----------------------------------------------------------------------------
// [  ]
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_  cout << "\t" << flush;            // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//             GLOBAL HELPER FUNCTIONS AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4; // globalVeryVeryVerbose =
                                       //                      veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
    case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is provided to facilitate development.  Note that this
        //   breathing test exercises basic functionality, but tests nothing.
        //   Note also that testing for 'const'-correctness of copy
        //   construction and assuring compile-time errors for re-binding
        //   (assignment) are negative tests that currently must be performed
        //   manually by selectively uncommenting code.
        //
        // Plan:
        //   Do what ever is necessary to bring this component to a level
        //   where it can be tested thoroughly.
        //
        // Tactics:
        //   - Ad-Hoc test data selection method
        //   - Bruit-Force and Loop-Based implementation techniques
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;
        {
            bdem_DescriptorStreamIn<bsl::istream> xyz;
            bdem_DescriptorStreamOut<bsl::ostream> abc;
            xyz.streamIn  = 0;  ASSERT(0 == xyz.streamIn);   // use xyz
            abc.streamOut = 0;  ASSERT(0 == abc.streamOut);  // use abc

            const bdem_Descriptor& int64attr = bdem_Properties::d_int64Attr;
            if (veryVerbose) {
                bsl::cout << "Int64 size = "  << int64attr.d_size
                          << ", alignment = " << int64attr.d_alignment
                          << bsl::endl;
            }
            int i = 0;
            bdem_Properties::
              d_intAttr.unsetConstruct(&i,
                                       bdem_AggregateOption::BDEM_PASS_THROUGH,
                                       0);
            if (veryVerbose) {
                bsl::cout << "Unset 'int' value = " << i << bsl::endl;
            }
        }
        if (veryVerbose) {
            cout << "\tTest that all properties are defined" << endl;
        }
        {
            {
                const bdem_Descriptor& d0  = bdem_Properties::d_voidAttr;
                const bdem_Descriptor& d1  = bdem_Properties::d_boolAttr;
                const bdem_Descriptor& d2  = bdem_Properties::d_charAttr;
                const bdem_Descriptor& d3  = bdem_Properties::d_shortAttr;
                const bdem_Descriptor& d4  = bdem_Properties::d_intAttr;
                const bdem_Descriptor& d5  = bdem_Properties::d_int64Attr;
                const bdem_Descriptor& d6  = bdem_Properties::d_floatAttr;
                const bdem_Descriptor& d7  = bdem_Properties::d_doubleAttr;
                const bdem_Descriptor& d8  = bdem_Properties::d_stringAttr;
                const bdem_Descriptor& d9  = bdem_Properties::d_datetimeAttr;
                const bdem_Descriptor& d10 = bdem_Properties::d_datetimeTzAttr;
                const bdem_Descriptor& d11 = bdem_Properties::d_dateAttr;
                const bdem_Descriptor& d12 = bdem_Properties::d_dateTzAttr;
                const bdem_Descriptor& d13 = bdem_Properties::d_timeAttr;
                const bdem_Descriptor& d14 = bdem_Properties::d_timeTzAttr;
            }

            {
                const bdem_Descriptor& d1  = bdem_Properties::d_boolArrayAttr;
                const bdem_Descriptor& d2  = bdem_Properties::d_charArrayAttr;
                const bdem_Descriptor& d3  = bdem_Properties::d_shortArrayAttr;
                const bdem_Descriptor& d4  = bdem_Properties::d_intArrayAttr;
                const bdem_Descriptor& d5  = bdem_Properties::d_int64ArrayAttr;
                const bdem_Descriptor& d6  = bdem_Properties::d_floatArrayAttr;
                const bdem_Descriptor& d7  =
                                            bdem_Properties::d_doubleArrayAttr;
                const bdem_Descriptor& d8  =
                                            bdem_Properties::d_stringArrayAttr;
                const bdem_Descriptor& d9  =
                                          bdem_Properties::d_datetimeArrayAttr;
                const bdem_Descriptor& d10 =
                                        bdem_Properties::d_datetimeTzArrayAttr;
                const bdem_Descriptor& d11 = bdem_Properties::d_dateArrayAttr;
                const bdem_Descriptor& d12 =
                                            bdem_Properties::d_dateTzArrayAttr;
                const bdem_Descriptor& d13 = bdem_Properties::d_timeArrayAttr;
                const bdem_Descriptor& d14 =
                                            bdem_Properties::d_timeTzArrayAttr;
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error: non-zero test status = " << testStatus << "."
             << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
