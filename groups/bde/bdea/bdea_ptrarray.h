// bdea_ptrarray.h           -*-C++-*-
#ifndef INCLUDED_BDEA_PTRARRAY
#define INCLUDED_BDEA_PTRARRAY

//@PURPOSE: Provide an in-place array of 'T *' values.
//
//@DEPRECATED: This component is being replaced with 'std::vector'.
//
//@CLASSES:
//   bdea_PtrArray: memory manager for in-place array of 'T *' values
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@SEE_ALSO: bdea_ptrarray2
//
//@DESCRIPTION: This component implements an in-place array of 'T *' values.
// The contained elements are stored contiguously in memory and can be accessed
// directly via the 'const' and non-'const' 'data()' methods.  Note that
// appending, inserting, and removing elements can potentially alter the memory
// addresses of other elements in the array.  Consider instead using
// 'bdea_ptrarray2' if the memory address of each contained element must
// remain constant for the life of the element, or to improve runtime
// performance if insert/remove operations are frequent.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
// Here the average case, A[f(N)], is the amortized cost, which is defined as
// the cost of 'N' successive invocations of the operation divided by 'N'.
//..
//     Operation           Worst Case          Average Case
//     ---------           ----------          ------------
//     DEFAULT CTOR        O[1]
//     COPY CTOR(N)        O[N]
//     N.DTOR()            O[1]
//     N.OP=(M)            O[M]
//     OP==(N,M)           O[min(N,M)]
//
//     N.append(value)     O[N]                A[1]
//     N.insert(value)     O[N]
//     N.replace(value)    O[1]
//     N.remove(index)     O[N]
//
//     N.OP[]()            O[1]
//     N.length()          O[1]
//..
///USAGE
///-----
// The following snippets of code illustrate how to create and use an array.
// First create an empty 'bdea_PtrArray<int>' 'a' to contain pointers to 'int'
// and populate it with two elements 'E1' and 'E2':
//..
//      int value1;
//      int value2;
//      int *E1 = &value1;
//      int *E2 = &value2;
//
//      bdea_PtrArray<int> a;           assert( 0 == a.length());
//
//      a.append(E1);                   assert( 1 == a.length());
//                                      assert(E1 == a[0]);
//
//      a.append(E2);                   assert( 2 == a.length());
//                                      assert(E1 == a[0]);
//                                      assert(E2 == a[1]);
//..
// Now assign a new value, 'E3', to the first element (index position 0) of
// array 'a'.
//..
//      int value3;
//      int * const E3 = &value3;
//
//      a[0] = E3;                      assert( 2 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E2 == a[1]);
//..
// Then insert a new value in the middle (index position 1) of array 'a'.
//..
//      int value4;
//      int * const E4 = &value4;
//
//      a.insert(1, E4);                assert( 3 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E4 == a[1]);
//                                      assert(E2 == a[2]);
//..
// Finally, remove the elements from array 'a':
//..
//      a.remove(2);                    assert( 2 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E4 == a[1]);
//
//      a.remove(0);                    assert( 1 == a.length());
//                                      assert(E4 == a[0]);
//
//      a.remove(0);                    assert( 0 == a.length());
//..
// Note that specifying an index outside of the valid range will result in
// undefined behavior.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_CHARARRAY
#include <bdema_chararray.h>
#endif

#ifndef INCLUDED_BDEU_PRINT
#include <bdeu_print.h>
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#define INCLUDED_OSTREAM
#endif

namespace BloombergLP {

class bdema_Allocator;

