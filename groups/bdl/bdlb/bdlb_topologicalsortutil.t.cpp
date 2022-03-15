// bdlb_topologicalsortutil.t.cpp                                     -*-C++-*-

#include <bdlb_topologicalsortutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bslim_testutil.h>

#include <bsl_algorithm.h>
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
// [2] sort(result, unsortedOut, relations)
//
// [3] sort(result, unsortedOut, relations)
//
// [4] sort(result, unsortedOut, relations)
//
// [5] sort(result, unsortedOut, relations)
//
// [6] sort(result, unsortedOut, relations)
//
// [7] sort(relationsBegin, relationsEnd, resultOutIter, unsortedOutIter)
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

                            // ================
                            // class CustomNode
                            // ================

class CustomNode {
    // 'CustomNode' is an example attribute class used in demonstrating
    // customizing 'TopologicalSortUtil::sort' using
    // 'TopologicalSortUtilEdgeTraits'.

    // DATA
    int d_identifier;

  public:
    // CREATORS
    explicit CustomNode(int identifier)
        // Create a custom node object with the specified 'identifier' value.
    : d_identifier(identifier)
    {
    }

    // ACCESSORS
    int identifier() const
        // Return the 'identifier' attribute of this object.
    {
        return d_identifier;
    }

    bool operator==(const CustomNode& lhs) const
        // Return 'true' if the specified 'lhs' has the same value as this
        // object.  Two 'CustomNode' objects have the same value if their
        // 'identifier' has the same value.
    {
        return d_identifier == lhs.d_identifier;
    }
};

                                // Aspects

bsl::ostream& operator<<(bsl::ostream& os, const CustomNode& node)
    // Print the specified 'node' to the specified 'os' output stream and
    // return 'os'.
{
    return os << "CustomNode{" << node.identifier() << '}';
}

template <class HASH_ALGORITHM>
inline
void hashAppend(HASH_ALGORITHM& algo, CustomNode const& key)
    // Append the hash input value of the specified 'key' to the specified
    // 'algo'.
{
    hashAppend(algo, key.identifier());
}

struct CustomComparator {
    // To sort the unsorted output.

    bool operator()(const CustomNode& lhs, const CustomNode& rhs)
        // Return 'true' if the specified 'lhs' should come before the
        // specified 'rhs' in sorting.
    {
        return lhs.identifier() < rhs.identifier();
    }
};

                            // ================
                            // class CustomEdge
                            // ================

class CustomEdge {
    // 'CustomMapping' is an example attribute class used in demonstrating
    // customizing 'TopologicalSortUtil::sort' using
    // 'TopologicalSortUtilEdgeTraits'.

    // DATA
    CustomNode d_from;
    CustomNode d_to;

  public:
    // CREATORS
    CustomEdge(CustomNode from, CustomNode to)
        // Create a custom mapping object with the specified 'from' and 'to'
        // attributes.
    : d_from(from)
    , d_to(to)
    {
    }

    CustomEdge(int fromId, int toId)
        // Create a custom mapping object with node sof the specified 'fromId'
        // and 'toId'.
    : d_from(fromId)
    , d_to(toId)
    {
    }

    // ACCESSORS
    CustomNode from() const
        // Return the 'from' attribute of this object.
    {
        return d_from;
    }

    CustomNode to() const
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

    typedef CustomNode NodeType;
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
    // This iterator type is an output iterator, hence 'iterator_category' is
    // 'bsl::output_iterator_tag'.

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
                        // =======================
                        // VerifyingOutputIterator
                        // =======================

template <class WRAPPED_ITERATOR>
class VerifyingOutputIterator {
    // This 'class' is an output-iterator that wraps another iterator type and
    // verifies proper output iterator use.  It asserts if not incremented
    // between assignments or incremented but not assigned.

