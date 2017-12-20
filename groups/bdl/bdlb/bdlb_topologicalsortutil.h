// bdlb_topologicalsortutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLB_TOPOLOGICALSORTUTIL
#define INCLUDED_BDLB_TOPOLOGICALSORTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to topologically sort a collection of inputs.
//
//@CLASSES:
//    bdlb::TopologicalSortUtil: utility for topologically sorting inputs
//    bdlb::TopologicalSortUtilEdgeTraits: customization point for edge types
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdlb::TopologicalSortUtil', to topologically sort a collection of inputs
// that describe a directed acyclic graph (also known as DAG).  A topological
// sort is useful for defining the order in which an input should be processed
// based on certain conditions.  As an example, consider two jobs B and C both
// of which have a dependency on job A, i.e., job A needs to be completed
// before B or C can be run.  Given such a requirement, a topological sort can
// provide an ordering of the three jobs such that A precedes B and C.  Note
// that there may be multiple orderings that might be correct (e.g.: A, B, then
// C or A, C then B; both are correct and satisfy the dependencies of A running
// before the B and C jobs; we don't care whether B precedes C or vice versa).
//
// The dependencies are specified in pairs (U,V) (read as U precedes V), which
// define the relations between U and V that the sort needs to maintain.  The
// elements in these pairs (e.g., U and V) make up a finite set S.  The output
// of the topological sort is an ordering of all elements of set S, such that
// all relations specified in the input (as pairs) are satisfied.  Note that
// such an ordering is only possible only if there are no cycles in the input
// dependencies.  For example, if A depends on B and B depends on A, then it is
// not possible to order A and B satisfying both the relations.  The routine
// 'sort' defined in this component returns 'false' if it detects a cycle while
// trying to sort the input.
//
// Complexity of the topological sort in this component is 'O(n+m)' where n is
// the number of input elements in the finite set S and m is the number of
// relations as specified by the input pairs.
//
// The 'sort' function is provided in two variants or flavors: simple and
// iterator-based.  The simple flavor is templated on the 'NODE_TYPE', and it
// provides topological sorting for the case where the input is a 'bsl::vector'
// of 'bsl::pair's, and the outputs are 'bsl::vector's.  The templated variant
// is highly customizable (templated) on both the input and the outputs.  The
// input may be any input iterator range that has a 'bsl::pair' 'value_type' or
// a 'value_type' with a 'bdlb::TopologicalSortUtilEdgeTraits' specialization.
// The 'value_type' is determined using 'bsl::iterator::traits'.  The two
// outputs are both defined as templated output iterators and they may have
// different types.  So (for example) the iterator based 'sort' may be used
// with Null 'OUTPUT_ITER' to answer the question "does this graph have
// cycles?" while not wasting memory in storing the sort results.
//
///Self-referencing Nodes
///----------------------
// Some graph representations may use self-referencing nodes to indicate nodes
// without any connection -- where a self referencing node in the context of
// this component is a input edge pair like (u, u).  The implementation of
// 'sort' in this component treats such input entries as cycles and fails
// sorting.
//
// You may still use this 'sort' implementation (to process your nodes in
// proper order) if your data structure contains such entries.  To process a
// graph having self-referencing nodes one may create a filtering iterator on
// top of the input that removes any self-referential edges from the input.
// Any filtered nodes could be treated as being sorted first.
//
///Concepts
///--------
// This component provides generic (templated) utilities.  This section
// describes the requirements of the type parameters (concepts) of these
// generic methods.
//
///'NODE_TYPE' Concept
///- - - - - - - - - -
// The input for a topological sort is supplied as pairs (though not
// necessarily 'bsl::pair's) of 'NODE_TYPE' values.  'NODE_TYPE' is aliased to
// 'NodeType' in 'TopologicalSortUtilEdgeTraits'.
//
// 'NODE_TYPE' shall be a value type that supports hashing using the
// 'bsl::hash<NODE_TYPE>' functor type and equality comparison using the
// 'bsl::equal_to<NODE_TYPE>' functor type.
//
// Notice that we have not provided a customization point for the hash functor
// type nor for the equality functor type because that would complicate the
// code greatly (esp. readability would suffer).
//
///'EdgeType' Concept
/// - - - - - - - - -
// The 'EdgeType' describes an edge as a (conceptual) pair of 'NODE_TYPE'
// values and is supplied as the input to the topological sort methods.
// 'EdgeType' is the 'value_type' of the 'INPUT_ITER' supplied to the 'sort'
// functions.  Conceptually it represents a pair (U,V) (where U and V are
// 'NodeType's) that means "U precedes V".  The 'EdgeType' supplied to a sort
// is typically a 'bsl::pair', but users may customize this by specializing the
// 'TopologicalSortUtilEdgeTraits' type.  'TopologicalSortUtil' determines the
// 'NodeType' and access operations on 'EdgeType' via the
// 'TopologicalSortUtilEdgeTraits' type.
//
// 'TopologicalSortUtilEdgeTraits' is specialized for 'bsl::pair<T,T>', the
// user needs to (partially or fully) specialize it for other types.  See the
// 'CustomEdge' specialization in the test driver for an example.
//
///'INPUT_ITER' Concept
/// - - - - - - - - - -
// 'INPUT_ITER' is an input iterator type used by the 'sort' functions, and its
// 'value_type' is 'EdgeType' .
//
///'OUTPUT_ITER' Concept
///- - - - - - - - - - -
// 'OUTPUT_ITER' is an output iterator for 'sort' functions, and its
// 'value_type' is 'NodeType'.
//
///'UNORDERED_ITER' Concept
/// - - - - - - - - - - - -
//
// 'UNORDERED_ITER' is a (second) output iterator for 'sort' functions, and its
// 'value_type' is 'NodeType'.  Note that 'OUTPUT_ITERATOR' and
// 'UNORDERED_ITER' are distinct types allowing the sorted output nodes to be
// collected in a different way from the unsorted output nodes(e.g., into a
// different type of container).
//
///USAGE:
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using Topological Sort for Calculating Formulas
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are evaluating formulas for a set of market data fields, where
// formulas can reference other fields as part of their calculation.  As an
// example, let's say we have a field 'k_bbgDefinedVwap' that is dependent on
// 'k_vwapTurnover' and 'k_vwapVolume'.  These fields in turn are dependent on
// 'k_tradeSize' and 'k_tradePrice' respectively.  So essentially the fields
// that are not dependent on any other field should be calculated first and
// then the dependent fields.  We can use the topological sort utility to
// provide us the order in which these fields should be calculated.
//
// First, we create the relations showcasing the above mentioned dependencies
// in the form of pairs (a,b) where b is dependent on a:
//..
//  enum FieldIds {
//      k_bbgDefinedVwap  = 0,
//      k_vwapTurnover    = 1,
//      k_vwapVolume      = 2,
//      k_tradeSize       = 3,
//      k_tradePrice      = 4
//  };
//
//  bsl::vector<bsl::pair<int, int> > relations;
//
//  relations.push_back(bsl::make_pair(static_cast<int>(k_vwapTurnover),
//                                     static_cast<int>(k_bbgDefinedVwap)));
//  relations.push_back(bsl::make_pair(static_cast<int>(k_vwapVolume),
//                                     static_cast<int>(k_bbgDefinedVwap)));
//  relations.push_back(bsl::make_pair(static_cast<int>(k_tradeSize),
//                                     static_cast<int>(k_vwapVolume)));
//  relations.push_back(bsl::make_pair(static_cast<int>(k_tradeSize),
//                                     static_cast<int>(k_vwapTurnover)));
//  relations.push_back(bsl::make_pair(static_cast<int>(k_tradePrice),
//                                     static_cast<int>(k_vwapTurnover)));
//..
// Now, we call the topological sort to get a topological order for the fields
// referenced in the relations:
//..
//  bsl::vector<int> results;
//  bsl::vector<int> unordered;
//  bool             sorted = TopologicalSortUtil::sort(&results,
//                                                      &unordered,
//                                                      relations);
//..
// Finally, we verify that the call to 'sort' populates the supplied 'results'
// with a sequence in sorted order (e.g.. 'k_tradeSize', 'k_tradePrice,
// 'k_vwapTurnover', 'k_vwapVolume', 'k_bbgDefinedVwap') and 'unsorted' will be
// empty because the input relationships do not contain a cycle.
//..
//  bool calculated[5] = { 0 };
//  assert(sorted == true);
//  assert(unordered.empty());
//
//  for (bsl::vector<int>::const_iterator iter = results.begin(),
//                                        end  = results.end();
//       iter != end; ++iter) {
//      switch (*iter) {
//        case k_bbgDefinedVwap: {
//          assert(calculated[k_vwapTurnover] == true);
//          assert(calculated[k_vwapVolume]   == true);
//
//          assert(calculated[k_bbgDefinedVwap] == false);
//          calculated[k_bbgDefinedVwap] = true;
//        } break;
//        case k_vwapTurnover: {
//          assert(calculated[k_tradeSize]  == true);
//          assert(calculated[k_tradePrice] == true);
//
//          assert(calculated[k_vwapTurnover] == false);
//          calculated[k_vwapTurnover] = true;
//        } break;
//        case k_vwapVolume: {
//          assert(calculated[k_tradeSize]  == true);
//
//          assert(calculated[k_vwapVolume] == false);
//          calculated[k_vwapVolume] = true;
//        } break;
//        case k_tradeSize: {
//          assert(calculated[k_vwapVolume]   == false);
//          assert(calculated[k_vwapTurnover] == false);
//
//          assert(calculated[k_tradeSize] == false);
//          calculated[k_tradeSize] = true;
//        } break;
//        case k_tradePrice: {
//          assert(calculated[k_vwapTurnover] == false);
//
//          assert(calculated[k_tradePrice] == false);
//          calculated[k_tradePrice] = true;
//        } break;
//        default:
//          assert(false);
//          break;
//      };
//  }
//
//  for (int i = 0; i < 5; ++i) {
//      assert(calculated[i] == true);
//  }
//..
///Example 2: Using Topological Sort with Cycles in Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs that contain a cycle.
//
// First, we define a set of inputs that have a cycle:
//..
//  enum FieldIds2 {
//      k_FIELD1 = 0,
//      k_FIELD2 = 1,
//      k_FIELD3 = 2
//  };
//
//  bsl::vector<bsl::pair<int, int> > relations2;
//
//  relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD2),
//                                      static_cast<int>(k_FIELD1)));
//  relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD3),
//                                      static_cast<int>(k_FIELD2)));
//  relations2.push_back(bsl::make_pair(static_cast<int>(k_FIELD1),
//                                      static_cast<int>(k_FIELD3)));
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::vector<int> results2;
//  bsl::vector<int> unordered2;
//  bool             sorted2 = TopologicalSortUtil::sort(&results2,
//                                                       &unordered2,
//                                                       relations2);
//..
// Finally, we verify whether the routine recognizes that there is a cycle and
// returns false, and the 3 nodes comprising the cycle are reported in
// 'unordered2':
//..
//  assert(sorted2           == false);
//  assert(unordered2.size() == 3);
//..
///Example 3: Using Topological Sort with Self Relations
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs that have input relations where predecessor
// and successor point to the same value, i.e. we have pairs of input like
// (u,u).  This example demonstrates that the 'sort' function handles such
// input as a cycle.  (See Example 6 for a way of processing such nodes and
// avoiding 'sort' reporting a cycle.)  First, we define such a set of inputs:
//..
//  enum FieldIds3 {
//      k_FIELD4 = 3,
//      k_FIELD5 = 4,
//      k_FIELD6 = 5
//  };
//
//  bsl::vector<bsl::pair<int, int> > relations3;
//
//  relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD4),
//                                      static_cast<int>(k_FIELD6)));
//  relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD5),
//                                      static_cast<int>(k_FIELD4)));
//  relations3.push_back(bsl::make_pair(static_cast<int>(k_FIELD4),
//                                      static_cast<int>(k_FIELD4)));
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::vector<int> results3;
//  bsl::vector<int> unordered3;
//  bool             sorted3 = TopologicalSortUtil::sort(&results3,
//                                                       &unordered3,
//                                                       relations3);
//..
// Finally, we verify that the self relations causes the cycle:
//..
//  assert(sorted3           == false);
//  assert(results3.size()   == 1);
//  assert(unordered3.size() == 2);
//
//  if (veryVeryVerbose) {
//      cout << "Size of results3 vector is "
//           << results3.size() << endl;
//
//      cout << "Size of unordered3 vector is "
//           << unordered3.size() << endl;
//  }
//
//  if (veryVeryVerbose)
//  {
//      for (bsl::size_t i = 0; i < results3.size(); ++i) {
//              cout << "results3[" << i << "] is " << results3[i] << "\n";
//      }
//
//      for (bsl::size_t i = 0; i < unordered3.size(); ++i) {
//              cout << "unordered3[" << i << "] is " << unordered3[i] << "\n";
//      }
//  }
//..
///Example 4: Using Topological Sort with Iterators as Input
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs that have input relations that conceptually
// follow the input requirements (of a listing of pairs of nodes) but are not
// physically stored in a 'bsl::vector' of 'bsl::pair' typed container.  Let's
// suppose the input is in a 'bsl::list' instead.  First, we define such a set
// of inputs:
//..
//  bsl::list<bsl::pair<int, int> > relations4;
//
//  relations4.push_back(bsl::make_pair(1, 2));
//  relations4.push_back(bsl::make_pair(1, 3));
//  relations4.push_back(bsl::make_pair(2, 3));
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::vector<int> results4;
//  bsl::vector<int> unordered4;
//  typedef bsl::back_insert_iterator<bsl::vector<int> > OutIter;
//  bool sorted4 = TopologicalSortUtil::sort(relations4.begin(),
//                                           relations4.end(),
//                                           OutIter(results4),
//                                           OutIter(unordered3));
//..
// Finally, we verify that the sort is successful, there are no nodes in the
// 'unordered' output (there is no cycle) and the nodes are listed in the
// proper order in 'results4':
//..
//  assert(sorted4           == true);
//  assert(unordered4.size() == 0);
//  assert(results4.size()   == 3);
//
//  assert(results4[0] == 1);
//  assert(results4[1] == 2);
//  assert(results4[2] == 3);
//..
///Example 5: Using Topological Sort with Iterators as Output
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want our result in a 'bsl::list' instead of a 'bsl::vector' and
// we do not care about the unordered elements so we do not want to pay for
// storing them if they exist.  First, we would define a Null Output Iterator
// that writes to nowhere:
//..
//  class NullOutputIterator {
//    public:
//      typedef void container_type;
//      typedef void value_type;
//      typedef void difference_type;
//      typedef void pointer;
//      typedef void reference;
//      typedef bsl::output_iterator_tag iterator_category;
//
//      template <class T>
//      NullOutputIterator& operator=(const T&)
//      {
//          return *this;
//      }
//
//      NullOutputIterator& operator*()
//      {
//          return *this;
//      }
//
//      NullOutputIterator& operator++()
//      {
//          return *this;
//      }
//
//      NullOutputIterator& operator++(int)
//      {
//          return *this;
//      }
//  };
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::list<int> results5;
//  typedef bsl::back_insert_iterator<bsl::list<int> > ListOutIter;
//  bool sorted5 = TopologicalSortUtil::sort(relations4.begin(),
//                                           relations4.end(),
//                                           ListOutIter(results5),
//                                           NullOutputIterator());
//..
// Finally, we verify that the sort is successful, and the 3 nodes are listed
// in the proper order in the 'results5' list:
//..
//  assert(sorted5           == true);
//  assert(results5.size()   == 3);
//
//  assert(results5[0] == 1);
//  assert(results5[1] == 2);
//  assert(results5[2] == 3);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_QUEUE
#include <bsl_queue.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif


