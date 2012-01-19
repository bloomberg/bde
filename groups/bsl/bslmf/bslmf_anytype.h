// bslmf_anytype.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ANYTYPE
#define INCLUDED_BSLMF_ANYTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a generic type to which any type can be converted.
//
//@CLASSES:
//  bslmf::AnyType: generic type to which any type can be converted
//  bslmf::TypeRep: meta-function for providing a reference to 'TYPE'
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: 'bslmf::AnyType' is a type to which any type can be implicitly
// converted.  This is useful for creating an overloaded function that is a
// catch-all for all types not explicitly provided for in other overloaded
// functions with the same name.  'bslmf::TypeRep' allows one to create a
// reference to a type.  In complex template programming, one is often dealing
// with unknown types, about the constructors of which one knows nothing.  One
// often needs an object of the given type, but since nothing is known about
// the constructors, one can't just construct and object of the type.
// 'bslmf::TypeRep' allows one to create a reference to the type.  Note that
// the 'rep' function in 'bslma::TypeRep' is not implemented, it must never be
// called at run time.
//
///Usage Example 1: 'bslmf::AnyType'
///-------------------------------
//..
//  struct X { };
//  struct Y { };
//  struct Z : public Y { };
//
//  inline bool isY(const bslmf::AnyType&) { return false; }
//  inline bool isY(const Y&)              { return true;  }
//
//  assert(! isY(X()));
//  assert(  isY(Y()));
//  assert(  isY(Z()));
//  assert(! isY(int()));
//  assert(! isY(4));
//  assert(! isY(4.0));
//  assert(! isY("The king is a fink!"));
//
//  X x;
//  Y y;
//  Z z;
//  assert(! isY(x));
//  assert(  isY(y));
//  assert(  isY(z));
//..
///Usage Example 2: 'bslmf::TypeRep'
///--------------------------------
//..
//  struct X {};
//  struct Y {};
//
//  struct HasHorridCtorX : public X {
//      HasHorridCtorX(int, double, X, char, char *, void *) {}
//          // It's inconvenient to actually create an object of this type
//          // because the constructor takes so many arguments.  It's also
//          // impossible because the c'tor is undefined.
//  };
//  struct HasHorridCtorY : public Y {
//      HasHorridCtorY(int, double, X, char, char *, void *) {}
//          // It's inconvenient to actually create an object of this type
//          // because the constructor takes so many arguments.  It's also
//          // impossible because the c'tor is undefined.
//  };
//
//  template <int i>
//  struct MetaInt { char d_array[i + 1]; };
//
//  #define METAINT_TO_UINT(metaint)   (sizeof(metaint) - 1)
//
//  MetaInt<1> isX(const X&);
//  MetaInt<0> isX(const bslmf::AnyType&);
//
//  assert(1 == METAINT_TO_UINT(isX(X())));
//  assert(0 == METAINT_TO_UINT(isX(Y())));
//  assert(1 == METAINT_TO_UINT(isX(bslmf::TypeRep<HasHorridCtorX>::rep())));
//  assert(0 == METAINT_TO_UINT(isX(bslmf::TypeRep<HasHorridCtorY>::rep())));
//  assert(0 == METAINT_TO_UINT(isX(3)));
//  assert(0 == METAINT_TO_UINT(isX(3.0)));
//  assert(0 == METAINT_TO_UINT(isX("The king is a fink!")));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // =============
                        // class AnyType
                        // =============

struct AnyType {
    // Any type can be converted into this type.

    template <typename TYPE> AnyType(const TYPE&) { }
        // This constructor will match any rvalue or any non-volatile lvalue.
        // A non-const version of this constructor is not necessary and will
        // cause some compilers to complain of ambiguities.

    template <typename TYPE> AnyType(const volatile TYPE&) { }
        // This constructor will match any volatile lvalue.  According to the
        // standard, it should NOT match an rvalue.  A non-const version of
        // this constructor is not necessary and will cause some compilers to
        // complain of ambiguities.
};

                        // =============
                        // class TypeRep
                        // =============

template <typename TYPE>
struct TypeRep {
    // Generate a reference to 'TYPE' for use in meta-functions.

    static TYPE& rep();
        // Provide a reference to a 'TYPE' object.  This function has no body
        // and must never be called at run time.  Thus, it does not matter if
        // 'TYPE' has a default constructor or not.
};

template <typename TYPE>
struct TypeRep<TYPE&> {
    // Generate a reference to 'TYPE' for use in meta-functions.  This is a
    // partial specialization of 'TypeRep' instantiated with a reference.

    static TYPE& rep();
        // Provide a reference to a 'TYPE' object.  This function has no body
        // and must never be called at run time.  Thus, it does not matter if
        // 'TYPE' has a default constructor or not.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
