// bslmf_detectnestedtrait.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#define INCLUDED_BSLMF_DETECTNESTEDTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide meta-function to test for nested trait.
//
//@CLASSES:
//   bslmf::DetectNestedTrait: meta-function
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:
//
///Usage
///-----

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // =======================
                        // class DetectNestedTrait
                        // =======================

template <class TYPE, template <class T> class TRAIT>
class DetectNestedTrait_Imp {
    // Implementation of class to detect whether the specified 'TRAIT'
    // parameter is associated with the specified 'TYPE' parameter using the
    // nested type trait mechanism.  The 'VALUE' constant will be true iff
    // 'TYPE' is convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'.

  private:
    static char check(NestedTraitDeclaration<TYPE, TRAIT>, int);
        // Declared but not defined.  This overload is selected if called with
        // a type convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

    static int check(MatchAnyType, ...);
        // Declared but not defined.  This overload is selected if called with
        // a type not convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

    // Not constructible
    DetectNestedTrait_Imp();
    DetectNestedTrait_Imp(const DetectNestedTrait_Imp&);
    ~DetectNestedTrait_Imp();

  public:
    // PUBLIC CONSTANTS

    enum { VALUE = (1 == sizeof(check(TypeRep<TYPE>::rep(), 0))) };
        // Non-zero if 'TRAIT' is associated with 'TYPE' using the nested type
        // trait mechanism; otherwise zero.

    typedef bsl::integral_constant<bool, VALUE> Type;
        // Type representing the result of this metafunction.  Equivalent to
        // 'true_type' if 'TRAIT' is associated with 'TYPE' using the nested
        // type trait mechanism; otherwise 'false_type'.
};

template <class TYPE, template <class T> class TRAIT>
struct DetectNestedTrait : DetectNestedTrait_Imp<TYPE, TRAIT>::Type {
    // Metafunction to detect whether the specified 'TRAIT' parameter is
    // associated with the specified 'TYPE' parameter using the nested type
    // trait mechanism.  Inherits from 'true_type' iff 'TYPE' is convertible
    // to 'NestedTraitDeclaration<TYPE, TRAIT>' and from 'false_type'
    // otherwise.
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_DETECTNESTEDTRAIT)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
