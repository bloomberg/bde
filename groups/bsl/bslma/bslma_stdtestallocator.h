// bslma_stdtestallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_STDTESTALLOCATOR
#define INCLUDED_BSLMA_STDTESTALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stl-compatible, 'bslma'-style allocator to track usage.
//
//@CLASSES:
//  bslma::StdTestAllocator: instrumented 'bslma-style' stl allocator template
//
//@MACROS:
//
//@SEE_ALSO: TBD
//
//@DESCRIPTION: TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_CONSTRUCTIONUTIL
#include <bslma_constructionutil.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTIONUTIL
#include <bslma_destructionutil.h>
#endif

#ifndef INCLUDED_BSLMA_TESTALLOCATOR
#include <bslma_testallocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {
namespace bslma {

                           // ======================
                           // class StdTestAllocator
                           // ======================

template <class TYPE>
class StdTestAllocator {
    // An STL-compatible test allocator that forwards allocation calls to an
    // underlying mechanism object of a type derived from
    // 'bslma::TestAllocator'.  This class template adheres to the allocator
    // requirements defined in section 20.1.5 [lib.allocator.requirements] of
    // the C++ standard and may be used to instantiate any [container] class
    // template that follows the STL allocator protocol.  The allocation
    // mechanism is chosen at run-time, giving the programmer run-time control
    // over how a container allocates and frees memory.

