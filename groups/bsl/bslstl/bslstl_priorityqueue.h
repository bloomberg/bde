// bslstl_queue.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_QUEUE
#define INCLUDED_BSLSTL_QUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter class template 'priority_queue'.
//
//@CLASSES:
//   bslstl::priority_queue: template of highest-priority-first data structure
//
//@SEE_ALSO: bslstl_queue, bslstl_stack
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component defines a class template, 'bsl::priority_queue',
// holding a container (of a parameterized type 'CONTAINER' containing elements
// of another parameterized type 'VALUE'), and adapting it to provide
// highest-priority-first data structure.  The component takes a third
// parameterized type 'COMPARATOR' for customized priorities comparison between
// two elements.
//
// An instantiation of 'priority_queue' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of held elements) and the
// sorted sequence of values (of held elements).  If 'priority_queue' is
// instantiated with a parameterized type 'VALUE' that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// A 'priority_queue' cannot be tested for equality, but its parameterized type
// 'VALUE' must be able to be tested for comparing less by its parameterized
// type 'COMPARATOR'.
//
// A 'priority_queue' meet the requirements of a container adapter in the C++
// standard [23.6].  The 'priority_queue' implemented here adheres to the C++11
// standard, except that it does not have interfaces that take rvalue
// references, 'initializer_lists', and 'emplace'.  Note that excluded C++11
// features are those that require (or are greatly simplified by) C++11
// compiler support.

///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of
// 'priority_queue' constructors determines how the held container (of
// parameterized 'CONTAINER') will allocate memory.  A 'priority_queue'
// supports allocators meeting the requirements of the C++11 standard
// [17.6.3.5] as long as the held container does.  In addition it supports
// scoped-allocators derived from the 'bslma_Allocator' memory allocation
// protocol.  Clients intending to use 'bslma' style allocators should use the
// template's default 'ALLOCATOR' type: The default type for the 'ALLOCATOR'
// template parameter, 'bsl::allocator',  provides a C++11 standard-compatible
// adapter for a 'bslma_Allocator' object.
//
///TODO Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'priority_queue': (skong25: the complexity of each operation depends
// on the adapted container, do we need this section here?)
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Pushing and Popping from a Priority Queue
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will define an array of integer, push them into a
// priority queue, and then pop them out.  The parameterized type 'VALUE' is
// 'int' in this example; the parameterized type 'CONTAINER' to be adapted is
// 'bsl::deque<int>'; the parameterized type 'COMPARATOR' is default to
// 'std::less'.
//
// First, we define an array of integers:
//..
//  const int intArray[] = {0, -2, INT_MAX, INT_MIN, -1, 1, 2};
//        int numInt     = sizeof(intArray) / sizeof(*intArray);
//..
// Then, we create a 'bsl::priority_queue' object via its default constructor
// to adapt the 'bsl::deque<int>' type, leaving the parameterized 'COMPARATOR'
// default to 'std::less':
//..
//  bsl::priority_queue<int, deque<int> > intPrQueue;
//..
// Now, using a 'for' loop, we push the integers in the previously created
// integer array into the priority queue:
//..
//  printf("Pushing...\n\n");
//  for (int i = 0; i < numInt; ++i) {
//      intPrQueue.push(intArray[i]);
//      printf("%d: %d\n", i, intPrQueue.top());
//  }
//..
// Notice that every time a new integer is pushed in, the priority queue
// adjusts its internal data structure to ensure the 'top' method always
// returns the highest priority (value) of elements held in the priority queue.
//
// Finally, using a second 'for' loop, we pop integers from the priority queue
// one by one:
//..
//  printf("\nPopping...\n\n");
//  for (int i = 0;i < numInt; ++i) {
//      printf("%d: %d\n", i, intPrQueue.top());
//      intPrQueue.pop();
//  }
//  assert(intPrQueue.empty());
//..
// Notice that every time an integer is popped out, the priority queue adjusts
// its internal data structure to ensure the 'top' method always returns the
// highest priority (value) of elements remaining in the priority queue.
//
// The screen output will be like the following:
//
//      Pushing...
//
//      0: 0
//      1: 0
//      2: 2147483647
//      3: 2147483647
//      4: 2147483647
//      5: 2147483647
//      6: 2147483647
//
//      Popping...
//
//      0: 2147483647
//      1: 2
//      2: 1
//      3: 0
//      4: -1
//      5: -2
//      6: -2147483648

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
    // This class is a value-semantic class template, adapting a container of
    // the parameterized 'CONTAINER' type that holds elements of the
    // parameterized 'VALUE' type, to provides a highest-priority-first data
    // structure, where the priorities of elements are compared by a comparator
    // of the parameterized 'COMPARATOR' type.

  protected:

    CONTAINER c;
    COMPARATOR comp;

  public:

    // PUBLIC TYPES
    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS
    explicit priority_queue();
        // Construct an empty priority, adapting a default-constructed
        // container of the parameterized 'CONTAINER' type.  Use a
        // default-constructed comparator of the parameterized 'COMPARATOR'
        // type to order priorities of elements.  Note that memory is supplied
        // by the currently installed default allocator.

    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Construct a priority queue, adapting the specified 'container',
        // having the specified 'comparator' to order priorities of elements
        // held in 'container'.  Note that memory is supplied by the currently
        // installed default allocator.

