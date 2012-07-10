// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter class template 'queue'.
//
//@CLASSES:
//   bslstl::queue: class template of a first-in-first-out data structure
//
//@SEE_ALSO: bslstl_priorityqueue, bslstl_stack
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component defines a class template, 'bsl::queue', holding
// a container (of a parameterized type 'CONTAINER' containing elements of
// another parameterized type 'VALUE'), and adapting it to provide a
// first-in-first-out queue data structure.
//
// An instantiation of 'queue' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of elements held) and the sequence
// of values (of held elements) in the order that they were pushed into the
// 'queue'.  If 'queue' is instantiated with a parameterized type 'VALUE' that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.
//
// 'queue' meets the requirements of a container adapter in the C++ standard
// [23.6].  The 'queue' implemented here adheres to the C++11 standard, except
// that it does not have methods that take rvalue references and
// 'initializer_lists'.  Note that excluded C++11 features are those that
// require C++11 compiler support.
//
///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of 'queue'
// constructors determines how the held container (of parameterized
// 'CONTAINER') will allocate memory.  A 'queue' supports
// allocators meeting the requirements of the C++11 standard [17.6.3.5] as long
// as the held container does.  In addition it supports scoped-allocators
// derived from the 'bslma_Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use 'bsl::allocator' as the
// 'ALLOCATOR' template parameter, providing a C++11 standard-compatible
// adapter for a 'bslma_Allocator' object.
//
///Operations
///----------
// The C++11 standard [23.6.3.1] declares any container type supporting
// operations 'front', 'back', 'push_back' and 'pop_front' can be used to
// instantiate the parameterized type 'CONTAINER'.  Below is a list of public
// methods of 'queue' class that effectively forward their implementations to
// corresponding operations in the held container (referenced as 'c').
//  +--------------------------------------+---------------------------+
//  | Public methods in 'queue'            | Operation in 'CONTAINER'  |
//  +======================================+===========================+
//  | void push(const value_type& value);  | c.push_back(value);       |
//  | void pop();                          | c.pop_front();            |
//  | reference front();                   | c.front();                |
//  | reference back();                    | c.back();                 |
//  +--------------------------------------+---------------------------+
//  | bool empty() const;                  | c.empty();                |
//  | size_type size() const;              | c.size();                 |
//  | const_reference front() const;       | c.front();                |
//  | const_reference back()  const;       | c.back();                 |
//  +--------------------------------------+---------------------------+
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Pushing and Popping from a Queue
///- - - - - - - - - - - - - - - - - - - - - -
// In this example, we will define an array of integer, push them into a queue,
// and then pop them out.  The parameterized type 'VALUE' is 'int' in this
// example and the container to be adapted is 'bsl::deque<int>' type.
//
// First, we define an array of integers:
//..
//  const int intArray[] = {0, -2, INT_MAX, INT_MIN, -1, 1, 2};
//  const int numInt     = sizeof(intArray) / sizeof(*intArray);
//..
// Then, we create a 'bsl::queue' object to adapt the 'bsl::deque<int>' type,
// using the default constructor of 'queue':
//..
//  bsl::queue<int> intQueue;
//..
// Now, using a 'for' loop, we push the integers from the previously defined
// array into the queue:
//..
//  for (int i = 0; i < numInt; ++i) {
//      intQueue.push(intArray[i]);
//      assert(intArray[i] == intQueue.back());
//  }
//..
// Notice that every time a new integer is pushed into the queue, the integer
// becomes the back element of the queue.  This is verified by invoking the
// 'back' accessor.
//
// Finally, using a second 'for' loop, we pop integers from the queue one by
// one:
//..
//  for (int i = 0; !intQueue.empty(); ++i) {
//      assert(intArray[i] == intQueue.front());
//      intQueue.pop();
//  }
//  assert(intQueue.empty());
//..
// Notice that every time an integer is popped out from the queue, the front
// element of the queue becomes the previously next pushed integer.  This is
// verified by invoking the 'front' accessor.  The sequence of integers popped
// out is in the exact same order as they were pushed in, according to the
// first-in-first-out property of the queue.

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
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
class queue
    // This class is a value-semantic class template, having a container of the
    // parameterized 'CONTAINER' type that holds elements of the parameterized
    // 'VALUE' type, to provide a first-in-first-out queue data structure.  The
    // container object held by a 'queue' class object is referenced as 'c' in
    // the following documentation.
{
    // FRIENDS
    template <class VALUE2, class CONTAINER2>
    friend bool operator==(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator!=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator< (const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator> (const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator<=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

    template <class VALUE2, class CONTAINER2>
    friend bool operator>=(const queue<VALUE2, CONTAINER2>&,
                           const queue<VALUE2, CONTAINER2>&);

  public:
    // TYPES
    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

  protected:
    // DATA
    CONTAINER c;    // container of elements that the queue holds, protected as
                    // required by C++11

  public:
    // CREATORS
    explicit queue();
        // Construct an empty queue having a container of the parameterized
        // 'CONTAINER' type.

    explicit queue(const CONTAINER& container);
        // Construct a queue having the specified 'container' that holds
        // elements of the parameterized 'VALUE' type.

    queue(const queue& original);
        // Construct a queue having the same value as that of the specified
        // 'original'.

    template <class ALLOCATOR>
    explicit
    queue(const ALLOCATOR& allocator,
          typename BloombergLP::bslmf_EnableIf<
              BloombergLP::bslmf_IsConvertible<
                           ALLOCATOR,
                           typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // Construct an empty queue that holds a default-constructed container
        // of the parameterized 'CONTAINER' type, and will use the specified
        // 'allocator' to supply memory.  Note that the 'ALLOCATOR' parameter
        // type has to be convertible to the allocator of the 'CONTAINER'
        // parameter type, 'CONTAINER::allocator_type'.  Otherwise this
        // constructor is disabled.

    template <class ALLOCATOR>
    queue(const CONTAINER& container,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf_EnableIf<
              BloombergLP::bslmf_IsConvertible<
                           ALLOCATOR,
                           typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // Construct an empty queue that holds the specified 'container', and
        // will use the specified 'allocator' to supply memory.  Note that the
        // 'ALLOCATOR' parameter type has to be convertible to the allocator of
        // the 'CONTAINER' parameter type, 'CONTAINER::allocator_type'.
        // Otherwise this constructor is disabled.

    template <class ALLOCATOR>
    queue(const queue& original,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf_EnableIf<
              BloombergLP::bslmf_IsConvertible<
                           ALLOCATOR,
                           typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // Construct a queue having the same value as that of the specified
        // 'original' that will use the specified 'allocator' to supply memory.
        // Note that the 'ALLOCATOR' parameter type has to be convertible to
        // the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    // MANIPULATORS
    void push(const value_type& value);
        // Insert a new element having the specified 'value' to the back of
        // this 'queue' object.  In effect, performs 'c.push_back(value);'.

    void pop();
        // Remove the front (the earliest pushed) element from this 'queue'
        // object.  In effect, performs 'c.pop_front();'.

    void swap(queue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.

    reference front();
        // Return the mutable reference to the front (the earliest pushed)
        // element from this 'queue' object.  In effect, performs
        // 'return c.front();'.

    reference back();
        // Return the mutable reference to the back (the latest pushed) element
        // of this 'queue' object.  In effect, performs 'return c.back();'.


    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'queue' object contains no elements, and
        // 'false' otherwise.  In effect, performs 'return c.empty();'.

    size_type size() const;
        // Return 'true' if this 'queue' object contains no elements, and
        // 'false' otherwise.  In effect, performs 'return c.size();'.

    const_reference front() const;
        // Return the immutable front (the earliest pushed) element from this
        // 'queue' object.  In effect, performs 'c.front()'.

    const_reference back() const;
        // Return the immutable back (the latest pushed) element from this
        // 'queue' object.  In effect, performs 'c.back()'.
};

// FREE FUNCTIONS
template <class VALUE, class CONTAINER>
inline
bool operator==(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'queue' objects have the same value
    // if the containers they adapt are compared equal.

template <class VALUE, class CONTAINER>
inline
bool operator!=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'queue' objects do not have the
    // same value if the containers they adapt are not compared equal.

template <class VALUE, class CONTAINER>
inline
bool operator< (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' queue is lexicographically less
    // than the specified 'rhs' queue, and 'false' otherwise.  A queue 'lhs'
    // is lexicographically less than another queue 'rhs' if the container
    // adapted by 'lhs' is lexicographically less than that adapted by 'rhs'.

template <class VALUE, class CONTAINER>
inline
bool operator> (const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' queue is lexicographically greater
    // than the specified 'rhs' queue, and 'false' otherwise.  A queue 'lhs'
    // is lexicographically greater than another queue 'rhs' if the container
    // adapted by 'lhs' is lexicographically greater than that adapted by
    // 'rhs'.

template <class VALUE, class CONTAINER>
inline
bool operator>=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' queue is lexicographically
    // greater-than or equal-to the specified 'rhs' queue, and 'false'
    // otherwise.  A queue 'lhs' is lexicographically greater-than or equal-to
    // another queue 'rhs' if the container adapted by 'lhs' is
    // lexicographically greater-than or equal-to that adapted by 'rhs'.

template <class VALUE, class CONTAINER>
inline
bool operator<=(const queue<VALUE, CONTAINER>& lhs,
                const queue<VALUE, CONTAINER>& rhs);
    // Return 'true' if the specified 'lhs' queue is lexicographically
    // less-than or equal-to the specified 'rhs' queue, and 'false' otherwise.
    // A queue 'lhs' is lexicographically less-than or equal-to another queue
    // 'rhs' if the container adapted by 'lhs' is lexicographically less-than
    // or equal-to that adapted by 'rhs'.

template <class VALUE, class CONTAINER>
inline
void swap(queue<VALUE, CONTAINER>& lhs,
          queue<VALUE, CONTAINER>& rhs);
    // Swap the value of the specified 'lhs' queue with the value of the
    // specified 'rhs' queue.

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
queue<VALUE, CONTAINER>::queue(const ALLOCATOR& allocator,
                               typename BloombergLP::bslmf_EnableIf<
                                   BloombergLP::bslmf_IsConvertible<
                                       ALLOCATOR,
                                       typename CONTAINER::allocator_type>
                                       ::VALUE>
                                   ::type *)
: c(allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const CONTAINER& container,
                               const ALLOCATOR& allocator,
                               typename BloombergLP::bslmf_EnableIf<
                                   BloombergLP::bslmf_IsConvertible<
                                       ALLOCATOR,
                                       typename CONTAINER::allocator_type>
                                       ::VALUE>
                                   ::type *)
: c(container, allocator)
{
}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
queue<VALUE, CONTAINER>::queue(const queue&     queue,
                               const ALLOCATOR& allocator,
                               typename BloombergLP::bslmf_EnableIf<
                                   BloombergLP::bslmf_IsConvertible<
                                       ALLOCATOR,
                                       typename CONTAINER::allocator_type>
                                       ::VALUE>
                                   ::type *)
: c(queue.c, allocator)
{
}

// MANIPULATORS
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
