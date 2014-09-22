// bslmf_addcv.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCV
#define INCLUDED_BSLMF_ADDCV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding top-level cv-qualifiers.
//
//@CLASSES:
//  bsl::add_cv: meta-function for adding top-level cv-qualifiers
//
//@SEE_ALSO: bslmf_removecv
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_cv', that may
// be used to add a top-level 'const'-qualifier and a top-level
// 'volatile'-qualifier to a type if it is not a reference type, nor a function
// type, nor already 'const'-qualified and 'volatile'-qualified at the
// top-level.
//
// 'bsl::add_cv' meets the requirements of the 'add_cv' template defined in the
// C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'const'-Qualifier and a 'volatile'-Qualifier to a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier and a 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we add a 'const'-qualifier and a 'volatile'-qualifier to 'MyType' using
// 'bsl::add_cv' and verify that the resulting type is the same as 'MyCvType':
//..
//  assert(true == (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDCONST
#include <bslmf_addconst.h>
#endif

#ifndef INCLUDED_BSLMF_ADDVOLATILE
#include <bslmf_addvolatile.h>
#endif

namespace bsl {

                         // =============
                         // struct add_cv
                         // =============

template <class TYPE>
struct add_cv {
    // This 'struct' template implements the 'add_cv' meta-function defined in
    // the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  If the (template parameter) 'TYPE' is not a
    // reference type, nor a function type, nor already 'const'-qualified and
    // 'volatile'-qualified at the top-level, then 'type' is an alias to 'TYPE'
    // with a top-level 'const'-qualifier and a 'volatile'-qualifier added;
    // otherwise, 'type' is an alias to 'TYPE'.

    // PUBLIC TYPES
    typedef typename add_const<typename add_volatile<TYPE>::type>::type type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE' with a
        // top-level 'const'-qualifier and 'volatile'-qualifier added if 'TYPE'
        // is not a reference type, nor a function type, nor already
        // 'const'-qualified and 'volatile'-qualified at the top-level;
        // otherwise, 'type' is an alias to 'TYPE'.
};

}  // close namespace bsl

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