namespace BloombergLP {
namespace bdlb {


                    // ===================================
                    // class TopologicalSortUtilEdgeTraits
                    // ===================================

template <class EDGE_TYPE>
struct TopologicalSortUtilEdgeTraits {
    // This 'struct' represents a customization point allowing clients to
    // supply input iterators to 'sort' working on types other than 'pair'.
    // Clients may specialize 'TopologicalSortUtilEdgeTraits' to supply the
    // following:
    //..
    //  typedef EDGE_TYPE EdgeType;
    //      // The type of the directed connection from one node to another
    //
    //  typedef user-defined NodeType;
    //      // Alias describing the output values from a sort, as well as the
    //      // results of the 'from' and 'to' functions of this edge traits
    //      // instance.  Or in other words, the node (identifier) type of the
    //      // directed acyclic graph.
    //
    //  static const NodeType& from(const EDGE_TYPE& input);
    //      // Return a 'const' reference to the "from" attribute of the
    //      // specified 'input'.  Note that the template parameter type
    //      // 'EDGE_TYPE' is an element in the input range to 'sort'.
    //
    //  static const NodeType& to(const EDGE_TYPE& input)
    //      // Return a 'const' reference to the "from" attribute of the
    //      // specified 'input'.  Note that the template parameter type
    //      // 'EDGE_TYPE' is an element in the input range to 'sort'.
    //..
};

template <class NODE_TYPE>
struct TopologicalSortUtilEdgeTraits<bsl::pair<NODE_TYPE, NODE_TYPE> > {
    // This 'struct' is a specialization (customization) of
    // 'TopologicalSortUtilEdgeTraits' for bsl::pair<T, T>.

