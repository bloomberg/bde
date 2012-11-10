// bslalg_typetraithasstliterators.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with STL-like iterators.
//
//@CLASSES:
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//
//@SEE_ALSO: bslmf_typetraits
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

template <typename TYPE>
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
