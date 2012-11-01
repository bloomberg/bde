// bslmf_nestedtraitdeclaration.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#define INCLUDED_BSLMF_NESTEDTRAITDECLARATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a way within a class to associate the class with traits
//
//@CLASSES:
//  NestedTraitDeclaration: A marker used to detect a nested trait declaration.
//
//@MACROS:
//  BSLMF_NESTED_TRAIT_DECLARATION: Associates a trait with a class
//  BSLMF_NESTED_TRAIT_DECLARATION_IF: Conditionally associates a trait
//
//@SEE_ALSO: bslmf_detectnestedtrait
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----

namespace BloombergLP {

namespace bslmf {

                        // ============================
                        // class NestedTraitDeclaration
                        // ============================

template <class TYPE, template <class T> class TRAIT, bool COND = true>
class NestedTraitDeclaration {
    // Class 'TYPE' will be convertible to
    // 'NestedTraitDeclaration<TYPE,TRAIT,true>' if 'TRAIT' is associated with
    // 'TYPE' using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  Nested trait
    // detection depends on 'COND' being true.  If 'COND' is false, the nested
    // trait detection will not see the conversion it is looking for and will
    // not associate 'TRAIT' with 'TYPE'.  This feature is used by
    // 'BSLMF_NESTED_TRAIT_DECLARATION_IF' to turn a trait on or off depending
    // on a compile-time condition (usually another trait).

  public:
    // PUBLIC TYPES
    typedef NestedTraitDeclaration Type;

    // CREATORS
    //! NestedTraitDeclaration();
    //! NestedTraitDeclaration(const  NestedTraitDeclaration&);
    //! NestedTraitDeclaration& operator=(const  NestedTraitDeclaration&);
    //! ~NestedTraitDeclaration();
};

                        // ====================================
                        // macro BSLMF_NESTED_TRAIT_DECLARATION
                        // ====================================

#define BSLMF_NESTED_TRAIT_DECLARATION(TYPE, TRAIT)                          \
    operator BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT>() const \
    {                                                                        \
        return BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT>();    \
    }                                                                        \

#define BSLMF_NESTED_TRAIT_DECLARATION_IF(TYPE, TRAIT, COND)                 \
    operator BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT,         \
                                                        COND >() const       \
    {                                                                        \
        return                                                               \
            BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT, COND >();\
    }                                                                        \

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_NESTEDTRAITDECLARATION)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