    // TYPES
    typedef bsl::pair<NODE_TYPE, NODE_TYPE> EdgeType;
        // The type of the directed connection from one node to another

    typedef NODE_TYPE NodeType;
        // The type of values of the nodes of the directed acyclic graph

    // CLASS METHODS
    static const NODE_TYPE& from(const EdgeType& edge);
        // Return a 'const' reference to the 'from' attribute of the specified
        // 'edge' object.

    static const NODE_TYPE& to(const EdgeType& edge);
        // Return a 'to' reference to the 'from' attribute of the specified
        // 'edge' object.
};

                        // ================================
                        // class TopologicalSortUtil_Helper
                        // ================================

template <class INPUT_ITER>
class TopologicalSortUtil_Helper {
    // This class template provides data structures required to sort the
    // partial unordered pairs into a total ordered set.  The value type of the
    // nodes(*) must be hashable and equality comparable by 'bsl::hash', and
    // 'bsl::equal_to' respectively.
    //
    // (*) The value type is determined by first getting the iterator's value
    //     type using 'bsl::iterator_traits<INPUT_ITER>::ValueType' and then
    //     using 'TopologicalSortUtilEdgeTraits::ValueType' on that result.

    // PRIVATE TYPES
    typedef typename bsl::iterator_traits<INPUT_ITER>::value_type
                                                                 IteratorValue;
    typedef TopologicalSortUtilEdgeTraits<IteratorValue>         EdgeTraits;
    typedef typename EdgeTraits::NodeType                        NodeType;

