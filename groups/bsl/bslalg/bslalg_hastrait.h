// bslalg_hastrait.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASTRAIT
#define INCLUDED_BSLALG_HASTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta function to identify if a type has a given trait.
//
//@CLASSES:
//  bslalg::HasTrait: trait detection mechanism
//
//@SEE_ALSO:
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

template <class TYPE, class TRAIT>
struct HasTrait {
    // This meta-function evaluates to 'bslmf::MetaInt<1>' if the parameterized
    // type 'TYPE' has the parameterized 'TRAIT', and to 'bslmf::MetaInt<0>'
    // otherwise.

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
