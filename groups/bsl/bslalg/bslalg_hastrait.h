// bslalg_hastrait.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASTRAIT
#define INCLUDED_BSLALG_HASTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for checking compile-time trait.
//
//@CLASSES:
//  bslalg::HasTrait: trait detection mechanism
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a meta-function, 'bslalg::HasTrait',
// for macros used to assign traits to user-defined class.  Traits are used
// to enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bslalg::TypeTraitBitwiseMoveable' may
// allow resizing an array of objects by simply calling 'std::memcpy' instead
// of invoking a copy-constructor on every object.  The usage example shows how
// to use the 'bslma::UsesBslmaAllocator' to propagate allocators to
// nested instances that may require them.
//
// This component should be used in conjunction with other components from the
// package 'bslalg'.  See the package-level documentation for an overview.  The
// most useful classes and macros defined in this component are:
//..
//  bslalg::HasTrait<TYPE, TRAIT>             This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//..
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {

namespace bslalg {

                       // ===============
                       // struct HasTrait
                       // ===============

template <typename TYPE, typename TRAIT>
struct HasTrait {
    // This meta-function evaluates to 'bslmf::MetaInt<1>' if the parameterized
    // type 'TYPE' has the parameterized 'TRAIT', and to 'bslmf::MetaInt<0>'
    // otherwise.  Note that 'TYPE' "has" the 'TRAIT' if
    // 'bslalg_TypeTraits<TYPE>' directly includes 'TRAIT' or else includes a
    // trait that implies 'TRAIT'.

  public:
    enum {
        VALUE = TRAIT::template
                       Metafunction<typename bsl::remove_cv<TYPE>::type>::value
    };

    typedef bslmf::MetaInt<VALUE> Type;
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