  public:
    // TYPES
    typedef WRAPPED_ITERATOR WrappedIterator;

public:
    // TYPES
    typedef typename WrappedIterator::container_type  container_type;
    typedef typename WrappedIterator::value_type      value_type;
    typedef typename WrappedIterator::difference_type difference_type;
    typedef typename WrappedIterator::pointer         pointer;
    typedef typename WrappedIterator::reference       reference;

    typedef bsl::output_iterator_tag iterator_category;
        // This type is an output iterator, hence 'iterator_category' is
        // 'bsl::output_iterator_tag'.  Since no other iterator functionality
        // is implemented even if the wrapped iterator could do more, the
        // wrapped one cannot.

  private:
    // DATA
    bool            d_assigned;
    bool            d_incremented;
    int             d_line;
    WrappedIterator d_iterator;

  public:
    // CREATORS
    template <class ITER_INIT>
    explicit VerifyingOutputIterator(int line, ITER_INIT init)
        // Create a 'VerifyingOutputIterator' with the wrapped iterator
        // constructed from the specified 'init' value and use the specified
        // 'line' number in asserts.
    : d_assigned(false)
    , d_incremented(false)
    , d_line(line)
    , d_iterator(init)
    {}

    explicit VerifyingOutputIterator(int line)
        // Create a 'VerifyingOutputIterator' with a default constructed
        // wrapped iterator that uses the specified 'line' number in asserts.
    : d_assigned(false)
    , d_incremented(false)
    , d_line(line)
    , d_iterator()
    {}

    template <class TYPE>
    VerifyingOutputIterator& operator=(const TYPE& value)
        // Set the iterator wrapped iterator to the specified 'value', set the
        // verification state such that an increment is required before the
        // next assignment, and return '*this'.  Report an error if the
        // iterator is not in the state where it is assignable.
    {
        ASSERTV(d_line, d_assigned, d_incremented,
                 false == d_assigned || true == d_incremented);

        d_incremented    = false;
        d_assigned       = true;

        d_iterator = value;

        return *this;
    }

    VerifyingOutputIterator& operator*()
        // Do nothing and return '*this'.
    {
        return *this;
    }

    VerifyingOutputIterator& operator++()
        // Increment the wrapped iterator, set this iterator to the state where
        // it is allowed to assign to it, and return '*this'.  Report an error
        // if the iterator has not been assigned yet in its current position.
    {
        ASSERTV(d_line, true == d_assigned && false == d_incremented);

        d_incremented = true;
        d_assigned    = false;

        ++d_iterator;

        return *this;
    }

