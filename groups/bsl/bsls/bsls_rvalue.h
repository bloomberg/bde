// bsls_rvalue.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLS_RVALUE
#define INCLUDED_BSLS_RVALUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a vocabulary type to enable move semantics.
//
//@CLASSES:
//  bsls::Rvalue: a template indicating that an object can be moved from
//  bsls::RvalueUtil: a namespace to hold utility functions for r-values
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a template, 'bsls::Rvalue' used to
// convey the information that an object will not be used anymore and its
// representation can be transferred elsewhere and a utilities struct
// 'bsls::RvalueUtil'. In C++11 terminology an object represented by a
// 'bsl::Rvalue<T>' can be moved from. With a C++11 implementation
// 'bsls::Rvalue<T>' is an alias template for 'T&&'. With a C++03
// implementation 'bsls::Rvalue<T>' is a class template providing lvalue access
// to an object whose representation can be transferred. The objective of this
// component is to provide a name for the concept of a movable object. Using a
// common name enables use of manual move semantics when using C++03. With
// C++11 automatic move semantics is enabled when moving objects known to the
// compiler to go out of scope.
//
// Using 'bsls::Rvalue<T>' to support movable type allows implementation of
// move semantics working with both C++03 and C++11 without conditional
// compilation of the user code. Only the implementation of the component
// bsls_rvalue uses conditional compilation to enable the appropriate
// implementation choice.
//
// For a consistent use across different versions of the C++ standard a few
// utility functions are provided in the utility class 'bsls::RvalueUtil'. This
// class contains functions for moving and forwarding objects. To use an
// identical notation to access an object with C++11 where 'bsls::Rvalue<T>' is
// just an lvalue of type 'T' and with C++03 where 'bsls::Rvalue<T>' is a class
// type referencing to an lvalue of type 'T', a function template
// 'bsls::RvalueUtil<T>::access(r)' is provided.
//
///Usage
///-----
// There are two sides of move semantics:
//  1. Classes or class templates that are _move-enabled_, i.e., which can
//     transfer their internal representation to another object in some
//     situations. To become move-enabled a class needs to implement, at
//     least, a move constructor. It should probably also implement a move
//     assignment.
//  2. Users of a potentially move-enabled class may take advantage of moving
//     objects by explicitly indicating that ownership of resources may be
//     transferred. When using C++11 the compiler can automatically detect
//     some situations where it is safe to move objects but this features is
//     not available with C++03.
//
// The usage example below demonstrate both use cases using a simplified
// version of 'std::vector<T>'. The class template is simplified to concentrate
// on the aspects relevant to 'bsls::Rvalue<T>'. Most of the operations are
// just normal implementations to create a container. The last two operations
// described are using move operations.
//
// The definition of the 'vector<TYPE>' class template is rather straight
// forward. For simplicity a few trivial operations are implemented directly
// in the class definition:
//..
//  template <class TYPE>
//  class vector
//  {
//      TYPE *d_begin;
//      TYPE *d_end;
//      TYPE *d_capacity;
//
//      static void copy(const TYPE *it, const TYPE *end, vector<TYPE> *to);
//          // This auxiliary function copies the range specified by 'it' and
//          // 'end' to vector specified by 'to'. The capacity of 'to' has to
//          // be at least 'end - it'.
//      static void swap(TYPE*& a, TYPE*& b);
//          // This auxiliary function swaps the specified pointers 'a' and
//          // 'b'.
//    public:
//      vector();
//          // Create an empty vector.
//      explicit vector(bsls::Rvalue<vector<TYPE> > other);
//          // Create a vector by transfering the content of the specified
//          // 'other'.
//      vector(const vector<TYPE>& other);
//          // Create a vector by copying the content of the specified 'other'.
//      vector& operator= (vector<TYPE> other);
//          // Assign a vector by copying the content of the specified 'other'.
//          // The function returns a reference to the object.
//      ~vector();
//          // Destroy the vector's elements and release any allocated memory.
//
//      TYPE&       operator[](int index)      { return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//      const TYPE& operator[](int index) const{ return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//      TYPE       *begin()       { return this->d_begin; }
//          // Return a pointer to the first element.
//      const TYPE *begin() const { return this->d_begin; }
//          // Return a pointer to the first element.
//      int capacity() const { return int(this->d_capacity - this->d_begin); }
//          // Return the capacity of the vector.
//      TYPE       *end()       { return this->d_end; }
//          // Return a pointer to the end of the range.
//      const TYPE *end() const { return this->d_end; }
//          // Return a pointer to the end of the range.
//
//      void push_back(const TYPE& value);
//          // Append a copy of the specified 'value' to the vector.
//      void push_back(bsls::Rvalue<TYPE> value);
//          // Append an object moving the specified 'value' to the new
//          // location.
//      void reserve(int newCapacity);
//          // Reserve enough capacity to fit at least as many elements as
//          // specified by 'newCapacity'.
//      int size() const { return int(this->d_end - this->d_begin); }
//          // Return the size of the object.
//      void swap(vector<TYPE>& other);
//          // Swap the content of the vector with the specified 'other'.
//  };
//..
// The class stores pointers to the begin and the end of the elements as well
// as a pointer to the current capacity. If there are no elements, null
// pointers are stored. There a number of accessors similar to the accessors
// used by 'std::vector<TYPE>'.
//
// The default constructor creates an empty 'vector<TYPE>' by simply
// initializing all member pointers to be null pointers:
//..
//  template <class TYPE>
//  vector<TYPE>::vector()
//      : d_begin()
//      , d_end()
//      , d_capacity() {
//  }
//..
// To leverage already implemented functionality some of the member functions
// operate on a temporary 'vector<TYPE>' and move the result into place using
// the 'swap()' member function that simply does a memberwise 'swap()' (the
// function swapping pointers is implemented here to avoid any dependency on
// functions defined in another level):
//..
//  template <class TYPE>
//  void vector<TYPE>::swap(TYPE*& a, TYPE*& b) {
//      TYPE *tmp = a;
//      a = b;
//      b = tmp;
//  }
//  template <class TYPE>
//  void vector<TYPE>::swap(vector<TYPE>& other) {
//      this->swap(this->d_begin, other.d_begin);
//      this->swap(this->d_end, other.d_end);
//      this->swap(this->d_capacity, other.d_capacity);
//  }
//..
// The copy constructor and the 'reserve()' function need to copy the elements
// in the 'vector<TYPE>'. They do so using the auxiliary function 'copy()'
// which should really use 'std::copy()' and 'std::back_inserter()' but doesn't
// to avoid issues with levelization:
//..
//  template <class TYPE>
//  void vector<TYPE>::copy(const TYPE *it, const TYPE *end, vector<TYPE> *to)
//  {
//      for (; it != end; ++it) {
//          new (to->d_end++) TYPE(*it);
//      }
//  }
//..
// This function template simply iterates over the range specified by 'it' and
// 'end' and constructs a copyt of the specified element at the 'd_end' of the
// destination 'to'. The 'copy()' function requires that 'to' has enough
// capacity to store the elements in the range from 'it' to 'end'.
//
// The member function 'reserve()' arranges for the 'vector<TYPE>' to have
// enough capacity for the number of elements specified as argument. If the
// 'vector<TYPE>' is empty all it needs to do is to allocate enough memory and
// set up the pointers 'd_begin', 'd_end', and 'd_capacity'. If the vector is
// not empty, it first creates an empty 'vector<TYPE>' and uses 'reserve()' on
// this temporary 'vector<TYPE>'. The function then uses 'copy()' to copy
// elements followed by 'swap()' to put the result into place:
//..
//  template <class TYPE>
//  void vector<TYPE>::reserve(int newCapacity) {
//      if (this->capacity() < newCapacity) {
//          if (this->d_begin) {
//              vector<TYPE> tmp;
//              tmp.reserve(newCapacity);
//              this->copy(this->begin(), this->end(), &tmp);
//              this->swap(tmp);
//          }
//          else {
//              int size = int(sizeof(TYPE) * newCapacity);
//              this->d_begin = static_cast<TYPE*>(operator new[](size));
//              this->d_end = this->d_begin;
//              this->d_capacity = this->d_begin + newCapacity;
//          }
//      }
//  }
//..
// Any allocated data and construct elements need to be release in the
// destructor that works its way from back to front:
//..
//  template <class TYPE>
//  vector<TYPE>::~vector() {
//      if (this->d_begin) {
//          while (this->d_begin != this->d_end--) {
//              this->d_end->~TYPE();
//          }
//          operator delete[](this->d_begin);
//      }
//  }
//..
// Using 'reserve()' and 'copy()' it is straight forward to implement the copy
// constructor: initialize the member pointers to null, call 'reserve()' to
// create sufficient capacity, and finally call 'copy()' with the source range
// and the object under construction:
//..
//  template <class TYPE>
//  vector<TYPE>::vector(const vector<TYPE>& other)
//      : d_begin()
//      , d_end()
//      , d_capacity() {
//      this->reserve(4 < other.size()? other.size(): 4);
//      this->copy(other.begin(), other.end(), this);
//  }
//..
// A simple copy assignment operator can be implemented in terms of copy
// constructor, 'swap()', and destructor (in a real implementaiton the copy
// assignment would probably try to use already allocated objects):
//..
//  template <class TYPE>
//  vector<TYPE>& vector<TYPE>::operator= (vector<TYPE> other) {
//      this->swap(other);
//      return *this;
//  }
//..
// To complete the normal C++03 operations of 'vector<TYPE>' the only remaining
// member function is 'push_back()'. This function calls 'reserve()' to obtain
// more capacity if the current capacity is filled and then constructs the new
// element at the location pointed to by 'd_end':
//..
//  template <class TYPE>
//  void vector<TYPE>::push_back(const TYPE& value) {
//      if (this->d_end == this->d_capacity) {
//          this->reserve(this->size()? int(1.5 * this->size()): 4);
//      }
//      new(this->d_end++) TYPE(value);
//  }
//..
// The first operation actually demonstrating the use of 'Rvalue<TYPE>' is the
// move constructor:
//..
//  template <class TYPE>
//  vector<TYPE>::vector(bsls::Rvalue<vector<TYPE> > other)
//      : d_begin(bsls::RvalueUtil::access(other).d_begin)
//      , d_end(bsls::RvalueUtil::access(other).d_end)
//      , d_capacity(bsls::RvalueUtil::access(other).d_capacity) {
//      vector<TYPE>& reference(other);
//      reference.d_begin = 0;
//      reference.d_end = 0;
//      reference.d_capacity = 0;
//  }
//..
// This constructor gets an 'Rvalue<vector<TYPE> >' passed as argument that
// indicates that the referenced objects can be modified as long as it is left
// in state meeting the class invariants. The implementation of this
// constructor copies the 'd_begin', 'd_end', and 'd_capacity' members of
// 'other'. Since 'other' is either an object of type 'Rvalue<vector<TYPE> >'
// (when compiling using a C++03 compiler) or an rvalue referene
// 'vector<TYPE>&&' the members are accessed using 'RvalueUtil::access(other)'
// to get a reference to a 'vector<TYPE>'. Within the body of the constructor
// an lvalue reference is obtained either via the conversion operator of
// 'Rvalue<T>' or directly as 'other' is just an lvalue whe compiiling with a
// C++11 compiler. This reference is used to set the pointer members of the
// object referenced by 'other' to '0' completing the move of the content to
// the object under construction.
//
// Finally, a move version of 'push_back()' is provided: it takes an
// 'Rvalue<TYPE>' as argument. The type of this argument indicates that the
// state can be transferred and after arranging enough capacity in the
// 'vector<TYPE>' object a new element is move constructed at the position
// 'd_end':
//..
//  template <class TYPE>
//  void vector<TYPE>::push_back(bsls::Rvalue<TYPE> value) {
//      if (this->d_end == this->d_capacity) {
//          this->reserve(this->size()? int(1.5 * this->size()): 4);
//      }
//      new(this->d_end++) TYPE(bsls::RvalueUtil::move(value));
//  }
//..
// To demonstrate the newly created 'vector<TYPE>' class in action, first a
// 'vector<int>' is created and filled with a few elements:
//..
//  vector<int> vector0;
//  for (int i = 0; i != 5; ++i) {
//      vector0.push_back(i);
//  }
//  for (int i = 0; i != 5; ++i) {
//      assert(vector0[i] == i);
//  }
//..
// To verify that copying of 'vector<TYPE>' objects works, a copy is created:
//..
//  vector<int> vector1(vector0);
//  assert(vector1.size() == vector0.size());
//  for (int i = 0; i != vector1.size(); ++i) {
//      assert(vector1[i] == vector0[i]);
//  }
//..
// When using moving this 'vector0' to a new location the representation of the
// new object should use the orginal 'begin()':
//..
//  const int *first = vector0.begin();
//  vector<int> vector1(bsls::RvalueUtil::move(vector0));
//  assert(first == vector1.begin());
//..
// When create a 'vector<vector<int> >' and using 'push_back()' on this object
// with 'vector2' a copy should be inserted:
//..
//  vector<vector<int> > vvector;
//  vvector.push_back(vector2);                          // copy
//  assert(vvector.size() == 1);
//  assert(vvector[0].size() == vector2.size());
//  assert(vvector[0].begin() != first);
//  for (int i = 0; i != 5; ++i) {
//      assert(vvector[0][i] == i);
//  }
//..
// When adding another element by moving 'vector2' the 'begin()' of the newly
// inserted element will be the same as 'first', i.e., the representation is
// transferred:
//..
//  vvector.push_back(bsls::RvalueUtil::move(vector2)); // move
//  assert(vvector.size() == 2);
//  assert(vvector[1].begin() == first);
//..
// Compiling this code with both C++03 and C++11 compilers shows that there is
// no need for conditional compilation in when using 'Rvalue<TYPE>' while move
// semantics is enabled in both modes.

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_BSLREMOVEREFERENCE
#include <bsls_bslremovereference.h>
#endif

