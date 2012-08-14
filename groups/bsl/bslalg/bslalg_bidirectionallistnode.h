// bslalg_bidirectionallistnode.h                                     -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLISTNODE
#define INCLUDED_BSLALG_BIDIRECTIONALLISTNODE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash table data structure for unordered containers 
//
//@CLASSES:
//   bslALG::BidirectionalListNode : Node holding a value in a bidirectional list
//
//@SEE_ALSO: bslalg_hashtableutil
//
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides ...
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

namespace BloombergLP
{
namespace bslalg
{

                        // ===========================
                        // class BidirectionalListNode
                        // ===========================

template <class VALUE_TYPE>
struct BidirectionalListNode : BidirectionalLink {
    typedef VALUE_TYPE    ValueType;

    ValueType  d_value;
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


} // namespace BloombergLP::bslalg

} // namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
