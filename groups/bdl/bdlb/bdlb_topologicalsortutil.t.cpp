// bdlb_topologicalsortutil.t.cpp                                     -*-C++-*-

#include <bdlb_topologicalsortutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslim_testutil.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_iterator.h>
#include <bsl_list.h>
#include <bsl_queue.h>
#include <bsl_set.h>
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
// The component under test is a utility template with two flavor of 'sort'
// functions that implement topological sorting of directed graphs with cycle
// detection.  One version of the function has a signature with (input and
// output) iterators while the other is templated on the type representing the
// nodes and uses 'bsl::vector<bsl::pair<TYPE, TYPE>>' as input and
// 'bsl::vector<TYPE>' as output.
//
// Class Methods:
//: o 'sort<INPUT_ITER, RESULT_ITER, UNORDERED_ITER>'
//: o 'sort<VALUE_TYPE>'
//
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [2] sort(result, unorderedList, relations)
//
// [3] sort(result, unorderedList, relations)
//
// [4] sort(result, unorderedList, relations)
//
// [5] sort(result, unorderedList, relations)
//
// [6] sort(result, unorderedList, relations)
//
// [7] sort(relationsBegin, relationsEnd, result, unordered)
// [7] TopologicalSortUtilEdgeTraits
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [8] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

//=============================================================================
//               GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

class CustomEdge {
    // 'CustomMapping' is an example attribute class used in demonstrating
    // customizing 'TopologicalSortUtil::sort' using
    // 'TopologicalSortUtilMappingTraits'.

    // DATA
    int d_from;
    int d_to;

  public:
    // CREATORS
    CustomEdge(int from, int to)
        // Create a custom mapping object with the specified 'from' and 'to'
        // attributes.
    : d_from(from)
    , d_to(to)
    {
    }

    // ACCESSORS
    int from() const
        // Return the 'from' attribute of this object.
    {
        return d_from;
    }

    int to() const
        // Return the 'to' attribute of this object.
    {
        return d_to;
    }
};

namespace BloombergLP {
namespace bdlb {

template <>
struct TopologicalSortUtilEdgeTraits<CustomEdge> {
    // This 'struct' 'TopologicalSortUtilEdgeTraits<CustomEdge>' customizes
    // 'TopologicalSortUtil::sort' to "understand" the 'CustomEdge' type.

    // TYPES
    typedef CustomEdge EdgeType;
        // The type that represents a connection in the graph.

    typedef int NodeType;
        // The type that represents a node/vertex of the graph.

    static NodeType from(const CustomEdge& edge)
        // Return the 'from' attribute of the specified 'edge' object.
    {
        return edge.from();
    }

    static NodeType to(const CustomEdge& edge)
        // Return the 'to' attribute of the specified 'edge' object.
    {
        return edge.to();
    }
};

}  // close package namespace
}  // close enterprise namespace

                              // ==================
                              // NullOutputIterator
                              // ==================

class NullOutputIterator {
    // This 'class' 'NullOutputIterator' is an iterator of the
    // 'output_iterator' category that supports all output iterator methods and
    // all methods do nothing.  It is also able to accept any type for output.

  public:
    // TYPES
    typedef void container_type;
        // This iterator type does not serve a specific container, hence the
        // 'container_type' is 'void'.

    typedef void value_type;
        // This iterator type does not serve a specific value type, hence the
        // 'value_type' is 'void'.

    typedef void difference_type;
        // 'difference_type' for output iterators is 'void' by definition.

    typedef void pointer;
        // 'pointer_type' for output iterators is 'void' by definition.

    typedef void reference;
        // 'reference' type for output iterators is 'void' by definition.

    typedef bsl::output_iterator_tag iterator_category;
        // This iterator type is an output iterator, hence 'iterator_category'
        // is 'bsl::output_iterator_tag'.

    template <class TYPE>
    NullOutputIterator& operator=(const TYPE&)
        // Do nothing and return '*this'.
    {
        return *this;
    }

    NullOutputIterator& operator*()
        // Do nothing and return '*this'.
    {
        return *this;
    }

    NullOutputIterator& operator++()
        // Do nothing and return '*this'.
    {
        return *this;
    }

    NullOutputIterator& operator++(int)
        // Do nothing and return '*this'.
    {
        return *this;
    }
};

