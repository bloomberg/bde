// bdlb_topologicalsortutil.t.cpp                                     -*-C++-*-

#include <bdlb_topologicalsortutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_vector.h>
#include <bsl_queue.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;
using namespace bdlb;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test... TODO
//
//-----------------------------------------------------------------------------
// [0] BREATHING TEST
// [1] constructors
// [8] ....
//-----------------------------------------------------------------------------


//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }


//=============================================================================
//                    STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " \
          << J << "\t" << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t" << #J << ": " << J \
          << "\t" << #K << ": " << K << "\t" << #L << ": " << L \
        << "\n"; aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                    SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value
#define Q(X) cout << "<! " #X " |>" << endl;  // Quote identifier literally
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) wihtout '\n'
#define L_ __LINE__
#define _T() cout << "\t" << flush;           // Print tab w/o newline


//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                                   MAIN
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
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
///Example 1: Using topological sort for calculating formulas
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are evaluating formulas for a set of market data fields, where
// formulas can reference other firelds as part of their calculation.  As an
// example let's say we have a field k_bbgDefinedVwap which is dependent on
// k_vwapTurnover and k_vwapVolume.  These fields in turn are dependent on
// k_tradeSize and k_tradePrice respectively.  So essentially the fields which
// are not dependent on any other field should be caluclated first and then the
// dependent fields.  We can use the topological sort utility to provide us the
// order in which these fields should be calculated.
//
// First, we create the relations showcasing the above mentioned dependencies
// in the form of pairs (a,b) where b is dependent on a:
//..
    enum FieldIds {
        k_bbgDefinedVwap  = 0,
        k_vwapTurnover    = 1,
        k_vwapVolume      = 2,
        k_tradeSize       = 3,
        k_tradePrice      = 4
    };

    bsl::vector<bsl::pair<int, int> > relations;

    relations.push_back(bsl::make_pair((int) k_vwapTurnover,
                                       (int) k_bbgDefinedVwap));
    relations.push_back(bsl::make_pair((int) k_vwapVolume,
                                       (int) k_bbgDefinedVwap));
    relations.push_back(bsl::make_pair((int) k_tradeSize,
                                       (int) k_vwapVolume));
    relations.push_back(bsl::make_pair((int) k_tradeSize,
                                       (int) k_vwapTurnover));
    relations.push_back(bsl::make_pair((int) k_tradePrice,
                                       (int) k_vwapTurnover));
//..
// Now, we call the topological sort to get a topological order for the fields
// referenced in the relations:
//..
    bsl::vector<int> results;
    bsl::vector<int> unordered;
    bool sorted = TopologicalSortUtil::sort(&results, &unordered, relations);
//..
// Finally, we verify that the order of the fields that the sort returns is
// topologically correct:
//..
    bool calculated[5] = { 0 };
    ASSERT(sorted == true);
    ASSERT(unordered.empty());

    for (bsl::vector<int>::const_iterator iter = results.begin(),
                                          end  = results.end();
         iter != end; ++iter) {
        switch (*iter) {
          case k_bbgDefinedVwap: {
            ASSERT(calculated[k_vwapTurnover] == true);
            ASSERT(calculated[k_vwapVolume]   == true);

            calculated[k_bbgDefinedVwap] = true;
          } break;
          case k_vwapTurnover: {
            ASSERT(calculated[k_tradeSize]  == true);
            ASSERT(calculated[k_tradePrice] == true);

            calculated[k_vwapTurnover] = true;
          } break;
          case k_vwapVolume: {
            ASSERT(calculated[k_tradeSize]  == true);

            calculated[k_vwapVolume] = true;
          } break;
          case k_tradeSize: {
            ASSERT(calculated[k_vwapVolume]   == false);
            ASSERT(calculated[k_vwapTurnover] == false);

            calculated[k_tradeSize] = true;
          } break;
          case k_tradePrice: {
            ASSERT(calculated[k_vwapTurnover] == false);

            calculated[k_tradePrice] = true;
          } break;
          default:
            ASSERT(false);
            break;
        };
    }
//..
///Example 2: Using topological sort with cycles in input
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which contain a cycle.
//
// First, we define a set of inputs which have a cycle:
//..
    enum FieldIds2 {
        FIELD1 = 0,
        FIELD2 = 1,
        FIELD3 = 2,
    };

    bsl::vector<bsl::pair<int, int> > relations2;

    relations2.push_back(bsl::make_pair((int) FIELD2,
                                        (int) FIELD1));
    relations2.push_back(bsl::make_pair((int) FIELD3,
                                        (int) FIELD2));
    relations2.push_back(bsl::make_pair((int) FIELD1,
                                        (int) FIELD3));
//..
// Now, we apply the topological sort routine on the input:
//..
    bsl::vector<int> results2;
    bsl::vector<int> unordered2;
    bool sorted2 = TopologicalSortUtil::sort(&results2, &unordered2, relations2);
//..
// Finally, we verify whether the routine recognizes that there is a cycle and
// returns false:
//..
    ASSERT(sorted2           == false);
    ASSERT(unordered2.size() == 3);
//..
///Example 3: Using topological sort with self relations
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which have input relations where predecessor
// and successor point to the same value. i.e. we have pairs of input like (u,u)
// First, we define such a set of inputs
//..
    enum FieldIds3 {
        FIELD4 = 3,
        FIELD5 = 4,
        FIELD6 = 5,
    };

    bsl::vector<bsl::pair<int, int> > relations3;

    relations3.push_back(bsl::make_pair((int) FIELD4,
                                        (int) FIELD6));
    relations3.push_back(bsl::make_pair((int) FIELD5,
                                        (int) FIELD4));
    relations3.push_back(bsl::make_pair((int) FIELD4,
                                        (int) FIELD4));
//..
// Now, we apply the topological sort routine on the input:
//..
    bsl::vector<int> results3;
    bsl::vector<int> unordered3;
    bool sorted3 = TopologicalSortUtil::sort(&results3, &unordered3, relations3);
//..
// Finally, we verify that the self relations causes the cycle:
//..
    ASSERT(sorted3           == false);
    ASSERT(results3.size()   == 1);
    ASSERT(unordered3.size() == 2);

    if (veryVeryVerbose) {
        cout << "Size of results3 vector is "
             << results3.size() << endl;

        cout << "Size of unordered3 vector is "
             << unordered3.size() << endl;
    }

    if (veryVeryVerbose)
    {
        for (int i = 0; i < results3.size(); ++i) {
                cout << "results3[" << i << "] is " << results3[i] << "\n";
        }

        for (int i = 0; i < unordered3.size(); ++i) {
                cout << "unordered3[" << i << "] is " << unordered3[i] << "\n";
        }
    }
//..
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
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
      } break;
      default: {
        cerr << "WARNING: CASE '" << test << "' NOT FOUND." << endl;
        testStatus = -1;
        break;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2017
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