//  explicit priority_queue(const COMPARATOR& comparator = COMPARATOR(),
//                                CONTAINER&& container  = CONTAINER ());

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last);
        // Construct an empty priority queue, adapting a default-constructed
        // container of the parameterized 'CONTAINER' type, and insert into the
        // container a sequence of 'value_type' elements starting at the
        // specified 'first', and ending immediately before the specified
        // 'last'.  Use a default-constructed comparator of the parameterized
        // 'COMPARATOR' type to order the priorities of elements.  Note that
        // memory is supplied by the currently installed default allocator.

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last,
                   const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Construct a priority queue, adapting the specified 'container',
        // having the specified 'comparator' to order the priorities of
        // elements, including those originally existed in 'container', and
        // those inserted into the 'container' from a sequence of 'value_type'
        // elements starting at the specified 'first', and ending immediately
        // before the specified 'last'.  Note that memory is supplied by the
        // currently installed default allocator.

//  template <class INPUT_ITERATOR>
//  priority_queue(INPUT_ITERATOR    first,
//                 INPUT_ITERATOR    last,
//                 const COMPARATOR& comparator = COMPARATOR(),
//                 CONTAINER&&       allocator  = CONTAINER ());

//  priority_queue(priority_queue&&);

    priority_queue(const priority_queue& original);
        // Construct a priority queue having the same value as the specified
        // 'original'.  Use the comparator from 'original' to order the
        // priorities of elements.  Note that memory is supplied by the
        // currently installed default allocator.

    template <class ALLOCATOR>
    explicit priority_queue(const ALLOCATOR& allocator);
        // Construct an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER' type and will use the
        // specified 'allocator' to supply memory.  Use a default-constructed
        // comparator of the parameterized 'COMPARATOR' type to order the
        // priorities of elements.  If the template parameter 'ALLOCATOR' is
        // 'bsl::allocator' (the default) then 'allocator' shall be convertible
        // to 'bslma_Allocator*'.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator, const ALLOCATOR& allocator);
        // Construct an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER'type and will use the
        // specified 'allocator' to supply memory, and the specified
        // 'comparator' to order priorities of elements in a
        // default-constructed container of the parameterized 'CONTAINER' type.
        // If the template parameter 'ALLOCATOR' is 'bsl::allocator' (the
        // default) then 'allocator' shall be convertible to
        // 'bslma_Allocator*'.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container,
                   const ALLOCATOR&  allocator);
        // Construct a priority queue that will use the specified 'allocator'
        // to supply memory, and the specified 'comparator' to order priorities
        // of elements in the specified 'container'.  If the template parameter
        // 'ALLOCATOR' is 'bsl::allocator' (the default) then 'allocator' shall
        // be convertible to 'bslma_Allocator*'.

//  template <class ALLOCATOR>
//  priority_queue(const COMPARATOR& comparator,
//                       CONTAINER&& container,
//                 const ALLOCATOR&  allocator);

    template <class ALLOCATOR>
    priority_queue(const priority_queue& original, const ALLOCATOR& allocator);
        // Construct a priority queue having the same value as the specified
        // 'original' that will use the specified 'allocator' to supply memory.
        // Use the comparator from 'original' to order the priorities of
        // elements.  If the template parameter 'ALLOCATOR' is 'bsl::allocator'
        // (the default) then 'allocator' shall be convertible to
        // 'bslma_Allocator*'.

//  template <class ALLOCATOR>
//  priority_queue(priority_queue&& original, const ALLOCATOR& allocator);


    // MANIPULATORS

//  priority_queue& operator=(priority_queue&& other);

    priority_queue& operator=(const priority_queue& other);
        // Assign to this 'priority_queue' object the value of the specified
        // 'other' and return a reference to this modifiable object.

    void push(const value_type& value);
        // Insert a new element having the specified priority 'value' into this
        // 'priority_queue' object.

//  void push(value_type&& value);

//  template <class... Args>
//  void emplace(Args&&... args);

    void pop();
        // Remove the top element from this 'priority_queue' object that has
        // the highest priority.  The behavior is undefined if there is
        // currently no elements in this object.

    void swap(priority_queue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'pirority_queue' object contains no elements,
        // and 'false' otherwise.

    size_type size() const;
        // Return the number of elements in this 'priority_queue' object.

    const_reference top() const;
        // Return the immutable element having the highest priority in this
        // 'priority_queue' object.  The behavior is undefined if there is
        // currently no elements in this object.
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
                                                   const priority_queue& other)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &other)) {
        c    = other.c;
        comp = other.comp;
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
void priority_queue<VALUE, CONTAINER, COMPARATOR>::swap(priority_queue& other)
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
