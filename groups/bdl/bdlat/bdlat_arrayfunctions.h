// bdlat_arrayfunctions.h                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#define INCLUDED_BDLAT_ARRAYFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining "array" functions.
//
//@CLASSES:
//  bdlat_ArrayFunctions: namespace for calling "array" functions
//
//@DESCRIPTION: The 'bdlat_ArrayFunctions' 'namespace' provided in this
// component defines parameterized functions that expose "array" behavior for
// "array" types.  See the {'bdlat'} package-level documentation for a full
// description of "array" types.
//
// The functions in this namespace allow users to:
//: o obtain the number of elements in an array ('size').
//: o set the number of elements in an array ('resize').
//: o manipulate an element in an array using a parameterized manipulator
//:   ('manipulateElement'). and
//: o access an element in an array using a parameterized accessor
//:   ('accessElement').
//
// A type becomes part of the 'bdlat' "array" framework by creating, in the
// namespace where the type is defined, overloads of the following two (free)
// functions and two (free) function templates.  Note that the placeholder
// 'YOUR_TYPE' is not a template argument and should be replaced with the name
// of the type being plugged into the framework.
//..
//  // MANIPULATORS
//  template <class MANIPULATOR>
//  int bdlat_arrayManipulateElement(YOUR_TYPE    *array,
//                                   MANIPULATOR&  manipulator,
//                                   int           index);
//      // Invoke the specified 'manipulator' on the address of the element at
//      // the specified 'index' of the specified 'array'.  Return the value
//      // from the invocation of 'manipulator'.  The behavior is undefined
//      // unless '0 <= index' and 'index < bdlat_arraySize(*array)'.
//
//  void resize(YOUR_TYPE *array, int newSize);
//      // Set the size of the specified modifiable 'array' to the specified
//      // 'newSize'.  If 'newSize > bdlat_arraySize(*array)', then
//      // 'newSize - bdlat_arraySize(*array)' elements with default values
//      // (i.e., 'ElementType()') are appended to 'array'.  If
//      // 'newSize < bdlat_arraySize(*array)', then the
//      // 'bdlat_arraySize(*array) - newSize' elements at the end of 'array'
//      // are destroyed.  The behavior is undefined unless '0 <= newSize'.
//
//  // ACCESSORS
//  template <class ACCESSOR>
//  int bdlat_arrayAccessElement(const YOUR_TYPE& array,
//                               ACCESSOR&        accessor,
//                               int              index);
//      // Invoke the specified 'accessor' on a 'const'-reference to the
//      // element at the specified 'index' of the specified 'array'.  Return
//      // the value from the invocation of 'accessor'.  The behavior is
//      // undefined unless '0 <= index' and 'index < bdlat_arraySize(array)'.
//
//  bsl::size_t bdlat_arraySize(const YOUR_TYPE& array);
//      // Return the number of elements in the specified 'array'.
//..
// The "array" type must also define two meta-functions in the
// 'bdlat_ArrayFunctions' namespace:
//
//: o the meta-function 'IsArray' contains a compile-time constant 'VALUE' that
//:   is non-zero if the parameterized 'TYPE' exposes "array" behavior, and
//:
//: o the 'ElementType' meta-function contains a 'typedef' 'Type' that
//:   specifies the type of the element stored in the parameterized "array"
//:   type.
//
// Note that 'bsl::vector<TYPE>' is already part of the 'bdlat'
// infrastructure for "array" types because this component also provides
// overloads of the required functions and meta-function specializations.
//
///Usage
//------
// The following code illustrate the usage of this component.
//
///Example 1: Defining an "Array" Type
// - - - - - - - - - - - - - - - - - -
// Suppose you had a type, 'mine::MyIntArray', that provides the essential
// features of an "array" type.
//..
//  namespace BloombergLP {
//  namespace mine {
//
//  class MyIntArray {
//
//      int         *d_data_p;
//      bsl::size_t  d_size;
//
//    public:
//      // CREATORS
//      MyIntArray()
//      : d_data_p(0)
//      , d_size(0)
//      {
//      }
//
//      ~MyIntArray()
//      {
//          bsl::free(d_data_p);
//      }
//
//      // MANIPULATORS
//      void resize(bsl::size_t newSize);
//
//      int& value(bsl::size_t index)
//      {
//          assert(index < d_size);
//
//          return d_data_p[index];
//      }
//
//      // ACCESSORS
//      const int& value(bsl::size_t index) const
//      {
//          assert(index < d_size);
//
//          return d_data_p[index];
//      }
//
//      bsl::size_t size() const
//      {
//          return d_size;
//      }
//  };
//
//  void MyIntArray::resize(bsl::size_t newSize)
//  {
//      // Always match buffer to size exactly.
//
//      if (d_size == newSize) {
//          return;                                                   // RETURN
//      }
//
//      int *newData = static_cast<int *>(bsl::malloc(sizeof(int)
//                                                  * newSize));
//      if (d_size < newSize) {
//          bsl::memcpy(newData, d_data_p, d_size * sizeof(int));
//          std::memset(newData + d_size,
//                      0,
//                      (newSize - d_size) * sizeof(int));
//
//      } else {
//          bsl::memcpy(newData, d_data_p, newSize);
//      }
//
//      bsl::free(d_data_p);
//      d_data_p = newData;
//      d_size   = newSize;
//  }
//
//  }  // close namespace mine
//  }  // close enterprise namespace
//..
// We can now make 'mine::MyIntArray' expose "array" behavior by implementing
// the necessary 'bdlat_ArrayFunctions' for 'MyIntArray' inside the 'mine'
// namespace and defining the required meta-functions withing the
// 'bdlat_ArrayFunctions' namespace.
//
// First, we should forward declare all the functions that we will implement
// inside the 'mine' namespace:
//..
//  namespace BloombergLP {
//  namespace mine {
//
//  // MANIPULATORS
//  template <class MANIPULATOR>
//  int bdlat_arrayManipulateElement(MyIntArray   *array,
//                                   MANIPULATOR&  manipulator,
//                                   int           index);
//      // Invoke the specified 'manipulator' on the address of the element at
//      // the specified 'index' of the specified 'array'.  Return the value
//      // from the invocation of 'manipulator'.  The behavior is undefined
//      // unless '0 <= index' and 'index < bdlat_arraySize(*array)'.
//
//  void bdlat_arrayResize(MyIntArray *array, int newSize);
//      // Set the size of the specified modifiable 'array' to the specified
//      // 'newSize'.  If 'newSize > bdlat_arraySize(*array)', then
//      // 'newSize - bdlat_arraySize(*array)' elements with default values
//      // (i.e., 'ElementType()') are appended to 'array'.  If
//      // 'newSize < bdlat_arraySize(*array)', then the
//      // 'bdlat_arraySize(*array) - newSize' elements at the end of 'array'
//      // are destroyed.  The behavior is undefined unless '0 <= newSize'.
//
//  // ACCESSORS
//  template <class ACCESSOR>
//  int bdlat_arrayAccessElement(const MyIntArray& array,
//                               ACCESSOR&         accessor,
//                               int               index);
//      // Invoke the specified 'accessor' on a 'const'-reference to the
//      // element at the specified 'index' of the specified 'array'.  Return
//      // the value from the invocation of 'accessor'.  The behavior is
//      // undefined unless '0 <= index' and 'index < bdlat_arraySize(array)'.
//
//  bsl::size_t bdlat_arraySize(const MyIntArray& array);
//      // Return the number of elements in the specified 'array'.
//
//  }  // close namespace mine
//  }  // close enterprise namespace
//..
// Then, we will implement these functions.  Recall that the two (non-template)
// functions should be defined in some '.cpp' file, unless you choose to make
// them 'inline' functions.
//..
//  namespace BloombergLP {
//  namespace mine {
//
//  // MANIPULATORS
//  template <class MANIPULATOR>
//  int bdlat_arrayManipulateElement(MyIntArray   *array,
//                                   MANIPULATOR&  manipulator,
//                                   int           index)
//  {
//      assert(array);
//      assert(0 <= index);
//      assert(static_cast<bsl::size_t>(index) < array->size());
//
//      return manipulator(&array->value(index));
//  }
//
//  void bdlat_arrayResize(MyIntArray *array, int newSize)
//  {
//      assert(array);
//      assert(0 <= newSize);
//
//      array->resize(newSize);
//  }
//
//  // ACCESSORS
//  template <class ACCESSOR>
//  int bdlat_arrayAccessElement(const MyIntArray& array,
//                               ACCESSOR&         accessor,
//                               int               index)
//  {
//      assert(0 <= index);
//      assert(static_cast<bsl::size_t>(index) < array.size());
//
//      return accessor(array.value(index));
//  }
//
//  bsl::size_t bdlat_arraySize(const MyIntArray& array)
//  {
//      return array.size();
//  }
//
//  }  // close namespace mine
//  }  // close enterprise namespace
//..
// Finally, we specialize the 'IsArray' and 'ElementType' meta-functions
// in the 'bdlat_ArrayFunctions' namespace for the
// 'mine::MyIntArray' type:
//..
//  namespace BloombergLP {
//  namespace bdlat_ArrayFunctions {
//
//  // TRAITS
//  template <>
//  struct IsArray<mine::MyIntArray> {
//      enum { VALUE = 1 };
//  };
//
//  template <>
//  struct ElementType<mine::MyIntArray> {
//      typedef int Type;
//  };
//
//  }  // close namespace bdlat_ArrayFunctions
//  }  // close enterprise namespace
//..
// This completes the 'bdlat' infrastructure for 'mine::MyIntArray' and
// allows the generic software to recognize the type as an array abstraction.
//
///Example 2: Using the Infrastructure Via General Methods
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlat' "array" framework provides a set of fundamental operations
// common to any "array" type.  We can build upon these operations to make our
// own utilities, or use them on our own types that are plugged into the
// framework, like 'mine::MyIntArray', which we created in {Example 1}.  For
// example, we can use the (fundamental) operations in the
// 'bdlat_ArrayFunctions' namespace to operate on 'mine::MyIntArray', even
// though they have no knowledge of that type in particular:
//..
//  void usageMakeArray()
//  {
//      BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<mine::MyIntArray>::VALUE);
//
//      mine::MyIntArray array;
//      assert(0 == bdlat_ArrayFunctions::size(array));
//
//      bdlat_ArrayFunctions::resize(&array, 8);
//      assert(8 == bdlat_ArrayFunctions::size(array));
//
//      bdlat_ArrayFunctions::resize(&array, 4);
//      assert(4 == bdlat_ArrayFunctions::size(array));
//  }
//..
// To perform operations on the elements of an array requires use of the
// functions that employ accessor and manipulator functors.  For example:
//..
//  template <class ELEMENT_TYPE>
//  class GetElementAccessor {
//
//      // DATA
//      ELEMENT_TYPE *d_element_p;
//
//    public:
//      // CREATORS
//      explicit GetElementAccessor(ELEMENT_TYPE *value)
//      : d_element_p(value)
//      {
//      }
//
//      // MANIPULATORS
//      int operator()(const ELEMENT_TYPE& elementValue)
//      {
//          *d_element_p = elementValue;
//          return 0;
//      }
//  };
//
//  template<class ELEMENT_TYPE>
//  class SetElementManipulator {
//
//      // DATA
//      ELEMENT_TYPE d_value;
//
//    public:
//      // CREATORS
//      SetElementManipulator(const ELEMENT_TYPE& value)
//      : d_value(value)
//      {
//      }
//
//      // ACCESSOR
//      int operator()(ELEMENT_TYPE *element) const
//      {
//          *element = d_value;
//          return 0;
//      }
//  };
//..
// Notice that these functors make few assumptions of 'ELEMENT_TYPE', merely
// that it is copy constructable and copy assignable.
//
// With these definitions we can now use the generic functions to set and
// get values from an 'mine::MyIntArray' object:
//..
//  void usageArrayElements()
//  {
//      mine::MyIntArray array;
//      bdlat_ArrayFunctions::resize(&array, 4);
//
//      // Confirm initial array elements from resize.
//
//      int                     value;
//      GetElementAccessor<int> accessor(&value);
//
//      for (int index = 0; index < 4; ++index) {
//          int rc = bdlat_ArrayFunctions::accessElement(array,
//                                                       accessor,
//                                                       index);
//          assert(0 == rc);
//          assert(0 == value)
//      }
//
//      // Set element 'index * 10' as its value;
//
//      for (int index = 0; index < 4; ++index) {
//          SetElementManipulator<int> manipulator(index * 10);
//
//          int rc = bdlat_ArrayFunctions::manipulateElement(&array,
//                                                           manipulator,
//                                                           index);
//          assert(0 == rc);
//      }
//
//      // Confirm new value of each element.
//
//      for (int index = 0; index < 4; ++index) {
//          int rc = bdlat_ArrayFunctions::accessElement(array,
//                                                       accessor,
//                                                       index);
//          assert(0          == rc);
//          assert(index * 10 == value);
//      }
//  }
//..
//
///Example 3: Defining Utility Functions
///- - - - - - - - - - - - - - - - - - -
// Creating functor objects for each operation can be tedious and error prone;
// consequently, those types are often executed via utility functions.
//
// Suppose we want to create utilities for getting and setting the elements of
// an arbitrary "array" type.  We might define a utility 'struct', 'ArrayUtil',
// a namespace for those functions:
//..
//  struct ArrayUtil {
//
//      // CLASS METHODS
//      template <class ARRAY_TYPE>
//      static int getElement(typename bdlat_ArrayFunctions
//                                     ::ElementType<ARRAY_TYPE>::Type *value,
//                          const ARRAY_TYPE&                           object,
//                          int                                         index)
//          // Load to the specified 'value' the element at the specified
//          // 'index' of the specified 'object' array.  Return 0 if the
//          // element is successfully loaded to 'value', and a non-zero value
//          // otherwise.  This function template requires that the specified
//          // 'ARRAY_TYPE' is a 'bdlat' "array" type.  The behavior is
//          // undefined unless '0 <= index' and
//          // 'index < bdlat_ArrayFunctions::size(object)'.
//      {
//          BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<ARRAY_TYPE>::VALUE);
//
//          typedef typename bdlat_ArrayFunctions
//                                 ::ElementType<ARRAY_TYPE>::Type ElementType;
//
//          GetElementAccessor<ElementType> elementAccessor(value);
//
//          return bdlat_ArrayFunctions::accessElement(object,
//                                                     elementAccessor,
//                                                     index);
//      }
//
//      template <class ARRAY_TYPE>
//      static int setElement(
//       ARRAY_TYPE                                                    *object,
//       int                                                            index,
//       const typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>
//                                                             ::Type&  value)
//          // Assign the specified 'value' to the element of the specified
//          // 'object' array at the specified 'index'.  Return 0 if the
//          // element is successfully assigned to 'value', and a non-zero
//          // value otherwise.  This function template requires that the
//          // specified 'ARRAY_TYPE' is a 'bdlat' "array" type.  The behavior
//          // is undefined unless '0 <= index' and
//          // 'index < bdlat_ArrayFunctions::size(*object)'.
//      {
//          BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<ARRAY_TYPE>::VALUE);
//
//          typedef typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>
//                                                          ::Type ElementType;
//
//          SetElementManipulator<ElementType> manipulator(value);
//
//          return bdlat_ArrayFunctions::manipulateElement(object,
//                                                         manipulator,
//                                                         index);
//      }
//  };
//..
// Now, we can use these functors to write generic utility functions for
// getting and setting the value types of arbitrary "array" classes.
//..
//  void myUsageScenario()
//  {
//      mine::MyIntArray array;
//      bdlat_ArrayFunctions::resize(&array, 4);
//
//      // Confirm initial values.
//
//      for (int index = 0; index < 4; ++index) {
//          int value;
//          int rc = ArrayUtil::getElement(&value, array, index);
//          assert(0 == rc);
//          assert(0 == value);
//      }
//
//      // Set element 'index * 10' as its value;
//
//      for (int index = 0; index < 4; ++index) {
//          int value = index * 10;
//          int rc     = ArrayUtil::setElement(&array, index, value);
//          assert(0 == rc);
//      }
//
//      // Confirm value of each element.
//
//      for (int index = 0; index < 4; ++index) {
//          int value;
//          int rc = ArrayUtil::getElement(&value, array, index);
//          assert(0           == rc);
//          assert(index * 10 == value);
//      }
//  }
//..
//
///Example 4: Achieving Type Independence
/// - - - - - - - - - - - - - - - - - - -
// Suppose we have another type such as 'your::YourFloatArray', shown below:
//..
//  namespace BloombergLP {
//  namespace your {
//
//  class MyFloatArray {
//
//      float       *d_data_p;
//      bsl::size_t  d_size;
//      bsl::size_t  d_capacity;
//
//    public:
//      // CREATORS
//      MyFloatArray()
//      : d_data_p(0)
//      , d_size(0)
//      {
//      }
//
//      ~MyFloatArray()
//      {
//          delete[] d_data_p;
//      }
//
//      // MANIPULATORS
//      void setSize(bsl::size_t newSize); // Too large for inline.
//
//      float& element(bsl::size_t index)
//      {
//          assert(index < d_size);
//
//          return d_data_p[index];
//      }
//
//      // ACCESSORS
//      const float& element(bsl::size_t index) const
//      {
//          assert(index < d_size);
//
//          return d_data_p[index];
//      }
//
//      bsl::size_t numElements() const
//      {
//          return d_size;
//      }
//
//      bsl::size_t capacity() const
//      {
//          return d_capacity;
//      }
//  };
//..
// Notice that while there are many similarities to 'mine::MyIntArray', there
// are also significant differences:
//: o The element type is 'float', not 'int'.
//: o Many of the accessors are named differently (e.g., 'numElements' instead
//:   of 'size', 'setSize' instead of 'resize').
//: o There is an additional attribute, 'capacity', because this class has a
//:   'setSize' method (not shown) that reduces calls to the heap by over
//:   allocating when the size is increased beyond the current capacity.
//
// Nevertheless, since 'your::YourFloatArray' also provides the functions
// and types expected by the 'bdlat' infrastructure (not shown) we can
// successfully use 'your::FloatArray' value instead of 'mine::MyIntArray'
// in the previous usage scenario, with no other changes:
//..
//  void yourUsageScenario()
//  {
//      your::YourFloatArray array;
//      bdlat_ArrayFunctions::resize(&array, 4);
//
//      // Confirm initial values.
//
//      for (int index = 0; index < 4; ++index) {
//          float value;
//          int   rc = ArrayUtil::getElement(&value, array, index);
//          assert(0   == rc);
//          assert(0.0 == value);
//      }
//
//      // Set element 'index * 10' as its value;
//
//      for (int index = 0; index < 4; ++index) {
//          float value = static_cast<float>(index * 10);
//          int   rc    = ArrayUtil::setElement(&array, index, value);
//          assert(0 == rc);
//      }
//
//      // Confirm value of each element.
//
//      for (int index = 0; index < 4; ++index) {
//          float value;
//          int   rc = ArrayUtil::getElement(&value, array, index);
//          assert(0                              == rc);
//          assert(static_cast<float>(index * 10) == value);
//      }
//  }
//..
// Notice that syntax and order of 'bdlat_ArrayFunctions' function
// calls have not been changed.  The only difference is that the element
// type has changed from 'int' to 'float'.
//
// Finally, instead of defining a new "array" type, we could substitute the
// existing type template 'bsl::vector'.  Note that this component
// provides specializations of the 'bdlat_ArrayFunctions' for that
// type.  Since the accessor and manipulator functions we created earlier are
// type neutral, we can simply drop 'bsl::vector<bsl::string>' into our
// familiar scenario:
//..
//  void anotherUsageScenario()
//  {
//      bsl::vector<bsl::string> array;  // STANDARD ARRAY TYPE
//      bdlat_ArrayFunctions::resize(&array, 4);
//
//      // Confirm initial values.
//
//      for (int index = 0; index < 4; ++index) {
//          bsl::string value;
//          int         rc = ArrayUtil::getElement(&value, array, index);
//          assert(0  == rc);
//          assert("" == value);
//      }
//
//      // Set element 'index * 10' as its value;
//
//      for (int index = 0; index < 4; ++index) {
//          bsl::ostringstream oss; oss << (index * 10);
//          int rc = ArrayUtil::setElement(&array, index, oss.str());
//          assert(0 == rc);
//      }
//
//      // Confirm value of each element.
//
//      for (int index = 0; index < 4; ++index) {
//          bsl::string value;
//          int         rc = ArrayUtil::getElement(&value, array, index);
//
//          bsl::ostringstream oss; oss << (index * 10);
//
//          assert(0         == rc);
//          assert(oss.str() == value);
//      }
//  }
//..

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>