    struct NodeInfo {
        // PUBLIC DATA
        int                   d_predecessorCount;
        bsl::vector<NodeType> d_successors;        // successors list

        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(NodeInfo, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit NodeInfo(bslma::Allocator *allocator = 0);
            // Create a 'NodeInfo' which holds the predecessor and successor
            // information of an input element.  Optionally, specify an
            // 'allocator' for needed memory.  If 'allocator' is 0, use the
            // globally supply default allocator instead.

        NodeInfo(const NodeInfo& original, bslma::Allocator *allocator = 0);
            // Create a 'NodeInfo' object having the same value as the
            // specified 'original' object.  Optionally specify an 'allocator'
            // used to supply memory.  If 'allocator' is 0, the currently
            // installed default allocator is used.
    };

    typedef bsl::unordered_map<NodeType, NodeInfo> SetInfo;

    // DATA
    SetInfo              d_setInfo;      // additional data structure needed
                                         // for topologically sorting the
                                         // elements

    bsl::queue<NodeType> d_orderedNodes;  // elements that are not dependent
                                          // on any other

    bool                 d_hasCycle;     // flag denoting whether cycles are
                                         // present
  private:
    // NOT IMPLEMENTED
      TopologicalSortUtil_Helper(const TopologicalSortUtil_Helper&);
      TopologicalSortUtil_Helper& operator=(const TopologicalSortUtil_Helper&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                        TopologicalSortUtil_Helper, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TopologicalSortUtil_Helper(INPUT_ITER        relationsBegin,
                                        INPUT_ITER        relationsEnd,
                                        bslma::Allocator *allocator = 0);
        // Create a helper class that holds the different data structures
        // required to sort in topological order the directed acyclic graph
        // described by the specified 'relationsBegin' and 'relationsEnd'.
        // Optionally, specify an 'allocator' for needed memory.  If
        // 'allocator' is 0, use the globally supply default allocator instead.

    // MANIPULATORS
    template <class RESULT_ITER>
    bool processNext(RESULT_ITER result);
        // Load an element that doesn't have any predecessors into the
        // specified 'result'.  Return 'false' if there is no such element left
        // or the input set is fully processed else return 'true'.  Note that
        // if this method detects cycles in the input set it will set the
        // 'hasCycle' flag to 'true' and return 'false'.

    // ACCESSORS
    bool hasCycle() const;
        // Return 'true' if relations specified at construction time contains a
        // cycle, otherwise returns 'false'.  The behavior is undefined unless
        // 'processNext' has been called, and the most recent invocation of
        // 'processNext' returned 'false'.

    template <class UNORDERED_ITER>
    void unordered(UNORDERED_ITER unordered) const;
        // Load elements that have not been sorted to the specified
        // 'unordered'.  Note that the behavior is undefined if this method is
        // called before the sort routine is finished (determined by
        // 'processNext' method returning 'false') and it has been determined
        // that there is a cycle that can be found by using the 'hasCycle'
        // method.
};