                        // ===================
                        // class bdea_PtrArray
                        // ===================

template <class T>
class bdea_PtrArray {
    // This template class implements an efficient, in-place array of 'T *'
    // values stored contiguously in memory where T is the template parameter.
    // The physical capacity of this array may grow, but never shrinks.
    // Capacity may be reserved initially via a  constructor, or at any time
    // thereafter by using the 'reserveCapacity' method; otherwise capacity
    // will  be increased automatically as needed.  The address returned by the
    // 'data()' methods is guaranteed not to change unless the logical length
    // exceeds the current capacity.  Note that capacity is not a logical
    // attribute of this object, and is not a part of its value.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison and 'ostream' printing.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.) This container
    // is *exception* *neutral* with no guarantee of rollback: if an exception
    // is thrown during the invocation of a method on a pre-existing instance,
    // the container is left in a valid state, but its value is undefined.  In
    // no event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

    bdema_CharArray d_array;

  private:
    static void initializeWithValue(T **array_p, T *value, int length);
        // Initialize each of the specified 'length' elements of the specified
        // 'array_p' array with the specified 'value'.  The behavior is
        // undefined unless 0 <= length.  Note that 'array_p' must refer
        // to sufficient memory to hold 'length' values.

  public:
    // TYPES
    struct Explicit { int d_i; Explicit(int i) : d_i(i) { } ~Explicit() { } };
        // Workaround until the keyword 'explicit' becomes ubiquitous.

