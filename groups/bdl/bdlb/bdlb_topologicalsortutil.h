// bdlb_topologicalsortutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLB_TOPOLOGICALSORTUTIL
#define INCLUDED_BDLB_TOPOLOGICALSORTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility to topologically sort a set of inputs.
//
//@CLASSES:
//    bdlb::TopologicalSortUtil: routine for topologically sorting inputs
//
//@SEE ALSO:
//
//@DESCRIPTION: This component provides an utility 'struct',
// 'TopologicalSortUtil' to topologically sort a set of inputs.  A topological
// sort is useful for defining the order in which an input should be processed
// based on certain conditions.  As an example consider two jobs B and C both
// of which have a dependency on job A, i.e. job A needs to be completed before
// B and C can be run.  Given such a requirement a topological sort can provide
// an ordering of the three jobs such that A precedes B and C.  Note that there
// may be multiple orderings that might be correct (e.g.: A, B and C or A, C
// and B, both are correct and satisfy the dependencies of A running before the
// B and C jobs, we don't care whether B precedes C or vice versa).
//
// The dependencies are specified in pairs (U,V) (read as U precedes V), which
// define the relations between U and V that the sort needs to maintain.  The
// elements in these pairs make up a finite set S, the output of the
// topological sort is an ordering of all elements of set S, such that all
// relations specified in the input (as pairs) are satisfied.  Note that such
// an ordering is only possible if there are no cycles in the input
// dependencies.  For example, if A depends on B and B depends on A then it is
// not possible to order A and B satisfying both the relations.  The routine
// 'sort' defined in this component returns 'false' if it detects a cycle while
// trying to sort the input.
//
// Complexity of the topological sort in this component is 'O(n+m)' where n is
// the number of input elements in the finite set S and m is the number of
// relations as specified by the input pairs.
//
///USAGE:
///-----
// This section illustrates intended use of this component.
//
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
//  relations.push_back(bsl::make_pair((int) k_vwapTurnover,
//                                     (int) k_bbgDefinedVwap));
//  relations.push_back(bsl::make_pair((int) k_vwapVolume,
//                                     (int) k_bbgDefinedVwap));
//  relations.push_back(bsl::make_pair((int) k_tradeSize,
//                                     (int) k_vwapVolume));
//  relations.push_back(bsl::make_pair((int) k_tradeSize,
//                                     (int) k_vwapTurnover));
//  relations.push_back(bsl::make_pair((int) k_tradePrice,
//                                     (int) k_vwapTurnover));
//..
// Now, we call the topological sort to get a topological order for the fields
// referenced in the relations:
//..
//  bsl::vector<int> results;
//  bsl::vector<int> unordered;
//  bool sorted = TopologicalSortUtil::sort(&results, &unordered, relations);
//..
// Finally, we verify that the order of the fields that the sort returns is
// topologically correct:
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
//          calculated[k_bbgDefinedVwap] = true;
//        } break;
//        case k_vwapTurnover: {
//          assert(calculated[k_tradeSize]  == true);
//          assert(calculated[k_tradePrice] == true);
//
//          calculated[k_vwapTurnover] = true;
//        } break;
//        case k_vwapVolume: {
//          assert(calculated[k_tradeSize]  == true);
//
//          calculated[k_vwapVolume] = true;
//        } break;
//        case k_tradeSize: {
//          assert(calculated[k_vwapVolume]   == false);
//          assert(calculated[k_vwapTurnover] == false);
//
//          calculated[k_tradeSize] = true;
//        } break;
//        case k_tradePrice: {
//          assert(calculated[k_vwapTurnover] == false);
//
//          calculated[k_tradePrice] = true;
//        } break;
//        default:
//          assert(false);
//          break;
//      };
//  }
//..
///Example 2: Using topological sort with cycles in input
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which contain a cycle.
//
// First, we define a set of inputs which have a cycle:
//..
//  enum FieldIds2 {
//      FIELD1 = 0,
//      FIELD2 = 1,
//      FIELD3 = 2
//  };
//
//  bsl::vector<bsl::pair<int, int> > relations2;
//
//  relations2.push_back(bsl::make_pair((int) FIELD2,
//                                      (int) FIELD1));
//  relations2.push_back(bsl::make_pair((int) FIELD3,
//                                      (int) FIELD2));
//  relations2.push_back(bsl::make_pair((int) FIELD1,
//                                      (int) FIELD3));
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::vector<int> results2;
//  bsl::vector<int> unordered2;
//  bool sorted2 = TopologicalSortUtil::sort(&results2,
//                                           &unordered2,
//                                            relations2);
//..
// Finally, we verify whether the routine recognizes that there is a cycle and
// returns false:
//..
//  assert(sorted2           == false);
//  assert(unordered2.size() == 3);
//..
///Example 3: Using topological sort with self relations
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a set of inputs which have input relations where predecessor
// and successor point to the same value, i.e. we have pairs of input like
// (u,u).  First, we define such a set of inputs:
//..
//  enum FieldIds3 {
//      FIELD4 = 3,
//      FIELD5 = 4,
//      FIELD6 = 5
//  };
//
//  bsl::vector<bsl::pair<int, int> > relations3;
//
//  relations3.push_back(bsl::make_pair((int) FIELD4,
//                                      (int) FIELD6));
//  relations3.push_back(bsl::make_pair((int) FIELD5,
//                                      (int) FIELD4));
//  relations3.push_back(bsl::make_pair((int) FIELD4,
//                                      (int) FIELD4));
//..
// Now, we apply the topological sort routine on the input:
//..
//  bsl::vector<int> results3;
//  bsl::vector<int> unordered3;
//  bool sorted3 = TopologicalSortUtil::sort(&results3,
//                                           &unordered3,
//                                            relations3);
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
//      for (int i = 0; i < results3.size(); ++i) {
//              cout << "results3[" << i << "] is " << results3[i] << "\n";
//      }
//
//      for (int i = 0; i < unordered3.size(); ++i) {
//              cout << "unordered3[" << i << "] is " << unordered3[i] << "\n";
//      }
//  }
//..
//

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#include <bsl_vector.h>
#include <bsl_queue.h>
#include <bsl_unordered_map.h>
#include <bsl_utility.h>


