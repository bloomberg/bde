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
// first-in-first-out data structure.  
//
// An instantiation of 'queue' is an allocator-aware, value-semantic type whose
// salient attributes are its size (number of elements held) and the sequence
// of values (of held elements) in the order that they were pushed into the
// 'queue'.  If 'queue' is instantiated with a parameterized type 'VALUE' that
// is not itself value-semantic, then it will not retain all of its
// value-semantic qualities.
//
// A 'queue' meets the requirements of a container adapter in the C++ standard
// [23.6].  The 'queue' implemented here adheres to the C++11 standard, except
// that it does not have interfaces that take rvalue references,
// 'initializer_lists', and 'emplace'.  Note that excluded C++11 features are
// those that require (or are greatly simplified by) C++11 compiler support.

///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of 'queue'
// constructors determines how the held container (of parameterized
// 'CONTAINER') will allocate memory.  A 'queue' supports
// allocators meeting the requirements of the C++11 standard [17.6.3.5] as long
// as the held container does.  In addition it supports scoped-allocators
// derived from the 'bslma_Allocator' memory allocation protocol.  Clients
// intending to use 'bslma' style allocators should use the template's default
// 'ALLOCATOR' type: The default type for the 'ALLOCATOR' template parameter,
// 'bsl::allocator',  provides a C++11 standard-compatible adapter for a
// 'bslma_Allocator' object.
//
///TODO Operations
///----------
// This section describes the run-time complexity of operations on instances of
// 'queue': (skong25: the complexity of each operation depends on the adapted
// container, do we need this section here?)
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
//        int numInt     = sizeof(intArray) / sizeof(*intArray);
//..
// Then, we create a 'bsl::queue' object to adapt the 'bsl::deque<int>' type,
// using the default constructor of 'queue':
//..
//  bsl::queue<int, deque<int> > intQueue;
//..
// Now, using a for loop, we push the integers in the previously defined array
// into the queue:
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
// Finally, using a second for loop, we pop integers from the queue one by one,
//..
//  for (int i = 0;i < numInt; ++i) {
//      assert(intArray[i] == intQueue.front()); 
//      intQueue.pop();
//  }
//  assert(intQueue.empty());
//..
// Notice that every time an integer is popped out from the queue, the front
// element of the queue becomes the previously next pushed integer.  This is
// verified by invoking the 'front' accessor.  The sequence of integers popped
// out is in the exact same order as they were pushed in, keeping the
// first-in-first-out property.

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
    // This class is a value-semantic class template, having a container of the
    // parameterized 'CONTAINER' type that holds elements of the parameterized
    // 'VALUE' type, to provide a first-in-first-out data structure.

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
        // Construct an empty queue having a container of the parameterized
        // 'CONTAINER' type.  Note that the memory is supplied by the currently
        // installed default allocator.

    explicit queue(const CONTAINER& container);
        // Construct a queue having the specified 'container' that holds
        // elements of the parameterized 'VALUE' type.  Note that the memory is
        // supplied by the currently installed default allocator.

//  explicit queue(CONTAINER&& = CONTAINER());

//  queue(queue&& q);

    queue(const queue& original);
        // Construct a queue having the same value as that of the specified
        // 'original'.  Note that the memory is supplied by the currently
        // installed default allocator.

    template <class ALLOCATOR>
    explicit queue(const ALLOCATOR& allocator);
        // Construct an empty queue that holds a default-constructed container
        // of the parameterized 'CONTAINER' type, and will use the specified
        // 'allocator' to supply memory.  If the template parameter 'ALLOCATOR'
        // is 'bsl::allocator' (the default) then 'allocator' shall be
        // convertible to 'bslma_Allocator*'.

    template <class ALLOCATOR>
    queue(const CONTAINER& container, const ALLOCATOR& allocator);
        // Construct an empty queue that holds the specified 'container', and
        // will use the specified 'allocator' to supply memory.  If the
        // template parameter 'ALLOCATOR' is 'bsl::allocator' (the default)
        // then 'allocator' shall be convertible to 'bslma_Allocator*'.

//  template <class ALLOCATOR>
//  queue(CONTAINER&& container, const ALLOCATOR& allocator);

    template <class ALLOCATOR>
    queue(const queue& original, const ALLOCATOR& allocator);
        // Construct a queue having the same value as that of the specified
        // 'original' that will use the specified 'allocator' to supply memory.
        // If the template parameter 'ALLOCATOR' is 'bsl::allocator' (the
        // default) then 'allocator' shall be convertible to
        // 'bslma_Allocator*'.


//  template <class ALLOCATOR>
//  queue(queue&& original, const ALLOCATOR& allocator);

    // MANIPULATORS

//  queue& operator=(queue&& other);

    queue& operator=(const queue& other);
        // Assign to this 'queue' object the value of the specified 'other' and
        // return a reference to this modifiable object.

    void push(const value_type& value);
        // Insert a new element have the specified 'value' to the back of this
        // 'queue' object.

//  void push(value_type&& value) { c.push_back(std::move(value)); }
//  template <class... Args> void emplace(Args&&... args)
//  { c.emplace_back(std::forward<Args>(args)...); }

    void pop();
        // Remove the front (the earliest pushed) element from this 'queue'
        // object.

    void swap(queue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.

    reference front();
        // Return the mutable front (the earliest pushed) element from this
        // 'queue' object.

    reference back();
        // Return the mutable back (the latest pushed) element from this
        // 'queue' object.


    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'queue' object contains no elements, and
        // 'false' otherwise.

    size_type size() const;
        // Return 'true' if this 'queue' object contains no elements, and
        // 'false' otherwise.

    const_reference front() const;
        // Return the immutable front (the earliest pushed) element from this
        // 'queue' object.

    const_reference back() const;
        // Return the immutable back (the latest pushed) element from this
        // 'queue' object.
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
queue<VALUE, CONTAINER>::operator=(const queue& other)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &other)) {
        c = other.c;
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