                        // =====================
                        // class TopologicalSort
                        // =====================

struct TopologicalSortUtil {
    // This 'struct' 'TopologicalSortUtil' provides a namespace for topological
    // sorting functions.

  public:
    // CLASS METHODS
    template <class INPUT_ITER, class RESULT_ITER, class UNORDERED_ITER>
    static bool sort(INPUT_ITER        relationsBegin,
                     INPUT_ITER        relationsEnd,
                     RESULT_ITER       result,
                     UNORDERED_ITER    unordered);
        // Sort the input elements in topological order and load the resulting
        // linear ordered set into the specified 'result' output, and if the
        // sort is unsuccessful (the input is not an acyclic directed graph)
        // load the elements that have not been ordered to the specified
        // 'unordered' output; the input elements are provided (conceptually)
        // as a sequence of pairs between the specified 'relationsBegin' and
        // 'relationsEnd', where the "from" element of the pair must precede
        // the "to" element in the resulting sort (see
        // 'TopologicalSortUtilEdgeTraits' for description of "from" and "to").
        // Return 'true' on success, and 'false' if the sort fails due to a
        // cycle in the input.  The input ('relationshipBegin' and
        // 'relationshipEnd') is provided as a sequence of (conceptual or
        // physical) pairs of the form (U, V) where U precedes V in the output.
        // The type 'bsl::iterator_traits<INPUT_ITER>::value_type' must either
        // be 'bsl::pair' where the 'first_type' and 'second_type' are the same
        // as 'bsl::iterator_traits<OUTPUT_ITER>::value_type' or
        // 'TopologicalSortUtilEdgeTraits' must be specialized for the type,
        // i.e., the supplied 'bsl::iterator_traits<INPUT_ITER>::value_type'
        // must support the following syntax:
        //..
        //  typedef typename bsl::iterator_traits<INPUT_ITER>::value_type
        //                                                           IterValue;
        //  typedef typename bsl::iterator_traits<RESULT_ITER>::value_type
        //                                                         ResultValue;
        //
        //  typedef typename TopologicalSortUtilEdgeTraits<IterValue>   Traits;
        //
        //  typedef typename Traits::NodeType NodeType;
        //
        //  for (INPUT_ITER it  = relationshipPairsBegin;
        //                  it != relationshipPairsEnd;
        //                  ++it) {
        //
        //      *result++ = Traits::from(*it);
        //      *result++ = Traits::to(*it);
        //  }
        //..
        // Note that even if the method returns 'false', 'result' may contain a
        // subset of elements in the right topological order, essentially
        // elements that the routine was able to sort before the cycle was
        // discovered and 'unordered' will contain the elements that the
        // routine was unable to sort.

