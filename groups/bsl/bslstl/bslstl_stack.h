// bslstl_stack.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_STACK
#define INCLUDED_BSLSTL_STACK

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant stack class.
//
//@CLASSES:
//   bslstl::stack: STL-compliant stack template
//
//@SEE_ALSO: bslstl_deque, bslstl_vector, bslstl_list, bslstl_queue,
//           bslstl_priorityqueue
//
//@AUTHOR: Bill Chapman (bchapman)
//
//@DESCRIPTION: This component describes 'bslstl::stack', a container adapter
// that takes an underlying container and provides a stack interface which the
// user accesses primarily through 'push', 'pop', and 'top' operations.  A
// 'deque' (the default), 'vector', or 'list' may be used, but in theory any
// container which supports 'push_back', 'pop_back', 'back', 'size', and
// 'get_allocator', plus a template specialization 'uses_allocator::type', may
// be used.
//
//
///Requirements of Parametrized 'CONTAINER' Type
///---------------------------------------------
// This class works with the container classes 'bsl::deque', 'bsl::vector', or
// 'bsl::list' passed to the 'CONTAINER' template class.  In theory another
// container class could be passed, but it must satisfy the following
// requirements:
//: o It must have the following public types:
//:   o value_type
//:   o reference
//:   o const_reference
//:   o size_type
//:   o allocator_type
//: o It must support the following methods, depending on what method of
//:   'stack' are used:
//:   o used constructors must take a parameter of type 'allocator_type'
//:   o void push_back(const value_type&)
//:   o void pop_back()
//:   o value_type& back()
//:   o size_type size()
//:   o allocator_type get_allocator()
//:   o '==', '!=', '<', '>', '<=', '>='
//:   o std::swap(CONTAINER&, CONTAINER&)
//
///Note on Parameterized 'VALUE' Type
///----------------------------------
// If the 'CONTAINER' type is specified, the 'VALUE' type specified is ignored
// and 'CONTAINER::value_type' is used in its place.  It is recommended, if
// 'CONTAINER' is specified, that a type equivalent to 'CONTAINER::value_type'
// be specified to the 'VALUE' template parameter so as not to mislead readers.
//
///Memory Allocation
///-----------------
// No memory allocator template arg is directly supplied to this class, the
// allocator type used is the allocator specified for the container class, and
// its type is available as 'CONTAINER::allocator_type'.  The value of the
// allocator used is available as 'stack.get_allocator()'.
//
///'bslma'-Style Allocators
///------------------------
// The constructors of this class take, as optional parameters, allocators of
// the object's parameterized 'CONTAINER::allocator_type' type, and allocators
// of this type are propagated to all constructors of the underlying container.
// In the case of container types 'bsl::deque' (the default type),
// 'bsl::vector', and 'bsl::list', 'CONTAINER::allocator_type' is
// 'bsl::allocator' which is implicitly convertible from 'bslma_Allocator *',
// and which can be converted to a 'bslma_Allocator *' through the 'mechanism'
// accessor.
//
// Hence if the underlying container takes 'bsl::allocator', then the 'stack'
// object can take 'bslma_Allocator *'s to supply memory allocation.  If no
// allocator is specified, 'allocator()' is used, which winds up using
// 'bslma_Default::allocator(0)'.
//
///Operations
///----------
// This section describes the run-time complexity of operations on instances
// of 'map':
//..
//  Legend
//  ------
//  'C'             - parameterized container-type of the stack
//  'V'             - 'C::value_type'
//  'c'             - container of type 'C'
//  'nc'            - number of elements in container 'c'
//  's', 't'        - two distinct objects of type 'stack<V, C>'
//  'n', 'm'        - number of elements in 'a' and 'b' respectively
//  'al             - a STL-style memory allocator
//  'v'             - an object of type 'V'
//
//
//  +----------------------------------------------------+--------------------+
//  | Note: the following estimations of operation complexity assume the      |
//  | underlying contianer is a 'bsl::deque', 'bsl::vector', or 'bsl::list'.  |
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | stack<V, C> s;    (default construction)           | O(1)               |
//  | stack<V, C> s(al);                                 |                    |
//  +----------------------------------------------------+--------------------+
//  | stack<V, C> s(c);                                  | O(nc)              |
//  | stack<V, C> s(c, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | stack<V, C> s(t);                                  | O(n)               |
//  | stack<V, C> s(t, al);                              |                    |
//  +----------------------------------------------------+--------------------+
//  | s.~stack(V, C>(); (destruction)                    | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s = t;          (assignment)                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s.push(v)                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s.pop()                                            | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s.top()                                            | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | s == t, s != t                                     | O(n)               |
//  +---------------------------------------------------+--------------------+
//  | s < t, s <= t, s > t, s >= t                       | O(n)               |
//  +----------------------------------------------------+--------------------+
//  | s.swap(t), swap(s,t)                               | depends on the     |
//  |                                                    | container; for     |
//  |                                                    | deque, vector, and |
//  |                                                    | list:              |
//  |                                                    | O(1) if 's' and    |
//  |                                                    | 't' use the same   |
//  |                                                    | allocator,         |
//  |                                                    | O(n + m) otherwise |
//  +----------------------------------------------------+--------------------+
//  | s.size()                                           | O(1) if 'C' is     |
//  |                                                    | deque or vector    |
//  +----------------------------------------------------+--------------------+
//  | s.empty()                                          | O(1)               |
//  +----------------------------------------------------+--------------------+
//  | get_allocator()                                    | O(1)               |
//  +----------------------------------------------------+--------------------+
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - - - - - - - - - - - - -
//..

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_STDEXCEPTUTIL
#include <bslstl_stdexceptutil.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLSTL_DEQUE
#include <bslstl_deque.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

