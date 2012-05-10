// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter template 'priority_queue'.
//
//@CLASSES:
//   bslstl::priority_queue: template of highest-priority-first data structure
//
//@SEE_ALSO: bslstl_stack, bslstl_queue
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component defines two template classes, 'bsl::queue' and
// 'bsl::priority_queue', both holding a container, and adapting the held
// container (of a parameterized type 'CONTAINER' containing elements of
// another parameterized type 'VALUE') to provide first-in-first-out and
// highest-priority-first data structures respectively.  The 'priority_queue'
// takes a third parameterized type 'COMPARATOR' for comparing priorities
// between two elements.
//
// An instantiation of 'queue' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of elements held) and the sequence
// of values (of held elements) in the order that they were pushed into the
// 'queue'.  If 'queue' is instantiated with a parameterized type 'VALUE' that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.
//
// An instantiation of 'priority_queue' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of elements held)
// and the sorted sequence of values (of held elements).  If 'priority_queue'
// is instantiated with a parameterized type 'VALUE' that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// A 'priority_queue' cannot be tested for equality, but its parameterized type
// 'VALUE' must be able to be tested for comparing less by its parameterized
// type 'COMPARATOR'.
//
// The 'queue' and 'priority_queue' meet the requirements of a container
// adapter in the C++ standard [23.6].  The 'queue' and 'priority_queue'
// implemented here adheres to the C++11 standard, except that it does
// not have interfaces that take rvalue references, 'initializer_lists', and
// 'emplace'.  Note that excluded C++11 features are those that require (or are
// greatly simplified by) C++11 compiler support.

///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of 'queue' and
// 'priority_queue' constructors determines how the held container (of
// parameterized 'CONTAINER') will allocate memory.  A 'queue' or
// 'priority_queue' supports allocators meeting the requirements of the C++11
// standard [17.6.3.5] as long as the held container does.  In addition it
// supports scoped-allocators derived from the 'bslma_Allocator' memory
// allocation protocol.  Clients intending to use 'bslma' style allocators
// should use the template's default 'ALLOCATOR' type: The default type for the
// 'ALLOCATOR' template parameter, 'bsl::allocator',  provides a C++11
// standard-compatible adapter for a 'bslma_Allocator' object.
//
///'bslma'-Style Allocators
/// - - - - - - - - - - - -
// If a 'queue' or 'priority_queue' object's parameterized 'ALLOCATOR' type is
// convertible from 'bslma_Allocator *', as is true for the default 'ALLOCATOR' type
// 'bsl::allocator', then that 'map' will conform to the standard behavior of
// a 'bslma' allocator enabled type.  Such a 'map' accepts an optional
// 'bslma_Allocator' argument at construction.  If a 'bslma_Allocator' object
// is explicitly supplied (at construction) it will be used to supply memory
// for the 'map', otherwise the 'map' will use the default allocator installed
// at the time of the map's construction (see 'bslma_default').  In addition
// to directly allocating memory from the indicated 'bslma_Allocator', a 'map'
// supplies that allocator to the constructors of contained objects of the
// parameterized 'KEY' and 'VALUE' types if those types define the
// 'bslalg_TypeTraitUsesBslmaAllocator' trait, respectively.

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace bsl {

                         // ====================
                         // class priority_queue
                         // ====================

template <class VALUE,
          class CONTAINER  = vector<VALUE>,
          class COMPARATOR = native_std::less<typename CONTAINER::value_type> >
class priority_queue {

  protected:

    CONTAINER c;
    COMPARATOR comp;

  public:

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS

    explicit priority_queue();
        // TBD

    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // TBD

//  explicit priority_queue(const COMPARATOR& comparator = COMPARATOR(),
//                                CONTAINER&& container  = CONTAINER ());

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last);
        // TBD

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last,
                   const COMPARATOR& comparator,
                   const CONTAINER&  container);

//  template <class INPUT_ITERATOR>
//  priority_queue(INPUT_ITERATOR    first,
//                 INPUT_ITERATOR    last,
//                 const COMPARATOR& comparator = COMPARATOR(),
//                 CONTAINER&&       allocator  = CONTAINER ());

//  priority_queue(priority_queue&&);

    priority_queue(const priority_queue&);

    template <class ALLOCATOR>
    explicit priority_queue(const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator, const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container,
                   const ALLOCATOR&  allocator);
        // TBD

//  template <class ALLOCATOR>
//  priority_queue(const COMPARATOR& comparator,
//                       CONTAINER&& container,
//                 const ALLOCATOR&  allocator);

    template <class ALLOCATOR>
    priority_queue(const priority_queue& original, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  priority_queue(priority_queue&& original, const ALLOCATOR& allocator);


    // MANIPULATORS

//  priority_queue& operator=(priority_queue&& rhs);

    priority_queue& operator=(const priority_queue& rhs);

    void push(const value_type& value);
        // TBD

//  void push(value_type&& value);

//  template <class... Args>
//  void emplace(Args&&... args);

    void pop();
        // TBD

    void swap(priority_queue& other);
        // TBD

    // ACCESSORS
    bool empty() const;
        // TBD

    size_type size() const;
        // TBD

    const_reference top() const;
        // TBD
};

// FREE FUNCTIONS

template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs);

// template <class VALUE,
//           class CONTAINER,
//           class COMPARATOR,
//           class ALLOCATOR>
// struct
// uses_allocator<priority_queue<VALUE, CONTAINER, COMPARATOR>, ALLOCATOR>
// : uses_allocator<CONTAINER, ALLOCATOR>::type
// {
// };

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // --------------------
                         // class priority_queue
                         // --------------------

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue()
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                       INPUT_ITERATOR    first,
                                                       INPUT_ITERATOR    last)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  INPUT_ITERATOR    first,
                                                  INPUT_ITERATOR    last,
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                const priority_queue& original)
: c(original.c)
, comp(original.comp)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                    const ALLOCATOR& allocator)
: c(allocator)
, comp(COMPARATOR())
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const ALLOCATOR&  allocator)
: c(allocator)
, comp(comparator)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container,
                                                  const ALLOCATOR&  allocator)
: c(container, allocator)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}


template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                               const priority_queue& original,
                                               const ALLOCATOR&      allocator)
: c(original.c, allocator)
, comp(original.comp)
{
}


// MANIPULATORS

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>&
priority_queue<VALUE, CONTAINER, COMPARATOR>::operator=(
                                                     const priority_queue& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        c    = rhs.c;
        comp = rhs.comp;
    }
    return *this;
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::push(
                                                       const value_type& value)
{
    c.push_back(value);
    native_std::push_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::pop()
{
    native_std::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::swap(
                                                         priority_queue& other)
{
    BloombergLP::bslalg_SwapUtil::swap(&c   , &other.c   );
    BloombergLP::bslalg_SwapUtil::swap(&comp, &other.comp);
}

// ACCESSORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
bool priority_queue<VALUE, CONTAINER, COMPARATOR>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::size_type
priority_queue<VALUE, CONTAINER, COMPARATOR>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::const_reference
priority_queue<VALUE, CONTAINER, COMPARATOR>::top() const
{
    return c.front();
}

// FREE FUNCTIONS
template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