//=============================================================================
//                                   MAIN
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3; (void)veryVerbose;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5; (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
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
///Example 1: Using Topological Sort for Calculating Formulas
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are evaluating formulas for a set of market data fields, where
// formulas can reference other fields as part of their calculation.  As an
// example, let's say we have a field k_bbgDefinedVwap that is dependent on
// k_vwapTurnover and k_vwapVolume.  These fields in turn are dependent on
// k_tradeSize and k_tradePrice respectively.  So essentially the fields that
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

    relations.push_back(bsl::make_pair(static_cast<int>(k_vwapTurnover),
                                       static_cast<int>(k_bbgDefinedVwap)));
    relations.push_back(bsl::make_pair(static_cast<int>(k_vwapVolume),
                                       static_cast<int>(k_bbgDefinedVwap)));
    relations.push_back(bsl::make_pair(static_cast<int>(k_tradeSize),
                                       static_cast<int>(k_vwapVolume)));
    relations.push_back(bsl::make_pair(static_cast<int>(k_tradeSize),
                                       static_cast<int>(k_vwapTurnover)));
    relations.push_back(bsl::make_pair(static_cast<int>(k_tradePrice),
                                       static_cast<int>(k_vwapTurnover)));
//..
// Now, we call the topological sort to get a topological order for the fields
// referenced in the relations:
//..
    bsl::vector<int> results;
    bsl::vector<int> unordered;
    bool             sorted = TopologicalSortUtil::sort(&results,
                                                        &unordered,
                                                        relations);
//..
// Finally, we verify that the call to 'sort' populates the supplied 'results'
// with a sequence in sorted order (e.g.. 'k_tradeSize', 'k_tradePrice,
// 'k_vwapTurnover', 'k_vwapVolume', 'k_bbgDefinedVwap') and 'unsorted' will be
// empty because the input relationships do not contain a cycle.
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

            ASSERT(calculated[k_bbgDefinedVwap] == false);
            calculated[k_bbgDefinedVwap] = true;
          } break;
          case k_vwapTurnover: {
            ASSERT(calculated[k_tradeSize]  == true);
            ASSERT(calculated[k_tradePrice] == true);

            ASSERT(calculated[k_vwapTurnover] == false);
            calculated[k_vwapTurnover] = true;
          } break;
          case k_vwapVolume: {
            ASSERT(calculated[k_tradeSize]  == true);

            ASSERT(calculated[k_vwapVolume] == false);
            calculated[k_vwapVolume] = true;
          } break;
          case k_tradeSize: {
            ASSERT(calculated[k_vwapVolume]   == false);
            ASSERT(calculated[k_vwapTurnover] == false);

            ASSERT(calculated[k_tradeSize] == false);
            calculated[k_tradeSize] = true;
          } break;
          case k_tradePrice: {
            ASSERT(calculated[k_vwapTurnover] == false);

            ASSERT(calculated[k_tradePrice] == false);
            calculated[k_tradePrice] = true;
          } break;
          default:
            ASSERT(false);
            break;
        };
    }

    for (int i = 0; i < 5; ++i) {
        ASSERT(calculated[i] == true);
    }
//..
///Example 2: Using Topological Sort with Cycles in Input
///-  - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which contain a cycle.
//
// First, we define a set of inputs which have a cycle:
//..
    enum FieldIds2 {
        k_FIELD1 = 0,
        k_FIELD2 = 1,
        k_FIELD3 = 2
    };

    bsl::vector<bsl::pair<int, int> > relations2;

    relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD2),
                                        static_cast<int>(k_FIELD1)));
    relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD3),
                                        static_cast<int>(k_FIELD2)));
    relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD1),
                                        static_cast<int>(k_FIELD3)));
//..
// Now, we apply the topological sort routine on the input:
//..
    bsl::vector<int> results2;
    bsl::vector<int> unordered2;
    bool             sorted2 = TopologicalSortUtil::sort(&results2,
                                                         &unordered2,
                                                         relations2);
//..
// Finally, we verify whether the routine recognizes that there is a cycle and
// returns false:
//..
    ASSERT(sorted2           == false);
    ASSERT(unordered2.size() == 3);
