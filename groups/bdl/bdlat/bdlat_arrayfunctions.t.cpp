// bdlat_arrayfunctions.t.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlat_arrayfunctions.h>

#include <bslim_testutil.h>

#include <bdlat_typetraits.h>

#include <bslalg_typetraits.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>   // for testing only
#include <bslmf_issame.h>             // for testing only

#include <bsl_algorithm.h> // 'bsl::max', 'bsl::copy', 'bsl::fill'
#include <bsl_cstdlib.h>   // 'bsl::free', 'bsl::malloc'
#include <bsl_cstring.h>   // 'bsl::memcpy', 'bsl::memset'
#include <bsl_iostream.h>
#include <bsl_sstream.h>   // 'bsl::ostringstream'
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
// [ 2] struct IsArray<TYPE>
// [ 2] struct ElementType<TYPE>
// [ 1] METHOD FORWARDING TEST
//-----------------------------------------------------------------------------
// [ 3] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

namespace Obj = bdlat_ArrayFunctions;

// ============================================================================
//                            CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                        // ===========================
                        // class GetValue<LVALUE_TYPE>
                        // ===========================

template <class LVALUE_TYPE>
class GetValue {
    // This visitor assigns the value of the visited member to
    // 'd_destination_p'.

    // PRIVATE DATA MEMBERS
    LVALUE_TYPE *d_lValue_p;  // held, not owned

  public:
    // CREATORS
    explicit GetValue(LVALUE_TYPE *lValue);

    // ACCESSORS
    int operator()(const LVALUE_TYPE& object) const;
        // Assign the specified 'object' to '*d_destination_p'.

    template <class RVALUE_TYPE>
    int operator()(const RVALUE_TYPE& object) const;
        // Do nothing with the specified 'object'.
};

                       // ==============================
                       // class AssignValue<RVALUE_TYPE>
                       // ==============================

template <class RVALUE_TYPE>
class AssignValue {
    // This visitor assigns 'd_value' to the visited member.

    // PRIVATE DATA MEMBERS
    RVALUE_TYPE d_value;

  public:
    // CREATORS
    explicit AssignValue(const RVALUE_TYPE& value);

    // ACCESSORS
    int operator()(RVALUE_TYPE *object) const;
        // Assign 'd_value' to the specified '*object'.

    template <class LVALUE_TYPE>
    int operator()(LVALUE_TYPE *object) const;
        // Do nothing with the specified 'object'.
};

                              // ================
                              // class FixedArray
                              // ================

namespace Test {

template <int SIZE, class TYPE>
class FixedArray
{
    // Fixed-sized array that conforms to the 'bdlat_ArrayFunctions'
    // interface.

    TYPE d_values[SIZE];
    int  d_length;

  public:
    FixedArray();

    // Compiler-generated functions:
    //  FixedArray(const FixedArray&);
    //  FixedArray& operator=(const FixedArray&);
    //  ~FixedArray();

    // MANIPULATORS
    void append(const TYPE& v);

    void resize(int newSize);

    template <class MANIPULATOR>
    int manipulateElement(MANIPULATOR& manip, int index);

    // ACCESSORS
    int length() const;

    template <class ACCESSOR>
    int accessElement(ACCESSOR& acc, int index) const;
};

// FREE MANIPULATORS
template <int SIZE, class TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(FixedArray<SIZE, TYPE> *array,
                                 MANIPULATOR&            manipulator,
                                 int                     index);

template <int SIZE, class TYPE>
void bdlat_arrayResize(FixedArray<SIZE, TYPE> *array, int newSize);

// FREE ACCESSORS
template <int SIZE, class TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(const FixedArray<SIZE, TYPE>& array,
                             ACCESSOR&                     accessor,
                             int                           index);

template <int SIZE, class TYPE>
bsl::size_t bdlat_arraySize(const FixedArray<SIZE, TYPE>& array);
    // Return the number of elements in the specified 'array'.

}  // close namespace Test

namespace BloombergLP {
namespace bdlat_ArrayFunctions {
    // META FUNCTIONS
    template <int SIZE, class TYPE>
    struct ElementType<Test::FixedArray<SIZE, TYPE> > {
        typedef TYPE Type;
    };

