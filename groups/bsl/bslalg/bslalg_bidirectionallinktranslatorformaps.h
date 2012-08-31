// bslalg_bidirectionallinktranslatorformaps.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKTRANSLATORFORMAPS
#define INCLUDED_BSLALG_BIDIRECTIONALLINKTRANSLATORFORMAPS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash table data structure for unordered containers 
//
//@CLASSES:
//   bslalg::BidirectionalLinkTranslatorForMaps : Node holding a value in a bidirectional list
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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE
#include <bslalg_bidirectionallinklistnode.h>
#endif

namespace BloombergLP
{
namespace bslalg
{

                          // ===============================
                          // Hashing policy implemenatations
                          // ===============================

template <class VALUE_TYPE>
struct BidirectionalLinkTranslatorForMaps { 
  public:
    typedef          VALUE_TYPE            ValueType;
    typedef typename ValueType::first_type KeyType;

    // Choosing to implement for each policy, to reduce the template mess.
    // With only two policies, not much is saved using a shared dependent base
    // class to provide a common implementation.
    // This is the key abstraction, turning 'BidirectionalLink*' into 'VALUE_TYPE&'
    static       ValueType& extractValue(      BidirectionalLink *link);
    static const ValueType& extractValue(const BidirectionalLink *link);

    static const KeyType& extractKey(const BidirectionalLink *link);
};


// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================


                  //-----------------------------------------
                  // class BidirectionalLinkTranslatorForMaps
                  //-----------------------------------------

template <class VALUE_TYPE>
inline
typename BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::ValueType&
BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return static_cast<BidirectionalLinkListNode<VALUE_TYPE> *>(link)->value();
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::ValueType&
BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::extractValue(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return
     static_cast<const BidirectionalLinkListNode<VALUE_TYPE> *>(link)->value();
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::KeyType&
BidirectionalLinkTranslatorForMaps<VALUE_TYPE>::extractKey(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return extractValue(link).first;
}

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
