// bslmf_nestedtraitdeclaration.h                                     -*-C++-*-
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

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

#ifdef __CDT_PARSER__
// Work around an Eclise CDT bug where it fails to parse the conditional trait
// declaration.  See internal DRQS 47839133.
#define BSLMF_NESTED_TRAIT_DECLARATION_IF(TYPE, TRAIT, COND)
#else
#define BSLMF_NESTED_TRAIT_DECLARATION_IF(TYPE, TRAIT, COND)                 \
    operator BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT,         \
                                                        COND >() const       \
    {                                                                        \
        return                                                               \
            BloombergLP::bslmf::NestedTraitDeclaration<TYPE, TRAIT, COND >();\
    }
#endif

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_NESTEDTRAITDECLARATION)

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
