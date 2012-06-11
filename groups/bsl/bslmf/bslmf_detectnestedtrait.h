// bslmf_detectnestedtrait.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#define INCLUDED_BSLMF_DETECTNESTEDTRAIT

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

#ifndef INCLUDED_BSLMF_ANYTYPE
#include <bslmf_anytype.h>
#endif

#ifndef INCLUDED_BSLMF_DECLARENESTEDTRAIT
#include <bslmf_declarenestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGERCONSTANT
#include <bslmf_integerconstant.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // ===================
                        // class DetectNestedTrait
                        // ===================

template <class TYPE, template <class T> class TRAIT>
class DetectNestedTrait_Imp {
    // Implementation of class to detect whether the specified 'TRAIT'
    // parameter is associated with the specified 'TYPE' parameter using the
    // nested type trait mechanism.  The 'VALUE' constant will be true iff
    // 'TYPE' is convertible to 'NestedTrait<TYPE, TRAIT>'.

  private:
    static char check(NestedTrait<TYPE, TRAIT>, int);
        // Declared but not defined.  This overload is selected if called with
        // a type convertible to 'NestedTrait<TYPE, TRAIT>'

    static int check(AnyType, ...);
        // Declared but not defined.  This overload is selected if called with
        // a type not convertible to 'NestedTrait<TYPE, TRAIT>'

    // Not constructible
    DetectNestedTrait_Imp();
    DetectNestedTrait_Imp(const DetectNestedTrait_Imp&);
    ~DetectNestedTrait_Imp();
    
  public:
    // PUBLIC CONSTANTS

    enum { VALUE = (1 == sizeof(check(TypeRep<TYPE>::rep(), 0))) };
        // Non-zero if 'TRAIT' is associated with 'TYPE' using the nested type
        // trait mechanism; otherwise zero.
};

template <class TYPE, template <class T> class TRAIT>
struct DetectNestedTrait :
    bslmf::integer_constant<bool, DetectNestedTrait_Imp<TYPE, TRAIT>::VALUE> {
    // Metafunction to detect whether the specified 'TRAIT' parameter is
    // associated with the specified 'TYPE' parameter using the nested type
    // trait mechanism.  Inherits from 'true_type' iff 'TYPE' is convertible
    // to 'NestedTrait<TYPE, TRAIT>' and from 'false_type' otherwise.
};

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

#endif // ! defined(INCLUDED_BSLMF_DETECTNESTEDTRAIT)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