    template <int SIZE, class TYPE>
    struct IsArray<Test::FixedArray<SIZE, TYPE> > : public bsl::true_type {
    };
}  // close namespace bdlat_ArrayFunctions
}  // close enterprise namespace

                        // ---------------------------
                        // class GetValue<LVALUE_TYPE>
                        // ---------------------------

// CREATORS

template <class LVALUE_TYPE>
GetValue<LVALUE_TYPE>::GetValue(LVALUE_TYPE *lValue)
: d_lValue_p(lValue)
{
}

// ACCESSORS

template <class LVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const LVALUE_TYPE& object) const
{
    *d_lValue_p = object;
    return 0;
}

template <class LVALUE_TYPE>
template <class RVALUE_TYPE>
int GetValue<LVALUE_TYPE>::operator()(const RVALUE_TYPE& object) const
{
    return -1;
}

                       // ------------------------------
                       // class AssignValue<RVALUE_TYPE>
                       // ------------------------------

// CREATORS

template <class RVALUE_TYPE>
AssignValue<RVALUE_TYPE>::AssignValue(const RVALUE_TYPE& value)
: d_value(value)
{
}

// ACCESSORS

template <class RVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(RVALUE_TYPE *object) const
{
    *object = d_value;
    return 0;
}

template <class RVALUE_TYPE>
template <class LVALUE_TYPE>
int AssignValue<RVALUE_TYPE>::operator()(LVALUE_TYPE *object) const
{
    return -1;
}

                           // ----------------------
                           // class Test::FixedArray
                           // ----------------------

template <int SIZE, class TYPE>
inline
Test::FixedArray<SIZE, TYPE>::FixedArray()
: d_length(0)
{
}

template <int SIZE, class TYPE>
inline
void Test::FixedArray<SIZE, TYPE>::append(const TYPE& v)
{
    d_values[d_length++] = v;
}

template <int SIZE, class TYPE>
void Test::FixedArray<SIZE, TYPE>::resize(int newSize)
{
    // If growing, then null out new elements
    for (int i = d_length; i < newSize; ++i) {
        d_values[i] = TYPE();
    }

    d_length = newSize;
}

template <int SIZE, class TYPE>
template <class MANIPULATOR>
inline
int Test::FixedArray<SIZE, TYPE>::manipulateElement(MANIPULATOR& manip,
                                                    int          index)
{
    return manip(&d_values[index]);
}

template <int SIZE, class TYPE>
inline
int Test::FixedArray<SIZE, TYPE>::length() const
{
    return d_length;
}

template <int SIZE, class TYPE>
template <class ACCESSOR>
inline
int Test::FixedArray<SIZE, TYPE>::accessElement(ACCESSOR& acc, int index) const
{
    return acc(d_values[index]);
}

// FREE MANIPULATORS
template <int SIZE, class TYPE, class MANIPULATOR>
int Test::bdlat_arrayManipulateElement(Test::FixedArray<SIZE, TYPE> *array,
                                       MANIPULATOR&                  manip,
                                       int                           index)
{
    return array->manipulateElement(manip, index);
}

template <int SIZE, class TYPE>
void Test::bdlat_arrayResize(Test::FixedArray<SIZE, TYPE> *array, int newSize)
{
    array->resize(newSize);
}

// FREE ACCESSORS
template <int SIZE, class TYPE, class ACCESSOR>
int Test::bdlat_arrayAccessElement(const Test::FixedArray<SIZE, TYPE>& array,
                                   ACCESSOR&                           acc,
                                   int                                 index)
{
    return array.accessElement(acc, index);
}

