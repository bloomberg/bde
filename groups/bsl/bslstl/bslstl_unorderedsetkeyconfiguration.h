// bslstl_unorderedsetkeyconfiguration.h                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION
#define INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a configuration class to use a whole object as its own key.
//
//@CLASSES:
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides an identity transformation.
// 'bslalg::HashTableImpUtil' has a static 'extractKey' function template
// that, given a 'value type', will represent objects stored in a data
// structure, will abstract out the 'key type' portion of that object.  In the
// case of the 'unordered_set' data structure, the 'key type' and the
// 'value type' are one and the same, so the 'extractKey' transformation is a
// trivial identity transformation.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslstl {

                       // ===================================
                       // struct UnorderedSetKeyConfiguration
                       // ===================================

template <class VALUE_TYPE>
struct UnorderedSetKeyConfiguration {
  public:
    typedef VALUE_TYPE ValueType;
    typedef ValueType  KeyType;

    // Choosing to implement for each configuration, to reduce the template
    // mess.  With only two policies, not much is saved using a shared
    // dependent base class to provide a common implementation.

    // CLASS METHODS
    static const KeyType& extractKey(const VALUE_TYPE& object);
        // Given a specified 'object', return a reference to the 'KeyType'
        // contained within that object.  In this case, the 'KeyType' returned
        // is simply the object itself.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      //------------------------------------
                      // struct UnorderedSetKeyConfiguration
                      //------------------------------------


template <class VALUE_TYPE>
inline
const typename UnorderedSetKeyConfiguration<VALUE_TYPE>::KeyType&
UnorderedSetKeyConfiguration<VALUE_TYPE>::extractKey(const VALUE_TYPE& object)
{
    return object;
}

}  // close namespace bslalg

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
