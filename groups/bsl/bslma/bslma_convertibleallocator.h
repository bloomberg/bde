// bslma_convertibleallocator.h                                       -*-C++-*-
#ifndef INCLUDED_BSLMA_CONVERTIBLEALLOCATOR
#define INCLUDED_BSLMA_CONVERTIBLEALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator type agnostic to allocator-awareness model.
//
//@CLASSES:
//  bslma::ConvertibleAllocator: convertible allocator type
//
//@SEE_ALSO: bslma_aamodel, bslma_aatypeutil
//
//@DESCRIPTION: This component provides a non-template class,
// 'bslma::ConvertibleAllocator', that allows a user to call a function that
// takes an allocator in a way that is agnostic to whether the allocator
// parameter is 'bslma::Allocator *' or an instantiation of
// 'bsl::allocator<TYPE>'.  'bslma::ConvertibleAllocator' can be constructed
// with either 'bsl::allocator' or a 'bslma::Allocator *', and provides
// conversions to both 'bsl::allocator' and 'bslma::Allocator *'.  Although
// especially useful for allowing code to construct objects generically,
// irrespective of the allocator model used by those objects, non-generic code
// can also benefit from using 'ConvertibleAllocator' as an intermediate type
// when constructing Allocator-Aware (AA) objects; such code would be
// agnostic to changes in the underlying type's AA model.
//
// An object of 'ConvertibleAllocator' type is almost always converted to a
// 'bsl::allocator' or 'bslma::Allocator *' object, rather than being used
// directly to allocate memory.  Nevertheless, it can directly be used as an
// STL-style allocator and is, thus, suitable for use in type-erased contexts
// where conversion would not occur, e.g., when supplying an allocator to a
// 'bsl::shared_ptr'.
//
///Usage
///-----
//
///Example 1: A Constructor Supporting Both *bsl-AA* and *legacy-AA* Models
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how 'ConvertibleAllocator' can be used to construct
// an Allocator-Aware (AA) object without regard to whether it is *legacy-AA*
// (using 'bslma::Allocator *') or *bsl-AA* (using 'bsl::allocator').  Consider
// a *bsl-AA* class, 'Wrapper', that wraps an object of template-parameter
// type, 'TYPE'.  First, we define the data members:
//..
//  template <class TYPE>
//  class Wrapper {
//      // Wrap an object of type 'TYPE'.
//
//      // DATA
//      bsl::allocator<char> d_allocator;
//      TYPE                 d_object;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//..
// Next, we define the constructors.  The constructors for 'Wrapper' would all
// take an optional 'allocator_type' argument, but the 'd_object' member might
// require a 'bslma::Allocator *' argument.  By converting the allocator to
// 'ConvertibleAllocator', we can be agnostic to that issue:
//..
//      // CREATORS
//      explicit Wrapper(const allocator_type& alloc = allocator_type())
//          : d_allocator(alloc)
//          , d_object(bslma::ConvertibleAllocator(alloc))
//          { }
//
//      Wrapper(const Wrapper&, const allocator_type& = allocator_type());
//
//..
// Next, we Finish up our class by creating accessors to get the allocator and
// wrapped object:
//..
//      // ACCESSORS
//      const allocator_type get_allocator() const { return d_allocator; }
//          // Return the allocator used to construct this object.
//
//      const TYPE& value() const { return d_object; }
//  };
//..
// Now, to see the effect of the constructor, we'll use a pair of simple AA
// classes, 'SampleBslAAType' and 'SampleLegcyAAType' that do nothing more than
// hold an allocator:
//..
//  class SampleBslAAType {
//      // Sample AA class that adheres to the bsl-AA interface.
//
//      // DATA
//      bsl::allocator<char> d_allocator;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//      // CREATORS
//      explicit SampleBslAAType(const allocator_type& alloc =allocator_type())
//          : d_allocator(alloc) { }
//
//      // ...
//
//      // ACCESSORS
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//
//  class SampleLegacyAAType {
//      // Sample AA class that adheres to the legacy-AA interface.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(SampleLegacyAAType,
//                                     bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit SampleLegacyAAType(bslma::Allocator *basicAllocator = 0)
//          : d_allocator_p(bslma::Default::allocator(basicAllocator)) { }
//
//      // ...
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//  };
//..
// Finally, in our main program, create an allocator and pass it to a couple of
// 'Wrapper' objects, one instantiated with 'SampleBslAAType' and the other
// instantiated with our 'SampleLegacyAAType'.  We verify that both were
// constructed appropriately, with the supplied allocator being used by the
// wrapped object:
//..
//  void main()
//  {
//      bslma::TestAllocator ta;
//      bsl::allocator<char> alloc(&ta);
//
//      Wrapper<SampleBslAAType> w1(alloc);
//      assert(alloc == w1.get_allocator());
//      assert(alloc == w1.value().get_allocator());
//
//      Wrapper<SampleLegacyAAType> w2(alloc);
//      assert(alloc == w2.get_allocator());
//      assert(&ta   == w2.value().allocator());
//  }
//..
// Note that a more complete example would use metaprogramming to choose the
// constructor arguments for the wrapped object so that non-AA types could be
// supported as well as types for which the allocator argument in preceded by
// an argument of type 'bsl::allocator_arg_t' (see 'bslma::ConstructionUtil').

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>

