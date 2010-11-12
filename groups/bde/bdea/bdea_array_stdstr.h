// bdea_array_stdstr.h           -*-C++-*-
#ifndef INCLUDED_BDEA_ARRAY_STDSTR
#define INCLUDED_BDEA_ARRAY_STDSTR

//@PURPOSE: Provide an in-place array of 'std::string' values.
//
//@DEPRECATED: This component is being replaced with 'std::vector'.
//
//@CLASSES:
//   bdea_Array<std::string>: memory manager for in-place array of
//                            'std::string' values
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@SEE_ALSO:  bdea_array2_str
//
//@DESCRIPTION: This component implements an in-place array of 'std::string'
// values.  The contained elements are stored contiguously in memory.
// Note that appending, inserting, and removing elements can potentially alter
// the memory addresses of other elements in the array.  Consider instead using
// 'bdea_array2_str' if the memory address of each contained element must
// remain constant for the life of the element, or to improve runtime
// performance if insert/remove operations are frequent.
//
// A hint can be specified in the array's constructor indicating that deletes
// from the array are infrequent.  When the hint is specified, the array will
// use a different allocator 'bdema_strAllocator' built from the supplied
// allocator.  Memory allocated for strings will be allocated using this
// allocator instead of the supplied allocator.  The allocator will not release
// memory when strings are deleted from the array.  The std::string destructor
// is never run on elements on the array.  The string allocator will release
// the memory to the underlying allocator when the array's destructor is
// invoked, when the removeAll() is invoked, or when the array's assignment
// operator is invoked.
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
//     N.DTOR()            O[N]
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
///USAGE 1
///-------
// The following snippets of code illustrate how to create and use an array.
// First create an empty 'bdea_Array<std::string>' 'a' and populate it with two
// elements 'E1' and 'E2':
//..
//      const std::string E1 = "a";
//      const std::string E2 = "bb";
//
//      bdea_Array<std::string> a;      assert( 0 == a.length());
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
//      const std::string E3 = "ccc";
//
//      a[0] = E3;                      assert( 2 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E2 == a[1]);
//..
// Then insert a new value in the middle (index position 1) of array 'a'.
//..
//      const std::string E4 = "dddd";
//
//      a.insert(1, E4);                assert( 3 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E4 == a[1]);
//                                      assert(E2 == a[2]);
//..
// Next, iterate over the elements in 'a', printing them in increasing order of
// their index positions [0 .. a.length() - 1].
//..
//      std::cout << '[';
//      int len = a.length();
//      for (int i = 0; i < len; ++i) {
//          std::cout << ' ' << a[i];
//      }
//      std::cout << " ]" << std::endl;
//..
// which produces the following output on 'stdout':
//..
//      [ ccc dddd bb ]
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
//
///USAGE 2
///-------
// When the number of deletes from the array are infrequent, a hint can be
// specified in the constructor.  The logical behavior of the array is
// identical.  However, performance associated with memory allocation and
// deallocation may improve.
//..
//
//      bdea_Array<std::string>
//                          b(bdea_Array<std::string>::INFREQUENT_DELETE_HINT);
//
//      const std::string E5 = "d987654321543215";
//      const std::string E6 = "e1234567890123456";
//      b.insert (0, E5);               assert( 1 == b.length());
//                                      assert(E5 == b[0]);
//
//      b.insert (1, E6);               assert( 2 == b.length());
//                                      assert(E6 == b[1]);
//
//..
// The memory used by the string will not be deallocated when the element is
// removed from the array. (Note that if sizeof(std::string) is greater than
// the  number of characters in the string, no memory may be allocated.)
//..
//      b.remove (0);                   assert( 1 == b.length());
//..
// When the array destructor runs, all memory allocated to strings in the array
// is deallocated.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMMETHODS
#include <bdex_instreammethods.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMMETHODS
#include <bdex_outstreammethods.h>
#endif

#ifndef INCLUDED_BDEMA_STRALLOCATOR
#include <bdema_strallocator.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_STRING
#include <string>
#define INCLUDED_STRING
#endif

namespace BloombergLP {

class bdema_Allocator;
template <class T> class bdea_Array;

