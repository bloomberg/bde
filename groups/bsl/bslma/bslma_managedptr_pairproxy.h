// bslma_managedptr_pairproxy.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR_PAIRPROXY
#define INCLUDED_BSLMA_MANAGEDPTR_PAIRPROXY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the internal state of a managed pointer class.
//
//@CLASSES:
//  bslma::ManagedPtr_PairProxy: internal state of a 'bslma::ManagedPtr' object
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class, 'bslma::ManagedPtr_PairProxy',
// that can be used to create a 'bsl::pair' with the same arguments.  It is
// provided in order to support a (deprecated) legacy API in 'ManagedPtr' as
// part of the transition from the 'bdema' package to 'bslma'.  With the
// transition, the 'bslma' managed pointer component is lower in the package
// group levelization than 'bsl::pair', so we instead return a type that is
// implicitly convertible to 'pair'.  Note that this implies that an implicit
// constructor has been added to 'bsl::pair'.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslma {

                     // ==========================
                     // class ManagedPtr_PairProxy
                     // ==========================

template <class POINTER_TYPE, class ASSOCIATED_TYPE>
struct ManagedPtr_PairProxy {
    // This class is a simple aggregate that may be used to construct a
    // 'bsl::pair' object.  It will be a POD if the (template parameter)
    // 'ASSOCIATED_TYPE' is a POD.  Note that the names of the data members
    // must exactly match those of 'bsl::pair' to support users simply acting
    // on the result of a function call from which a pair proxy is returned,
    // and not assigning that result to a 'bsl::pair' first.

    POINTER_TYPE    *first;
    ASSOCIATED_TYPE  second;
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                    // --------------------------
                    // class ManagedPtr_PairProxy
                    // --------------------------

}  // close package namespace

// TYPE TRAITS
namespace bslmf {

template <class POINTER_TYPE, class ASSOCIATED_TYPE>
struct IsBitwiseMoveable<bslma::ManagedPtr_PairProxy<POINTER_TYPE,
                                                     ASSOCIATED_TYPE> >
    : IsBitwiseMoveable<ASSOCIATED_TYPE>::type
{
};

}  // close namespace bslmf
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
