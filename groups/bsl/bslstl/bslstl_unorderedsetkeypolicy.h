// bslalg_bidirectionallinktranslatorforsets.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKTRANSLATORFORSETS
#define INCLUDED_BSLALG_BIDIRECTIONALLINKTRANSLATORFORSETS

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

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINKLISTNODE
#include <bslalg_bidirectionalnode.h>
#endif

namespace BloombergLP
{
namespace bslstl
{


                          // ===============================
                          // BidirectionalLinkTranslatorForSets
                          // ===============================

template <class VALUE_TYPE>
struct UnorderedSetKeyPolicy { 
  public:
    typedef VALUE_TYPE ValueType;
    typedef ValueType  KeyType;

    // Choosing to implement for each policy, to reduce the template mess.
    // With only two policies, not much is saved using a shared dependent base
    // class to provide a common implementation.
    // This is the key abstraction, turning 'BidirectionalLink*' into 'VALUE_TYPE&'
    
    static const KeyType& extractKey(const VALUE_TYPE& obj);
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //-------------------------------------
                        // class BidirectionalLinkTranslatorForSets
                        //-------------------------------------

template <class VALUE_TYPE>
inline
typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::ValueType&
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractValue(
                                                       BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return static_cast<BidirectionalNode<VALUE_TYPE> *>(link)->value();
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::ValueType&
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractValue(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return
     static_cast<const BidirectionalNode<VALUE_TYPE> *>(link)->value();
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::KeyType&
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractKey(
                                                 const BidirectionalLink *link)
{
    BSLS_ASSERT_SAFE(link);

    return extractValue(link);
}

// NEW 
template <class VALUE_TYPE>
inline
typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::MappedType& 
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractMappedValue(
                                                               VALUE_TYPE& obj)
{
        return obj;
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::MappedType& 
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractMappedValue(
                                                         const VALUE_TYPE& obj)
{
        return obj;
}

template <class VALUE_TYPE>
inline
const typename BidirectionalLinkTranslatorForSets<VALUE_TYPE>::KeyType& 
BidirectionalLinkTranslatorForSets<VALUE_TYPE>::extractKey(const VALUE_TYPE& obj)
{
        return obj;
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