#include <bsls_keyword.h>


namespace BloombergLP {
namespace bslma {

                        // ==========================
                        // class ConvertibleAllocator
                        // ==========================

class ConvertibleAllocator : public bsl::allocator<char> {
    // Allocator class convertible to both 'bsl::allocator<TYPE>' and
    // 'bslma::Allocator *'.  This type meets the requirements for an STL
    // allocator in its own right, and thus is suitable for use in situations
    // such as type erasure, that demand an allocator but do not provide a
    // target to convert to.

    // PRIVATE TYPES
    typedef bsl::allocator<char> Base;

    // PRIVATE MANIPULATORS
    ConvertibleAllocator& operator=(const ConvertibleAllocator&)
                                                          BSLS_KEYWORD_DELETED;
        // DECLARED BUT NOT DEFINED

  public:
    // CREATORS
    ConvertibleAllocator(Allocator *basicAllocator);                // IMPLICIT
        // Create a 'ConvertibleAllocator' that allocates from the specified
        // 'basicAllocator' mechanism.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    template <class TP>
    ConvertibleAllocator(const bsl::allocator<TP>& other);          // IMPLICIT
        // Create a 'ConvertibleAllocator' that allocates from the specified
        // 'other' allocator.

    template <class TP>
    ConvertibleAllocator(bslmf::MovableRef<bsl::allocator<TP> > other);
                                                                    // IMPLICIT
        // Create a 'ConvertibleAllocator' that allocates from the specified
        // 'other' allocator.

    //! ConvertibleAllocator(const ConvertibleAllocator&) = default;
    //! ~ConvertibleAllocator() = default;

    // ACCESSORS
    operator bslma::Allocator *() const;
        // Return the mechanism used to allocate memory.
};

// FREE FUNCTIONS
inline bool
operator==(const ConvertibleAllocator& lhs, const ConvertibleAllocator& rhs);
template <class TYPE>
inline bool
operator==(const ConvertibleAllocator& lhs, const bsl::allocator<TYPE>& rhs);
template <class TYPE>
inline bool
operator==(const bsl::allocator<TYPE>& lhs, const ConvertibleAllocator& rhs);
inline bool operator==(const ConvertibleAllocator& lhs, Allocator *rhs);
inline bool operator==(Allocator *lhs, const ConvertibleAllocator& rhs);
    // Return 'true' if the specified 'lhs' and specified 'rhs' allocators
    // compare equal when both are implicitly converted to
    // 'bsl::allocator<char>'; else 'false'.

inline bool
operator!=(const ConvertibleAllocator& lhs, const ConvertibleAllocator& rhs);
template <class TYPE>
inline bool
operator!=(const ConvertibleAllocator& lhs, const bsl::allocator<TYPE>& rhs);
template <class TYPE>
inline bool
operator!=(const bsl::allocator<TYPE>& lhs, const ConvertibleAllocator& rhs);
inline bool
operator!=(const ConvertibleAllocator& lhs, Allocator *rhs);
inline bool
operator!=(Allocator *lhs, const ConvertibleAllocator& rhs);
    // Return 'true' if the specified 'lhs' and specified 'rhs' allocators
    // compare unequal when both are implicitly converted to
    // 'bsl::allocator<char>'; else 'false'.

}  // close package namespace

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

namespace bslma {

