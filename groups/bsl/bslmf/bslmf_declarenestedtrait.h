// bslmf_declarenestedtrait.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_DECLARENESTEDTRAIT
#define INCLUDED_BSLMF_DECLARENESTEDTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:
//
//@CLASSES:
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----

namespace BloombergLP {

namespace bslmf {

                        // ===================
                        // class NestedTrait
                        // ===================

template <class TYPE, template <class T> class TRAIT>
class NestedTrait {
    // Class 'TYPE' will be convertible to 'NestedTrait<TYPE, TRAIT>' if
    // 'TRAIT' is associated with 'TYPE' using the
    // 'BSLMF_DECLARE_NESTED_TRAIT' macro.

  public:
    // PUBLIC TYPES
    typedef NestedTrait Type;

    // CREATORS
    //! NestedTrait();
    //! NestedTrait(const  NestedTrait&);
    //! NestedTrait& operator=(const  NestedTrait&);
    //! ~NestedTrait();
};

#define BSLMF_DECLARE_NESTED_TRAIT(TYPE, TRAIT)                         \
    operator BloombergLP::bslmf::NestedTrait<TYPE, TRAIT>() const {     \
        return BloombergLP::bslmf::NestedTrait<TYPE, TRAIT>();          \
    }

// FREE OPERATORS

}  // close package namespace

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS

// CREATORS

// MANIPULATORS

// ACCESSORS

// FREE OPERATORS

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLMF_DECLARENESTEDTRAIT)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
