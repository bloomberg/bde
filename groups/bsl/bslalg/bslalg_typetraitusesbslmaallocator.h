// bslalg_typetraitusesbslmaallocator.h                               -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#define INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive trait for types that use 'bslma' allocators.
//
//@CLASSES:
//  bslalg::TypeTraitUsesBslmaAllocator: uses 'bslma' allocators
//
//@SEE_ALSO: bslmf_typetraits, bslalg_constructorproxy
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitUsesBslmaAllocator'.  A type 'T' with this trait uses an
// allocator derived from 'bslma::Allocator' to allocate memory.  Such a type
// *must* have a "copy" constructor with prototype
// 'T(const T&, bslma::Allocator*)'.  It usually also has a "default"
// constructor with prototype 'T(bslma::Allocator*)', and it may also have
// additional constructors with various numbers of arguments, that take an
// optional 'bslma::Allocator*' last argument.
//
// This component is used by virtually all 'bslalg' components for providing
// primitives that ensure that a 'bslma' allocator is always passed through
// properly to a class creator if that class has the 'bslma' allocator trait.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bslalg {

                 //==========================================
                 // struct TypeTraitUsesBslmaAllocator
                 //==========================================

struct TypeTraitUsesBslmaAllocator {
    // A type 'T' with this trait uses an allocator derived from
    // 'bslma::Allocator' to allocate memory.  Such a type *must* have a "copy"
    // constructor with prototype 'T(const T&, bslma::Allocator*)'.  It usually
    // also has a "default" constructor with prototype 'T(bslma::Allocator*)',
    // and it may also have additional constructors with various numbers of
    // arguments, that take an optional 'bslma::Allocator*' last argument.  The
    // allocator argument is usually defaulted, so that these two constructors
    // operate as true copy and default constructors.  The default constructor
    // is usually declared 'explicit' to avoid implicit conversion from
    // 'bslma::Allocator*'.  Generic containers use this trait to determine if
    // their contained elements use allocators.  Types that use
    // 'bslma::Allocator' should always have this trait defined.  (Note that
    // container classes instantiated with 'std::allocator' should also be
    // assigned this trait, since 'std::allocator' is built on
    // 'bslma::Allocator'.  )

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslma::UsesBslmaAllocator>
    {
        // This class template ties the 'bslalg::TypeTraitBitwiseMoveable'
        // trait tag to the 'bslma::UsesBslmaAllocator' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslma::UsesBslmaAllocator<TYPE> { };
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