namespace BloombergLP {

namespace bsls {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
    && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)

template <class TYPE>
using Rvalue = TYPE&&;

struct RvalueUtil {
    // This 'struct' provides a collection of utility functions operating on
    // objects of type 'Rvalue<TYPE>'. The primary use of these utilities to
    // create a consistent notation for using the C++03 'Rvalue<TYPE>' objects
    // and the C++11 'TYPE&&' rvalue references.

    template <class TYPE>
    static TYPE& access(TYPE& rvalue);
        // Obtain a reference to the object references by the specified
        // 'rvalue' object. This reference can also be obtained by a conversion
        // of 'rvalue' to 'TYPE&' in contexts where a conversion is viable.
        // When a conversion isn't applicable, e.g., when caling a member of
        // 'TYPE' the reference can be accessed using 'access()'. Since the
        // same notation should be applicable to the C++03 'Rvalue<TYPE>'
        // objects and a C++11 rvalue reference 'TYPE&&' a member function
        // cannot be used.
    template <class TYPE>
    static typename bsls::BslRemoveReference<TYPE>::type&& move(TYPE&& lvalue);
        // Get an rvalue reference of type 'Rvalue<TYPE>' from the specified
        // 'lvalue'. For a C++03 implementation this function behaves like a
        // factory for 'Rvalue<TYPE> objects. For a C++11 implementation this
        // function behaves exactly like 'std::move(lvalue)'.
};

// ----------------------------------------------------------------------------

template <class TYPE>
inline TYPE& RvalueUtil::access(TYPE& rvalue) {
    return rvalue;
}

template <class TYPE>
inline
typename bsls::BslRemoveReference<TYPE>::type&& RvalueUtil::move(TYPE&& lvalue)
{
    return static_cast<typename bsls::BslRemoveReference<TYPE>::type&&>(
                                                                       lvalue);
}

// ----------------------------------------------------------------------------

#else // support rvalue references and alias templates

struct RvalueUtil;

template <class TYPE>
class Rvalue
    // The class template 'Rvalue<TYPE>' provides a reference to an object of
    // type 'TYPE' whose state will not be counted upon for later use. Put
    // differently, a function receiving an object this class template can
    // transfer ("move") the representation to a different object and leave the
    // referenced object in an unspecified, although valid (i.e., it obeys all
    // class invariants), state. With C++11 an rvalue reference ('TYPE&&') is
    // used to represent the same semantics.
{
    friend struct RvalueUtil;
    TYPE *d_pointer;

    // PRIVATE CONSTRUCTORS
    explicit Rvalue(TYPE *pointer);
        // Create an 'Rvalue<TYPE>' object referencing the object pointed to by
        // the specified 'pointer'. The behavior is undefined if 'pointer' does
        // not point to an object. This constructor is private because a C++11
        // rvalue reference cannot be created this. For information on how to
        // create objects of type 'Rvalue<TYPE>' see 'RvalueUtil::move()'.

  public:
    // ACCESSORS
    operator TYPE&() const;
        // Return a reference to the referenced object. In contexts where a
        // reference to an object of type 'TYPE' is needed, an 'Rvalue<TYPE>'
        // behaves like such a reference. For information on how to access the
        // the reference in contexts where no conversion can be used see
        // 'RvalueUtil::access()'.
};

struct RvalueUtil {
    // This 'struct' provides a collection of utility functions operating on
    // objects of type 'Rvalue<TYPE>'. The primary use of these utilities to
    // create a consistent notation for using the C++03 'Rvalue<TYPE>' objects
    // and the C++11 'TYPE&&' rvalue references.