    VerifyingOutputIterator operator++(int)
        // Increment the wrapped iterator, set this iterator to the state where
        // it is allowed to assign to it, and return '*this'.  An error is
        // reported if the iterator has not been assigned-to yet in its current
        // position.
    {
        ASSERTV(d_line, false == d_incremented);

        VerifyingOutputIterator rv(*this);

        d_incremented    = true;
        d_assigned       = false;

        ++d_iterator;

        return rv;
    }
};
                  // ====================================
                  // Verifying Inserter Factory Functions
                  // ====================================

template <class Container>
VerifyingOutputIterator<bsl::insert_iterator<Container> >
verifyingInserter(int                          line,
                  Container&                   container,
                  typename Container::iterator position)
    // Return a 'VerifyingOutputIterator' that reports issues using the
    // specified 'line' number, and wraps a 'bsl::insert_iterator' for the
    // specified 'container' that inserts after the specified 'position'.
{
    typedef VerifyingOutputIterator<bsl::insert_iterator<Container> >
                                                       VerifyingInsertIterator;
    return VerifyingInsertIterator(line, bsl::inserter(container, position));
}

template <class Container>
VerifyingOutputIterator<bsl::back_insert_iterator<Container> >
verifyingBackInserter(int line, Container& container)
    // Return a 'VerifyingOutputIterator' that reports issues using the
    // specified 'line' number, and wraps a 'bsl::back_insert_iterator' for the
    // specified 'container'.
{
    typedef VerifyingOutputIterator<bsl::back_insert_iterator<Container> >
                                                   VerifyingBackInsertIterator;
    return VerifyingBackInsertIterator(line, bsl::back_inserter(container));
}

#define U_INSERTER(c)       verifyingInserter(__LINE__, c, c.end())

#define U_BACK_INSERTER(c) verifyingBackInserter(__LINE__, c)

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
    bsl::vector<int> unsorted;
    bool             sorted = TopologicalSortUtil::sort(&results,
                                                        &unsorted,
                                                        relations);
//..
// Finally, we verify that the call to 'sort' populates the supplied 'results'
// with a sequence in sorted order (e.g.. 'k_tradeSize', 'k_tradePrice,
// 'k_vwapTurnover', 'k_vwapVolume', 'k_bbgDefinedVwap') and 'unsorted' will be
// empty because the input relationships do not contain a cycle.
//..
    bool calculated[5] = { 0 };
    ASSERT(sorted == true);
    ASSERT(unsorted.empty());

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
    bsl::vector<int> unsorted2;
    bool             sorted2 = TopologicalSortUtil::sort(&results2,
                                                         &unsorted2,
                                                         relations2);
//..
// Finally, we verify whether the routine recognizes that there is a cycle and
// returns false:
//..
    ASSERT(sorted2           == false);
    ASSERT(unsorted2.size() == 3);
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
    bsl::vector<int> unsorted3;
    bool             sorted3 = TopologicalSortUtil::sort(&results3,
                                                         &unsorted3,
                                                         relations3);
//..
// Finally, we verify that the self relations causes the cycle:
//..
    ASSERT(sorted3           == false);
    ASSERT(results3.size()   == 1);
    ASSERT(unsorted3.size() == 2);

    if (veryVeryVerbose) {
        cout << "Size of results3 vector is "
             << results3.size() << endl;

        cout << "Size of unsorted3 vector is "
             << unsorted3.size() << endl;
    }