    template <class NODE_TYPE>
    static bool sort(
           bsl::vector<NODE_TYPE>                               *result,
           bsl::vector<NODE_TYPE>                               *unorderedList,
           const bsl::vector<bsl::pair<NODE_TYPE, NODE_TYPE> >&  relations);
        // Sort the elements of 'NODE_TYPE' in topological order determined by
        // the specified 'relations' and load the resulting linear ordered set
        // to the specified 'result'.  If the sort is unsuccessful, load the
        // elements that have not been ordered to the specified 'unorderedList'
        // list.  The input relations are provided as pairs of the form (U, V)
        // where U precedes V in the output.  Return 'false' if sort fails due
        // to a cycle in the input else return 'true' if sort successful.  Note
        // that even if the method returns 'false', 'result' may contain a
        // subset of elements in the right topological order, essentially
        // elements that the routine was able to sort before the cycle was
        // discovered and 'unorderedList' will contain the elements that the
        // routine was unable to sort.
};

// ============================================================================
//                 INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                   // -----------------------------------
                   // class TopologicalSortUtilEdgeTraits
                   // -----------------------------------

template <class NODE_TYPE>
inline
const NODE_TYPE&
TopologicalSortUtilEdgeTraits<bsl::pair<NODE_TYPE, NODE_TYPE> >::
from(const EdgeType& edge)
{
    return edge.first;
}

template <class NODE_TYPE>
inline
const NODE_TYPE&
TopologicalSortUtilEdgeTraits<bsl::pair<NODE_TYPE, NODE_TYPE> >::
to(const EdgeType& edge)
{
    return edge.second;
}