namespace bsl {

template <class VALUE, class CONTAINER = deque<VALUE> >
class stack {
  public:
    // PUBLIC TYPES

    // Note that we never use 'VALUE' in the imp except in the default value of
    // 'CONTAINER'.  If 'CONTAINER' is specified, then 'CONTAINER::value_type'
    // is used for everything and 'VALUE' is ignored.

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef CONTAINER                           container_type;
    typedef typename CONTAINER::allocator_type  allocator_type;

  private:
    // PRIVATE DATA
    container_type  d_container;

  protected:
    // PROTECTED DATA
    container_type& c;    // We are required by the standard to have the
                          // container be a protected variable named 'c'.  Just
                          // a reference to 'd_container'.

    // TRAITS
    typedef bslalg_PassThroughTrait<
                               container_type,
                               bslalg_TypeTraitUsesBslmaAllocator>::Type Trait;
    BSLALG_DECLARE_NESTED_TRAITS(stack, Trait);

  private:
    // FRIENDS
    template <class V, class C>
    friend bool operator==(const stack<V, C>&, const stack<V, C>&);
    template <class V, class C>
    friend bool operator!=(const stack<V, C>&, const stack<V, C>&);
    template <class V, class C>
    friend bool operator< (const stack<V, C>&, const stack<V, C>&);
    template <class V, class C>
    friend bool operator> (const stack<V, C>&, const stack<V, C>&);
    template <class V, class C>
    friend bool operator<=(const stack<V, C>&, const stack<V, C>&);
    template <class V, class C>
    friend bool operator>=(const stack<V, C>&, const stack<V, C>&);