namespace BloombergLP {
namespace bdlb {
                        // ================================
                        // class TopologicalSortUtil_Helper
                        // ================================

template <class VALUE_TYPE>
class TopologicalSortUtil_Helper {
    // This class provides data structures required to sort the partial
    // unordered pairs into a total ordered set.

    // PRIVATE TYPES
    struct NodeInfo {
        // PUBLIC DATA
        int                     d_predecessorCount;
        bsl::vector<VALUE_TYPE> d_successors;      // successors list

        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(NodeInfo, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit NodeInfo(bslma::Allocator *allocator);
            // Create a 'NodeInfo' which holds the predecessor and successor
            // information of an input element.  Optionally, specify an
            // 'allocator' for needed memory.  If 'allocator' is 0, use the
            // globally supply default allocator instead.
    };

    typedef bsl::vector< bsl::pair<VALUE_TYPE, VALUE_TYPE> > Relations;
    typedef bsl::unordered_map<VALUE_TYPE, NodeInfo *>       SetInfo;

    // DATA
    SetInfo                 d_setInfo;     // additional data structure needed
                                           // for topologically sorting the
                                           // elements
    bsl::queue<VALUE_TYPE>  d_orderedNodes; // elements which are not dependent
                                            // on any other
    bool                    d_hasCycle;    // flag denoting whether cycles are
                                           // present
    bslma::Allocator       *d_allocator_p; // for memory

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(
                        TopologicalSortUtil_Helper, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TopologicalSortUtil_Helper(
        const bsl::vector< bsl::pair<VALUE_TYPE, VALUE_TYPE> >& relations,
        bslma::Allocator                                       *allocator = 0);
        // Create a helper class which holds the different data structures
        // required to sort the specified 'relations' in topological order.
        // Optionally, specify an 'allocator' for needed memory.  If
        // 'allocator' is 0, use the globally supplied default allocator
        // instead.

    ~TopologicalSortUtil_Helper();
        // Destroy this object.

    // MANIPULATORS
    bool nextProcessed(VALUE_TYPE *result);
        // Load an element which doesn't have any predecessors into the
        // specified 'result'.  Return 'false' if there is no such element left
        // or the input set is fully processed else return 'true'.  Note that
        // if this method detects cycles in the input set it will set the
        // 'd_hasCycle' flag to 'true' and return 'false'.  Please use the
        // 'hasCycle' accessor to access the value of the flag.

    // ACCESSORS
    bool hasCycle() const;
        // Returns 'true' if 'd_hasCycle' flag has been set else returns
        // 'false'.

    void unordered(bsl::vector<VALUE_TYPE> *unorderedList) const;
        // Load elements which have not been sorted to the specified
        // 'unorderedList'.  Note that the behavior is undefined if this method
        // is called before the sort routine is finished (determined by
        // 'nextProcessed' method returning 'false') and it has been determined
        // that there is a cycle which can be found by using the 'hasCycle'
        // method.
};