                    // -----------------------------------
                    // struct ConvertibleAllocator_ImpUtil
                    // -----------------------------------

struct ConvertibleAllocator_ImpUtil {
    // This 'struct' provides a namespace for non-friend implementation helpers
    // for 'ConvertibleAllocator'.

    // CLASS METHODS
    static bool isEqual(const bsl::allocator<char>& lhs,
                        const bsl::allocator<char>& rhs);
        // Return 'true' if the specified 'lhs' allocator is equal to the
        // specified 'rhs' allocator; otherwise 'false'.
};

inline
bool ConvertibleAllocator_ImpUtil::isEqual(const bsl::allocator<char>& lhs,
                                           const bsl::allocator<char>& rhs)
{
    return lhs == rhs;
}

                // ----------------------------------------------
                // Trait UsesBslmaAllocator<ConvertibleAllocator>
                // ----------------------------------------------

template <>
struct UsesBslmaAllocator<ConvertibleAllocator> : bsl::false_type {
    // 'ConvertibleAllocator' is not an allocator-aware container, despite
    // being implicitly convertible from 'bslma::Allocator *'.  This
    // specialization of 'BslmaUsesBslmaAllocator' forces the trait to be false
    // for 'ConvertibleAllocator'.
};

                        // ---------------------------
                        // struct ConvertibleAllocator
                        // ---------------------------

// CREATORS
inline
ConvertibleAllocator::ConvertibleAllocator(Allocator *basicAllocator)
    : Base(Default::allocator(basicAllocator))
{
}

template <class TP>
inline
ConvertibleAllocator::ConvertibleAllocator(const bsl::allocator<TP>& other)
    : Base(other)
{
}

template <class TP>
inline
ConvertibleAllocator::ConvertibleAllocator(
                                  bslmf::MovableRef<bsl::allocator<TP> > other)
    : Base(bslmf::MovableRefUtil::access(other))
{
}

// ACCESSORS
inline
ConvertibleAllocator::operator Allocator *() const
{
    return Base::mechanism();
}

}  // close package namespace


// FREE FUNCTIONS
inline bool
bslma::operator==(const ConvertibleAllocator& lhs,
                  const ConvertibleAllocator& rhs)
{
    return ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

template <class TYPE>
inline bool
bslma::operator==(const ConvertibleAllocator& lhs,
                  const bsl::allocator<TYPE>& rhs)
{
    return ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

template <class TYPE>
inline bool
bslma::operator==(const bsl::allocator<TYPE>& lhs,
                  const ConvertibleAllocator& rhs)
{
    return ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

inline bool
bslma::operator==(const ConvertibleAllocator& lhs, Allocator *rhs)
{
    return ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

inline bool
bslma::operator==(Allocator *lhs, const ConvertibleAllocator& rhs)
{
    return ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

inline bool
bslma::operator!=(const ConvertibleAllocator& lhs,
                  const ConvertibleAllocator& rhs)
{
    return ! ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

template <class TYPE>
inline bool
bslma::operator!=(const ConvertibleAllocator& lhs,
                  const bsl::allocator<TYPE>& rhs)
{
    return ! ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

template <class TYPE>
inline bool
bslma::operator!=(const bsl::allocator<TYPE>& lhs,
                  const ConvertibleAllocator& rhs)
{
    return ! ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

inline bool
bslma::operator!=(const ConvertibleAllocator& lhs, Allocator *rhs)
{
    return ! ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

inline bool
bslma::operator!=(Allocator *lhs, const ConvertibleAllocator& rhs)
{
    return ! ConvertibleAllocator_ImpUtil::isEqual(lhs, rhs);
}

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_CONVERTIBLEALLOCATOR)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