template <int SIZE, class TYPE>
bsl::size_t Test::bdlat_arraySize(const Test::FixedArray<SIZE, TYPE>& array)
{
    return array.length();
}

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

    namespace BloombergLP {
    namespace your {

    class YourFloatArray {

        float       *d_data_p;
        bsl::size_t  d_size;
        bsl::size_t  d_capacity;

      public:
        // CREATORS
        YourFloatArray()
        : d_data_p(0)
        , d_size(0)
        {
        }

        ~YourFloatArray()
        {
            delete[] d_data_p;
        }

        // MANIPULATORS
        void setSize(bsl::size_t newSize);  // Too large for inline.

        float& element(bsl::size_t index)
        {
            ASSERT(index < d_size);

            return d_data_p[index];
        }

        // ACCESSORS
        const float& element(bsl::size_t index) const
        {
            ASSERT(index < d_size);

            return d_data_p[index];
        }

        bsl::size_t numElements() const
        {
            return d_size;
        }

        bsl::size_t capacity() const
        {
            return d_capacity;
        }
    };

    void YourFloatArray::setSize(bsl::size_t newSize)
    {
        if (d_size == newSize) {
            return;                                                   // RETURN
        }

        if (!d_data_p) {
            d_data_p   = new float[newSize]();  // allocate and initialize
            d_size     = newSize;
            d_capacity = newSize;
            return;                                                   // RETURN
        }

        if (newSize < d_size) {
            return;                                                   // RETURN
        }

        if (newSize <= d_capacity) {
            bsl::fill(d_data_p + d_size, d_data_p + newSize, 0.0);
            d_size = newSize;
            return;                                                   // RETURN
        }

        d_capacity = bsl::max(static_cast<bsl::size_t>(1u), d_capacity * 2);

        float *newData = new float[d_capacity];

        bsl::copy(d_data_p, d_data_p + d_size, newData);
        bsl::fill(newData + d_size, newData + newSize, 0.0);

        delete[] d_data_p;
        d_data_p = newData;
        d_size   = newSize;
    }

    // MANIPULATORS
    template <class MANIPULATOR>
    int bdlat_arrayManipulateElement(YourFloatArray *array,
                                     MANIPULATOR&    manipulator,
                                     int             index);
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified 'array'.  Return the value
        // from the invocation of 'manipulator'.  The behavior is undefined
        // unless '0 <= index' and 'index < bdlat_arraySize(*array)'.

    void bdlat_arrayResize(YourFloatArray *array, int newSize);
        // Set the size of the specified modifiable 'array' to the specified
        // 'newSize'.  If 'newSize > size(array)', then 'newSize - size(array)'
        // elements with default values (i.e., 'ElementType()') are appended to
        // 'array'.  If 'newSize < size(array)', then the
        // 'size(array) - newSize' elements at the end of 'array' are
        // destroyed.  The behavior is undefined unless '0 <= newSize'.

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_arrayAccessElement(const YourFloatArray& array,
                                 ACCESSOR&             accessor,
                                 int                   index);
        // Invoke the specified 'accessor' on a 'const'-reference to the
        // element at the specified 'index' of the specified 'array'.  Return
        // the value from the invocation of 'accessor'.  The behavior is
        // undefined unless '0 <= index' and 'index < bdlat_arraySize(array)'.

    bsl::size_t bdlat_arraySize(const YourFloatArray& array);
        // Return the number of elements in the specified 'array'.

    // MANIPULATORS
    template <class MANIPULATOR>
    int bdlat_arrayManipulateElement(YourFloatArray *array,
                                     MANIPULATOR&    manipulator,
                                     int             index)
    {
        ASSERT(array);
        ASSERT(0 <= index);
        ASSERT(static_cast<bsl::size_t>(index) < array->numElements());

        return manipulator(&array->element(index));
    }

    void bdlat_arrayResize(YourFloatArray *array, int newSize)
    {
        ASSERT(array);
        ASSERT(0 <= newSize);

        array->setSize(newSize);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_arrayAccessElement(const YourFloatArray& array,
                                 ACCESSOR&             accessor,
                                 int                   index)
    {
        ASSERT(0 <= index);
        ASSERT(static_cast<bsl::size_t>(index) < array.numElements());

        return accessor(array.element(index));
    }

    bsl::size_t bdlat_arraySize(const YourFloatArray& array)
    {
        return array.numElements();
    }

    }  // close namespace your

    namespace bdlat_ArrayFunctions {

    // TRAITS
    template <>
    struct IsArray<your::YourFloatArray> : public bsl::true_type {
    };

    template <>
    struct ElementType<your::YourFloatArray> {
        typedef float Type;
    };

    }  // close namespace bdlat_ArrayFunctions
    }  // close enterprise namespace

///Usage
//------
// The following code illustrate the usage of this component.
//
///Example 1: Defining an "Array" Type
// - - - - - - - - - - - - - - - - - -
// Suppose you had a type, 'mine::MyIntArray', that provides the essential
// features of an "array" type.
//..
    namespace BloombergLP {
    namespace mine {

    class MyIntArray {

        int         *d_data_p;
        bsl::size_t  d_size;

      public:
        // CREATORS
        MyIntArray()
        : d_data_p(0)
        , d_size(0)
        {
        }

        ~MyIntArray()
        {
            bsl::free(d_data_p);
        }

        // MANIPULATORS
        void resize(bsl::size_t newSize);

        int& value(bsl::size_t index)
        {
            ASSERT(index < d_size);

            return d_data_p[index];
        }

        // ACCESSORS
        const int& value(bsl::size_t index) const
        {
            ASSERT(index < d_size);

            return d_data_p[index];
        }

        bsl::size_t size() const
        {
            return d_size;
        }
    };

    void MyIntArray::resize(bsl::size_t newSize)
    {
        // Always match buffer to size exactly.

        if (d_size == newSize) {
            return;                                                   // RETURN
        }

        int *newData = static_cast<int *>(bsl::malloc(sizeof(int)
                                                    * newSize));
        if (d_size < newSize) {
            bsl::memcpy(newData, d_data_p, d_size * sizeof(int));
            std::memset(newData + d_size,
                        0,
                        (newSize - d_size) * sizeof(int));

        } else {
            bsl::memcpy(newData, d_data_p, newSize);
        }

        bsl::free(d_data_p);
        d_data_p = newData;
        d_size   = newSize;
    }

    }  // close namespace mine
    }  // close enterprise namespace
//.
// We can now make 'mine::MyIntArray' expose "array" behavior by implementing
// the necessary 'bdlat_ArrayFunctions' for 'MyIntArray' inside the 'mine'
// namespace and defining the required meta-functions withing the
// 'bdlat_ArrayFunctions' namespace.
//
// First, we should forward declare all the functions that we will implement
// inside the 'mine' namespace:
//..
    namespace BloombergLP {
    namespace mine {

    // MANIPULATORS
    template <class MANIPULATOR>
    int bdlat_arrayManipulateElement(MyIntArray   *array,
                                     MANIPULATOR&  manipulator,
                                     int           index);
        // Invoke the specified 'manipulator' on the address of the element at
        // the specified 'index' of the specified 'array'.  Return the value
        // from the invocation of 'manipulator'.  The behavior is undefined
        // unless '0 <= index' and 'index < bdlat_arraySize(*array)'.

    void bdlat_arrayResize(MyIntArray *array, int newSize);
        // Set the size of the specified modifiable 'array' to the specified
        // 'newSize'.  If 'newSize > bdlat_arraySize(*array)', then
        // 'newSize - bdlat_arraySize(*array)' elements with default values
        // (i.e., 'ElementType()') are appended to 'array'.  If
        // 'newSize < bdlat_arraySize(*array)', then the
        // 'bdlat_arraySize(*array) - newSize' elements at the end of 'array'
        // are destroyed.  The behavior is undefined unless '0 <= newSize'.

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_arrayAccessElement(const MyIntArray& array,
                                 ACCESSOR&         accessor,
                                 int               index);
        // Invoke the specified 'accessor' on a 'const'-reference to the
        // element at the specified 'index' of the specified 'array'.  Return
        // the value from the invocation of 'accessor'.  The behavior is
        // undefined unless '0 <= index' and 'index < bdlat_arraySize(array)'.

    bsl::size_t bdlat_arraySize(const MyIntArray& array);
        // Return the number of elements in the specified 'array'.

    }  // close namespace mine
    }  // close enterprise namespace
//..
// Then, we will implement these functions.  Recall that the two (non-template)
// functions should be defined in some '.cpp' file, unless you choose to make
// them 'inline' functions.
//..
    namespace BloombergLP {
    namespace mine {

    // MANIPULATORS
    template <class MANIPULATOR>
    int bdlat_arrayManipulateElement(MyIntArray   *array,
                                     MANIPULATOR&  manipulator,
                                     int           index)
    {
        ASSERT(array);
        ASSERT(0 <= index);
        ASSERT(static_cast<bsl::size_t>(index) < array->size());

        return manipulator(&array->value(index));
    }

    void bdlat_arrayResize(MyIntArray *array, int newSize)
    {
        ASSERT(array);
        ASSERT(0 <= newSize);

        array->resize(newSize);
    }

    // ACCESSORS
    template <class ACCESSOR>
    int bdlat_arrayAccessElement(const MyIntArray& array,
                                 ACCESSOR&         accessor,
                                 int               index)
    {
        ASSERT(0 <= index);
        ASSERT(static_cast<bsl::size_t>(index) < array.size());

        return accessor(array.value(index));
    }

    bsl::size_t bdlat_arraySize(const MyIntArray& array)
    {
        return array.size();
    }

    }  // close namespace mine
    }  // close enterprise namespace
//..
// Finally, we specialize the 'IsArray' and 'ElementType' meta-functions
// in the 'bdlat_ArrayFunctions' namespace for the
// 'mine::MyIntArray' type:
//..
    namespace BloombergLP {
    namespace bdlat_ArrayFunctions {

    // TRAITS
    template <>
    struct IsArray<mine::MyIntArray> : public bsl::true_type {
    };

    template <>
    struct ElementType<mine::MyIntArray> {
        typedef int Type;
    };

    }  // close namespace bdlat_ArrayFunctions
    }  // close enterprise namespace
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
    void usageMakeArray()
    {
        BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<mine::MyIntArray>::value);

        mine::MyIntArray array;
        ASSERT(0 == bdlat_ArrayFunctions::size(array));

        bdlat_ArrayFunctions::resize(&array, 8);
        ASSERT(8 == bdlat_ArrayFunctions::size(array));

        bdlat_ArrayFunctions::resize(&array, 4);
        ASSERT(4 == bdlat_ArrayFunctions::size(array));
    }
