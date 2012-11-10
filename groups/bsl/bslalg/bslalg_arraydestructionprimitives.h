// bslalg_arraydestructionprimitives.h                                -*-C++-*-
#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#define INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that destroy arrays.
//
//@CLASSES:
//  bslalg::ArrayDestructionPrimitives: namespace for array algorithms
//
//@SEE_ALSO: bslalg_scalarprimitives, bslalg_typetraits
//
//@DESCRIPTION: This component provides utilies to destroy arrays with a
// uniform interface, but selecting a different implementation according to the
// traits possessed by the underlying type.
//
// The traits under consideration by this component are:
//..
//  Trait                             Note
//  -----                             -------------------------------------
//   bsl::is_trivially_copyable       Expressed in English as "TYPE has the
//                                    bit-wise copyable trait", or "TYPE is
//                                    bit-wise copyable", this trait also
//                                    implies that destructor calls can be
//                                    elided with no effect on observable
//                                    behavior.
//
//..
///Usage
///-----
// In this section we show intended use of this component.  Note that this
// component is for use by the 'bslstl' package.  Other clients should use the
// STL algorithms (in header '<algorithm>' and '<memory>').
//
///Example 1: Destroy Arrays of 'int' and 'Integer' Wrapper Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use 'bslalg::ArrayDestructionPrimitives' to destroy
// both an array of integer scalars and an array of 'MyInteger' objects.
// Calling the 'destroy' method on an array of integers is a no-op while
// calling the 'destroy' method on an array of objects of 'MyInteger' class
// invokes the destructor of each of the objects in the array.
//
// First, we define a 'MyInteger' class that contains an integer value:
//..
//  class MyInteger {
//      // This class represents an integer value.
//
//      int d_intValue;  // integer value
//
//    public:
//      // CREATORS
//      MyInteger();
//          // Create a 'MyInteger' object having integer value '0'.
//
//      explicit MyInteger(int value);
//          // Create a 'MyInteger' object having the specified 'value'.
//
//      ~MyInteger();
//          // Destroy this object.
//
//      // ACCESSORS
//      int getValue() const;
//          // Return the integer value contained in this object.
//  };
//..
// Then, we create an array of of objects, 'myIntegers', of type 'MyInteger'
// (note that we 'bsls::ObjectBuffer' to allow us to safely invoke the
// destructor explicitly):
//..
//  bsls::ObjectBuffer<MyInteger> arrayBuffer[5];
//  MyInteger *myIntegers = &arrayBuffer[0].object();
//  for (int i = 0;i < 5; ++i) {
//      new (myIntegers + i) MyInteger(i);
//  }
//..
// Now, we define a primitive integer array:
//..
//  int scalarIntegers[] = { 0, 1, 2, 3, 4 };
//..
// Finally, we use the uniform 'bslalg::ArrayDestructionPrimitives:destroy'
// method to destroy both 'myIntegers' and 'scalarIntegers':
//..
//  bslalg::ArrayDestructionPrimitives::destroy(myIntegers, myIntegers + 5);
//  bslalg::ArrayDestructionPrimitives::destroy(scalarIntegers,
//                                              scalarIntegers + 5);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // size_t
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>  // memset, memcpy, memmove
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

namespace bslalg {

                  // =================================
                  // struct ArrayDestructionPrimitives
                  // =================================

struct ArrayDestructionPrimitives {
    // This 'struct' provides a namespace for a suite of utility functions that
    // destroy arrays of elements of the parameterized type 'TARGET_TYPE'.
    // Depending on the traits of 'TARGET_TYPE', the destructor may be invoked,
    // or not (optimized away by no-op).

    // PRIVATE CLASS METHODS
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE        *begin,
                        TARGET_TYPE        *end,
                        bsl::true_type);
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE        *begin,
                        TARGET_TYPE        *end,
                        bsl::false_type);
        // Destroy each instance of 'TARGET_TYPE' in the array beginning at the
        // specified 'begin' address and ending immediately before the
        // specified 'end' address.  Use the destructor of the parameterized
        // 'TARGET_TYPE', or do nothing if the 'TARGET_TYPE' is bit-wise
        // copyable (i.e., if the last argument is of type
        // 'bsl::true_type').  Note that the last argument is for
        // overloading resolution only and its value is ignored.

  public:
    // CLASS METHODS
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *begin, TARGET_TYPE *end);
        // Destroy of the elements in the segment of an array of parameterized
        // 'TARGET_TYPE' beginning at the specified 'begin' address and ending
        // immediately before the specified 'end' address, as if by calling the
        // destructor on each object.  If 'begin == 0' and 'end == 0' this
        // function has no effect.  The behavior is undefined unless either (1)
        // 'begin <= end', 'begin != 0', and 'end != 0', or (2)
        // 'begin == 0 && end == 0'.  Note that this method does not deallocate
        // any memory (except memory deallocated by the element destructor
        // calls).
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                  // ---------------------------------
                  // struct ArrayDestructionPrimitives
                  // ---------------------------------

// PRIVATE CLASS METHODS
template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE       *begin,
                                         TARGET_TYPE       *end,
                                         bsl::true_type)
{
    // 'BitwiseCopyable' is a valid surrogate for 'HasTrivialDestructor'.

#ifdef BDE_BUILD_TARGET_SAFE
    bsls::Types::size_type numBytes = (const char*)end - (const char*)begin;
    std::memset(begin, 0xa5, numBytes);
#else
    (void)begin;
    (void)end;
#endif
}

template <class TARGET_TYPE>
void ArrayDestructionPrimitives::destroy(TARGET_TYPE       *begin,
                                         TARGET_TYPE       *end,
                                         bsl::false_type)
{
    for (; begin != end; ++begin) {
        begin->~TARGET_TYPE();
    }
}

// CLASS METHODS
template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end)
{
    BSLS_ASSERT_SAFE(begin || !end);
    BSLS_ASSERT_SAFE(end   || !begin);
    BSLS_ASSERT_SAFE(begin <= end);

    destroy(begin, end,
            typename bsl::is_trivially_copyable<TARGET_TYPE>::type());
}

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