//..
///Example 3: Using Topological Sort with Self Relations
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which have input relations where predecessor
// and successor point to the same value. i.e. we have pairs of input like
// (u,u).  Such input is considered a cycle by the 'sort' functions, and this
// example demonstrates that behavior.  Some systems use such (u,u) pairs to
// represent standalone nodes with no connection.  See Example 6 for a way
// handle such data.
//
// First, we define the set of inputs:
//..
    enum FieldIds3 {
        k_FIELD4 = 3,
        k_FIELD5 = 4,
        k_FIELD6 = 5
    };

    bsl::vector<bsl::pair<int, int> > relations3;

    relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD4),
                                        static_cast<int>(k_FIELD6)));
    relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD5),
                                        static_cast<int>(k_FIELD4)));
    relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD4),
                                        static_cast<int>(k_FIELD4)));
//..
// Now, we apply the topological sort routine on the input:
//..
    bsl::vector<int> results3;
    bsl::vector<int> unordered3;
    bool             sorted3 = TopologicalSortUtil::sort(&results3,
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
        for (bsl::size_t i = 0; i < results3.size(); ++i) {
                cout << "results3[" << i << "] is " << results3[i] << "\n";
        }

        for (bsl::size_t i = 0; i < unordered3.size(); ++i) {
                cout << "unordered3[" << i << "] is " << unordered3[i] << "\n";
        }
    }
//..
///Example 4: Using Topological Sort with Iterators as Input
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which have input relations that conceptually
// follow the input requirements (of a listing of pairs of nodes) but are not
// physically stored in a 'bsl::vector' of 'bsl::pair' typed container.  Let's
// suppose the input is in a 'bsl::list' instead.  First, we define such a set
// of inputs:
//..
    bsl::list<bsl::pair<int, int> > relations4;

    relations4.push_back(bsl::make_pair(1, 2));
    relations4.push_back(bsl::make_pair(1, 3));
    relations4.push_back(bsl::make_pair(2, 3));
//..
// Now, we apply the topological sort routine on the input:
//..
    bsl::vector<int> results4;
    bsl::vector<int> unordered4;
    typedef bsl::back_insert_iterator<bsl::vector<int> > OutIter;
    bool sorted4 = TopologicalSortUtil::sort(relations4.begin(),
                                             relations4.end(),
                                             OutIter(results4),
                                             OutIter(unordered3));
//..
// Finally, we verify that the sort is successful, there are no nodes in the
// 'unordered' output (there is no cycle) and the nodes are listed in the
// proper order in 'results4':
//..
    ASSERT(sorted4           == true);
    ASSERT(unordered4.size() == 0);
    ASSERT(results4.size() == 3);

    ASSERT(results4[0] == 1);
    ASSERT(results4[1] == 2);
    ASSERT(results4[2] == 3);
//..
///Example 5: Using Topological Sort with Iterators as Output
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want our result in a 'bsl::list' instead of a 'bsl::vector' and
// we do not care about the unordered elements so we do not want to pay for
// storing them if they exist.  First, we would define a Null Output Iterator
// that writes to nowhere.  See 'NullOutputIterator' before 'main' (local
// classes cannot be templates and cannot have member templates in C++03).
//
// Now, we apply the topological sort routine on the input:
//..
    bsl::list<int> results5;
    typedef bsl::back_insert_iterator<bsl::list<int> > ListOutIter;
    bool sorted5 = TopologicalSortUtil::sort(relations4.begin(),
                                             relations4.end(),
                                             ListOutIter(results5),
                                             NullOutputIterator());