                      // =============================
                      // class bdea_Array<std::string>
                      // =============================

template<>
class bdea_Array<std::string> {
    // This class implements an efficient, in-place array of 'std::string'
    // values stored contiguously in memory.  The physical capacity of this
    // array may grow, but never shrinks.  Capacity may be reserved initially
    // via a constructor, or at any time thereafter by using the
    // 'reserveCapacity' method; otherwise capacity will be increased
    // automatically as needed.  Note that capacity is not a logical attribute
    // of this object, and is not a part of its value.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class).  This container
    // is *exception* *neutral* with no guarantee of rollback: if an exception
    // is thrown during the invocation of a method on a pre-existing instance,
    // the container is left in a valid state, but its value is undefined.  In
    // no event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

    std::string *d_array_p;    // dynamically allocated array (d_size elements)
    int d_size;                // physical capacity of this array (in elements)
    int d_length;              // logical length of this array (in elements)

    bdema_Allocator *d_allocator_p; // holds (but doesn't own) memory allocator
    bdema_Allocator *d_strAllocator_p; // supply memory for strings

  private:
    void appendImp(const std::string& item);
        // Append to the end of this array the value of the specified 'item'.
        // This method is used by inline append function when the array must
        // resize to accomodate the 'item'.

    void reserveCapacityImp(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The address
        // returned by the 'data()' methods is guaranteed not to change unless
        // 'length()' subsequently exceeds 'numElements'.  The behavior is
        // undefined unless 0 <= numElements.  If an exception is thrown during
        // reallocation, the value of the array is left unchanged.  Note that
        // this function is called only when insufficient capacity exists.

  public:
    // TYPES
    struct Explicit { int d_i; Explicit(int i) : d_i(i) { } ~Explicit() { } };
        // Workaround until the keyword 'explicit' becomes ubiquitous.

    struct InitialCapacity { int d_i;  ~InitialCapacity() { }
                        InitialCapacity(const Explicit& i) : d_i(i.d_i) { } };
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdea_Array<std::string>
        //                      x(bdea_Array<std::string>::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

    enum Hint {
        // Indicate the extent to which memory will be reallocated in contained
        // string arrays.  If a string array is typically created and then
        // used without significant modifications, both space and runtime
        // performance may be improved by specifying the INFREQUENT_DELETE_HINT
        // hint at construction.  The default is to allocate and deallocate
        // each string individually.

        NO_HINT,                // Default behavior.  Memory for strings in a
                                // string array is allocated and deallocated
                                // individually.

        INFREQUENT_DELETE_HINT  // Few string array modifications are expected.
                                // If elements of a string array are replaced
                                // or removed, the memory for that string is
                                // not available until the string array itself
                                // is destroyed.
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: replaced by 'maxSupportedBdexVersion()'

    // CREATORS
    bdea_Array(bdema_Allocator *basicAllocator = 0);
    bdea_Array(Hint             infrequentDeleteHint,
               bdema_Allocator *basicAllocator = 0);
    bdea_Array(const Explicit&  initialLength,
               bdema_Allocator *basicAllocator = 0);
    bdea_Array(const Explicit&  initialLength,
               Hint             infrequentDeleteHint,
               bdema_Allocator *basicAllocator = 0);
    bdea_Array(const Explicit&     initialLength,
               const std::string&  initialValue,
               bdema_Allocator    *basicAllocator = 0);
        // Create an in-place array.  By default, the array is empty.
        // Optionally specify the 'initialLength' of the array.  Array elements
        // are initialized with the specified 'initialValue', or to an empty
        // string if 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  Optionally specify the
        // intended-use 'infrequentDeleteHint' as 'INFREQUENT_DELETE_HINT' to
        // indicate a relatively small expected number of string array
        // reallocations.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 0 <= initialLength.

    bdea_Array(const Explicit&     initialLength,
               const std::string&  initialValue,
               Hint                infrequentDeleteHint,
               bdema_Allocator    *basicAllocator = 0);
    bdea_Array(const InitialCapacity&  numElements,
               bdema_Allocator        *basicAllocator = 0);
        // Create an in-place array with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  Optionally specify the 'basicAllocator'
        // used to supply memory.  Optionally specify the intended-use
        // 'infrequentDeleteHint' as 'INFREQUENT_DELETE_HINT' to indicate a
        // relatively small expected number of string array reallocations.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 0 <= numElements.

    bdea_Array(const InitialCapacity&  numElements,
               Hint                    infrequentDeleteHint,
               bdema_Allocator        *basicAllocator = 0);
    bdea_Array(const char      **srcCArray,
               int               numElements,
               bdema_Allocator  *basicAllocator = 0);
        // Create an in-place array initialized with the specified
        // 'numElements' leading values from the specified 'srcCArray'.
        // Optionally specify the 'basicAllocator' used to supply memory.
        // Optionally specify the intended-use 'infrequentDeleteHint' as
        // 'INFREQUENT_DELETE_HINT' to indicate a relatively small expected
        // number of string array reallocations.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.  The behavior is
        // undefined unless 0 <= numElements.  Note that 'srcArray' must refer
        // to sufficient memory to hold 'numElements' values.

    bdea_Array(const char      **srcCArray,
               int               numElements,
               Hint              infrequentDeleteHint,
               bdema_Allocator  *basicAllocator = 0);
    bdea_Array(const bdea_Array<std::string>&  original,
               bdema_Allocator                *basicAllocator = 0);
    bdea_Array(const bdea_Array<std::string>&  original,
               Hint                            infrequentDeleteHint,
               bdema_Allocator                *basicAllocator = 0);
        // Create an in-place array initialized to the value of the specified
        // 'original' array.  Optionally specify the 'basicAllocator' used to
        // supply memory.  Optionally specify the intended-use
        // 'infrequentDeleteHint' as 'INFREQUENT_DELETE_HINT' to indicate a
        // relatively small expected number of string array reallocations.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdea_Array();
        // Destroy this object.

    // MANIPULATORS
    bdea_Array<std::string>& operator=(const bdea_Array<std::string>& rhs);
        // Assign to this array the value of the specified 'rhs' array and
        // return a reference to this modifiable array.

    std::string& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    void append(const std::string& item);
        // Append to the end of this array the value of the specified 'item'.
        // Note that this function is logically equivalent to the following:
        // insert(length(), item).

    void append(const bdea_Array<std::string>& srcArray);
        // Append to the end of this array the values in the specified
        // 'srcArray'.  Note that this function is logically equivalent to the
        // following: insert(length(), srcArray).

    void append(const bdea_Array<std::string>& srcArray,
                int                            srcIndex,
                int                            numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless 0 <= srcIndex,
        // 0 <= numElements, and srcIndex + numElements <= srcArray.length().
        // Note that this function is logically equivalent to the following:
        // insert(length(), srcArray, srcIndex, numElements).

    std::string *data();
        // Return the address of the internal memory representation of the
        // contiguous, modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    void insert(int dstIndex, const std::string& item);
        // Insert into this array at the specified 'dstIndex' the value of the
        // specified 'item'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by one index position.  The behavior is
        // undefined unless 0 <= dstIndex <= length().

    void insert(int dstIndex, const bdea_Array<std::string>& srcArray);
        // Insert into this array, beginning the specified 'dstIndex', the
        // values in the specified 'srcArray'.  All values with initial indices
        // at or above 'dstIndex' are shifted up by 'srcArray.length()' index
        // positions.  The behavior is undefined unless
        // 0 <= dstIndex <= length().

    void insert(int                            dstIndex,
                const bdea_Array<std::string>& srcArray,
                int                            srcIndex,
                int                            numElements);
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

    void replace(int dstIndex, const std::string& item);
        // Replace the value at the specified 'dstIndex' in this array with the
        // value of the specified 'item'.  The behavior is undefined unless
        // 0 <= dstIndex < length().  Note that this function is provided only
        // for consistency with 'bdea_Array' and is logically equivalent to
        // (*this)[dstIndex] = item;

    void replace(int                            dstIndex,
                 const bdea_Array<std::string>& srcArray,
                 int                            srcIndex,
                 int                            numElements);
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

    void setLength(int newLength);
    void setLength(int newLength, const std::string& initialValue);
        // Set the length of this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to an empty string if 'initialValue'
        // is not specified.  The behavior is undefined unless 0 <= newLength.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamInRaw(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamIn(stream, version)'

    void swap(int index1, int index2);
        // Efficiently swap the values at the specified indices, 'index1' and
        // 'index2'.  The behavior is undefined unless 0 <= index1 < length()
        // and 0 <= index2 < length().

    // ACCESSORS
    const std::string& operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    const std::string *data() const;
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

    std::ostream& streamOut(std::ostream& stream) const;
        // Format the members of this array to the specified output 'stream'
        // and return a reference to the modifiable 'stream'.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// FREE OPERATORS
int operator==(const bdea_Array<std::string>& lhs,
               const bdea_Array<std::string>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays have the same value,
    // and 0 otherwise.  Two arrays have the same value if they have the same
    // length and same element value at each respective index position.

inline
int operator!=(const bdea_Array<std::string>& lhs,
               const bdea_Array<std::string>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays do not have the same
    // value, and 0 otherwise.  Two arrays have do not have the same value if
    // they have different lengths or differ in at least one index position.

inline
std::ostream& operator<<(std::ostream&                  stream,
                         const bdea_Array<std::string>& array);
    // Write the specified 'array' to the specified output 'stream' and return
    // a reference to the modifiable 'stream'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                // ------------
                                // MANIPULATORS
                                // ------------
inline
std::string& bdea_Array<std::string>::operator[](int index)
{
    return d_array_p[index];
}

inline
std::string *bdea_Array<std::string>::data()
{
    return d_array_p;
}

inline
void bdea_Array<std::string>::replace(int dstIndex, const std::string& item)
{
    d_array_p[dstIndex] = item;
}

inline
void bdea_Array<std::string>::swap(int index1, int index2)
{
    d_array_p[index1].swap(d_array_p[index2]);
}

inline
void bdea_Array<std::string>::append(const std::string& item)
{
    if (d_length < d_size) {
        new(&d_array_p[d_length]) std::string(item, d_strAllocator_p);
        ++d_length;
    }
    else {
        appendImp(item);
    }
}

inline void bdea_Array<std::string>::reserveCapacity(int numElements)
{
    if (d_size < numElements) {
        reserveCapacityImp(numElements);
    }
}

template <class STREAM>
inline
STREAM& bdea_Array<std::string>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int newLength;
            stream.getLength(newLength);
            if (!stream) {
                return stream;                                  // RETURN
            }

            setLength(newLength);

            for (int i = 0; i < d_length; ++i) {
                bdex_InStreamMethods<std::string>::
                                         bdexStreamIn(stream, d_array_p[i], 1);
                if (!stream) break;
            }

          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;                                              // RETURN
}

template <class STREAM>
inline
STREAM& bdea_Array<std::string>::streamInRaw(STREAM& stream, int version)
{
    return bdexStreamIn(stream, version);
}

                                // ---------
                                // ACCESSORS
                                // ---------

inline
const std::string& bdea_Array<std::string>::operator[](int index) const
{
    return d_array_p[index];
}

inline
const std::string *bdea_Array<std::string>::data() const
{
    return d_array_p;
}

inline
int bdea_Array<std::string>::length() const
{
    return d_length;
}

template <class STREAM>
inline
STREAM& bdea_Array<std::string>::bdexStreamOut(STREAM& stream,
                                               int     version) const
{
    switch (version) {
      case 1: {
        stream.putLength(d_length);
        for (int i = 0; i < d_length; ++i) {
            bdex_OutStreamMethods<std::string>::
                                        bdexStreamOut(stream, d_array_p[i], 1);
        }
      } break;
    }
    return stream;
}

template <class STREAM>
inline
STREAM& bdea_Array<std::string>::streamOutRaw(STREAM& stream,
                                              int     version) const
{
    return bdexStreamOut(stream, version);
}

                                // --------------
                                // FREE OPERATORS
                                // --------------

inline
int operator!=(const bdea_Array<std::string>& lhs,
               const bdea_Array<std::string>& rhs)
{
    return !(lhs == rhs);
}

inline
std::ostream& operator<<(std::ostream&                  stream,
                         const bdea_Array<std::string>& array)
{
    return array.streamOut(stream);
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