    if (veryVeryVerbose)
    {
        for (bsl::size_t i = 0; i < results3.size(); ++i) {
                cout << "results3[" << i << "] is " << results3[i] << "\n";
        }

        for (bsl::size_t i = 0; i < unsorted3.size(); ++i) {
                cout << "unsorted3[" << i << "] is " << unsorted3[i] << "\n";
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
    bsl::vector<int> unsorted4;
    typedef bsl::back_insert_iterator<bsl::vector<int> > OutIter;
    bool sorted4 = TopologicalSortUtil::sort(relations4.begin(),
                                             relations4.end(),
                                             OutIter(results4),
                                             OutIter(unsorted3));
//..
// Finally, we verify that the sort is successful, there are no nodes in the
// 'unsorted' output (there is no cycle) and the nodes are listed in the
// proper order in 'results4':
//..
    ASSERT(sorted4           == true);
    ASSERT(unsorted4.size() == 0);
    ASSERT(results4.size() == 3);

    ASSERT(results4[0] == 1);
    ASSERT(results4[1] == 2);
    ASSERT(results4[2] == 3);
//..
///Example 5: Using Topological Sort with Iterators as Output
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want our result in a 'bsl::list' instead of a 'bsl::vector' and
// we do not care about the unsorted elements so we do not want to pay for
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
        //: 4 Verify that 'sort' returned 'true', the resulting 'unsorted'
        //:   'vector' is empty, and 'results' contains an acceptable
        //:   topological ordering of the nodes.  Note that for brevity and
        //:   simplicity of the testing code we verify the *exact* order of the
        //:   elements.  As noted in the component documentation there may be
        //:   other valid orders.
        //
        // Testing:
        //   sort(relationsBegin, relationsEnd, resultOutIter, unsortedOutIter)
        //   TopologicalSortUtilEdgeTraits
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CUSTOM EDGE CLASS TEST" << endl
                          << "======================" << endl;

        if (veryVerbose) cout << "Iterator interface\n";
        {
            bsl::vector<CustomEdge> relations;

            relations.emplace_back(1, 2);
            relations.emplace_back(2, 3);
            relations.emplace_back(3, 1);

            bsl::vector<CustomNode> results;
            bsl::vector<CustomNode> unsorted;
            bool sorted = TopologicalSortUtil::sort(relations.begin(),
                                                    relations.end(),
                                                    U_BACK_INSERTER(results),
                                                    U_BACK_INSERTER(unsorted));
            ASSERT(false == sorted);
            ASSERT(results.empty());

            ASSERTV(unsorted.size(), unsorted.size() == 3);
            bsl::sort(unsorted.begin(), unsorted.end(), CustomComparator());
            ASSERTV(unsorted[0], unsorted[0].identifier() == 1);
            ASSERTV(unsorted[1], unsorted[1].identifier() == 2);
            ASSERTV(unsorted[2], unsorted[2].identifier() == 3);

            // Make it sortable
            relations.back() = CustomEdge(2, 4);

            results.clear();
            unsorted.clear();
            sorted = TopologicalSortUtil::sort(relations.begin(),
                                               relations.end(),
                                               U_BACK_INSERTER(results),
                                               U_BACK_INSERTER(unsorted));
            ASSERT(true  == sorted);
            ASSERT(unsorted.empty());

            ASSERTV(results.size(), 4 == results.size());
            ASSERTV(results[0], results[0].identifier() == 1);
            ASSERTV(results[1], results[1].identifier() == 2);
            ASSERTV(results[2], results[2].identifier() == 3);
            ASSERTV(results[3], results[3].identifier() == 4);
        }

        if (veryVerbose) cout << "Vector interface\n";
        {
            bsl::vector<bsl::pair<CustomNode, CustomNode> > relations;

            relations.emplace_back(CustomNode(1), CustomNode(2));
            relations.emplace_back(CustomNode(2), CustomNode(3));
            relations.emplace_back(CustomNode(3), CustomNode(1));

            bsl::vector<CustomNode> results;
            bsl::vector<CustomNode> unsorted;
            bool sorted = TopologicalSortUtil::sort(&results,
                                                    &unsorted,
                                                    relations);
            ASSERT(false == sorted);
            ASSERT(results.empty());

            ASSERTV(unsorted.size(), unsorted.size() == 3);
            bsl::sort(unsorted.begin(), unsorted.end(), CustomComparator());
            ASSERTV(unsorted[0], unsorted[0].identifier() == 1);
            ASSERTV(unsorted[1], unsorted[1].identifier() == 2);
            ASSERTV(unsorted[2], unsorted[2].identifier() == 3);

            // Make it sortable
            relations.back().first  = CustomNode(2);
            relations.back().second = CustomNode(4);

            results.clear();
            unsorted.clear();
            sorted = TopologicalSortUtil::sort(&results, &unsorted, relations);
            ASSERT(true  == sorted);
            ASSERT(unsorted.empty());

            ASSERTV(results.size(), 4 == results.size());
            ASSERTV(results[0], results[0].identifier() == 1);
            ASSERTV(results[1], results[1].identifier() == 2);
            ASSERTV(results[2], results[2].identifier() == 3);
            ASSERTV(results[3], results[3].identifier() == 4);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CYCLE IS DETECTED TEST
        //   This case test that a cycle in the graphs is detected.
        //
        // Concerns:
        //: 1 A graph with a cycle does not sort successfully.
        //:
        //: 2 The cycle is reported in the 'unsorted' argument.
        //
        // Plan:
        //: 1 Create a set of input edges that contains a cycle.
        //:
        //: 2 Call the simple (non-iterator) version of the 'sort' function.
        //:
        //: 3 Verify that 'sort' returned 'false', the resulting 'unsorted'
        //:   'vector' contains the nodes in the cycle, and 'results' is empty.
        //:   Note that for brevity and simplicity of the test driver we are
        //:   testing the 'unsorted' nodes in a static order; however the
        //:   algorithm contract does not make any promise about the order in
        //:   which the offending nodes are reported.
        //
        // Testing:
        //   sort(result, unsortedOut, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CYCLE IS DETECTED TEST" << endl
                          << "======================" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(3, 1));

        bsl::vector<int> results;
        bsl::vector<int> unsorted;
        bool             sorted = TopologicalSortUtil::sort(&results,
                                                            &unsorted,
                                                            relations);
        ASSERT(false == sorted);
        ASSERT(results.empty());

        ASSERTV(unsorted.size(), unsorted.size() == 3);

        bsl::sort(unsorted.begin(), unsorted.end());
        ASSERTV(unsorted[0], unsorted[0] == 1);
        ASSERTV(unsorted[1], unsorted[1] == 2);
        ASSERTV(unsorted[2], unsorted[2] == 3);
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
        //: 3 Verify that 'sort' returned 'false', the resulting 'unsorted'
        //:   'vector' contains the single input node, and 'results' is empty.
        //
        // Testing:
        //   sort(result, unsortedOut, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SELF REFERENCE IS CYCLE TEST" << endl
                          << "============================" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 1));