  public:
    // CREATORS
    explicit
    stack(const allocator_type& allocator = allocator_type());
        // Constuct an empty stack.  If 'allocator' is not supplied, a
        // default-constructed object of the parameterized 'allocator_type'
        // type is used.  If the template parameter 'CONTAINER::allocator_type'
        // is 'bsl::allocator' (the default) then 'allocator', if supplied, may
        // be of any type convertible to 'bslma_Allocator *', which is
        // implicitly convertible to 'bsl::allocator'.  If the template
        // parameter 'CONTAINER::allocator' is 'bsl::allocator' and 'allocator'
        // is not supplied, the currently installed bslma default allocator
        // will be used to supply memory.
    explicit
    stack(const CONTAINER&      container,
          const allocator_type& allocator = allocator_type());
        // Constuct an stack out of a copy of the specified 'container'.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // parameterized 'allocator_type' type is used.  If the template
        // parameter 'CONTAINER::allocator_type' is 'bsl::allocator' (the
        // default) then 'allocator', if supplied, may be of any type
        // convertible to 'bslma_Allocator *', which is implicitly convertible
        // to 'bsl::allocator'.  If the template parameter
        // 'CONTAINER::allocator' is 'bsl::allocator' and 'allocator' is not
        // supplied, the currently installed bslma default allocator will be
        // used to supply memory.  Copies of the objects in 'container', if
        // any, will populate the the newly-formed stack, with the element
        // accessed by 'container.back()' being the top of the stack.
    stack(const stack&          original,
          const allocator_type& allocator = allocator_type());
        // Construct a copy of the specified stack 'original', using the
        // speciffied 'allocator' to allocate memory.  If 'allocator' is not
        // supplied, a default-constructed object of the parameterized
        // 'allocator_type' type is used.  If the template parameter
        // 'CONTAINER::allocator_type' is 'bsl::allocator' (the default) then
        // 'allocator', if supplied, may be of any type convertible to
        // 'bslma_Allocator *', which is implicitly convertible to
        // 'bsl::allocator'.  If the template parameter 'CONTAINER::allocator'
        // is 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed bslma default allocator will be used to supply memory.

#ifdef BSLSTL_STACK_DO_RVALUES
    explicit
    stack(CONTAINER&&           container,
          const allocator_type& allocator = allocator_type());
        // TBD

    explicit
    stack(stack&&               original,
          const allocator_type& allocator = allocator_type());
        // TBD
#endif
    // MANIPULATORS
    stack& operator=(const stack& rhs);
#ifdef BSLSTL_STACK_DO_RVALUES
    stack& operator=(stack&& rhs);
        // TBD
#endif
#ifdef BSLSTL_STACK_DO_RVALUES
    template <class... Args>
    void emplace(Args&&... args);
        // TBD
#endif

    void pop();
        // Erase the top element from this stack.  The behavior is undefined if
        // the stack is empty.

    void push(const value_type& x);
        // Push a copy of 'x' to the top of the stack.

#ifdef BSLSTL_STACK_DO_RVALUES
    void push(value_type&& x);
        // TBD
#endif

    void swap(stack& other);
        // Swap the contents of this stack with the specified stack 'other'.

    reference top();
        // Return a reference to the element at the top of this stack.  The
        // behavior is undefined if the stack is empty.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if no objects are contained in this stack and 'false'
        // otherwise.

    allocator_type get_allocator() const;
        // Return the allocator used by this vector to supply memory.

    size_type size() const;
        // Return the number of elements contained in this stack.

    const_reference top() const;
        // Return a non-modifiable reference to the element at the top of this
        // stack.  The behavior is undefined if the stack is empty.
};

#if 0
// 'uses_allocator' and 'is_constructible' depend on the underlying 'container'
// having these constructs, which 'deque', 'vector', and 'list' don't yet.

