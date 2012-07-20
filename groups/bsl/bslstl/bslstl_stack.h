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
//@DESCRIPTION: This component describes a single class template
// 'bslstl::stack', a container adapter that takes an underlying container and
// provides a stack interface which the user accesses primarily through 'push',
// 'pop', and 'top' operations.  A 'deque' (the default), 'vector', or 'list'
// may be used, but any container which supports 'push_back', 'pop_back',
// 'back', and 'size', plus a template specialization 'uses_allocator::type',
// may be used.
//
///Requirements of Parametrized 'CONTAINER' Type
///---------------------------------------------
// This class works with the container classes 'bsl::deque', 'bsl::vector', or
// 'bsl::list' passed to the 'CONTAINER' template class.  Another container
// class could be passed, but it must satisfy the following requirements:
//: o It must have the following public types:
//:   o value_type
//:   o reference
//:   o const_reference
//:   o size_type
//: o It must support the following methods, depending on what method of
//:   'stack' are used:
//:   o void push_back(const value_type&)
//:   o void pop_back()
//:   o value_type& back()
//:   o size_type size()
//:   o '==', '!=', '<', '>', '<=', '>='
//:   o 'operator='
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
// allocator type used is the allocator specified for the container class.
// Some functions of this template only exist if type
// 'CONTAINER::allocator_type' exists, and if it does exist it is assumed to be
// the allocator type used by 'CONTAINER', and that 'CONTAINER' supports
// constructors of this type.
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
//..
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1:
/// - - - - - - - - - - - - - - - -
// In this example, we demonstrate the basic functionality of the 'stack'
// container adapter.
//
// First, we create a couple of allocators, the test allocator 'ta' and the
// default allocator 'da'.
//..
//  bslma_TestAllocator ta;
//  bslma_TestAllocator da;                    // Default Allocator
//  bslma_DefaultAllocatorGuard guard(&da);
//..
// Then, we create a stack of strings:
//..
//  stack<string> mX(&ta);        const stack<string>& X = mX;
//..
// Next, we observe that the newly created stack is empty.
//..
//  assert(X.empty());
//  assert(0 == X.size());
//..
// Then, we push a few strings onto the stack.  After each push, the last thing
// pushed is what's on top of the stack:
//..
//  mX.push("woof");
//  assert(X.top() == "woof");
//  mX.push("arf");
//  assert(X.top() == "arf");
//  mX.push("meow");
//  assert(X.top() == "meow");
//..
// Next, we verify that we have 3 objects in the stack:
//..
//  assert(3 == X.size());
//  assert(!X.empty());
//..
// Then, we pop an item off the stack and see that the item pushed before it is
// now on top of the stack:
//..
//  mX.pop();
//  assert(X.top() == "arf");
//..
// Next, we create a long string (long enough that taking a copy of it will
// require memory allocation).  Note that 's' uses the default allocator.
//..
//  const string s("supercalifragisticexpialidocious"
//                 "supercalifragisticexpialidocious"
//                 "supercalifragisticexpialidocious"
//                 "supercalifragisticexpialidocious");
//..
// Then, we monitor both memory allocators:
//..
//  bslma_TestAllocatorMonitor tam(&ta);
//  bslma_TestAllocatorMonitor dam(&da);
//..
// Now, we push the large string onto the stack:
//..
//  mX.push(s);
//  assert(s == X.top());
//..
// Finally, we observe that the memory allocated to store the large string in
// the stack came from the allocator passed to the stack at construction, and
// not from the default allocator.
//..
//  assert(tam.isTotalUp());
//  assert(dam.isTotalSame());
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_stack.h> instead of <bslstl_stack.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

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

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
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
    // 'CONTAINER'.  We use 'CONTAINER::value_type' for everything, which means
    // that if 'CONTAINER' is specified, then 'VALUE' is ignored.

    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

  private:
    // PRIVATE DATA
    container_type  d_container;    // container in which objects are stored

  protected:
    // PROTECTED DATA
    container_type& c;    // We are required by the standard to have the
                          // container be a protected variable named 'c'.  Just
                          // a reference to 'd_container'.

  public:
    // TRAITS
    typedef typename BloombergLP::bslalg::PassthroughTrait<
                 container_type,
                 BloombergLP::bslalg::TypeTraitUsesBslmaAllocator>::Type Trait;
    BSLALG_DECLARE_NESTED_TRAITS(stack, Trait);

  private:
    // FRIENDS
    template <class VAL, class CONT>
    friend bool operator==(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator!=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator< (const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator> (const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator<=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);
    template <class VAL, class CONT>
    friend bool operator>=(const stack<VAL, CONT>&, const stack<VAL, CONT>&);

  public:
    // CREATORS
    stack();

    template <class ALLOCATOR>
    explicit stack(const ALLOCATOR& allocator,
                   typename BloombergLP::bslmf::EnableIf<
                       BloombergLP::bslmf::IsConvertible<
                           ALLOCATOR,
                           typename CONTAINER::allocator_type>::VALUE>
                       ::type * = 0);
        // Construct an empty stack.  If 'allocator' is not supplied, a
        // default-constructed object of the parameter-derived 'allocator_type'
        // type is used.  If the template parameter 'CONTAINER::allocator_type'
        // is 'bsl::allocator' (the default) then 'allocator', if supplied, may
        // be of any type convertible to 'bslma_Allocator *', which is
        // implicitly convertible to 'bsl::allocator'.  If 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed bslma default allocator will be used to supply memory.

    explicit
    stack(const CONTAINER& container);

    template <class ALLOCATOR>
    stack(const CONTAINER& container,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf::EnableIf<
              BloombergLP::bslmf::IsConvertible<
                  ALLOCATOR,
                  typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // Construct an stack out of a copy of the specified 'container'.  If
        // 'allocator' is not supplied, a default-constructed object of the
        // parameter-derived 'allocator_type' type is used.  If
        // 'allocator_type' is 'bsl::allocator' (the default) then 'allocator',
        // if supplied, may be of any type convertible to 'bslma_Allocator *',
        // which is implicitly convertible to 'bsl::allocator'.  If
        // 'allocator_type' is 'bsl::allocator' and 'allocator' is not
        // supplied, the currently installed bslma default allocator will be
        // used to supply memory.  Copies of the objects in 'container', if
        // any, will populate the the created stack, with the element accessed
        // by 'container.back()' being the top of the stack.

    stack(const stack&     original);

    template <class ALLOCATOR>
    stack(const stack&     original,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf::EnableIf<
              BloombergLP::bslmf::IsConvertible<
                  ALLOCATOR,
                  typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // Construct a copy of the specified stack 'original', using the
        // specified 'allocator' to allocate memory.  If 'allocator' is not
        // supplied, a default-constructed object of the parameter-derived
        // 'allocator_type' type is used.  If 'allocator_type' is
        // 'bsl::allocator' (the default) then 'allocator', if supplied, may be
        // of any type convertible to 'bslma_Allocator *', which is implicitly
        // convertible to 'bsl::allocator'.  If 'allocator_type' is
        // 'bsl::allocator' and 'allocator' is not supplied, the currently
        // installed bslma default allocator will be used to supply memory.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    explicit
    stack(CONTAINER&&                               container);

    template <class ALLOCATOR>
    stack(CONTAINER&&      container,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf::EnableIf<
              BloombergLP::bslmf::IsConvertible<
                  ALLOCATOR,
                  typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // TBD

    explicit
    stack(stack&&          original);

        // TBD
    stack(stack&&          original,
          const ALLOCATOR& allocator,
          typename BloombergLP::bslmf::EnableIf<
              BloombergLP::bslmf::IsConvertible<
                  ALLOCATOR,
                  typename CONTAINER::allocator_type>::VALUE>
              ::type * = 0);
        // TBD
#endif
    // MANIPULATORS
    stack& operator=(const stack& rhs);
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
    template <class... Args>
    void emplace(Args&&... args);
        // TBD
#endif

    void pop();
        // Erase the top element from this stack.  The behavior is undefined if
        // the stack is empty.

    void push(const value_type& value);
        // Push a copy of 'x' to the top of the stack.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    void push(value_type&& value);
        // TBD
#endif

    void swap(stack& other);
        // Swap the contents of this stack with the specified stack 'other'.

    reference top();
        // Return a reference to the element at the top of this stack.  The
        // behavior is undefined if the stack is empty.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this stack contains no elements and 'false'
        // otherwise.

    size_type size() const;
        // Return the number of elements contained in this stack.

    const_reference top() const;
        // Return a reference providing non-modifiable access to the element at
        // the top of this stack.  The behavior is undefined if the stack is
        // empty.
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
    // Swap the contents of 'lhs' and 'rhs'.  The behavior is undefined unless
    // 'lhs' and 'rhs' use the same allocator.

//=============================================================================
//                          INLINE FUNCTION DEFINITIONS
//=============================================================================

// CREATORS
template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack()
: d_container()
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(const ALLOCATOR& allocator,
                               typename BloombergLP::bslmf::EnableIf<
                                   BloombergLP::bslmf::IsConvertible<
                                       ALLOCATOR,
                                       typename CONTAINER::allocator_type>
                                       ::VALUE>
                                   ::type *)
: d_container(allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const CONTAINER& container)
: d_container(container)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           const CONTAINER& container,
                           const ALLOCATOR& allocator,
                               typename BloombergLP::bslmf::EnableIf<
                                   BloombergLP::bslmf::IsConvertible<
                                       ALLOCATOR,
                                       typename CONTAINER::allocator_type>
                                       ::VALUE>
                                   ::type *)
: d_container(container, allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(const stack& original)
: d_container(original.d_container)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           const stack&     original,
                           const ALLOCATOR& allocator,
                           typename BloombergLP::bslmf::EnableIf<
                               BloombergLP::bslmf::IsConvertible<
                                   ALLOCATOR,
                                   typename CONTAINER::allocator_type>
                                   ::VALUE>
                               ::type *)
: d_container(original.d_container, allocator)
, c(d_container)
{}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(CONTAINER&&           container)
: d_container(bsl::move(container))
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           CONTAINER&&                               container,
                           const ALLOCATOR& allocator,
                           typename BloombergLP::bslmf::EnableIf<
                               BloombergLP::bslmf::IsConvertible<
                                   ALLOCATOR,
                                   typename CONTAINER::allocator_type>
                                   ::VALUE>
                               ::type *)
: d_container(bsl::move(container), allocator)
, c(d_container)
{}

template <class VALUE, class CONTAINER>
inline
stack<VALUE, CONTAINER>::stack(stack&&               original)
: d_container(bsl::move(original.d_container))
, c(d_container)
{}

template <class VALUE, class CONTAINER>
template <class ALLOCATOR>
inline
stack<VALUE, CONTAINER>::stack(
                           stack&&          original,
                           const ALLOCATOR& allocator,
                           typename BloombergLP::bslmf::EnableIf<
                               BloombergLP::bslmf::IsConvertible<
                                   ALLOCATOR,
                                   typename CONTAINER::allocator_type>
                                   ::VALUE>
                               ::type *)
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)

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
void stack<VALUE, CONTAINER>::push(const value_type& value)
{
    d_container.push_back(value);
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::push(value_type&& value)
{
    d_container.push_back(std::move(value));
}

#endif

template <class VALUE, class CONTAINER>
inline
void stack<VALUE, CONTAINER>::swap(stack& other)
{
    BloombergLP::bslalg::SwapUtil::swap(&d_container, &other.d_container);
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
    return 0 == d_container.size();
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
