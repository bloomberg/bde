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
//  bslma::ManagedPtr_PairProxy: internal state of a bcema_ManagedPtr object
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class that can be used to create a
// 'bsl::pair' with the same arguments.  It is provided in order to support a
// (deprecated) legacy API in 'ManagedPtr' as part of the transition from the
// 'bdema' package to 'bslma'.  After the transition, the component is lower
// in the package group levelization than 'bsl::pair', so we instead return a
// type that will be implicitly convertible to 'pair'.  Note that this implies
// adding an implicit constructor to 'bsl::pair'.

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
    // 'bsl::pair' object.  It will be a POD is the (template parameter)
    // 'ASSOCIATED_TYPE' is a POD.
    // Note that the names of the data members must exactly match those of
    // 'bsl::pair' to support users simply acting on the results of the
    // function call and not assigning that result to a 'bsl::pair' first.

    POINTER_TYPE    *first;
    ASSOCIATED_TYPE  second;
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
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
    : IsBitwiseMoveable<ASSOCIATED_TYPE>::type {};

}  // close package namespace
}  // close enterprise namespace

#endif


// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