    template <class TYPE>
    static TYPE& access(Rvalue<TYPE> rvalue);
        // Obtain a reference to the object references by the specified
        // 'rvalue' object. This reference can also be obtained by a conversion
        // of 'rvalue' to 'TYPE&' in contexts where a conversion is viable.
        // When a conversion isn't applicable, e.g., when caling a member of
        // 'TYPE' the reference can be accessed using 'access()'. Since the
        // same notation should be applicable to the C++03 'Rvalue<TYPE>'
        // objects and a C++11 r-value reference 'TYPE&&' a member function
        // cannot be used.
    template <class TYPE>
    static Rvalue<TYPE> move(TYPE& lvalue);
        // Get an rvalue reference of type 'Rvalue<TYPE>' from the specified
        // 'lvalue'. For a C++03 implementation this function behaves like a
        // factory for 'Rvalue<TYPE> objects. For a C++11 implementation this
        // function behaves exactly like 'std::move(value)'.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

template <class TYPE>
inline Rvalue<TYPE>::Rvalue(TYPE *pointer)
    : d_pointer(pointer) {
    BSLS_ASSERT(0 != pointer);
}

template <class TYPE>
inline Rvalue<TYPE>::operator TYPE&() const {
    return *d_pointer;
}

template <class TYPE>
inline TYPE& RvalueUtil::access(Rvalue<TYPE> rvalue) {
    return rvalue;
}

template <class TYPE>
inline Rvalue<TYPE> RvalueUtil::move(TYPE& lvalue) {
    return Rvalue<TYPE>(&lvalue);
}

// ----------------------------------------------------------------------------

#endif // support rvalue references and alias templates


}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
