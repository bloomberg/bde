// bdema_chararray.h           -*-C++-*-
#ifndef INCLUDED_BDEMA_CHARARRAY
#define INCLUDED_BDEMA_CHARARRAY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-place array of 'char' values.
//
//@DEPRECATED: Use 'bsl::vector<char>' instead.
//
//@CLASSES:
//   bdema_CharArray: memory manager for in-place array of 'char' values
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements an in-place array of 'char' values.
// The contained elements are stored contiguously in memory and can be accessed
// directly via the 'const' and non-'const' 'data()' methods.  Note that
// appending, inserting, and removing elements can potentially alter the memory
// addresses of other elements in the array.  (Note that this component does
// not support 'bdex' serialization).
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, 'O[f(N,M)]', where the names 'N' and 'M' also refer
// to the number of respective elements in each container (i.e., its
// 'length()').  Here the average case, 'A[f(N)]', is the amortized cost, which
// is defined as the cost of 'N' successive invocations of the operation
// divided by 'N':
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
///Usage
///-----
// The following snippets of code illustrate how to create and use an array.
// First create an empty 'bdema_CharArray' 'a' and populate it with two
// elements 'E1' and 'E2':
//..
//      const char E1 = 'A';
//      const char E2 = 'B';
//
//      bdema_CharArray a;              assert( 0 == a.length());
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
//      const char E3 = 'C';
//
//      a[0] = E3;                      assert( 2 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E2 == a[1]);
//..
// Then insert a new value in the middle (index position 1) of array 'a'.
//..
//      const char E4 = 'D';
//
//      a.insert(1, E4);                assert( 3 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E4 == a[1]);
//                                      assert(E2 == a[2]);
//..
// Next, iterate over the elements in 'a', printing them in increasing order of
// their index positions [0 .. a.length() - 1].
//..
//      bsl::cout << '[';
//      int len = a.length();
//      for (int i = 0; i < len; ++i) {
//          bsl::cout << ' ' << a[i];
//      }
//      bsl::cout << " ]" << bsl::endl;
//..
// which produces the following output on 'stdout':
//..
//      [ C D B ]
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

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // =====================
                        // class bdema_CharArray
                        // =====================

class bdema_CharArray {
    // This class implements an efficient, in-place array of 'char' values
    // stored contiguously in memory.  The physical capacity of this array may
    // grow, but never shrinks.  Capacity may be reserved initially via a
    // constructor, or at any time thereafter by using the 'reserveCapacity'
    // method; otherwise capacity will be increased automatically as needed.
    // The address returned by the 'data()' methods is guaranteed not to change
    // unless the logical length exceeds the current capacity.  Note that
    // capacity is not a logical attribute of this object, and is not a part of
    // its value.
    //
    // More generally, this container class supports an abridged set of
    // *value* *semantic* operations, including copy construction, assignment,
    // and equality comparison.  (A precise operational definition of when two
    // instances have the same value can be found in the description of
    // 'operator==' for the class).  This container is *exception* *neutral*
    // with no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing instance, the container is
    // left in a valid state, but its value is undefined.  In no event is
    // memory leaked.  Finally, *aliasing* (e.g., using all or part of an
    // object as both source and destination) is supported in all cases.

    // DATA
    char *d_array_p;  // dynamically allocated array ('d_size' elements)
    int   d_size;     // physical capacity of this array (in elements)
    int   d_length;   // logical length of this array (in elements)

    bslma_Allocator *d_allocator_p; // holds (but doesn't own) memory allocator

  private:
    // PRIVATE MANIPULATORS
    void increaseSize();
        // Increase the physical capacity of this array by at least one
        // element.