#include <bslmf_integralconstant.h>

#include <bsl_cstddef.h>
#include <bsl_cstdlib.h>
#include <bsl_vector.h>

namespace BloombergLP {

                       // ==============================
                       // namespace bdlat_ArrayFunctions
                       // ==============================

namespace bdlat_ArrayFunctions {
    // This 'namespace' provides functions that expose "array" behavior for
    // "array" types.  Specializations are provided for 'bsl::vector<TYPE>'.
    // See the component-level documentation for more information.

    // META-FUNCTIONS
    template <class TYPE>
    struct ElementType;
        // This meta-function should contain a typedef 'Type' that specifies
        // the type of element stored in an array of the parameterized 'TYPE'.

    template <class TYPE>
    struct IsArray {
        // This 'struct' should be specialized for third-party types that are
        // need to expose "array" behavior.  See the component-level
        // documentation for further information.

        // TYPES
        enum {
            VALUE = 0
        };
    };

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int manipulateElement(TYPE         *array,
                          MANIPULATOR&  manipulator,
                          int           index);
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified 'array'.  Return the value
        // from the invocation of 'manipulator'.  The behavior is undefined
        // unless '0 <= index' and 'index < size(*array)'.

    template <class TYPE>
    void resize(TYPE *array, int newSize);
        // Set the size of the specified modifiable 'array' to the specified
        // 'newSize'.  If 'newSize > size(array)', then 'newSize - size(array)'
        // elements with default values are appended to 'array'.  If
        // 'newSize < size(array)', then the 'size(array) - newSize' elements
        // at the end of 'array' are destroyed.  The behavior is undefined
        // unless '0 <= newSize'.

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int accessElement(const TYPE& array,
                      ACCESSOR&   accessor,
                      int         index);
        // Invoke the specified 'accessor' on the non-modifiable element at the
        // specified 'index' of the specified 'array'.  Return the value from
        // the invocation of 'accessor'.  The behavior is undefined unless
        // '0 <= index' and 'index < size(array)'.