//..
// To perform operations on the elements of an array requires use of the
// functions that employ accessor and manipulator functors.  For example:
//..
    template <class ELEMENT_TYPE>
    class GetElementAccessor {

        // DATA
        ELEMENT_TYPE *d_element_p;

      public:
        // CREATORS
        explicit GetElementAccessor(ELEMENT_TYPE *value)
        : d_element_p(value)
        {
        }

        // MANIPULATORS
        int operator()(const ELEMENT_TYPE& elementValue)
        {
            *d_element_p = elementValue;
            return 0;
        }
    };

    template<class ELEMENT_TYPE>
    class SetElementManipulator {

        // DATA
        ELEMENT_TYPE d_value;

      public:
        // CREATORS
        SetElementManipulator(const ELEMENT_TYPE& value)
        : d_value(value)
        {
        }

        // ACCESSOR
        int operator()(ELEMENT_TYPE *element) const
        {
            *element = d_value;
            return 0;
        }
    };
//..
// Notice that these functors make few assumptions of 'ELEMENT_TYPE', merely
// that it is copy constructable and copy assignable.
//
// With these definitions we can now use the generic functions to set and
// get values from an 'mine::MyIntArray' object:
//..
    void usageArrayElements()
    {
        mine::MyIntArray array;
        bdlat_ArrayFunctions::resize(&array, 4);

        // Confirm initial array elements from resize.

        int                     value;
        GetElementAccessor<int> accessor(&value);

        for (int index = 0; index < 4; ++index) {
            int rc = bdlat_ArrayFunctions::accessElement(array,
                                                         accessor,
                                                         index);
            ASSERT(0 == rc);
            ASSERT(0 == value)
        }

        // Set element 'index * 10' as its value;

        for (int index = 0; index < 4; ++index) {
            SetElementManipulator<int> manipulator(index * 10);

            int rc = bdlat_ArrayFunctions::manipulateElement(&array,
                                                             manipulator,
                                                             index);
            ASSERT(0 == rc);
        }

        // Confirm new value of each element.

        for (int index = 0; index < 4; ++index) {
            int rc = bdlat_ArrayFunctions::accessElement(array,
                                                         accessor,
                                                         index);
            ASSERT(0          == rc);
            ASSERT(index * 10 == value);
        }
    }
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
    struct ArrayUtil {

        // CLASS METHODS
        template <class ARRAY_TYPE>
        static int getElement(typename bdlat_ArrayFunctions
                                       ::ElementType<ARRAY_TYPE>::Type *value,
                            const ARRAY_TYPE&                           object,
                            int                                         index)
            // Load to the specified 'value' the element at the specified
            // 'index' of the specified 'object' array.  Return 0 if the
            // element is successfully loaded to 'value', and a non-zero value
            // otherwise.  This function template requires that the specified
            // 'ARRAY_TYPE' is a 'bdlat' "array" type.  The behavior is
            // undefined unless '0 <= index' and
            // 'index < bdlat_ArrayFunctions::size(object)'.
        {
            BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<ARRAY_TYPE>::value);

            typedef typename bdlat_ArrayFunctions
                                   ::ElementType<ARRAY_TYPE>::Type ElementType;

            GetElementAccessor<ElementType> elementAccessor(value);

            return bdlat_ArrayFunctions::accessElement(object,
                                                       elementAccessor,
                                                       index);
        }

        template <class ARRAY_TYPE>
        static int setElement(
         ARRAY_TYPE                                                    *object,
         int                                                            index,
         const typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>
                                                               ::Type&  value)
            // Assign the specified 'value' to the element of the specified
            // 'object' array at the specified 'index'.  Return 0 if the
            // element is successfully assigned to 'value', and a non-zero
            // value otherwise.  This function template requires that the
            // specified 'ARRAY_TYPE' is a 'bdlat' "array" type.  The behavior
            // is undefined unless '0 <= index' and
            // 'index < bdlat_ArrayFunctions::size(*object)'.
        {
            BSLMF_ASSERT(bdlat_ArrayFunctions::IsArray<ARRAY_TYPE>::value);

            typedef typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>
                                                            ::Type ElementType;

            SetElementManipulator<ElementType> manipulator(value);

            return bdlat_ArrayFunctions::manipulateElement(object,
                                                           manipulator,
                                                           index);
        }
    };
