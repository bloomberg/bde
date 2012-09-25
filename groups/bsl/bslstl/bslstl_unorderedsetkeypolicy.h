// bslstl_unorderedsetkeypolicy.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYPOLICY
#define INCLUDED_BSLSTL_UNORDEREDSETKEYPOLICY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a policy class to use a whole object as its own key.
//
//@CLASSES:
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides an identity transformation.
// 'bslalg::HashTableImpUtil' has a static 'extractKey' function template
// that, given a 'value type', will represent objects stored in a data
// structure, will abstract out the 'key type' portion of that object.  In the
// case of the 'unordered_set' data structure, the 'key type' and the
// 'value type' are one and the same, so the 'extractKey' transformation is a
// trivial identity transformation.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslstl {

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
    static const KeyType& extractKey(const VALUE_TYPE& object);
        // Given a specified 'object', return a reference to the 'KeyType'
        // contained within that object.  In this case, the 'KeyType' returned
        // is simply the object itself.
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
UnorderedSetKeyPolicy<VALUE_TYPE>::extractKey(const VALUE_TYPE& object)
{
    return object;
}

}  // close namespace bslalg

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
