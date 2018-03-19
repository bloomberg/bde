// bdlb_transformiterator.h                                           -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSFORMITERATOR
#define INCLUDED_BDLB_TRANSFORMITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: To be the best transform iterator possible, given the constraints.
//
//@CLASSES:
// bdlb::TransformIterator: Be a darn good transform iterator.
//
//@DESCRIPTION: 'bdlb::TransformIterator' iterates and transforms.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
/// - - - - -
//
//..
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

namespace BloombergLP {
namespace bdlb {

// Allow 'TransformIterator' to be returned by value
//  BDE_VERIFY pragma: push
//  BDE_VERIFY pragma: -AR01

template <class FUNCTOR, class ITERATOR>
class TransformIterator {
    // PRIVATE TYPES
    typedef bsl::iterator_traits<ITERATOR> it;

  public:
    // PUBLIC TYPES
    typedef typename it::difference_type   difference_type;
    typedef typename it::value_type        value_type;
    typedef typename it::pointer           pointer;
    typedef typename it::reference         reference;
    typedef typename it::iterator_category iterator_category;

  private:
    // PRIVATE DATA
    bslalg::ConstructorProxy<ITERATOR>  d_iteratorProxy;
    bslalg::ConstructorProxy<FUNCTOR>   d_functorProxy;
    bslma::Allocator                   *d_allocator_p;

  public:
    // PUBLIC CREATORS
    TransformIterator(const ITERATOR&   iterator,
                      FUNCTOR           functor,
                      bslma::Allocator *basicAllocator = 0);
        // Create a 'TransformIterator' using the specified 'iterator' and
        // 'functor'.  Optionally specify 'basicAllocator'.

    TransformIterator(const TransformIterator&  other,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a copy of the specified 'TransformIterator' object 'other'.
        // Optionally specify 'basicAllocator'.

    // PUBLIC MANIPULATORS
    TransformIterator& operator=(const TransformIterator& other);
        // Assign the specified 'other' object to this one.

    TransformIterator& operator++();
        // Increment the iterator of this object.

    TransformIterator operator++(int);
        // Create a copy of this object, increment the iterator of this object,
        // then return the copy.

    TransformIterator& operator--();
        // Decrement the iterator of this object.

    TransformIterator operator--(int);
        // Create a copy of this object, decrement the iterator of this object,
        // then return the copy.

    value_type operator*();
        // Return the result of applying the functor to the dereferenced
        // iterator.

    FUNCTOR& functor();
        // Return the functor of this object.

    ITERATOR& iterator();
        // Return the iterator of this object.

    // PUBLIC ACCESSORS
    value_type operator*() const;
        // Return the result of applying the functor to the dereferenced
        // iterator.

    bslma::Allocator *allocator() const;
        // Return the allocator of this object.

    const FUNCTOR& functor() const;
        // Return the functor of this object.

    const ITERATOR& iterator() const;
        // Return the iterator of this object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TransformIterator,
                                   bslma::UsesBslmaAllocator)
};

                          //------------------------
                          // class TransformIterator
                          //------------------------

// CREATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                              const ITERATOR&   iterator,
                                              FUNCTOR           functor,
                                              bslma::Allocator *basicAllocator)
: d_iteratorProxy(iterator, basicAllocator)
, d_functorProxy(functor, basicAllocator)
, d_allocator_p(basicAllocator)
{
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>::TransformIterator(
                                      const TransformIterator&  other,
                                      bslma::Allocator         *basicAllocator)
: d_iteratorProxy(other.d_iteratorProxy.object(), basicAllocator)
, d_functorProxy(other.d_functorProxy.object(), basicAllocator)
{
}

// MANIPULATORS
template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator=(const TransformIterator& other)
{
    d_iteratorProxy.object() = other.d_iteratorProxy.object();
    d_functorProxy.object()  = other.d_functorProxy.object();

    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator++()
{
    ++d_iteratorProxy.object();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator++(int)
{
    return TransformIterator(d_iteratorProxy.object()++,
                             d_functorProxy.object());
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>&
TransformIterator<FUNCTOR, ITERATOR>::operator--()
{
    --d_iteratorProxy.object();
    return *this;
}

template <class FUNCTOR, class ITERATOR>
inline
TransformIterator<FUNCTOR, ITERATOR>
TransformIterator<FUNCTOR, ITERATOR>::operator--(int)
{
    return TransformIterator(d_iteratorProxy.object()--,
                             d_functorProxy.object());
}

// ACCESSORS
template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::value_type
TransformIterator<FUNCTOR, ITERATOR>::operator*()
{
    return d_functorProxy.object()(*d_iteratorProxy.object());
}

template <class FUNCTOR, class ITERATOR>
inline
typename TransformIterator<FUNCTOR, ITERATOR>::value_type
TransformIterator<FUNCTOR, ITERATOR>::operator*() const
{
    return d_functorProxy.object()(*d_iteratorProxy.object());
}

template <class FUNCTOR, class ITERATOR>
inline
bslma::Allocator *TransformIterator<FUNCTOR, ITERATOR>::allocator() const
{
    return d_allocator_p;
}

template <class FUNCTOR, class ITERATOR>
inline
FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor()
{
    return d_functorProxy.object();
}

template <class FUNCTOR, class ITERATOR>
inline
const FUNCTOR& TransformIterator<FUNCTOR, ITERATOR>::functor() const
{
    return d_functorProxy.object();
}

template <class FUNCTOR, class ITERATOR>
inline
ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator()
{
    return d_iteratorProxy.object();
}

template <class FUNCTOR, class ITERATOR>
inline
const ITERATOR& TransformIterator<FUNCTOR, ITERATOR>::iterator() const
{
    return d_iteratorProxy.object();
}

// FREE FUNCTIONS
template <class FUNCTOR, class ITERATOR>
inline
bool operator==(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs)
    // Return 'true' iff the specified 'lhs' and 'rhs' have equal iterators and
    // functors.
{
    return lhs.iterator() == rhs.iterator() && lhs.functor() == rhs.functor();
}

template <class FUNCTOR, class ITERATOR>
inline
bool operator!=(const TransformIterator<FUNCTOR, ITERATOR>& lhs,
                const TransformIterator<FUNCTOR, ITERATOR>& rhs)
    // Return 'true' iff the specified 'lhs' and 'rhs' have unequal iterators
    // or functors.
{
    return lhs.iterator() != rhs.iterator() || lhs.functor() != rhs.functor();
}

    //  BDE_VERIFY pragma: pop
}  // close package namespace
}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