                            // ==============
                            // uses_allocator
                            // ==============

template <class VALUE, class CONTAINER, class ALLOCATOR>
struct uses_allocator<stack<VALUE, CONTAINER>, ALLOCATOR>
: uses_allocator<CONTAINER, ALLOCATOR>::type {
    // doc TBD
};

#endif

//=============================================================================
//                              FREE OPERATORS
//=============================================================================

template <class VALUE, class CONTAINER>
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the equality comparison of the containers in 'lhs'
    // and 'rhs', respectively.

template <class VALUE, class CONTAINER>
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the inequality comparison of the containers in
    // 'lhs' and 'rhs', respectively.

template <class VALUE, class CONTAINER>
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the 'less than' comparison of the containers in
    // 'lhs' and 'rhs', respectively.

template <class VALUE, class CONTAINER>
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the 'greater than' comparison of the containers in
    // 'lhs' and 'rhs', respectively.

template <class VALUE, class CONTAINER>
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the 'less than or equal to' comparison of the
    // containers in 'lhs' and 'rhs', respectively.

template <class VALUE, class CONTAINER>
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs);
    // Return the result of the 'greater than or equal to' comparison of the
    // containers in 'lhs' and 'rhs', respectively.

template <class VALUE, class CONTAINER>
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs);
    // Swap the contents of 'lhs' and 'rhs'.  The behavior is undefined if
    // 'lhs' and 'rhs' use different memory allocators.

//=============================================================================
//                          INLINE FUNCTION DEFNITIONS
//=============================================================================

// CREATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const allocator_type& allocator)
                            
: d_container(allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const CONTAINER&      container,
                               const allocator_type& allocator)
: d_container(container, allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const stack&          original,
                               const allocator_type& allocator)
: d_container(original.d_container, allocator)
, c(d_container)
{}

#ifdef BSLSTL_STACK_DO_RVALUES

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(CONTAINER&&           container,
                               const allocator_type& allocator)
: d_container(bsl::move(container), allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(stack&&               original,
                               const allocator_type& allocator)
: d_container(bsl::move(original.d_container), allocator)
, c(d_container)
{}

#endif

// MANIPULATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>& stack<VALUE, CONTAINER>::operator=(const stack& rhs)
{
    d_container = rhs.d_container;

    return *this;
}

#ifdef BSLSTL_STACK_DO_RVALUES

template <class VALUE, class CONTAINER>
inline
stack& stack<VALUE, CONTAINER>::operator=(stack&& rhs)
{
    if (get_allocator() == rhs.get_allocator()) {
        d_container = bsl::move(rhs.d_container);
    }
    else {
        d_container = static_cast<const stack>(rhs);
    }

    return *this;
}

template <class... Args>
inline
void emplace(Args&&... args)
{
    // This is just copied from the standard.  I am not familiar with this
    // template syntax or what 'emplace' means. -- Bill Chapman 4/20/2012

    d_container.emplace_back(std::forward<Args>(args)...);
}


#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::pop()
{
    d_container.pop_back();
}

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(const value_type& x)
{
    d_container.push_back(x);
}

#ifdef BSLSTL_STACK_DO_RVALUES

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(value_type&& x)
{
    d_container.push_back(std::move(x));
}

#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::swap(stack& other)
{
    using std::swap; swap(d_container, other.d_container);
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::reference stack<VALUE, CONTAINER>::top()
{
    return d_container.back();
}

// ACCESSORS
template <class VALUE, class CONTAINER>
inline
bool stack<VALUE, CONTAINER>::empty() const
{
    return d_container.empty();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::allocator_type stack<VALUE,
                                         CONTAINER>::get_allocator() const
{
    return d_container.get_allocator();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::size_type stack<VALUE, CONTAINER>::size() const
{
    return d_container.size();
}

template <class VALUE, class CONTAINER>
inline
typename CONTAINER::const_reference stack<VALUE, CONTAINER>::top() const
{
    return d_container.back();
}

// FREE OPERATORS
template <class VALUE, class CONTAINER>
inline
bool operator==(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container == rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator!=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container != rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator< (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container < rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator> (const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container > rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator<=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container <= rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
bool operator>=(const stack<VALUE, CONTAINER>& lhs,
                const stack<VALUE, CONTAINER>& rhs)
{
    return lhs.d_container >= rhs.d_container;
}

template <class VALUE, class CONTAINER>
inline
void swap(stack<VALUE, CONTAINER>& lhs,
          stack<VALUE, CONTAINER>& rhs)
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