    void reserveCapacityImp(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The address
        // returned by the 'data()' methods is guaranteed not to change unless
        // 'length()' subsequently exceeds 'numElements'.  The behavior is
        // undefined unless '0 <= numElements'.  If an exception is thrown
        // during reallocation, the value of the array is left unchanged.  Note
        // that this function is called only when insufficient capacity exists.

  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdema_CharArray x(bdema_CharArray::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

        int d_i;
        explicit InitialCapacity(int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    enum Hint {
        // Indicates allocation strategy for capacity-reserving constructor.

        EXACT_SIZE_HINT = 0,  // Reserve exactly the requested size.
        POWER_OF_2_HINT       // Round size up to nearest power of 2.
    };

    // CREATORS
    bdema_CharArray(bslma_Allocator *basicAllocator = 0);
    explicit
    bdema_CharArray(int              initialLength,
                    bslma_Allocator *basicAllocator = 0);
    bdema_CharArray(int              initialLength,
                    char             initialValue,
                    bslma_Allocator *basicAllocator = 0);
        // Create an in-place array.  By default, the array is empty.
        // Optionally specify the 'initialLength' of the array.  Array elements
        // are initialized with the specified 'initialValue', or to '\0' if
        // 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialLength'.
        //
        // Note: The default constructor should be 'explicit', but since this
        // package is below 'bdealg', we depend on convertibility from
        // 'bslma_Allocator*' for auto-detection of the 'UsesBdemaAllocator'
        // trait.

    explicit
    bdema_CharArray(const InitialCapacity& numElements,
                    bslma_Allocator       *basicAllocator = 0);
    bdema_CharArray(const InitialCapacity& numElements,
                    Hint                   sizingHint,
                    bslma_Allocator       *basicAllocator = 0);
        // Create an in-place array with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  The address returned by the 'data()'
        // methods is guaranteed not to change unless 'length()' exceeds
        // 'numElements'.  Optionally specify 'sizingHint' as
        // 'bdema_CharArray::EXACT_SIZE_HINT' to indicate that the initial
        // capacity will be *exactly* 'numElements', or as
        // 'bdema_CharArray::POWER_OF_2_HINT' to indicate that the initial
        // capacity will be rounded up from 'numElements' to the nearest
        // non-negative power of two.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 <= numElements'.

    bdema_CharArray(const char      *srcArray,
                    int              numElements,
                    bslma_Allocator *basicAllocator = 0);
        // Create an in-place array initialized with the specified
        // 'numElements' leading values from the specified 'srcArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= numElements'.  Note
        // that 'srcArray' must refer to sufficient memory to hold
        // 'numElements' values.

    bdema_CharArray(const bdema_CharArray&  original,
                    bslma_Allocator        *basicAllocator = 0);
        // Create an in-place array initialized to the value of the specified
        // 'original' array.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdema_CharArray();
        // Destroy this object.

    // MANIPULATORS
    bdema_CharArray& operator=(const bdema_CharArray& rhs);
        // Assign to this array the value of the specified 'rhs' array and
        // return a reference to this modifiable array.

    char& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    void append(char item);
        // Append to the end of this array the value of the specified 'item'.
        // Note that this function is logically equivalent to:
        //..
        //   insert(length(), item)
        //..

    void append(const bdema_CharArray& srcArray);
        // Append to the end of this array the values in the specified
        // 'srcArray'.  Note that this function is logically equivalent to the
        // following:
        //..
        //   insert(length(), srcArray)
        //..

    void append(const bdema_CharArray& srcArray,
                int                    srcIndex,
                int                    numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless '0 <= srcIndex',
        // '0 <= numElements', and
        // 'srcIndex + numElements <= srcArray.length()'.  Note that this
        // function is logically equivalent to:
        //..
        //   insert(length(), srcArray, srcIndex, numElements)
        //..

    void append(const char *srcArray, int srcIndex, int numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless 0 '<= srcIndex' and
        // '0 <= numElements'.  Note that 'srcArray' must refer to sufficient
        // memory to hold 'srcIndex + numElements' values.  Note that this
        // function is logically equivalent to:
        //..
        //   insert(length(), srcArray, srcIndex, numElements)
        //..

    char *data();
        // Return the address of the internal memory representation of the
        // contiguous, modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    void insert(int dstIndex, char item);
        // Insert into this array at the specified 'dstIndex' the value of the
        // specified 'item'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by one index position.  The behavior is
        // undefined unless '0 <= dstIndex <= length()'.

    void insert(int dstIndex, const bdema_CharArray& srcArray);
        // Insert into this array, beginning the specified 'dstIndex', the
        // values in the specified 'srcArray'.  All values with initial indices
        // at or above 'dstIndex' are shifted up by 'srcArray.length()' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insert(int                    dstIndex,
                const bdema_CharArray& srcArray,
                int                    srcIndex,
                int                    numElements);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numElements' values from the specified 'srcArray',
        // beginning at the specified 'srcIndex'.  All values with initial
        // indices at or above 'dstIndex' are shifted up by 'numElements' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', '0 <= srcIndex', '0 <= numElements',
        // and 'srcIndex + numElements <= srcArray.length()'.

    void insert(int         dstIndex,
                const char *srcArray,
                int         srcIndex,
                int         numElements);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numElements' values from the specified 'srcArray',
        // beginning at the specified 'srcIndex'.  All values with initial
        // indices at or above 'dstIndex' are shifted up by 'numElements' index
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', '0 <= srcIndex' and '0 <= numElements'.
        // Note that 'srcArray' must refer to sufficient memory to hold
        // 'srcIndex + numElements' values.

    void remove(int index);
        // Remove from this array the value at the specified 'index'.  All
        // values with initial indices above 'index' are shifted down by one
        // index position.  The behavior is undefined unless
        // '0 <= index < length()'.

    void remove(int index, int numElements);
        // Remove from this array, beginning at the specified 'index', the
        // specified 'numElements' values.  All values with initial indices at
        // or above 'index' + 'numElements' are shifted down by 'numElements'
        // index positions.  The behavior is undefined unless '0 <= index',
        // '0 <= numElements', and 'index + numElements <= length()'.

    void removeAll();
        // Remove all elements from this array.  Note that 'length()' is now 0.

    void replace(int dstIndex, char item);
        // Replace the value at the specified 'dstIndex' in this array with the
        // value of the specified 'item'.  The behavior is undefined unless
        // '0 <= dstIndex < length()'.  Note that this function is logically
        // equivalent to
        //..
        //   (*this)[dstIndex] = item;
        //..

    void replace(int                    dstIndex,
                 const bdema_CharArray& srcArray,
                 int                    srcIndex,
                 int                    numElements);
        // Replace the specified 'numElements' values beginning at the
        // specified 'dstIndex' in this array with values from the specified
        // 'srcArray' beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numElements', 'dstIndex + numElements <= length()', and
        // 'srcIndex + numElements <= srcArray.length()'.  Note that this
        // function is logically equivalent to (but is more efficient than):
        //..
        //   remove(dstIndex, numElements);
        //   insert(dstIndex, srcArray, srcIndex, numElements);
        //..
        // except that problems resulting from aliasing are handled correctly.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The address
        // returned by the 'data()' methods is guaranteed not to change unless
        // 'length()' subsequently exceeds 'numElements'.  The behavior is
        // undefined unless '0 <= numElements'.  Note that if
        // 'numElements <= length()', this operation has no effect.  If an
        // exception is thrown during reallocation, the value of the array is
        // left unchanged.

    void reserveCapacityRaw(int numElements);
        // Reserve *exactly* *enough* capacity to accommodate the specified
        // 'numElements' values without reallocation.  The address returned by
        // the 'data()' methods is guaranteed not to change unless 'length()'
        // subsequently exceeds 'numElements'.  The behavior is undefined
        // unless '0 <= numElements'.  Note that if 'numElements <= length()',
        // this operation has no effect.  Warning: Inappropriate use of this
        // method such as repeatedly calling
        //..
        //   reserveCapacityRaw(a.length() + 1);
        //   a.append( ... );
        //..
        // may lead to very poor runtime behavior.

    void setLengthRaw(int newLength);
        // Set the length of the this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are left _uninitialized_.
        // The behavior is undefined unless '0 <= newLength'.

    void setLength(int newLength);
    void setLength(int newLength, char initialValue);
        // Set the length of this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to '\0' if 'initialValue' is not
        // specified.  The behavior is undefined unless '0 <= newLength'.

    void swap(int index1, int index2);
        // Efficiently swap the values at the specified indices, 'index1' and
        // 'index2'.  The behavior is undefined unless '0 <= index1 < length()'
        // and '0 <= index2 < length()'.

    // ACCESSORS
    const char& operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // '0 <= index < length()'.

    const char *data() const;
        // Return the address of the internal memory representation of the
        // contiguous, non-modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range '[ data() .. data() + (length() - 1) ]'
        // is undefined.

    int length() const;
        // Return the number of elements in this array.
};

// FREE OPERATORS
bool operator==(const bdema_CharArray& lhs, const bdema_CharArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays have the same
    // value, and 'false' otherwise.  Two arrays have the same value if they
    // have the same length and same element value at each respective index
    // position.

inline
bool operator!=(const bdema_CharArray& lhs, const bdema_CharArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays do not have the
    // same value, and 'false' otherwise.  Two arrays have do not have the same
    // value if they have different lengths or differ in at least one index
    // position.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------------
                        // class bdema_CharArray
                        // ---------------------

// MANIPULATORS
inline
char& bdema_CharArray::operator[](int index)
{
    return d_array_p[index];
}

inline
char *bdema_CharArray::data()
{
    return d_array_p;
}

inline
void bdema_CharArray::append(char item)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_array_p[d_length++] = item;
}

inline
void bdema_CharArray::removeAll()
{
    d_length = 0;
}

inline
void bdema_CharArray::replace(int dstIndex, char item)
{
    d_array_p[dstIndex] = item;
}

inline
void bdema_CharArray::swap(int index1, int index2)
{
    char tmp = d_array_p[index1];
    d_array_p[index1] = d_array_p[index2];
    d_array_p[index2] = tmp;
}

inline void bdema_CharArray::reserveCapacity(int numElements)
{
    if (d_size < numElements) {
        reserveCapacityImp(numElements);
    }
}

inline
void bdema_CharArray::setLengthRaw(int newLength)
{
    if (newLength > d_size) {
        reserveCapacityImp(newLength);
    }
    d_length = newLength;
}

// ACCESSORS
inline
const char& bdema_CharArray::operator[](int index) const
{
    return d_array_p[index];
}

inline
const char *bdema_CharArray::data() const
{
    return d_array_p;
}

inline
int bdema_CharArray::length() const
{
    return d_length;
}

// FREE OPERATORS
inline
bool operator!=(const bdema_CharArray& lhs, const bdema_CharArray& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