                 // ------------------------------------------
                 // class TopologicalSortUtil_Helper::NodeInfo
                 // ------------------------------------------

// CREATORS
template <class INPUT_ITER>
inline
TopologicalSortUtil_Helper<INPUT_ITER>::NodeInfo::NodeInfo(
                                                   bslma::Allocator *allocator)
: d_predecessorCount(0)
, d_successors(allocator)
{
}

template <class INPUT_ITER>
inline
TopologicalSortUtil_Helper<INPUT_ITER>::NodeInfo::NodeInfo(
                                                   const NodeInfo&   original,
                                                   bslma::Allocator *allocator)
: d_predecessorCount(original.d_predecessorCount)
, d_successors(original.d_successors, allocator)
{
}
                      // --------------------------------
                      // class TopologicalSortUtil_Helper
                      // --------------------------------

// CREATORS
template <class INPUT_ITER>
TopologicalSortUtil_Helper<INPUT_ITER>::TopologicalSortUtil_Helper(
                                              INPUT_ITER        relationsBegin,
                                              INPUT_ITER        relationsEnd,
                                              bslma::Allocator *allocator)
: d_setInfo(allocator)
, d_orderedNodes(allocator)
, d_hasCycle(false)
{
    // Iterate through the partial ordered set and create the input from the
    // pairs

    for (INPUT_ITER iter = relationsBegin; iter != relationsEnd; ++iter) {
        const NodeType& from = EdgeTraits::from(*iter);
        const NodeType& to = EdgeTraits::to(*iter);
        ++d_setInfo[to].d_predecessorCount;
        d_setInfo[from].d_successors.push_back(to);
    }

    // Now iterate through the set and find nodes that are independent i.e.
    // which don't have any predecessors and hence are already ordered.  We put
    // these nodes into a non dependent elements queue.


    typename SetInfo::const_iterator setIter = d_setInfo.begin();
    for (; setIter != d_setInfo.end(); ++setIter ) {
        if (0 == setIter->second.d_predecessorCount) {
            d_orderedNodes.push(setIter->first);
        }
    }
}

// MANIPULATORS
template <class INPUT_ITER>
template <class RESULT_ITER>
bool TopologicalSortUtil_Helper<INPUT_ITER>::processNext(RESULT_ITER result)
{
    // process element with no predecessors

    if (! d_orderedNodes.empty()) {

        NodeType                   processed = d_orderedNodes.front();
        typename SetInfo::iterator processedNode = d_setInfo.find(processed);

        // iterate through the successor list of the node and reduce
        // predecessor count of each successor.  Further if this count becomes
        // zero add it 'd_orderedNodes'.

        const NodeInfo&                                 processedNodeInfo =
                                                         processedNode->second;
        typename bsl::vector<NodeType>::const_iterator  sListIter =
                                      (processedNodeInfo.d_successors).begin();
        typename bsl::vector<NodeType>::const_iterator  endIter =
                                        (processedNodeInfo.d_successors).end();

        for (; sListIter != endIter; ++sListIter) {

            typename SetInfo::iterator successor =
                                                  d_setInfo.find((*sListIter));
            NodeType                   sValue = successor->first;
            NodeInfo&                  sNodeInfo = successor->second;

            // update predecessor count

            --(sNodeInfo.d_predecessorCount);

            // add node to ordered if all predecessors processed

            if ((sNodeInfo.d_predecessorCount) == 0) {
                d_orderedNodes.push(sValue);
            }
        }

        // remove this node from the set since its already processed

        d_setInfo.erase(processedNode);

        // remove this node from the d_orderedNodes and return

        *result = processed;
        d_orderedNodes.pop();
        return true;                                                  // RETURN
    }

    // If the queue is empty but the input set is not then we have at least one
    // cycle.  Set the flag and return.

    d_hasCycle = !d_setInfo.empty();

    return false;
}

// ACCESSORS
template <class INPUT_ITER>
inline
bool TopologicalSortUtil_Helper<INPUT_ITER>::hasCycle() const
{
    return d_hasCycle;
}

template <class INPUT_ITER>
template <class UNORDERED_ITER>
    void TopologicalSortUtil_Helper<INPUT_ITER>::unordered(
                                            UNORDERED_ITER unorderedList) const
{

    BSLS_ASSERT(d_hasCycle == true);

    // load unorderedList that the elements that are still present in the set

    for (typename SetInfo::const_iterator elemIter = d_setInfo.begin(),
                                          endIter  = d_setInfo.end();
                                          elemIter != endIter; ++ elemIter) {
        *unorderedList = elemIter->first;
        ++unorderedList;
    }
}

