// bdlb_topologicalsortutil.t.cpp                                     -*-C++-*-

#include <bdlb_topologicalsortutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_queue.h>
#include <bsl_string.h>
#include <bsl_vector.h>

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
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__
#define _T() cout << "\t" << flush;           // Print tab w/o newline


//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

struct Node {
    // A simple node structure for testing explicit HASH and EQUALS functors.

    // DATA
    int d_number;

    // CREATORS
    Node()
    : d_number(-1)
    {
    }

    Node(int number)
    : d_number(number)
    {
    }
};

struct NodeHash {
    // The explicit HASH functor for the simple node structure.

    bsl::size_t operator()(const Node &node) const {
        return bsl::hash<int>()(node.d_number);
    }
};

struct NodeEqual {
    // The explicit EQUALS functor for the simple node structure.

    typedef Node first_argument_type;
    typedef Node second_argument_type;
    typedef bool result_type;

    bool operator()(const Node &lhs, const Node &rhs) const {
        return lhs.d_number == rhs.d_number;
    }
};

//=============================================================================
//                                   MAIN
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
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
// formulas can reference other fields as part of their calculation.  As an
// example let's say we have a field k_bbgDefinedVwap which is dependent on
// k_vwapTurnover and k_vwapVolume.  These fields in turn are dependent on
// k_tradeSize and k_tradePrice respectively.  So essentially the fields which
// are not dependent on any other field should be calculated first and then the
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
    bool sorted2 = TopologicalSortUtil::sort(&results2,
                                             &unordered2,
                                             relations2);
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
// and successor point to the same value. i.e. we have pairs of input like
// (u,u).  First, we define such a set of inputs
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
    bool sorted3 = TopologicalSortUtil::sort(&results3,
                                             &unordered3,
                                             relations3);
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
      case 4: {
        // --------------------------------------------------------------------
        // EXPLICIT HASH AND EQUAL TEST
        //   This case tests sorting with explicitly specified HASH and EQUAL
        //   functors (for the 'unordered_map' used during sorting.
        //
        // Concerns:
        //: 1 The code successfully compiles and links.
        //:
        //: 2 The graphs is successfully topologically sorted.
        //:
        //: 2 No nodes are missing from the result.
        //
        // Testing:
        //   sort
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXPLICIT HASH AND EQUAL TEST" << endl
                          << "============================" << endl;

        bsl::vector<bsl::pair<Node, Node> > relations;

        relations.push_back(bsl::make_pair<Node, Node>(1, 2));
        relations.push_back(bsl::make_pair<Node, Node>(2, 3));
        relations.push_back(bsl::make_pair<Node, Node>(4, 5));

        bsl::vector<Node> results;
        bsl::vector<Node> unordered;
        bool sorted = TopologicalSortUtil::sort<Node, NodeHash, NodeEqual>(
                                                                    &results,
                                                                    &unordered,
                                                                    relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 5);

        LOOP_ASSERT(results[0].d_number, results[0].d_number == 4);
        LOOP_ASSERT(results[1].d_number, results[1].d_number == 1);
        LOOP_ASSERT(results[2].d_number, results[2].d_number == 5);
        LOOP_ASSERT(results[3].d_number, results[3].d_number == 2);
        LOOP_ASSERT(results[4].d_number, results[4].d_number == 3);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TWO GRAPHS TEST
        //   This case tests sorting of two disconnected graphs.
        //
        // Concerns:
        //: 1 The two graphs are successfully topologically sorted.
        //:
        //: 2 No nodes are missing from the result.
        //
        // Testing:
        //   sort
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EDGE TEST" << endl
                          << "=========" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(4, 5));

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool sorted = TopologicalSortUtil::sort(&results,
                                                &unordered,
                                                relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 5);

        LOOP_ASSERT(results[0], results[0] == 4);
        LOOP_ASSERT(results[1], results[1] == 1);
        LOOP_ASSERT(results[2], results[2] == 5);
        LOOP_ASSERT(results[3], results[3] == 2);
        LOOP_ASSERT(results[4], results[4] == 3);

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // EDGE TEST
        //   This case test sorting of zero and one relations graphs.
        //
        // Concerns:
        //: 1 Zero edges result in successful sort and zero sorted nodes.
        //:
        //: 2 One edge results in successful sort and two sorted nodes.
        //
        // Testing:
        //   sort
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EDGE TEST" << endl
                          << "=========" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool sorted = TopologicalSortUtil::sort(&results,
                                                &unordered,
                                                relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 0);

        // One edge

        relations.push_back(bsl::make_pair(1, 2));

        sorted = TopologicalSortUtil::sort(&results, &unordered, relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 2);

        LOOP_ASSERT(results[0], results[0] == 1);
        LOOP_ASSERT(results[1], results[1] == 2);

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

        bsl::vector<bsl::pair<bsl::string, bsl::string> > relations;

        relations.push_back(bsl::make_pair(bsl::string("3"),
                                           bsl::string("8")));

        relations.push_back(bsl::make_pair(bsl::string("3"),
                                           bsl::string("10")));

        relations.push_back(bsl::make_pair(bsl::string("5"),
                                           bsl::string("11")));

        relations.push_back(bsl::make_pair(bsl::string("7"),
                                           bsl::string("8")));

        relations.push_back(bsl::make_pair(bsl::string("7"),
                                           bsl::string("11")));

        relations.push_back(bsl::make_pair(bsl::string("8"),
                                           bsl::string("9")));

        relations.push_back(bsl::make_pair(bsl::string("11"),
                                           bsl::string("2")));

        relations.push_back(bsl::make_pair(bsl::string("11"),
                                           bsl::string("9")));

        relations.push_back(bsl::make_pair(bsl::string("11"),
                                           bsl::string("10")));

        bsl::vector<bsl::string> results;
        bsl::vector<bsl::string> unordered;
        const bool sorted = TopologicalSortUtil::sort(&results,
                                                      &unordered,
                                                      relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 8);

        LOOP_ASSERT(results[0], results[0] == "7");
        LOOP_ASSERT(results[1], results[1] == "3");
        LOOP_ASSERT(results[2], results[2] == "5");
        LOOP_ASSERT(results[3], results[3] == "8");
        LOOP_ASSERT(results[4], results[4] == "11");
        LOOP_ASSERT(results[5], results[5] == "2");
        LOOP_ASSERT(results[6], results[6] == "9");
        LOOP_ASSERT(results[7], results[7] == "10");

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