        bsl::vector<int> results;
        bsl::vector<int> unsorted;
        const bool       sorted = TopologicalSortUtil::sort(&results,
                                                            &unsorted,
                                                            relations);
        ASSERT(false == sorted);
        ASSERT(results.empty());

        ASSERTV(unsorted.size(), unsorted.size() == 1);

        ASSERTV(unsorted[0], unsorted[0] == 1);
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
        //: 3 Verify that 'sort' returned 'true', the resulting 'unsorted'
        //:   'vector' is empty, and 'results' contains the nodes a proper
        //:   order.  Note that for brevity and simplicity of the testing code
        //:   we verify the *exact* order of the elements.  As noted in the
        //:   component documentation there may be other valid orders.
        //
        // Testing:
        //   sort(result, unsortedOut, relations)
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
        bsl::vector<int> unsorted;
        bool             sorted = TopologicalSortUtil::sort(&results,
                                                            &unsorted,
                                                            relations);
        ASSERT(true == sorted);
        ASSERT(unsorted.empty());

        ASSERTV(results.size(), results.size() == 5);

        ASSERTV(results[0], results[0] == 4);
        ASSERTV(results[1], results[1] == 1);
        ASSERTV(results[2], results[2] == 5);
        ASSERTV(results[3], results[3] == 2);
        ASSERTV(results[4], results[4] == 3);

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
        //: 3 Verify that 'sort' returned 'true', the resulting 'unsorted'
        //:   'vector' is empty, and 'results' contains the nodes a proper
        //:   order.  Note that for brevity and simplicity of the testing code
        //:   we verify the *exact* order of the elements.  As noted in the
        //:   component documentation there may be other valid orders.
        //
        // Testing:
        //   sort(result, unsortedOut, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TWO GRAPHS TEST" << endl
                          << "===============" << endl;

        bsl::vector<bsl::pair<int, int> > relations;

        relations.push_back(bsl::make_pair(1, 2));
        relations.push_back(bsl::make_pair(2, 3));
        relations.push_back(bsl::make_pair(4, 5));

        bsl::vector<int> results;
        bsl::vector<int> unsorted;
        bool             sorted = TopologicalSortUtil::sort(&results,
                                                            &unsorted,
                                                            relations);
        ASSERT(true == sorted);
        ASSERT(unsorted.empty());

        ASSERTV(results.size(), results.size() == 5);