//..
// Now, we can use these functors to write generic utility functions for
// getting and setting the value types of arbitrary "array" classes.
//..
    void myUsageScenario()
    {
        mine::MyIntArray array;
        bdlat_ArrayFunctions::resize(&array, 4);

        // Confirm initial values.

        for (int index = 0; index < 4; ++index) {
            int value;
            int rc = ArrayUtil::getElement(&value, array, index);
            ASSERT(0 == rc);
            ASSERT(0 == value);
        }

        // Set element 'index * 10' as its value;

        for (int index = 0; index < 4; ++index) {
            int value = index * 10;
            int rc     = ArrayUtil::setElement(&array, index, value);
            ASSERT(0 == rc);
        }

        // Confirm value of each element.

        for (int index = 0; index < 4; ++index) {
            int value;
            int rc = ArrayUtil::getElement(&value, array, index);
            ASSERT(0           == rc);
            ASSERT(index * 10 == value);
        }
    }
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
//          ASSERT(index < d_size);
//
//          return d_data_p[index];
//      }
//
//      // ACCESSORS
//      const float& element(bsl::size_t index) const
//      {
//          ASSERT(index < d_size);
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
    void yourUsageScenario()
    {
        your::YourFloatArray array;
        bdlat_ArrayFunctions::resize(&array, 4);

        // Confirm initial values.

        for (int index = 0; index < 4; ++index) {
            float value;
            int   rc = ArrayUtil::getElement(&value, array, index);
            ASSERT(0   == rc);
            ASSERT(0.0 == value);
        }

        // Set element 'index * 10' as its value;

        for (int index = 0; index < 4; ++index) {
            float value = static_cast<float>(index * 10);
            int   rc    = ArrayUtil::setElement(&array, index, value);
            ASSERT(0 == rc);
        }

        // Confirm value of each element.

        for (int index = 0; index < 4; ++index) {
            float value;
            int   rc = ArrayUtil::getElement(&value, array, index);
            ASSERT(0                              == rc);
            ASSERT(static_cast<float>(index * 10) == value);
        }
    }
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
    void anotherUsageScenario()
    {
        bsl::vector<bsl::string> array;  // STANDARD ARRAY TYPE
        bdlat_ArrayFunctions::resize(&array, 4);

        // Confirm initial values.

        for (int index = 0; index < 4; ++index) {
            bsl::string value;
            int         rc = ArrayUtil::getElement(&value, array, index);
            ASSERT(0  == rc);
            ASSERT("" == value);
        }

        // Set element 'index * 10' as its value;

        for (int index = 0; index < 4; ++index) {
            bsl::ostringstream oss; oss << (index * 10);
            int rc = ArrayUtil::setElement(&array, index, oss.str());
            ASSERT(0 == rc);
        }

        // Confirm value of each element.

        for (int index = 0; index < 4; ++index) {
            bsl::string value;
            int         rc = ArrayUtil::getElement(&value, array, index);

            bsl::ostringstream oss; oss << (index * 10);

            ASSERT(0         == rc);
            ASSERT(oss.str() == value);
        }
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        usageMakeArray();
        usageArrayElements();

             myUsageScenario();
           yourUsageScenario();
        anotherUsageScenario();

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING META-FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   struct IsArray
        //   struct ElementType
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting meta-functions"
                          << "\n======================" << endl;

        ASSERT(0 == bdlat_ArrayFunctions::IsArray<int>::value);

        typedef
            Obj::ElementType<Test::FixedArray<9, short> >::Type FAElementType;
        ASSERT(1 ==
           (bdlat_ArrayFunctions::IsArray<Test::FixedArray<3, char> >::value));
        ASSERT(1 == (bslmf::IsSame<FAElementType, short>::value));

        typedef Obj::ElementType<bsl::vector<int> >::Type VecElementType;
        ASSERT(1 == bdlat_ArrayFunctions::IsArray<bsl::vector<int> >::value);
        ASSERT(1 == (bslmf::IsSame<VecElementType, int>::value));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // METHOD FORWARDING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "METHOD FORWARDING TEST" << endl
                          << "======================" << endl;

        {
            if (verbose) cout << "Testing forwarding with Test::FixedArray"
                              << endl;
            Test::FixedArray<10, int> mV; Test::FixedArray<10, int>& V = mV;
            mV.append(66);
            mV.append(77);

            ASSERT(2 == Obj::size(V));

            int              value;
            GetValue<int>    getter(&value);
            AssignValue<int> setter1(33);
            AssignValue<int> setter2(44);

            Obj::accessElement(V, getter, 0); ASSERT(66 == value);
            Obj::accessElement(V, getter, 1); ASSERT(77 == value);

            Obj::manipulateElement(&mV, setter1, 0);
            Obj::manipulateElement(&mV, setter2, 1);

            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);

            Obj::resize(&mV, 5);
            ASSERT(5 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 3); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 4); ASSERT( 0 == value);

            Obj::resize(&mV, 3);
            ASSERT(3 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);

            Obj::resize(&mV, 0);
            ASSERT(0 == Obj::size(V));
        }

        {
            if (verbose) cout << "Testing vector specialization" << endl;
            bsl::vector<int> mV;  const bsl::vector<int>& V = mV;
            mV.push_back(66);
            mV.push_back(77);

            ASSERT(2 == Obj::size(V));

            int              value;
            GetValue<int>    getter(&value);
            AssignValue<int> setter1(33);
            AssignValue<int> setter2(44);

            Obj::accessElement(V, getter, 0); ASSERT(66 == value);
            Obj::accessElement(V, getter, 1); ASSERT(77 == value);

            Obj::manipulateElement(&mV, setter1, 0);
            Obj::manipulateElement(&mV, setter2, 1);

            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);

            Obj::resize(&mV, 5);
            ASSERT(5 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 3); ASSERT( 0 == value);
            Obj::accessElement(V, getter, 4); ASSERT( 0 == value);

            Obj::resize(&mV, 3);
            ASSERT(3 == Obj::size(V));
            Obj::accessElement(V, getter, 0); ASSERT(33 == value);
            Obj::accessElement(V, getter, 1); ASSERT(44 == value);
            Obj::accessElement(V, getter, 2); ASSERT( 0 == value);

            Obj::resize(&mV, 0);
            ASSERT(0 == Obj::size(V));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

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