    template <class TYPE>
    bsl::size_t size(const TYPE& array);
        // Return the number of elements in the specified 'array'.

}  // close namespace bdlat_ArrayFunctions

                          // ========================
                          // bsl::vector declarations
                          // ========================

namespace bdlat_ArrayFunctions {
    // This namespace declaration adds the implementation of the "array" traits
    // for 'bsl::vector' to 'bdlat_ArrayFunctions'.  Note that 'bsl::vector' is
    // the canonical "array" type.

    // META-FUNCTIONS
    template <class TYPE, class ALLOC>
    struct IsArray<bsl::vector<TYPE, ALLOC> > : bsl::true_type {
    };

    template <class TYPE, class ALLOC>
    struct ElementType<bsl::vector<TYPE, ALLOC> > {
        typedef TYPE Type;
    };

    // MANIPULATORS
    template <class TYPE, class ALLOC, class MANIPULATOR>
    int bdlat_arrayManipulateElement(bsl::vector<TYPE, ALLOC> *array,
                                     MANIPULATOR&              manipulator,
                                     int                       index);

    template <class TYPE, class ALLOC>
    void bdlat_arrayResize(bsl::vector<TYPE, ALLOC> *array, int newSize);

    // ACCESSORS
    template <class TYPE, class ALLOC, class ACCESSOR>
    int bdlat_arrayAccessElement(const bsl::vector<TYPE, ALLOC>& array,
                                 ACCESSOR&                       accessor,
                                 int                             index);

