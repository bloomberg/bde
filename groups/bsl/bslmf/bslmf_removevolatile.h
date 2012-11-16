// bslmf_removevolatile.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#define INCLUDED_BSLMF_REMOVEVOLATILE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'volatile'-qualifier
//
//@CLASSES:
//  bsl::remove_volatile: meta-function for removing 'volatile'-qualifier
//
//@SEE_ALSO: bslmf_addvolatile
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_volatile',
// that may be used to remove any top-level 'volatile'-qualifier from a type.
//
// 'bsl::remove_volatile' meets the requirements of the 'remove_volatile'
// template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the 'volatile'-qualifier of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any 'volatile'-qualifier from a particular
// type.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
//  typedef int          MyType;
//  typedef volatile int MyVolatileType;
//..
// Now, we remove the 'volatile'-qualifier from 'MyVolatileType' using
// 'bsl::remove_volatile' and verify that the resulting type is the same as
// 'MyType':
//..
//  assert(true == (bsl::is_same<bsl::remove_volatile<MyVolatileType>::type,
//                                                            MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

                         // ======================
                         // struct remove_volatile
                         // ======================

template <typename TYPE>
struct remove_volatile {
    // This 'struct' template implements the 'remove_volatile' meta-function
    // defined in the C++11 standard [meta.trans.cv], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 'TYPE' except that any top-level
    // 'volatile'-qualifier has been removed.  Note that this generic default
    // template provides a 'type' that is an alias to 'TYPE' for when 'TYPE' is
    // not 'volatile'-qualified.  A template specialization is provided (below)
    // that removes the 'volatile'-qualifier for when 'TYPE' is
    // 'volatile'-qualified.

    typedef TYPE type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

template <typename TYPE>
struct remove_volatile<TYPE volatile> {
     // This partial specialization of 'bsl::remove_volatile', for when the
     // (template parameter) 'TYPE' is 'volatile'-qualified, provides a
     // 'typedef' 'type' that has the 'volatile'-qualifier removed.

    typedef TYPE type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with the 'volatile'-qualifier removed.
};

}  // close namespace bsl

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
