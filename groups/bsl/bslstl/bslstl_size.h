// bslstl_size.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_SIZE
#define INCLUDED_BSLSTL_SIZE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'size' and 'ssize' free functions.
//
//@FUNCTIONS:
//  size_t bsl::size(<array>)
//  size_t bsl::size(<container>)
//  ptrdiff_t bslstl::ssize(<array>)
//  ptrdiff_t bslstl::ssize(<container>)
//
//@DESCRIPTION: This component provides the free functions 'size' and 'ssize',
// two overloads of each one, the first to take a container that has a 'size'
// accessor, and the second to take an array, in both cases to return the
// number of elements in the array or container.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A 'Stack' class
/// - - - - - - - - - - - - -
// Suppose we want to create a 'Stack' template class that contains a
// fixed-length container whose elements can be accessed via 'operator[]'.  The
// contained container could be a 'vector', 'deque', 'std::array', or a raw
// array.
//
// For the 'vector', 'deque', and 'std::array', we can call
// '<container>.size()' to the get the capacity of the fixed-length container,
// but that won't work in the case of a raw array, so we use 'bsl::size'.
//
// First, we we declare the 'class':
//..
//  template <class CONTAINER, class ELEMENT = typename CONTAINER::value_type>
//  class Stack {
//      // TYPES
//      typedef ELEMENT value_type;
//
//      // DATA
//      CONTAINER      d_container;
//      std::size_t    d_numElements;
//
//    public:
//      // CREATORS
//      Stack();
//          // Create a 'Stack'.  Use this constructor when the type of
//          // 'CONTAINER' default-constructs to the desired length.
//
//      explicit
//      Stack(std::size_t initialSize);
//          // Create a 'Stack'.  Use this constructor when the type of
//          // 'CONTAINER' default constructs to 0 length, and has a
//          // single-argument constructor that takes a 'size_t' to grow it to
//          // the specified 'initialSize'.
//
//      Stack(const Stack& original);
//          // Copy this 'Stack' from the specified 'original'.
//
//      // ~Stack() = default;
//
//      // MANIPULATORS
//      void push(const value_type& value);
//          // Push the specified 'value' onto the stack.  The behavior is
//          // undefined if the stack is full.
//
//      value_type pop();
//          // Pop the value from the top of the stack and return it.  The
//          // behavior is undefined if the stack is empty.
//
//      // ACCESSOR
//      std::size_t size() const;
//          // Return the number of elements stored in the stack.
//
//      const value_type& top() const;
//          // Return a reference to the object at the top of the stack.
//
//      BSLS_KEYWORD_CONSTEXPR std::size_t capacity() const;
//          // Return the capacity of the stack.
//  };
//..
// Next, we declare all the methods other than 'capacity()':
//..
//  // CREATORS
//  template <class CONTAINER, class ELEMENT>
//  Stack<CONTAINER, ELEMENT>::Stack()
//  : d_numElements(0)
//  {}
//
//  template <class CONTAINER, class ELEMENT>
//  Stack<CONTAINER, ELEMENT>::Stack(std::size_t initialSize)
//  : d_container(initialSize)
//  , d_numElements(0)
//  {}
//
//  template <class CONTAINER, class ELEMENT>
//  Stack<CONTAINER, ELEMENT>::Stack(const Stack& original)
//  : d_numElements(0)
//  {
//      for (std::size_t uu = 0; uu < original.size(); ++uu) {
//          d_container[uu] = original.d_container[uu];
//      }
//
//      d_numElements = original.d_numElements;
//  }
//
//  // MANIPULATORS
//  template <class CONTAINER, class ELEMENT>
//  void Stack<CONTAINER, ELEMENT>::push(const value_type& value)
//  {
//      assert(d_numElements < this->capacity());
//
//      d_container[d_numElements++] = value;
//  }
//
//  template <class CONTAINER, class ELEMENT>
//  typename Stack<CONTAINER, ELEMENT>::value_type
//  Stack<CONTAINER, ELEMENT>::pop()
//  {
//      assert(0 < d_numElements);
//
//      return d_container[--d_numElements];
//  }
//
//  // ACCESSORS
//  template <class CONTAINER, class ELEMENT>
//  std::size_t Stack<CONTAINER, ELEMENT>::size() const
//  {
//      return d_numElements;
//  }
//
//  template <class CONTAINER, class ELEMENT>
//  const typename Stack<CONTAINER, ELEMENT>::value_type&
//  Stack<CONTAINER, ELEMENT>::top() const
//  {
//      assert(0 < d_numElements);
//
//      return d_container[d_numElements - 1];
//  }
//..
// Now, we declare 'capacity' and use 'bsl::size' to get the length of the
// owned 'd_container'.
//
//  template <class CONTAINER, class ELEMENT>
//  inline
//  BSLS_KEYWORD_CONSTEXPR std::size_t
//  Stack<CONTAINER, ELEMENT>::capacity() const
//  {
//      return bsl::size(d_container);
//  }
//..
// Finally, in 'main', we use our new class based on several types of owned
// 'CONTAINER's: raw array, 'bsl::vector', 'bsl::deque', and 'bsl::array'.
//..
//  typedef int Array[10];
//  Stack<Array, int> aStack;
//
//  assert(aStack.capacity() == 10);
//  assert(aStack.size() == 0);
//
//  aStack.push(5);
//  aStack.push(7);
//  aStack.push(2);
//
//  assert(aStack.size() == 3);
//
//  assert(2 == aStack.top());
//  assert(2 == aStack.pop());
//
//  Stack<bsl::vector<int> > vStack(10);
//
//  assert(vStack.capacity() == 10);
//  assert(vStack.size() == 0);
//
//  vStack.push(5);
//  vStack.push(7);
//  vStack.push(2);
//
//  assert(vStack.size() == 3);
//
//  assert(2 == vStack.top());
//  assert(2 == vStack.pop());
//
//  Stack<bsl::deque<int> > dStack(10);
//
//  assert(dStack.capacity() == 10);
//  assert(dStack.size() == 0);
//
//  dStack.push(5);
//  dStack.push(7);
//  dStack.push(2);
//
//  assert(dStack.size() == 3);
//
//  assert(2 == dStack.top());
//  assert(2 == dStack.pop());
//
//  Stack<bsl::array<int, 10> > baStack;
//
//  assert(baStack.capacity() == 10);
//  assert(baStack.size() == 0);
//
//  baStack.push(5);
//  baStack.push(7);
//  baStack.push(2);
//
//  assert(baStack.size() == 3);
//
//  assert(2 == baStack.top());
//  assert(2 == baStack.pop());
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#if 201703L <= BSLS_COMPILERFEATURES_CPLUSPLUS
# define BSLSTL_SIZE_SIZE_NATIVE 1
#else
# define BSLSTL_SIZE_SIZE_NATIVE 0
#endif

