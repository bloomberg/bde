// bdlmxxx_properties.t.cpp                                           -*-C++-*-

#include <bdlmxxx_properties.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//                                  TBD
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
            bdlmxxx::DescriptorStreamIn<bsl::istream> xyz;
            bdlmxxx::DescriptorStreamOut<bsl::ostream> abc;
            xyz.streamIn  = 0;  ASSERT(0 == xyz.streamIn);   // use xyz
            abc.streamOut = 0;  ASSERT(0 == abc.streamOut);  // use abc

            const bdlmxxx::Descriptor& int64attr = bdlmxxx::Properties::s_int64Attr;
            if (veryVerbose) {
                bsl::cout << "Int64 size = "  << int64attr.d_size
                          << ", alignment = " << int64attr.d_alignment
                          << bsl::endl;
            }
            int i = 0;
            bdlmxxx::Properties::
              d_intAttr.unsetConstruct(&i,
                                       bdlmxxx::AggregateOption::BDEM_PASS_THROUGH,
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
                const bdlmxxx::Descriptor& d0  = bdlmxxx::Properties::s_voidAttr;
                const bdlmxxx::Descriptor& d1  = bdlmxxx::Properties::s_boolAttr;
                const bdlmxxx::Descriptor& d2  = bdlmxxx::Properties::s_charAttr;
                const bdlmxxx::Descriptor& d3  = bdlmxxx::Properties::s_shortAttr;
                const bdlmxxx::Descriptor& d4  = bdlmxxx::Properties::s_intAttr;
                const bdlmxxx::Descriptor& d5  = bdlmxxx::Properties::s_int64Attr;
                const bdlmxxx::Descriptor& d6  = bdlmxxx::Properties::s_floatAttr;
                const bdlmxxx::Descriptor& d7  = bdlmxxx::Properties::s_doubleAttr;
                const bdlmxxx::Descriptor& d8  = bdlmxxx::Properties::s_stringAttr;
                const bdlmxxx::Descriptor& d9  = bdlmxxx::Properties::s_datetimeAttr;
                const bdlmxxx::Descriptor& d10 = bdlmxxx::Properties::s_datetimeTzAttr;
                const bdlmxxx::Descriptor& d11 = bdlmxxx::Properties::s_dateAttr;
                const bdlmxxx::Descriptor& d12 = bdlmxxx::Properties::s_dateTzAttr;
                const bdlmxxx::Descriptor& d13 = bdlmxxx::Properties::s_timeAttr;
                const bdlmxxx::Descriptor& d14 = bdlmxxx::Properties::s_timeTzAttr;
            }

            {
                const bdlmxxx::Descriptor& d1  = bdlmxxx::Properties::s_boolArrayAttr;
                const bdlmxxx::Descriptor& d2  = bdlmxxx::Properties::s_charArrayAttr;
                const bdlmxxx::Descriptor& d3  = bdlmxxx::Properties::s_shortArrayAttr;
                const bdlmxxx::Descriptor& d4  = bdlmxxx::Properties::s_intArrayAttr;
                const bdlmxxx::Descriptor& d5  = bdlmxxx::Properties::s_int64ArrayAttr;
                const bdlmxxx::Descriptor& d6  = bdlmxxx::Properties::s_floatArrayAttr;
                const bdlmxxx::Descriptor& d7  =
                                            bdlmxxx::Properties::s_doubleArrayAttr;
                const bdlmxxx::Descriptor& d8  =
                                            bdlmxxx::Properties::s_stringArrayAttr;
                const bdlmxxx::Descriptor& d9  =
                                          bdlmxxx::Properties::s_datetimeArrayAttr;
                const bdlmxxx::Descriptor& d10 =
                                        bdlmxxx::Properties::s_datetimeTzArrayAttr;
                const bdlmxxx::Descriptor& d11 = bdlmxxx::Properties::s_dateArrayAttr;
                const bdlmxxx::Descriptor& d12 =
                                            bdlmxxx::Properties::s_dateTzArrayAttr;
                const bdlmxxx::Descriptor& d13 = bdlmxxx::Properties::s_timeArrayAttr;
                const bdlmxxx::Descriptor& d14 =
                                            bdlmxxx::Properties::s_timeTzArrayAttr;
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

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