                        // =====================
                        // class TopologicalSort
                        // =====================

struct TopologicalSortUtil {
    // This class provides a namespace for a topological sort utility.

  public:
    // CLASS METHODS
      template <class VALUE_TYPE>
    static bool sort(
         bsl::vector<VALUE_TYPE>                               *result,
         bsl::vector<VALUE_TYPE>                               *unorderedList,
         const bsl::vector<bsl::pair<VALUE_TYPE, VALUE_TYPE> >& relations,
         bslma::Allocator                                      *allocator = 0);
        // Sort the elements of 'VALUE_TYPE' in topological order determined by
        // the specified 'relations' and load the resulting linear ordered set
        // to the specified 'result'.  If the sort is unsuccessful load the
        // elements which have not been ordered to the specified
        // 'unorderedList' list.  The input relations are provided as pairs of
        // the form (U, V) where U precedes V in the output.  Optionally,
        // specify an 'allocator' for memory.  If 'allocator' is 0, use the
        // globally supply default allocator instead.  Return 'false' if sort
        // fails due to a cycle in the input else return 'true' if sort
        // successful.  Optionally use the specified 'HASH' and 'EQUAL' functor
        // types in the 'unordered_map' used temporarily during processing.
        // Note that even if the method returns false 'result' can contain a
        // subset of elements in the right topological order, essentially
        // elements which the routine was able to sort before the cycle was
        // discovered and 'unorderedList' will contain the elements which the
        // routine was unable to sort.
};

                 // ------------------------------------------
                 // class TopologicalSortUtil_Helper::NodeInfo
                 // ------------------------------------------

// CREATORS
template <class VALUE_TYPE>
TopologicalSortUtil_Helper<VALUE_TYPE>::NodeInfo::NodeInfo(
                                                   bslma::Allocator *allocator)
: d_predecessorCount(0)
, d_successors(allocator)
{
}

