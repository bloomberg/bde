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

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
