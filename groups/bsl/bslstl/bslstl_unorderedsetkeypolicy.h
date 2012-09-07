// bslstl_unorderedsetkeypolicy.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYPOLICY
#define INCLUDED_BSLSTL_UNORDEREDSETKEYPOLICY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a hash table data structure for unordered containers 
//
//@CLASSES:
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
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

namespace BloombergLP
{

namespace bslstl
{

                          // ============================
                          // struct UnorderedSetKeyPolicy
                          // ============================

template <class VALUE_TYPE>
struct UnorderedSetKeyPolicy { 
  public:
    typedef VALUE_TYPE ValueType;
    typedef ValueType  KeyType;

    // Choosing to implement for each policy, to reduce the template mess.
    // With only two policies, not much is saved using a shared dependent base
    // class to provide a common implementation.
    
    // CLASS METHODS
    static const KeyType& extractKey(const VALUE_TYPE& obj);
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        //-----------------------------
                        // struct UnorderedSetKeyPolicy
                        //-----------------------------


template <class VALUE_TYPE>
inline
const typename UnorderedSetKeyPolicy<VALUE_TYPE>::KeyType& 
UnorderedSetKeyPolicy<VALUE_TYPE>::extractKey(const VALUE_TYPE& obj)
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