    template <class TYPE, class ALLOC>
    bsl::size_t bdlat_arraySize(const bsl::vector<TYPE, ALLOC>& array);

}  // close namespace bdlat_ArrayFunctions

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -------------------------
                       // namespace-level functions
                       // -------------------------

// MANIPULATORS
template <class TYPE, class MANIPULATOR>
inline
int bdlat_ArrayFunctions::manipulateElement(TYPE         *array,
                                            MANIPULATOR&  manipulator,
                                            int           index)
{
    return bdlat_arrayManipulateElement(array, manipulator, index);
}

template <class TYPE>
inline
void bdlat_ArrayFunctions::resize(TYPE *array, int newSize)
{
    bdlat_arrayResize(array, newSize);
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_ArrayFunctions::accessElement(const TYPE& array,
                                        ACCESSOR&   accessor,
                                        int         index)
{
    return bdlat_arrayAccessElement(array, accessor, index);
}

template <class TYPE>
inline
bsl::size_t bdlat_ArrayFunctions::size(const TYPE& array)
{
    return bdlat_arraySize(array);
}

                          // -----------------------
                          // bsl::vector definitions
                          // -----------------------

// MANIPULATORS
template <class TYPE, class ALLOC, class MANIPULATOR>
inline
int bdlat_ArrayFunctions::bdlat_arrayManipulateElement(
                                         bsl::vector<TYPE, ALLOC> *array,
                                         MANIPULATOR&              manipulator,
                                         int                       index)
{
    TYPE& element = (*array)[index];
    return manipulator(&element);
}

template <class TYPE, class ALLOC>
inline
void bdlat_ArrayFunctions::bdlat_arrayResize(bsl::vector<TYPE, ALLOC> *array,
                                             int                       newSize)
{
    array->resize(newSize);
}

// ACCESSORS
template <class TYPE, class ALLOC, class ACCESSOR>
inline
int bdlat_ArrayFunctions::bdlat_arrayAccessElement(
                                      const bsl::vector<TYPE, ALLOC>& array,
                                      ACCESSOR&                       accessor,
                                      int                             index)
{
    return accessor(array[index]);
}

template <class TYPE, class ALLOC>
inline
bsl::size_t bdlat_ArrayFunctions::bdlat_arraySize(
                                         const bsl::vector<TYPE, ALLOC>& array)
{
    return array.size();
}

}  // close enterprise namespace

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