    struct InitialCapacity { int d_i; ~InitialCapacity() { }
                        InitialCapacity(const Explicit& i) : d_i(i.d_i) { } };
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdea_PtrArray x(bdea_PtrArray::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

    // CREATORS
    bdea_PtrArray(bdema_Allocator *basicAllocator = 0);
    bdea_PtrArray(const Explicit&  initialLength,
                  bdema_Allocator *basicAllocator = 0);
    bdea_PtrArray(const Explicit&  initialLength,
                  T               *initialValue,
                  bdema_Allocator *basicAllocator = 0);
        // Create an in-place array.  By default, the array is empty.
        // Optionally specify the 'initialLength' of the array.  Array elements
        // are initialized with the specified 'initialValue', or to 0 if
        // 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 0 <= initialLength.

    bdea_PtrArray(const InitialCapacity&  numElements,
                  bdema_Allocator        *basicAllocator = 0);
        // Create an in-place array with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  The address returned by the 'data()'
        // methods is guaranteed not to change unless 'length()' exceeds
        // 'numElements'.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless 
        // 0 <= numElements.

    bdea_PtrArray(T        * const *srcArray,
                  int               numElements,
                  bdema_Allocator  *basicAllocator = 0);
        // Create an in-place array initialized with the specified
        // 'numElements' leading values from the specified 'srcArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 0 <= numElements.  Note that
        // 'srcArray' must refer to sufficient memory to hold 'numElements'
        // values.

    bdea_PtrArray(const bdea_PtrArray<T>& original,
                  bdema_Allocator        *basicAllocator = 0);
        // Create an in-place array initialized to the value of the specified
        // 'original' array.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdea_PtrArray();
        // Destroy this object.

    // MANIPULATORS
    bdea_PtrArray<T>& operator=(const bdea_PtrArray<T>& rhs);
        // Assign to this array the value of the specified 'rhs' array and
        // return a reference to this modifiable array.

    T *& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    void append(T *item);
        // Append to the end of this array the value of the specified 'item'.
        // Note that this function is logically equivalent to the following:
        // insert(length(), item).

    void append(const bdea_PtrArray& srcArray);
        // Append to the end of this array the values in the specified
        // 'srcArray'.  Note that this function is logically equivalent to the
        // following: insert(length(), srcArray).

    void append(const bdea_PtrArray& srcArray,
                int                  srcIndex,
                int                  numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless 0 <= srcIndex,
        // 0 <= numElements, and srcIndex + numElements <= srcArray.length().
        // Note that this function is logically equivalent to the following:
        // insert(length(), srcArray, srcIndex, numElements).

    void append(T * const *srcArray,
                int        srcIndex,
                int        numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless 0 <= srcIndex,
        // 0 <= numElements, and srcIndex + numElements <= srcArray.length().
        // Note that this function is logically equivalent to the following:
        // insert(length(), srcArray, srcIndex, numElements).

    T **data();
        // Return the address of the internal memory representation of the
        // contiguous, modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    void insert(int dstIndex, T *item);
        // Insert into this array at the specified 'dstIndex' the value of the
        // specified 'item'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by one index position.  The behavior is
        // undefined unless 0 <= dstIndex <= length().

    void insert(int dstIndex, const bdea_PtrArray& srcArray);
        // Insert into this array, beginning the specified 'dstIndex', the
        // values in the specified 'srcArray'.  All values with initial indices
        // at or above 'dstIndex' are shifted up by 'srcArray.length()' index
        // positions.  The behavior is undefined unless
        // 0 <= dstIndex <= length().

    void insert(int                  dstIndex,
                const bdea_PtrArray& srcArray,
                int                  srcIndex,
                int                  numElements);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numElements' values from the specified 'srcArray',
        // beginning at the specified 'srcIndex'.  All values with initial
        // indices at or above 'dstIndex' are shifted up by 'numElements' index
        // positions.  The behavior is undefined unless
        // 0 <= dstIndex <= length(), 0 <= srcIndex, 0 <= numElements, and
        // srcIndex + numElements <= srcArray.length().

    void insert(int        dstIndex,
                T * const *srcArray,
                int        srcIndex,
                int        numElements);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numElements' values from the specified 'srcArray',
        // beginning at the specified 'srcIndex'.  All values with initial
        // indices at or above 'dstIndex' are shifted up by 'numElements' index
        // positions.  The behavior is undefined unless
        // 0 <= dstIndex <= length(), 0 <= srcIndex, 0 <= numElements, and
        // srcIndex + numElements <= srcArray.length().

    void remove(int index);
        // Remove from this array the value at the specified 'index'.  All
        // values with initial indices above 'index' are shifted down by one
        // index position.  The behavior is undefined unless
        // 0 <= index < length().

    void remove(int index, int numElements);
        // Remove from this array, beginning at the specified 'index', the
        // specified 'numElements' values.  All values with initial indices at
        // or above 'index' + 'numElements' are shifted down by 'numElements'
        // index positions.  The behavior is undefined unless 0 <= index,
        // 0 <= numElements, and index + numElements <= length().

    void removeAll();
        // Remove all elements from this array.  Note that 'length()' is now 0.

    void replace(int dstIndex, T *item);
        // Replace the value at the specified 'dstIndex' in this array with the
        // value of the specified 'item'.  The behavior is undefined unless
        // 0 <= dstIndex < length().  Note that this function is provided only
        // for consistency with 'bdea_Array' and is logically equivalent to
        // (*this)[dstIndex] = item;

    void replace(int                  dstIndex,
                 const bdea_PtrArray& srcArray,
                 int                  srcIndex,
                 int                  numElements);
        // Replace the specified 'numElements' values beginning at the
        // specified 'dstIndex' in this array with values from the specified
        // 'srcArray' beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless 0 <= dstIndex, 0 <= srcIndex, 0 <= numElements,
        // dstIndex + numElements <= length(), and
        // srcIndex + numElements <= srcArray.length().  Note that this
        // function is logically equivalent to (but is more efficient than)
        //..
        //   remove(dstIndex, numElements);
        //   insert(dstIndex, srcArray, srcIndex, numElements)
        //..
        // except that problems resulting from aliasing are handled correctly.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The address
        // returned by the 'data()' methods is guaranteed not to change unless
        // 'length()' subsequently exceeds 'numElements'.  The behavior is
        // undefined unless 0 <= numElements.  Note that if
        // numElements <= length(), this operation has no effect.
        // If an exception is thrown during reallocation, the value of the
        // array is left unchanged.

    void reserveCapacityRaw(int numElements);
        // Reserve *exactly* *enough* capacity to accommodate the specified
        // 'numElements' values without reallocation.  The address returned by
        // the 'data()' methods is guaranteed not to change unless 'length()'
        // subsequently exceeds 'numElements'.  The behavior is undefined
        // unless 0 <= numElements.  Note that if numElements <= length(),
        // this operation has no effect.
        // Warning: Inappropriate use of this method such as repeatedly calling
        //..
        //    reserveCapacityRaw(a.length() + 1);
        //    a.append( ... );
        //..
        // may lead to very poor runtime behavior.

    void setLengthRaw(int newLength);
        // Set the length of the this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are left _uninitialized_.
        // The behavior is undefined unless 0 <= newLength.

    void setLength(int newLength);
    void setLength(int newLength, T *initialValue);
        // Set the length of this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to 0 if 'initialValue' is not
        // specified.  The behavior is undefined unless 0 <= newLength.

    void swap(int index1, int index2);
        // Efficiently swap the values at the specified indices, 'index1' and
        // 'index2'.  The behavior is undefined unless 0 <= index1 < length()
        // and 0 <= index2 < length().

    // ACCESSORS
    T * const & operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    T * const *data() const;
        // Return the address of the internal memory representation of the
        // contiguous, non-modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    int length() const;
        // Return the number of elements in this array.

    std::ostream& print(std::ostream& stream,
                   int      level,
                   int      spacesPerLevel) const;
        // Format the element values in this array to the specified output
        // 'stream' at the (absolute value of the) specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.  Making
        // 'level' negative suppress indentation for the first line only.  The
        // behavior is undefined unless 0 <= spacesPerLevel.

    int isEqual(const bdea_PtrArray<T>& rhs) const;
        // Return 1 if the this and 'rhs' arrays have the same value,
        // and 0 otherwise.  Two arrays have the same value if they have the
        // same length and same element value at each respective index
        // position.  Called by the operator== free operator.

    std::ostream& ostreamOut(std::ostream& stream) const;
        // Write this array to the specified output 'stream' in some
        // reasonable (single-line) format, and return a reference to 'stream'.
        // Called by the operator<< free operator.
};

// FREE OPERATORS
template <class T>
inline
int operator==(const bdea_PtrArray<T>& lhs, const bdea_PtrArray<T>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays have the same value,
    // and 0 otherwise.  Two arrays have the same value if they have the same
    // length and same element value at each respective index position.

template <class T>
inline
int operator!=(const bdea_PtrArray<T>& lhs, const bdea_PtrArray<T>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays do not have the same
    // value, and 0 otherwise.  Two arrays have do not have the same value if
    // they have different lengths or differ in at least one index position).

template <class T>
inline
std::ostream& operator<<(std::ostream& stream, const bdea_PtrArray<T>& array);
    // Write the specified 'array' to the specified output 'stream' in some
    // reasonable (single-line) format, and return a reference to 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

              // ---------------------------------------------
              // inlined methods used by other inlined methods
              // ---------------------------------------------

template <class T>
inline
T **bdea_PtrArray<T>::data()
{
    return (T **) d_array.data();
}

template <class T>
inline
T * const * bdea_PtrArray<T>::data() const
{
    return (T *const *) d_array.data();
}

template <class T>
inline
int bdea_PtrArray<T>::length() const
{
    return d_array.length() / sizeof(T *);
}

                        // ---------------
                        // PRIVATE METHODS
                        // ---------------

template <class T>
inline
void bdea_PtrArray<T>::initializeWithValue(T    **array_p,
                                           T     *value,
                                           int    length)
{
    for (int i = 0; i < length; ++i) {
        array_p[i] = value;
    }
}

                                // ------------
                                // MANIPULATORS
                                // ------------

template <class T>
inline
T *& bdea_PtrArray<T>::operator[](int index)
{
    return (T *&) data()[index];
}

template <class T>
inline
void bdea_PtrArray<T>::append(T *item)
{
    int len = length();
    d_array.setLengthRaw(d_array.length() + sizeof(T *));
    data()[len] = item;
}

template <class T>
inline
void bdea_PtrArray<T>::removeAll()
{
    d_array.removeAll();
}

template <class T>
inline
void bdea_PtrArray<T>::replace(int dstIndex, T *item)
{
    data()[dstIndex] = item;
}

template <class T>
inline
void bdea_PtrArray<T>::swap(int index1, int index2)
{
    T **array_p = data();
    T * tmp = array_p[index1];
    array_p[index1] = array_p[index2];
    array_p[index2] = tmp;
}

                                // ---------
                                // ACCESSORS
                                // ---------

template <class T>
inline
T * const & bdea_PtrArray<T>::operator[](int index) const
{
    return (T * const&) data()[index];
}

                        // --------
                        // CREATORS
                        // --------

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(bdema_Allocator *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(sizeof(T *)), basicAllocator)
{
}

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(const Explicit&  initialLength,
                                bdema_Allocator *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(initialLength.d_i > 0
                                           ? initialLength.d_i * sizeof(T *)
                                           : sizeof(T *)),
          bdema_CharArray::POWER_OF_2_HINT,
          basicAllocator)
{
    d_array.setLengthRaw(initialLength.d_i * sizeof(T *));
    std::memset(data(), 0, initialLength.d_i * sizeof(T *));
}

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(const Explicit&  initialLength,
                                T               *initialValue,
                                bdema_Allocator *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(initialLength.d_i > 0
                                           ? initialLength.d_i * sizeof(T *)
                                           : sizeof(T *)),
          bdema_CharArray::POWER_OF_2_HINT,
          basicAllocator)
{
    d_array.setLengthRaw(initialLength.d_i * sizeof(T *));
    bdea_PtrArray<T>::initializeWithValue(data(),
                                          initialValue,
                                          initialLength.d_i);
}

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(const InitialCapacity&  numElements,
                                bdema_Allocator        *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(numElements.d_i > 0
                                           ? numElements.d_i * sizeof(T *)
                                           : sizeof(T *)),
          basicAllocator)
{
}

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(T * const       *srcArray,
                                int              numElements,
                                bdema_Allocator *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(numElements > 0
                                           ? numElements * sizeof(T *)
                                           : sizeof(T *)),
          bdema_CharArray::POWER_OF_2_HINT,
          basicAllocator)
{
    d_array.setLengthRaw(numElements * sizeof(T *));
    std::memcpy(data(), srcArray, numElements * sizeof(T *));
}

template <class T>
inline
bdea_PtrArray<T>::bdea_PtrArray(const bdea_PtrArray<T>&  original,
                                bdema_Allocator         *basicAllocator)
: d_array(bdema_CharArray::InitialCapacity(original.length() > 0
                                           ? original.length() * sizeof(T *)
                                           : sizeof(T *)),
          bdema_CharArray::POWER_OF_2_HINT,
          basicAllocator)
{
    d_array.setLengthRaw(original.length() * sizeof(T *));
    std::memcpy(data(), original.data(), original.length() * sizeof(T *));
}

template <class T>
inline
bdea_PtrArray<T>::~bdea_PtrArray()
{
}

                        // ------------
                        // MANIPULATORS
                        // ------------

template <class T>
inline
bdea_PtrArray<T>& bdea_PtrArray<T>::operator=(const bdea_PtrArray<T>& rhs)
{
    d_array = rhs.d_array;

    return *this;
}

template <class T>
inline
void bdea_PtrArray<T>::append(const bdea_PtrArray<T>& srcArray)
{
    d_array.append(srcArray.d_array);
}

template <class T>
inline
void bdea_PtrArray<T>::append(const bdea_PtrArray<T>& srcArray,
                              int                     srcIndex,
                              int                     numElements)
{
    d_array.append(srcArray.d_array,
                   srcIndex * sizeof(T *),
                   numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::append(T * const *srcArray,
                              int        srcIndex,
                              int        numElements)
{
    d_array.append((const char *)srcArray,
                   srcIndex * sizeof(T *),
                   numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::insert(int dstIndex, T *item)
{
    d_array.insert(dstIndex * sizeof(T *),
                   (const char *)&item,
                   0,
                   sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::insert(int dstIndex, const bdea_PtrArray<T>& srcArray)
{
    d_array.insert(dstIndex * sizeof(T *), srcArray.d_array);
}

template <class T>
inline
void bdea_PtrArray<T>::insert(int                      dstIndex,
                              const bdea_PtrArray<T>&  srcArray,
                              int                      srcIndex,
                              int                      numElements)
{
    d_array.insert(dstIndex * sizeof(T *),
                   srcArray.d_array,
                   srcIndex * sizeof(T *),
                   numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::insert(int        dstIndex,
                              T * const *srcArray,
                              int        srcIndex,
                              int        numElements)
{
    d_array.insert(dstIndex * sizeof(T *),
                   (const char *)srcArray,
                   srcIndex * sizeof(T *),
                   numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::remove(int index)
{
    d_array.remove(index * sizeof(T *), sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::remove(int index, int numElements)
{
    d_array.remove(index * sizeof(T *), numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::replace(int                      dstIndex,
                               const bdea_PtrArray<T>&  srcArray,
                               int                      srcIndex,
                               int                      numElements)
{
    d_array.replace(dstIndex * sizeof(T *),
                    srcArray.d_array,
                    srcIndex * sizeof(T *),
                    numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::reserveCapacity(int numElements)
{
    d_array.reserveCapacity(numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::reserveCapacityRaw(int numElements)
{
    d_array.reserveCapacityRaw(numElements * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::setLengthRaw(int newLength)
{
    d_array.setLengthRaw(newLength * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::setLength(int newLength)
{
    d_array.setLength(newLength * sizeof(T *));
}

template <class T>
inline
void bdea_PtrArray<T>::setLength(int newLength, T *initialValue)
{
    int len = length();
    d_array.setLengthRaw(newLength * sizeof(T *));
    if (newLength > len) {
        bdea_PtrArray<T>::initializeWithValue(data() + len,
                                              initialValue,
                                              newLength - len);
    }
}

                        // ---------
                        // ACCESSORS
                        // ---------

template <class T>
inline
std::ostream& bdea_PtrArray<T>::print(std::ostream& stream,
                                 int      level,
                                 int      spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[\n";

    if (level < 0) {
        level = -level;
    }

    int levelPlus1 = level + 1;
    int len = length();
    T * const *array_p = data();

    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        bdeu_Print::printPtr(stream, array_p[i]);
        stream << '\n';
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    return stream << "]\n";
}

template <class T>
inline
std::ostream& bdea_PtrArray<T>::ostreamOut(std::ostream& stream) const
{
    return stream << *this;
}

template <class T>
inline
int bdea_PtrArray<T>::isEqual(const bdea_PtrArray<T>& rhs) const
{
    return d_array == rhs.d_array;
}

                        // --------------
                        // FREE OPERATORS
                        // --------------

template <class T>
inline
int operator==(const bdea_PtrArray<T>& lhs, const bdea_PtrArray<T>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class T>
inline
int operator!=(const bdea_PtrArray<T>& lhs, const bdea_PtrArray<T>& rhs)
{
    return !(lhs == rhs);
}

template <class T>
inline
std::ostream& operator<<(std::ostream& stream, const bdea_PtrArray<T>& array)
{
    int len = array.length();
    T * const *array_p = array.data();

    stream << '[';
    for (int i = 0; i < len; ++i) {
        stream << ' ';
        bdeu_Print::printPtr(stream, array_p[i]);
    }
    return stream << " ]";
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