//..
// Finally, we verify that the sort is successful, and the 3 nodes are listed
// in the proper order in the 'results5' list:
//..
    ASSERT(sorted5           == true);
    ASSERT(results5.size()   == 3);

    ASSERT(*results5.begin() == 1);
    results5.pop_front();
    ASSERT(*results5.begin() == 2);
    results5.pop_front();
    ASSERT(*results5.begin() == 3);
    results5.pop_front();
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CUSTOM EDGE CLASS TEST
        //   This case tests the usage of custom edge class instead of
        //   'bsl::pair'.
        //
        // Concerns:
        //: 1 The code compiles thanks to the specialization of the
        //:   'TopologicalSortUtilEdgeTraits' template.  See the custom
        //:   class and the specialization before 'main' (C++03 does not
        //:   support local classes in templates.)
        //
        // Plan:
        //: 1 Create a set of input using a custom edge (edge) type instead
        //:   of 'bsl::pair'.
        //:
        //: 2 Call the iterator version of the 'sort' function.  (We could call
        //:   the simple (non-iterator) version as well.)
        //:
        //: 3 Verify that with the fully specialized
        //:   'TopologicalSortUtilEdgeTraits' the code compiles and links.
        //:
        //: 4 Verify that 'sort' returned 'true', the resulting 'unordered'
        //:   'vector' is empty, and 'results' contains an acceptable
        //:   topological ordering of the nodes.  Note that for brevity and
        //:   simplicity of the testing code we verify the *exact* order of the
        //:   elements.  As noted in the component documentation there may be
        //:   other valid orders.
        //
        // Testing:
        //   sort(relationsBegin, relationsEnd, result, unordered)
        //   TopologicalSortUtilEdgeTraits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CUSTOM EDGE CLASS TEST" << endl
                          << "======================" << endl;

        bsl::vector<CustomEdge> relations;

        relations.emplace_back(1, 2);
        relations.emplace_back(2, 3);
        relations.emplace_back(3, 1);

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        typedef bsl::back_insert_iterator<bsl::vector<int> > OutIter;
        bool sorted = TopologicalSortUtil::sort(relations.begin(),
                                                relations.end(),
                                                OutIter(results),
                                                OutIter(unordered));
        ASSERT(false == sorted);
        ASSERT(results.empty());

        ASSERT(unordered.size() == 3);

        LOOP_ASSERT(unordered[0], unordered[0] == 3);
        LOOP_ASSERT(unordered[1], unordered[1] == 2);
        LOOP_ASSERT(unordered[2], unordered[2] == 1);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CYCLE IS DETECTED TEST
        //   This case test that a cycle in the graphs is detected.
        //
        // Concerns:
        //: 1 A graph with a cycle does not sort successfully.
        //:
        //: 2 The cycle is reported in the 'unordered' argument.
        //
        // Plan:
        //: 1 Create a set of input edges that contains a cycle.
        //:
        //: 2 Call the simple (non-iterator) version of the 'sort' function.
        //:
        //: 3 Verify that 'sort' returned 'false', the resulting 'unordered'
        //:   'vector' contains the nodes in the cycle, and 'results' is empty.
        //:   Note that for brevity and simplicity of the test driver we are
        //:   testing the 'unordered' nodes in a static order; however the
        //:   algorithm contract does not make any promise about the order in
        //:   which the offending nodes are reported.
        //
        // Testing:
        //   sort(result, unorderedList, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CYCLE IS DETECTED TEST" << endl
                          << "======================" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(3, 1));

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool             sorted = TopologicalSortUtil::sort(&results,
                                                            &unordered,
                                                            relations);
        ASSERT(false == sorted);
        ASSERT(results.empty());

        ASSERT(unordered.size() == 3);

        LOOP_ASSERT(unordered[0], unordered[0] == 3);
        LOOP_ASSERT(unordered[1], unordered[1] == 2);
        LOOP_ASSERT(unordered[2], unordered[2] == 1);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // SELF REFERENCE IS CYCLE TEST
        //   This case proves that a self referencing node is reported as a
        //   cycle.
        //
        // Concerns:
        //: 1 A graph with a self referencing node does not sort successfully.
        //:
        //: 2 A graph with a self referencing node reports a cycle.
        //
        // Plan:
        //: 1 Create a set of input edges that contains a single connection
        //:   from the same node to the same node.  In other words: a
        //:   self-referencing node.
        //:
        //: 2 Call the simple (non-iterator) version of the 'sort' function.
        //:
        //: 3 Verify that 'sort' returned 'false', the resulting 'unordered'
        //:   'vector' contains the single input node, and 'results' is empty.
        //
        // Testing:
        //   sort(result, unorderedList, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SELF REFERENCE IS CYCLE TEST" << endl
                          << "============================" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 1));

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool             sorted = TopologicalSortUtil::sort(&results,
                                                            &unordered,
                                                            relations);
        ASSERT(false == sorted);
        ASSERT(results.empty());

        ASSERT(unordered.size() == 1);

        LOOP_ASSERT(unordered[0], unordered[0] == 1);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DUPLICATED INPUT ELEMENTS TEST
        //   This case tests sorting of a graph that has duplicated edges in
        //   the input.
        //
        // Concerns:
        //: 1 The graph is successfully topologically sorted.
        //:
        //: 2 No nodes are missing from the result.
        //:
        //: 3 No nodes are duplicated in the result.
        //
        // Plan:
        //: 1 Create a set of edges that comprise the graphs, duplicate some of
        //:   the edges.
        //:
        //: 2 Call the simple (non-iterator) version of the 'sort' function.
        //:
        //: 3 Verify that 'sort' returned 'true', the resulting 'unordered'
        //:   'vector' is empty, and 'results' contains the nodes a proper
        //:   order.  Note that for brevity and simplicity of the testing code
        //:   we verify the *exact* order of the elements.  As noted in the
        //:   component documentation there may be other valid orders.
        //
        // Testing:
        //   sort(result, unorderedList, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "DUPLICATED INPUT ELEMENTS TEST" << endl
                          << "==============================" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(1, 2)); // duplicated
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(4, 5));
        relations.push_back(bsl::make_pair(4, 5)); // duplicated

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool             sorted = TopologicalSortUtil::sort(&results,
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
        // Plan:
        //: 1 Create a set of edges that comprise two graphs, two sets of nodes
        //:   that have no connecting edge between them.
        //:
        //: 2 Call the simple (non-iterator) version of the 'sort' function.
        //:
        //: 3 Verify that 'sort' returned 'true', the resulting 'unordered'
        //:   'vector' is empty, and 'results' contains the nodes a proper
        //:   order.  Note that for brevity and simplicity of the testing code
        //:   we verify the *exact* order of the elements.  As noted in the
        //:   component documentation there may be other valid orders.
        //
        // Testing:
        //   sort(result, unorderedList, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TWO GRAPHS TEST" << endl
                          << "===============" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(4, 5));

        bsl::vector<int> results;
        bsl::vector<int> unordered;
        bool             sorted = TopologicalSortUtil::sort(&results,
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
        // EDGE CASES TEST
        //   This case test sorting of zero and one relation graphs.
        //
        // Concerns:
        //: 1 Zero edges result in successful sort and zero sorted nodes.
        //:
        //: 2 One edge results in successful sort and two sorted nodes.
        //
        // Plan:
        //: 1 Create empty input of 'vector' of 'pair's of 'string's.
        //:
        //: 2 Invoke the simple (non-iterator) version of 'sort'.
        //:
        //: 3 Verify that the function returns 'true' and both the 'result',
        //:   and the 'unordered' 'vector's are empty.
        //:
        //: 4 Add a single edge (edge as in graph theory, not to be confused
        //:   with edge cases in software testing) to the input.
        //:
        //: 5 Invoke the simple (non-iterator) version of 'sort'.
        //:
        //: 6 Verify that the function returns 'true', the 'result' 'vector'
        //:   contains the nodes in the same order as they were in the edge,
        //:   and the 'unordered' 'vector' is empty.
        //
        // Testing:
        //   sort(result, unorderedList, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EDGE CASES TEST" << endl
                          << "===============" << endl;

        bsl::vector<bsl::pair<bsl::string, bsl::string> > relations;

        bsl::vector<bsl::string> results;
        bsl::vector<bsl::string> unordered;
        bool                     sorted = TopologicalSortUtil::sort(&results,
                                                                    &unordered,
                                                                    relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 0);

        // One edge

        relations.push_back(bsl::make_pair(bsl::string("1"),
                                           bsl::string("2")));

        sorted = TopologicalSortUtil::sort(&results, &unordered, relations);
        ASSERT(true == sorted);
        ASSERT(unordered.empty());

        ASSERT(results.size() == 2);

        LOOP_ASSERT(results[0], results[0] == "1");
        LOOP_ASSERT(results[1], results[1] == "2");

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
        //:  1 Create the input of 'vector' of 'pair's of 'int's that describes
        //:    a directed acyclic graph.
        //:
        //:  2 Create two empty 'vectors' 'results', and 'unordered' for the
        //:    two output arguments of the 'sort' function.
        //:
        //:  3 Run the simple (non-iterator) variant of the 'sort' function.
        //:
        //:  4 Verify that the sort is successful ('sort' returns 'true')
        //:
        //:  5 verify that the 'unordered' output is empty.
        //:
        //:  6 Verify the order of nodes/vertexes in the 'results' vector.
        //:    Note that for brevity and simplicity of the testing code we
        //:    verify the *exact* order of the elements.  As noted in the
        //:    component documentation there may be other valid orders.
        //:
        //:  7 Create the input of 'list' of 'pair's of 'int's that describes a
        //:    directed acyclic graph.
        //:
        //:  2 Create an empty 'vector' for 'results', and an empty 'set' for
        //:    'unordered' for the two output arguments of the 'sort' function.
        //:
        //:  3 Run the iterator variant of the 'sort' function using the input
        //:    'bsl::list::begin' and 'bsl::list::end' for the input range, a
        //:    back insert iterator for the 'results' 'vector' and an insert
        //:    iterator for the 'unordered' 'set'.
        //:
        //:  4 Verify that the sort is successful ('sort' returns 'true')
        //:
        //:  5 verify that the 'unordered' output is empty.
        //:
        //:  6 Verify the order of nodes/vertexes in the 'results' vector.
        //:    Note that for brevity and simplicity of the testing code we
        //:    verify the *exact* order of the elements.  As noted in the
        //:    component documentation there may be other valid orders.
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (veryVerbose) cout << "Simplified interface" << endl;
        {
            bsl::vector<bsl::pair<int, int> > relations;

            relations.push_back(bsl::make_pair(3, 8));
            relations.push_back(bsl::make_pair(3, 10));
            relations.push_back(bsl::make_pair(5, 11));
            relations.push_back(bsl::make_pair(7, 8));
            relations.push_back(bsl::make_pair(7, 11));
            relations.push_back(bsl::make_pair(8, 9));
            relations.push_back(bsl::make_pair(11, 2));
            relations.push_back(bsl::make_pair(11, 9));
            relations.push_back(bsl::make_pair(11, 10));

            bsl::vector<int> results;
            bsl::vector<int> unordered;
            const bool       sorted = TopologicalSortUtil::sort(&results,
                                                                &unordered,
                                                                relations);
            ASSERT(true == sorted);
            ASSERT(unordered.empty());

            LOOP_ASSERT(results.size(), results.size() == 8);

            LOOP_ASSERT(results[0], results[0] == 7);
            LOOP_ASSERT(results[1], results[1] == 3);
            LOOP_ASSERT(results[2], results[2] == 5);
            LOOP_ASSERT(results[3], results[3] == 8);
            LOOP_ASSERT(results[4], results[4] == 11);
            LOOP_ASSERT(results[5], results[5] == 2);
            LOOP_ASSERT(results[6], results[6] == 9);
            LOOP_ASSERT(results[7], results[7] == 10);
        }

        if (veryVerbose) cout << "Iterator interface" << endl;
        {
            bsl::list<bsl::pair<int, int> > relations;

            relations.push_back(bsl::make_pair(3, 8));
            relations.push_back(bsl::make_pair(3, 10));
            relations.push_back(bsl::make_pair(5, 11));
            relations.push_back(bsl::make_pair(7, 8));
            relations.push_back(bsl::make_pair(7, 11));
            relations.push_back(bsl::make_pair(8, 9));
            relations.push_back(bsl::make_pair(11, 2));
            relations.push_back(bsl::make_pair(11, 9));
            relations.push_back(bsl::make_pair(11, 10));

            bsl::vector<int> results;
            bsl::set<int>    unordered;
            typedef bsl::back_insert_iterator<bsl::vector<int> > OutIter;
            const bool sorted = TopologicalSortUtil::sort(
                                                    relations.begin(),
                                                    relations.end(),
                                                    OutIter(results),
                                                    inserter(unordered,
                                                             unordered.end()));
            ASSERT(true == sorted);
            ASSERT(unordered.empty());

            LOOP_ASSERT(results.size(), results.size() == 8);

            LOOP_ASSERT(results[0], results[0] == 7);
            LOOP_ASSERT(results[1], results[1] == 3);
            LOOP_ASSERT(results[2], results[2] == 5);
            LOOP_ASSERT(results[3], results[3] == 8);
            LOOP_ASSERT(results[4], results[4] == 11);
            LOOP_ASSERT(results[5], results[5] == 2);
            LOOP_ASSERT(results[6], results[6] == 9);
            LOOP_ASSERT(results[7], results[7] == 10);
        }

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