// We have observed that clang-10.0.0 does not support 'std::ssize' in C++20,
// and we are speculating that later releases of the compiler will support it.

#if 202002L <= BSLS_COMPILERFEATURES_CPLUSPLUS &&                             \
    (!defined(BSLS_PLATFORM_CMP_CLANG) ||                                     \
        (BSLS_PLATFORM_CMP_CLANG && 100000 < BSLS_PLATFORM_CMP_VERSION))
# define BSLSTL_SIZE_SSIZE_NATIVE 1
#else
# define BSLSTL_SIZE_SSIZE_NATIVE 0
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)    &&                    \
    201103L <= BSLS_COMPILERFEATURES_SUPPORT_CPLUSPLUS
# define BSLSTL_SIZE_SSIZE_ADVANCED_IMPL 1
#else
# define BSLSTL_SIZE_SSIZE_ADVANCED_IMPL 0
#endif

#if BSLSTL_SIZE_SIZE_NATIVE
# include <iterator>    // 'size' in C++17, 'ssize' in C++20
#endif

#if !BSLSTL_SIZE_SSIZE_NATIVE
# include <bsls_keyword.h>
# include <cstddef>          // 'size_t', 'ptrdiff_t'
# if BSLSTL_SIZE_SSIZE_ADVANCED_IMPL
#   include <type_traits>    // 'common_type', 'make_signed'
# endif
#endif

namespace bsl {

                                    // ====
                                    // size
                                    // ====

#if BSLSTL_SIZE_SIZE_NATIVE

using std::size;

#else

# if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR auto size(const CONTAINER& container) ->
                                                     decltype(container.size())
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# else

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR std::size_t size(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# endif

template <class TYPE, std::size_t DIMENSION>
inline
BSLS_KEYWORD_CONSTEXPR std::size_t size(const TYPE (&)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    return DIMENSION;
}

#endif

                                    // =====
                                    // ssize
                                    // =====

#if BSLSTL_SIZE_SSIZE_NATIVE

using std::ssize;

#else

# if BSLSTL_SIZE_SSIZE_ADVANCED_IMPL

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR auto ssize(const CONTAINER& container) ->
             std::common_type_t<std::ptrdiff_t,
                                std::make_signed_t<decltype(container.size())>>
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# else

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR std::ptrdiff_t ssize(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# endif

template <class TYPE, std::ptrdiff_t DIMENSION>
inline
BSLS_KEYWORD_CONSTEXPR std::ptrdiff_t ssize(const TYPE (&)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    return DIMENSION;
}

#endif

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
