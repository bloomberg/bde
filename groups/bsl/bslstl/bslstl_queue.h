// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter template 'queue'.
//
//@CLASSES:
//   bslstl::queue: template of first-in first-out data structure
//
//@SEE_ALSO: bslstl_stack, bslstl_priorityqueue
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

                             // ===========
                             // class queue
                             // ===========

template <class VALUE, class CONTAINER = deque<VALUE> >
class queue {

    // FRIENDS
    template<class VALUE2, class CONTAINER2>
    friend bool operator==(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator!=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator< (const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator> (const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator<=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);

    template<class VALUE2, class CONTAINER2>
    friend bool operator>=(const queue<VALUE2, CONTAINER2>& lhs,
                           const queue<VALUE2, CONTAINER2>& rhs);


  protected:

    CONTAINER c;

  public:

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS

    explicit queue();
        // TBD

    explicit queue(const CONTAINER& container);
        // TBD

//  explicit queue(CONTAINER&& = CONTAINER());

//  queue(queue&& q);

    queue(const queue& original);

    template <class ALLOCATOR>
    explicit queue(const ALLOCATOR& allocator);
        // TBD

    template <class ALLOCATOR>
    queue(const CONTAINER& container, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  queue(CONTAINER&& container, const ALLOCATOR& allocator);

    template <class ALLOCATOR>
    queue(const queue& original, const ALLOCATOR& allocator);
        // TBD

//  template <class ALLOCATOR>
//  queue(queue&& original, const ALLOCATOR& allocator);

    // MANIPULATORS

//  queue& operator=(queue&& rhs);

    queue& operator=(const queue& rhs);
        // TBD

    void push(const value_type& value);
        // TBD

//  void push(value_type&& value) { c.push_back(std::move(value)); }
//  template <class... Args> void emplace(Args&&... args)
//  { c.emplace_back(std::forward<Args>(args)...); }

    void pop();
        // TBD

    void swap(queue& other);
        // TBD

    // ACCESSORS
    bool empty() const;
        // TBD

    size_type size() const;
        // TBD

    reference front();
        // TBD

    const_reference front() const;
        // TBD

    reference back();
        // TBD

    const_reference back() const;
        // TBD

};

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // TBD

template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs);
    // TBD

// template <class VALUE, class CONTAINER, class ALLOCATOR>
// struct uses_allocator<queue<VALUE, CONTAINER>, ALLOCATOR>
// : uses_allocator<CONTAINER, ALLOCATOR>::type
// {
// };


// ==========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                              // -----------
                              // class queue
                              // -----------

// CREATORS
template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue()
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container)
: c(container)
{
}

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>::queue(const queue& original)
: c(original.c)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const ALLOCATOR& allocator)
: c(allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container,
                               const ALLOCATOR& allocator)
: c(container, allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const queue&     queue,
                               const ALLOCATOR& allocator)
: c(queue.c, allocator)
{
}

// MANIPULATORS

template <class VALUE, class CONTAINER>
inline
queue<VALUE, CONTAINER>&
queue<VALUE, CONTAINER>::operator=(const queue& rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &rhs)) {
        c = rhs.c;
    }
    return *this;
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::push(const value_type& value)
{
    c.push_back(value);
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::pop()
{
    c.pop_front();
}

template <class VALUE, class CONTAINER>
inline
void queue<VALUE, CONTAINER>::swap(queue& q)
{
    BloombergLP::bslalg_SwapUtil::swap(&c, &q.c);
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool queue<VALUE, CONTAINER>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::size_type
queue<VALUE, CONTAINER>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::front()
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::front() const
{
    return c.front();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::reference
queue<VALUE, CONTAINER>::back()
{
    return c.back();
}

template <class VALUE, class CONTAINER>
inline
typename queue<VALUE, CONTAINER>::const_reference
queue<VALUE, CONTAINER>::back() const
{
    return c.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c == rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c < rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c != rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c > rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c >= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs)
{
    return lhs.c <= rhs.c;
}

template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs)
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
