// bslalg_typetraithasstliterators.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having STL-like iterators.
//
//@CLASSES:
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasStlIterators'.  A 'TYPE' that has this trait fulfills
// the following requirements, where 'mX' is a modifiable object and 'X' a
// non-modifiable object of 'TYPE':
//..
//  Valid expression     Type              Note
//  ----------------     ----              ----
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).
//
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).  The value type of this
//                                         iterator is not modifiable.
//
//  mX.begin()           iterator          Similar to standard containers
//  mX.end()             iterator
//  X.begin()            const_iterator
//  X.end()              const_iterator
//  X.cbegin()           const_iterator
//  X.cend()             const_iterator
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace BloombergLP {

namespace bslalg {

template <class TYPE>
struct HasStlIterators : bslmf::DetectNestedTrait<TYPE, HasStlIterators>
{
};

                        //================================
                        // struct TypeTraitHasStlIterators
                        //================================

struct TypeTraitHasStlIterators {
    // A type with this trait defines (at minimum) the nested types 'iterator'
    // and 'const_iterator' and the functions 'begin()' and 'end()' having the
    // standard STL semantics.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, HasStlIterators>
    {
        // This class template ties the 'bslalg::TypeTraitHasStlIterators'
        // trait tag to the 'bslmf::HasStlIterators' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : HasStlIterators<TYPE>::type { };
};

}  // close package namespace


}  // close enterprise namespace

#endif

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
