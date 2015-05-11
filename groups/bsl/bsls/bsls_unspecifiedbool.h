// bsls_unspecifiedbool.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#define INCLUDED_BSLS_UNSPECIFIEDBOOL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class supporting the 'unspecified bool' idiom.
//
//@CLASSES:
//  bsls::UnspecifiedBool: class template for the 'unspecified bool' idiom.
//
//@DESCRIPTION: This component should *not* be used outside of the 'bsl'
// package at this time.
//
// This component provides a class template that can be used to manufacture an
// "unspecified boolean type" that is distinct for each class that instantiates
// it.  Note that classes supplying an implicit conversion to an unspecified
// bool type will be equality comparable (using 'operator==' and 'operator!=')
// through this conversion.  Private equality and inequality operators should
// be added to the class definition unless this comparison is desired.  It is
// important that each class produces a distinct unspecified bool type, as
// otherwise objects of different class types would compare equal through this
// same conversion.  Note that this component should become redundant when all
// of our compilers support "explicit conversion operators", a new feature of
// C++11.  An 'explicit operator bool()' conversion operator is superior to
// this C++98 idiom in all ways.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Smart Pointer
///- - - - - - - - - - - - - - - - -
// A common requirement for "smart pointer" types is to emulate the native
// pointer types and, in particular, support testing for "null" or "empty"
// pointer values as a simple boolean conversion in 'if' and 'while' clauses.
// We here demonstrate how to create a simple smart pointer type, 'SimplePtr',
// using this component to implement a safe the boolean conversion.
//
// An object of type 'SimplePtr' holds a pointer value, but does not claim
// ownership or any responsibility for the lifetime of the referenced object.
// A 'SimplePtr' object acts as a "simple" native pointer.
//
// First, we create the 'SimplePtr' class, define its data members, creators
// and manipulators:
//..
//  template <class TYPE>
//  class SimplePtr
//  {
//      // This class holds a pointer to a single object, and provides a subset
//      // of the regular pointer operators.  For example, objects of this
//      // class can be dereferenced with 'operator*' and tested as a boolean
//      // value to determine if null.  Conversely, this class does not support
//      // pointer arithmetic.
//
//    private:
//      // DATA
//      TYPE *d_ptr_p;  // address of the referenced object
//
//      // PRIVATE ACCESSORS
//      bool operator==(const SimplePtr &);  // = delete;
//      bool operator!=(const SimplePtr &);  // = delete;
//          // Suppress equality-comparison operations on objects of this
//          // class.
//
//    public:
//      // CREATORS
//      explicit SimplePtr(TYPE *ptr = 0) : d_ptr_p(ptr) {}
//          // Create a 'SimplePtr' having the value of the specified 'ptr'.
//
//      //! ~SimplePtr() = default;
//          // Destroy this object.
//
//      // ACCESSORS
//      TYPE& operator*() const  { return *d_ptr_p; }
//          // Return a reference to the object pointed to by this
//          // 'SimplePtr'.
//
//      TYPE *operator->() const { return d_ptr_p; }
//          // Return the held 'd_ptr_p'.
//..
// Next, we define, for convenience, an alias for a unique type that is
// implicitly convertible to 'bool' (note that we pass the current template
// instantiation to the 'bsls::UnspecifiedBool' template to guarantee a unique
// name, even for different instantiations of this same 'SimplePtr' template):
//..
//  // TYPES
//  typedef typename bsls::UnspecifiedBool<SimplePtr>::BoolType BoolType;
//..
// Now, we can define a boolean conversion operator that tests whether or not
// this 'SimplePtr' object is holding a null pointer, or a valid address:
//..
//      operator BoolType() const {
//          return bsls::UnspecifiedBool<SimplePtr>::makeValue(d_ptr_p);
//      }
//  }; // class SimplePtr
//..
// Note that we do not need to define 'operator!' as this single boolean
// conversion operator is invoked with the correct semantics when the user
// tries that operator.
//
// Finally, we write a simple test function, creating a couple of 'SimplePtr'
// objects, one "null", and the other with a well-defined address.
//..
//  void runTests() {
//      SimplePtr<int> p1;  // default ctor sets to null
//      assert(!p1);
//
//      int            i = 3;
//      SimplePtr<int> p2(&i);
//
//      if (p2) {
//          assert(3 == *p2);
//      }
//  }
//..
// Notice that 'SimplePtr' objects behave as native pointers.  They should be
// tested before dereferencing (as they could be null).

namespace BloombergLP {

namespace bsls {

                         // =====================
                         // class UnspecifiedBool
                         // =====================

template<class BSLS_HOST_TYPE>
class UnspecifiedBool {
    // This class provides a member, 'd_member', whose pointer-to-member may be
    // used as an "unspecified boolean type" for implicit conversion operators.

  private:
    // DATA
    int d_member;   // This data member is used solely for taking its address
                    // to return a non-null pointer-to-member.  Note that the
                    // *value* of 'd_member' is not used.

  public:
    // TYPES
    typedef int UnspecifiedBool::* BoolType;
        // Alias of a distinct type that is implicitly convertible to 'bool',
        // but does not promote to 'int'.

    // CLASS METHODS
    static BoolType falseValue();
        // Return a value that converts to the 'bool' value 'false'.

    static BoolType trueValue();
        // Return a value that converts to the 'bool' value 'true'.

    static BoolType makeValue(bool predicate);
        // Return a value that converts to the 'bool' value 'true' if the
        // specified predicate is 'true', and the 'bool' value 'false'
        // otherwise.
};


// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
template<class BSLS_HOST_TYPE>
inline
typename UnspecifiedBool<BSLS_HOST_TYPE>::BoolType
UnspecifiedBool<BSLS_HOST_TYPE>::falseValue()
{
    return 0;
}

template<class BSLS_HOST_TYPE>
inline
typename UnspecifiedBool<BSLS_HOST_TYPE>::BoolType
UnspecifiedBool<BSLS_HOST_TYPE>::trueValue()
{
    return &UnspecifiedBool::d_member;
}

template<class BSLS_HOST_TYPE>
inline
typename UnspecifiedBool<BSLS_HOST_TYPE>::BoolType
UnspecifiedBool<BSLS_HOST_TYPE>::makeValue(bool predicate)
{
    return predicate ? trueValue() : falseValue();
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