                      // --------------------------------
                      // class TopologicalSortUtil_Helper
                      // --------------------------------

// CREATORS
template <typename VALUE_TYPE>
TopologicalSortUtil_Helper<VALUE_TYPE>::TopologicalSortUtil_Helper(
             const bsl::vector< bsl::pair<VALUE_TYPE, VALUE_TYPE> >& relations,
             bslma::Allocator                                       *allocator)
: d_setInfo      (allocator)
, d_orderedNodes (allocator)
, d_hasCycle     (false)
, d_allocator_p  (bslma::Default::allocator(allocator))
{
    // Iterate through the partial ordered set and create the input from the
    // pairs

    typename Relations::const_iterator iter = relations.begin();
    for (; iter != relations.end(); ++iter) {
        typename SetInfo::iterator pIter = d_setInfo.find(iter->first);
        if (pIter == d_setInfo.end()) {
            // Create new node info and add it to set info.

            NodeInfo *nodeInfo = new (*d_allocator_p) NodeInfo(d_allocator_p);
            bsl::pair<typename SetInfo::iterator, bool> result =
                     d_setInfo.insert(bsl::make_pair(iter->first, nodeInfo));
            pIter = result.first;
        }

        typename SetInfo::iterator sIter = d_setInfo.find(iter->second);
        if (sIter == d_setInfo.end()) {
           // Create new node info and add it to set info.

            NodeInfo *nodeInfo = new (*d_allocator_p) NodeInfo(d_allocator_p);
            bsl::pair<typename SetInfo::iterator, bool> result =
                    d_setInfo.insert(bsl::make_pair(iter->second, nodeInfo));
            sIter = result.first;
        }

        // add predecessor count for each successor of the pair being iterated
        // currently

        ++sIter->second->d_predecessorCount;

        // add the successor to the list

        pIter->second->d_successors.push_back(sIter->first);
    }

    // Now iterate through the set and find nodes which are independent i.e.
    // which don't have any predecessors and hence are already ordered.  We put
    // these nodes into a non dependent elements queue.


    typename SetInfo::const_iterator setIter = d_setInfo.begin();
    for (; setIter != d_setInfo.end(); ++setIter ) {
        if (setIter->second->d_predecessorCount == 0) {
            d_orderedNodes.push(setIter->first);
        }
    }
}

template <typename VALUE_TYPE>
TopologicalSortUtil_Helper<VALUE_TYPE>::~TopologicalSortUtil_Helper()
{
    // Iterate through the set and delete any elements which have not been
    // processed.

    typename SetInfo::const_iterator iter = d_setInfo.begin();
    for (; iter != d_setInfo.end(); ++iter) {
        d_allocator_p->deleteObject(iter->second);
    }
}

// MANIPULATORS
template <typename VALUE_TYPE>
bool TopologicalSortUtil_Helper<VALUE_TYPE>::nextProcessed(VALUE_TYPE *result)
{
    // process element with no predecessors

    if (! d_orderedNodes.empty()) {

        VALUE_TYPE processed = d_orderedNodes.front();
        typename SetInfo::iterator processedNode = d_setInfo.find(processed);

        // iterate through the successor list of the node and reduce
        // predecessor count of each successor.  Further if this count becomes
        // zero add it 'd_orderedNodes'.

        NodeInfo *processedNodeInfo = processedNode->second;
        typename bsl::vector<VALUE_TYPE>::iterator sListIter =
                                     (processedNodeInfo->d_successors).begin();
        typename bsl::vector<VALUE_TYPE>::iterator endIter =
                                      (processedNodeInfo->d_successors).end();

        for(; sListIter != endIter; ++sListIter) {

           typename SetInfo::iterator successor = d_setInfo.find((*sListIter));
           VALUE_TYPE sValue                    = successor->first;
           NodeInfo  *sNodeInfo                 = successor->second;

           // update predecessor count

           --(sNodeInfo->d_predecessorCount);

           // add node to ordered if all predecessors processed

            if ((sNodeInfo->d_predecessorCount) == 0) {
                d_orderedNodes.push(sValue);
            }
        }

        // remove this node from the set since its already processed

        d_allocator_p->deleteObject(processedNodeInfo);
        d_setInfo.erase(processedNode);

        // remove this node from the d_orderedNodes and return

        *result = processed;
        d_orderedNodes.pop();
        return true;                                                  // RETURN
    }

    // If the queue is empty but the input set is not then we have at least one
    // cycle.  Set the flag and return.

    if (! d_setInfo.empty()) {
        // set cycle flag

        d_hasCycle = true;
    }
    return false;
}

// ACCESSORS
template <typename VALUE_TYPE>
bool TopologicalSortUtil_Helper<VALUE_TYPE>::hasCycle() const
{
    return d_hasCycle;
}

template <typename VALUE_TYPE>
void TopologicalSortUtil_Helper<VALUE_TYPE>::unordered(
                                  bsl::vector<VALUE_TYPE> *unorderedList) const
{

    BSLS_ASSERT(d_hasCycle == true);

    // load unorderedList with the elements which are still present in the set

    for(typename SetInfo::const_iterator elemIter = d_setInfo.begin(),
                                         endIter  = d_setInfo.end();
                                         elemIter != endIter; ++ elemIter) {
        unorderedList->push_back(elemIter->first);
    }
}

                        // -------------------------
                        // class TopologicalSortUtil
                        // -------------------------

// MANIPULATORS
template <typename VALUE_TYPE>
bool TopologicalSortUtil::sort(
              bsl::vector<VALUE_TYPE>                           *result,
              bsl::vector<VALUE_TYPE>                           *unorderedList,
              const bsl::vector<bsl::pair<VALUE_TYPE, VALUE_TYPE> >&
                                                                 relations,
              bslma::Allocator                                  *allocator)
{
    TopologicalSortUtil_Helper<VALUE_TYPE> tSortHelper(relations, allocator);

    VALUE_TYPE processed;
    while (tSortHelper.nextProcessed(&processed)) {
        result->push_back(processed);
    }

    if (tSortHelper.hasCycle()) {
        // load unorderedList with unordered elements

        tSortHelper.unordered(unorderedList);
        return false;                                                 // RETURN
    }
    return true;
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
