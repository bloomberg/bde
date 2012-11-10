// bslstl_unorderedmapkeyconfiguration.h                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION
#define INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class template to extract keys as the 'first' attribute.
//
//@CLASSES:
//   bslalg::UnorderedMapKeyConfiguration : extracts 'key' from 'value' type
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@DESCRIPTION: This component will, given an object of a value type consisting
// of a key type and some other information, return a const reference to only
// the key type within that object.  The object passed will be of parametrized
// type 'VALUE_TYPE', for which a type 'VALUE_TYPE::first_type' must be
// defined and be of the key type, and for which the operation '.first' must be
// defined and must yield the object of the key type.
//
// 'bslalg::HashTableImpUtil' has a static 'extractKey' function template that,
// given a 'value type', will represent objects stored in a data structure,
// will abstract out the 'key type' portion of that object.  In the case of the
// 'unordered_map' data structure, the 'value type' will be 'bsl::pair', and
// the key type will 'bsl::pair::first_type'.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bslstl {

                      // ===================================
                      // struct UnorderedMapKeyConfiguration
                      // ===================================

template <class VALUE_TYPE>
struct UnorderedMapKeyConfiguration {
  public:
    typedef          VALUE_TYPE             ValueType;
    typedef typename ValueType::first_type  KeyType;

    // Choosing to implement for each configuration, to reduce the template
    // mess.  With only two policies, not much is saved using a shared
    // dependent base class to provide a common implementation.  This is the
    // key abstraction, turning 'bslalg::BidirectionalLink*' into 'VALUE_TYPE&'

    // CLASS METHODS
    static const KeyType& extractKey(const VALUE_TYPE& obj);
        // Return the member 'first' of the specified object 'obj'.
        // 'obj.first' must of of type 'VALUE_TYPE::first_type', which is the
        // 'key' portion of 'obj'.
};

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                       //-----------------------------------
                       // class UnorderedMapKeyConfiguration
                       //-----------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
inline
const typename UnorderedMapKeyConfiguration<VALUE_TYPE>::KeyType&
UnorderedMapKeyConfiguration<VALUE_TYPE>::extractKey(const VALUE_TYPE& obj)
{
    return obj.first;
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