                        // -------------------------
                        // class TopologicalSortUtil
                        // -------------------------

// MANIPULATORS
template <class INPUT_ITER, class RESULT_ITER, class UNORDERED_ITER>
bool TopologicalSortUtil::sort(INPUT_ITER        relationsBegin,
                               INPUT_ITER        relationsEnd,
                               RESULT_ITER       result,
                               UNORDERED_ITER    unordered)
{
    TopologicalSortUtil_Helper<INPUT_ITER> tSortHelper(relationsBegin,
                                                       relationsEnd);

    while (tSortHelper.processNext(result));

    if (tSortHelper.hasCycle()) {
        // load unorderedList with unordered elements

        tSortHelper.unordered(unordered);
        return false;                                                 // RETURN
    }
    return true;
}

template <class NODE_TYPE>
bool TopologicalSortUtil::sort(
           bsl::vector<NODE_TYPE>                               *result,
           bsl::vector<NODE_TYPE>                               *unorderedList,
           const bsl::vector<bsl::pair<NODE_TYPE, NODE_TYPE> >&  relations)
{
    typedef bsl::vector<NODE_TYPE> vector_type;

    return sort(relations.begin(),
                relations.end(),
                bsl::back_insert_iterator<vector_type>(*result),
                bsl::back_insert_iterator<vector_type>(*unorderedList));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2017
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