        ASSERTV(results[0], results[0] == 4);
        ASSERTV(results[1], results[1] == 1);
        ASSERTV(results[2], results[2] == 5);
        ASSERTV(results[3], results[3] == 2);
        ASSERTV(results[4], results[4] == 3);
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
        //:   and the 'unsorted' 'vector's are empty.
        //:
        //: 4 Add a single edge (edge as in graph theory, not to be confused
        //:   with edge cases in software testing) to the input.
        //:
        //: 5 Invoke the simple (non-iterator) version of 'sort'.
        //:
        //: 6 Verify that the function returns 'true', the 'result' 'vector'
        //:   contains the nodes in the same order as they were in the edge,
        //:   and the 'unsorted' 'vector' is empty.
        //
        // Testing:
        //   sort(result, unsortedOut, relations)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EDGE CASES TEST" << endl
                          << "===============" << endl;

        bsl::vector<bsl::pair<bsl::string, bsl::string> > relations;

        bsl::vector<bsl::string> results;
        bsl::vector<bsl::string> unsorted;
        bool                     sorted = TopologicalSortUtil::sort(&results,
                                                                    &unsorted,
                                                                    relations);
        ASSERT(true == sorted);
        ASSERT(unsorted.empty());

        ASSERT(results.size() == 0);

        // One edge

        relations.push_back(bsl::make_pair(bsl::string("1"),
                                           bsl::string("2")));

        sorted = TopologicalSortUtil::sort(&results, &unsorted, relations);
        ASSERT(true == sorted);
        ASSERT(unsorted.empty());

        ASSERTV(results.size(), results.size() == 2);

        ASSERTV(results[0], results[0] == "1");
        ASSERTV(results[1], results[1] == "2");
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
        //:  2 Create two empty 'vectors' 'results', and 'unsorted' for the
        //:    two output arguments of the 'sort' function.
        //:
        //:  3 Run the simple (non-iterator) variant of the 'sort' function.
        //:
        //:  4 Verify that the sort is successful ('sort' returns 'true')
        //:
        //:  5 verify that the 'unsorted' output is empty.
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
        //:    'unsorted' for the two output arguments of the 'sort' function.
        //:
        //:  3 Run the iterator variant of the 'sort' function using the input
        //:    'bsl::list::begin' and 'bsl::list::end' for the input range, a
        //:    back insert iterator for the 'results' 'vector' and an insert
        //:    iterator for the 'unsorted' 'set'.
        //:
        //:  4 Verify that the sort is successful ('sort' returns 'true')
        //:
        //:  5 verify that the 'unsorted' output is empty.
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
            bsl::vector<int> unsorted;
            const bool       sorted = TopologicalSortUtil::sort(&results,
                                                                &unsorted,
                                                                relations);
            ASSERT(true == sorted);
            ASSERT(unsorted.empty());

            ASSERTV(results.size(), results.size() == 8);

            ASSERTV(results[0], results[0] == 7);
            ASSERTV(results[1], results[1] == 3);
            ASSERTV(results[2], results[2] == 5);
            ASSERTV(results[3], results[3] == 8);
            ASSERTV(results[4], results[4] == 11);
            ASSERTV(results[5], results[5] == 2);
            ASSERTV(results[6], results[6] == 9);
            ASSERTV(results[7], results[7] == 10);
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
            bsl::set<int>    unsorted;
            const bool sorted = TopologicalSortUtil::sort(
                                                      relations.begin(),
                                                      relations.end(),
                                                      U_BACK_INSERTER(results),
                                                      U_INSERTER(unsorted));
            ASSERT(true == sorted);
            ASSERT(unsorted.empty());

            ASSERTV(results.size(), results.size() == 8);

            ASSERTV(results[0], results[0] == 7);
            ASSERTV(results[1], results[1] == 3);
            ASSERTV(results[2], results[2] == 5);
            ASSERTV(results[3], results[3] == 8);
            ASSERTV(results[4], results[4] == 11);
            ASSERTV(results[5], results[5] == 2);
            ASSERTV(results[6], results[6] == 9);
            ASSERTV(results[7], results[7] == 10);
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
// Copyright 2018 Bloomberg Finance L.P.
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