    // DATA
    TestAllocator *d_mechanism;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StdTestAllocator,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(StdTestAllocator,
                                   bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(StdTestAllocator,
                                   bslmf::IsBitwiseEqualityComparable);
        // Declare nested type traits for this class.

    // PUBLIC TYPES
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef TYPE           *pointer;
    typedef const TYPE     *const_pointer;
    typedef typename bsl::conditional<bsl::is_void<TYPE>::value,
                                      void,
                                      TYPE&>::type reference;
    typedef typename bsl::conditional<bsl::is_void<TYPE>::value,
                                      void,
                                      const TYPE&>::type const_reference;
    // typedef TYPE&           reference;
    // typedef const TYPE&     const_reference;
    typedef TYPE            value_type;

    template <class ANY_TYPE>
    struct rebind {
        // This nested 'struct' template, parameterized by 'ANY_TYPE', provides
        // a namespace for an 'other' type alias, which is a 'StdTestAllocator'
        // type following the same template as this one but that allocates
        // elements of 'ANY_TYPE'.  Note that this 'StdTestAllocator' type is
        // convertible to and from 'other' for any type, including 'void'.

        typedef StdTestAllocator<ANY_TYPE> other;
    };

    // CREATORS
    StdTestAllocator();
        // TBD: fix comment to mention that the default allocator has to be
        // a test allocator.
        // Create a proxy object which will forward allocation calls to the
        // object pointed to by 'bslma::Default::defaultAllocator()'.
        // Postcondition:
        //..
        //  this->mechanism() == bslma::Default::defaultAllocator();
        //..

    StdTestAllocator(Allocator *mechanism);                         // IMPLICIT
        // TBD: fix comment to mention that 'mechanism' has to be a test
        // allocator.
        // Convert a 'bslma::Allocator' pointer to an test allocator object
        // which forwards allocation calls to the object pointed to by the
        // specified 'mechanism'.  If 'mechanism' is 0, then the currently
        // installed default allocator is used instead.  Postcondition:
        // '0 == mechanism || this->mechanism() == mechanism'.

    StdTestAllocator(const StdTestAllocator& original);
        // Create a proxy object using the same mechanism as the specified
        // 'original'.  Postcondition: 'this->mechanism() == rhs.mechanism()'.

    template <class ANY_TYPE>
    StdTestAllocator(const StdTestAllocator<ANY_TYPE>& rhs);
        // Create a proxy object sharing the same mechanism object as the
        // specified 'rhs'.  The newly constructed test allocator will compare
        // equal to 'rhs', even though they are instantiated on different
        // types.  Postcondition: 'this->mechanism() == rhs.mechanism()'.

    //! ~StdTestAllocator();
        // Destroy this object.  Note that this does not delete the object
        // pointed to by 'mechanism()'.  Also note that this method's
        // definition is compiler generated.

    //! StdTestAllocator& operator=(const StdTestAllocator& rhs);
        // Assign to this object the value of the specified 'rhs'.
        // Postcondition: 'this->mechanism() == rhs->mechanism()'.  Note that
        // this does not delete the object pointed to by the previous value of
        // 'mechanism()'.  Also note that this method's definition is compiler
        // generated.

    // MANIPULATORS
    pointer allocate(size_type n, const void *hint = 0);
        // Allocate enough (properly aligned) space for the specified 'n'
        // objects of (template parameter) 'TYPE' by calling 'allocate' on the
        // mechanism object.  The optionally specified 'hint' argument is
        // ignored by this test allocator type.  The behavior is undefined
        // unless 'n <= max_size()'.

    void deallocate(pointer p, size_type n = 1);
        // Return memory previously allocated with 'allocate' to the underlying
        // mechanism object by calling 'deallocate' on the mechanism object
        // with the specified 'p'.  The optionally specified 'n' argument is
        // ignored by this test allocator type.

    template <class ELEMENT_TYPE>
    void construct(ELEMENT_TYPE *address, const TYPE& val);
        // Copy-construct an object of (template parameter) 'TYPE' from the
        // specified 'val' at the memory address specified by 'p'.  Do not
        // directly allocate memory.  The behavior is undefined unless 'p' is
        // not properly aligned for objects of the given 'TYPE'.

    template <class ELEMENT_TYPE>
    void destroy(ELEMENT_TYPE *address);
        // Call the 'TYPE' destructor for the object pointed to by the
        // specified 'p'.  Do not directly deallocate any memory.

    // ACCESSORS
    pointer address(reference x) const;
        // Return the address of the object referred to by the specified 'x',
        // even if the (template parameter) 'TYPE' overloads the unary
        // 'operator&'.

    const_pointer address(const_reference x) const;
        // Return the address of the object referred to by the specified 'x',
        // even if the (template parameter) 'TYPE' overloads the unary
        // 'operator&'.

    size_type max_size() const;
        // Return the maximum number of elements of (template parameter) 'TYPE'
        // that can be allocated using this test allocator.  Note that there is
        // no guarantee that attempts at allocating fewer elements than the
        // value returned by 'max_size' will not throw.

    TestAllocator *mechanism() const;
        // Return a pointer to the mechanism object to which this proxy
        // forwards allocation and deallocation calls.
};

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const StdTestAllocator<T1>& lhs,
                const StdTestAllocator<T2>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' are proxies for the same
    // 'bslma::TestAllocator' object.  This is a practical implementation of
    // the STL requirement that two allocators compare equal if and only if
    // memory allocated from one can be deallocated from the other.  Note that
    // the two allocators need not be instantiated on the same type in order to
    // compare equal.

template <class T1, class T2>
inline
bool operator!=(const StdTestAllocator<T1>& lhs,
                const StdTestAllocator<T2>& rhs);
    // Return 'true' unless the specified 'lhs' and 'rhs' are proxies for the
    // same 'bslma::TestAllocator' object, in which case return 'false'.  This
    // is a practical implementation of the STL requirement that two allocators
    // compare equal if and only if memory allocated from one can be
    // deallocated from the other.  Note that the two allocators need not be
    // instantiated on the same type in order to compare equal.

template <class TYPE>
inline
bool operator==(const StdTestAllocator<TYPE>& lhs,
                const TestAllocator          *rhs);
    // Return 'true' if the specified 'lhs' is a proxy for the specified 'rhs',
    // and 'false' otherwise.

template <class TYPE>
inline
bool operator!=(const StdTestAllocator<TYPE>& lhs,
                const TestAllocator          *rhs);
    // Return 'true' unless the specified 'lhs' is a proxy for the specified
    // 'rhs', in which case return 'false'.

template <class TYPE>
inline
bool operator==(const TestAllocator          *lhs,
                const StdTestAllocator<TYPE>& rhs);
    // Return 'true' if the specified 'rhs' is a proxy for the specified 'lhs',
    // and 'false' otherwise.

template <class TYPE>
inline
bool operator!=(const TestAllocator          *lhs,
                const StdTestAllocator<TYPE>& rhs);
    // Return 'true' unless the specified 'rhs' is a proxy for the specified
    // 'lhs', in which case return 'false'.


// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                             // ----------------------
                             // class StdTestAllocator
                             // ----------------------

// LOW-LEVEL ACCESSORS
template <class TYPE>
inline
TestAllocator *StdTestAllocator<TYPE>::mechanism() const
{
    return d_mechanism;
}

// CREATORS
template <class TYPE>
inline
StdTestAllocator<TYPE>::StdTestAllocator()
: d_mechanism(dynamic_cast<TestAllocator *>(Default::defaultAllocator()))
{
    BSLS_ASSERT_SAFE(d_mechanism);
}

template <class TYPE>
inline
StdTestAllocator<TYPE>::StdTestAllocator(Allocator *mechanism)
: d_mechanism(dynamic_cast<TestAllocator *>(Default::allocator(mechanism)))
{
    BSLS_ASSERT_SAFE(d_mechanism);
}

template <class TYPE>
inline
StdTestAllocator<TYPE>::StdTestAllocator(const StdTestAllocator& original)
: d_mechanism(original.mechanism())
{
    BSLS_ASSERT_SAFE(d_mechanism);
}

template <class TYPE>
template <class ANY_TYPE>
inline
StdTestAllocator<TYPE>::StdTestAllocator(const StdTestAllocator<ANY_TYPE>& rhs)
: d_mechanism(rhs.mechanism())
{
    BSLS_ASSERT_SAFE(d_mechanism);
}

// MANIPULATORS
template <class TYPE>
inline
typename StdTestAllocator<TYPE>::pointer
StdTestAllocator<TYPE>::allocate(typename StdTestAllocator::size_type  n,
                          const void                    *hint)
{
    BSLS_ASSERT_SAFE(n <= this->max_size());

    (void) hint;  // suppress unused parameter warning
    return static_cast<pointer>(d_mechanism->allocate(n * sizeof(TYPE)));
}

template <class TYPE>
inline
void StdTestAllocator<TYPE>::deallocate(typename StdTestAllocator::pointer   p,
                                        typename StdTestAllocator::size_type n)
{
    (void) n;  // suppress unused parameter warning
    d_mechanism->deallocate(p);
}

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void StdTestAllocator<TYPE>::construct(ELEMENT_TYPE *address, const TYPE& val)
{
    new (static_cast<void*>(address)) ELEMENT_TYPE(val);
}

template <class TYPE>
template <class ELEMENT_TYPE>
inline
void StdTestAllocator<TYPE>::destroy(ELEMENT_TYPE *address)
{
    DestructionUtil::destroy(address);
}

// ACCESSORS
template <class TYPE>
inline
typename StdTestAllocator<TYPE>::const_pointer
StdTestAllocator<TYPE>::address(const_reference x) const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

template <class TYPE>
inline
typename StdTestAllocator<TYPE>::pointer
StdTestAllocator<TYPE>::address(reference x) const
{
    return BSLS_UTIL_ADDRESSOF(x);
}

template <class TYPE>
inline
typename StdTestAllocator<TYPE>::size_type
StdTestAllocator<TYPE>::max_size() const
{
    // Return the largest value, 'v', such that 'v * sizeof(T)' fits in a
    // 'size_type'.

    // We will calculate MAX_NUM_BYTES based on our knowledge that
    // 'bslma::Allocator::size_type' is just an alias for 'std::size_t'.  First
    // demonstrate that is true:

    BSLMF_ASSERT((bsl::is_same<Allocator::size_type, std::size_t>::value));

    static const std::size_t MAX_NUM_BYTES    = ~std::size_t(0);
    static const std::size_t MAX_NUM_ELEMENTS = MAX_NUM_BYTES / sizeof(TYPE);

    return MAX_NUM_ELEMENTS;
}

// FREE OPERATORS
template <class T1, class T2>
inline
bool operator==(const StdTestAllocator<T1>& lhs,
                const StdTestAllocator<T2>& rhs)
{
    return lhs.mechanism() == rhs.mechanism();
}

template <class T1, class T2>
inline
bool operator!=(const StdTestAllocator<T1>& lhs,
                const StdTestAllocator<T2>& rhs)
{
    return ! (lhs == rhs);
}

template <class TYPE>
inline
bool operator==(const StdTestAllocator<TYPE>& lhs,
                const TestAllocator          *rhs)
{
    return lhs.mechanism() == rhs;
}

template <class TYPE>
inline
bool operator!=(const StdTestAllocator<TYPE>& lhs,
                const TestAllocator          *rhs)
{
    return ! (lhs == rhs);
}

template <class TYPE>
inline
bool operator==(const TestAllocator          *lhs,
                const StdTestAllocator<TYPE>& rhs)
{
    return lhs == rhs.mechanism();
}

template <class TYPE>
inline
bool operator!=(const TestAllocator          *lhs,
                const StdTestAllocator<TYPE>& rhs)
{
    return ! (lhs == rhs);
}

}  // close package namespace
}  // close enterprise namespace

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
