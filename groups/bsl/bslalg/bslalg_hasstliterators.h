// bslalg_hasstliterators.h                                           -*-C++-*-
#ifndef INCLUDED_BSLALG_HASSTLITERATORS
#define INCLUDED_BSLALG_HASSTLITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a tag type used to detect STL-like iterators traits.
//
//@CLASSES:
//  bslalg::HasStlIterators: tag type to detect STL-like iterators traits
//
//@SEE_ALSO: bslmf_detecttestedtraits, bslalg_typetraithasstliterators
//
//@DESCRIPTION: This component defines a tag type 'HasStlIterators' derived
// from 'bslmf::DetectNestedTrait' type.  The type defines a metafunction that
// detects if a class has STL-like iterators, and provides a type
// 'HasStilIterators<TYPE>::value' which aliases 'true_type' if 'TYPE' has
// STL-like iterators, and 'false_type' otherwise.
//
// A 'TYPE' that has this trait fulfills the following requirements, where 'mX'
// is a modifiable object and 'X' a non-modifiable object of 'TYPE':
//..
//  Valid expression     Type              Note
//  ----------------     ----              ----
//  TYPE::iterator                         Iterator type (has 'operator->',
//                                         'operator*', and possibly more
//                                         depending on the iterator
//                                         category).
//
//  TYPE::const_iterator                   Iterator type (has 'operator->',
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

namespace BloombergLP {

namespace bslalg {

                        //=======================
                        // struct HasStlIterators
                        //=======================

template <class TYPE>
struct HasStlIterators : bslmf::DetectNestedTrait<TYPE, HasStlIterators>
    // This class detects if the specified class 'TYPE' has STL-like iterators.
    // If the class 'TYPE' provides STL-like iterators then
    // 'HasStlIterators<TYPE>::value == true', and
    // 'HasStlIterators<TYPE>::value == false' otherwise.
{
};

}  // close package namespace

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
