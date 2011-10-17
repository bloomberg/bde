// bslstl_rbtree_node.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_RBTREE_NODE
#define INCLUDED_BSLSTL_RBTREE_NODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: 
//
//@CLASSES:
//   bslstl::RbTree_Node: 
//
//@SEE_ALSO:
//
//@AUTHOR: 
//
//@DESCRIPTION: 
//
///Usage
///-----

#ifndef BSLALG_RBTREENODE
#include <bslalg_rbtreenode.h>
#endif

namespace BloombergLP {
namespace bslstl {

                        // =================
                        // class RbTree_Node
                        // =================


template <class VALUE_TYPE>
class RbTree_Node : public bslalg::RbTreeNode {

  public:
       
    // PUBLIC DATA
    VALUE_TYPE d_value;  // payload value

  private:
    // The following functions are not defined because a RbTreeNode should
    // never be constructed, destructored, or assigned.  The 'd_value' member
    // should be separately constructed and destroyed using an allocator's
    // 'construct' and 'destroy' methods.

    RbTree_Node();                              // Declared but not defined
    RbTree_Node(const RbTree_Node&);            // Declared but not defined
    RbTree_Node& operator=(const RbTree_Node&); // Declared but not defined
    ~RbTree_Node();                             // Declared but not defined
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

}  // close namespace bslstl
}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
