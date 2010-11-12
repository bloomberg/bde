// bdea_array.h              -*-C++-*-
#ifndef INCLUDED_BDEA_ARRAY
#define INCLUDED_BDEA_ARRAY

//@PURPOSE: Provide an in-place templatized array of 'T' values.
//
//@DEPRECATED: This component is being replaced with 'std::vector'.
//
//@CLASSES:
//   bdea_Array: memory manager for in-place array of 'T' values
//
//@AUTHOR: Paul Staniforth (pstaniforth)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements an in-place templatized array of 'T'
// values.  The contained elements are stored contiguously in memory.
// Note that appending, inserting, and removing elements can potentially alter
// the memory addresses of other elements in the array.  Consider instead using
// 'bdea_Array2' if the memory address of each contained element must remain
// constant for the life of the element, or to improve runtime performance if
// insert/remove operations are frequent.
//
// 'T' may not be 'bdema_Allocator' or 'bdema_Allocator *' and may not be
// 'const'.
// The type 'T' must have a public default constructor, a public copy
// constructor and a public destructor.  The array does not use the assignment
// operator '=' of 'T'.
//
// 'T' may optionally have a 'bdema_Allocator *' argument to its constructors.
// If 'T' does have an allocator argument, the array invokes the constructors
// with allocator arguments.  The signatures for the constructors should be:
//..
//     T(const T& original, bdema_Allocator *basicAllocator = 0);
//     T(bdema_Allocator *basicAllocator = 0);
//..
// Otherwise the signatures for the constructors should be:
//..
//     T(const T& original);
//     T();
//..
// If 'T' has a constructor with 'void *' parameter, the constructors without
// allocators are called.  To change this default behavior, the
// 'bdealg_TypeTraitsUsesBdemaAllocator' trait must be used.
// 'T' may not be 'bdema_Allocator' or 'bdema_Allocator *' and may not be
// qualified by 'const'.
//
// 'bdea_Array' uses the 'bdemf_IsBitwiseMoveable' and
// 'bdemf_IsBitwiseCopyable' traits when applied to the type 'T'.  When
// 'bdemf_IsBitwiseMoveable' is applied to 'T', 'bdea_Array' will perform
// bitwise moves on existing array elements.  When 'bdemf_IsBitwiseCopyable'
// is applied to 'T', 'bdea_Array' will perform bitwise copies on new array
// elements and will never call the destructor for 'T'.
//
// By default, the array parameter 'T' has neither the bitwise moveable nor
// bitwise copyable traits.  All objects of this type are copied or moved using
// the copy constructor.  The destructor of 'T' is called before copying/moving
// any element to an already initialized location.
//
// The bdex_OutStream operator<<(bdea_OutStream&, const T&) and
// bdex_InStream operator>>(bdea_InStream&, T&) are optional.  If they exist,
// the corresponding array >> and << operators may be used.
//
// The std::ostream& operator<<(std::ostream&, const T&) for the array
// parameter 'T' is optional.  If it exists, the array's ostream operator<<
// may be used.
//
// 'T' may optionally have the equality operator ==.  If this operator is
// defined, the array's equality and inequality operators == and != may be
// used.  The array does not use the inequality operator of 'T'.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
// Here the average case, A[f(N)], is the amortized cost, which is defined as
// the cost of 'N' successive invocations of the operation divided by 'N'.
//..
//     Operation           Worst Case     WorstCase       Average Case
//                         Bitwise Copy   Otherwise
//                         Trait
//     ---------           ------------   ---------       ------------
//     DEFAULT CTOR        O[1]           O[1]
//     COPY CTOR(N)        O[N]           O[N]
//     N.DTOR()            O[1]           O[N]
//     N.OP=(M)            O[M]           O[M]
//     OP==(N,M)           O[min(N,M)]    O[min(N,M)]
//
//     N.append(value)     O[N]           O[N]            A[1]
//     N.insert(value)     O[N]           O[N]
//     N.replace(value)    O[1]           O[1]
//     N.remove(index)     O[N]           O[N]
//     N.removeAll()       O[1]           O[N]
//
//     N.OP[]()            O[1]           O[1]
//     N.length()          O[1]           O[1]
//..
///USAGE 1
///-------
// The following snippets of code illustrate how to create and use an array
// using a simple struct 'SampleStruct'.
//..
// struct SampleStruct {
//     int x;
// };
//..
// Define a trait to indicate that the struct 'SampleStruct' can be copied
// with a bitwise copy.
//..
//
// template <> struct bdemf_IsBitwiseCopyable<SampleStruct>
// {
//     enum { Result = 1 };
// };
//..
// Create an empty 'bdea_Array' 'a' to contains
// elements of type 'SampleStruct':
//..
//  const SampleStruct E1 = { 1 };
//  const SampleStruct E2 = { 2 };
//
//  bdea_Array<SampleStruct> a(&testAllocator);  assert( 0 == a.length());
//
//..
// Populate the array 'a' with two elements 'E1' and 'E2':
//..
//  a.append(E1);                 assert( 1 == a.length());
//                                assert(E1.x == a[0].x);
//
//  a.append(E2);                 assert( 2 == a.length());
//                                assert(E1.x == a[0].x);
//                                assert(E2.x == a[1].x);
//..
// Now assign a new value, 'E3', to the first element (index position 0) of
// array 'a'.
//..
//  const SampleStruct E3 = { 3 };
//
//  a[0] = E3;                    assert( 2 == a.length());
//                                assert(E3.x == a[0].x);
//                                assert(E2.x == a[1].x);
//..
// Then insert a new value in the middle (index position 1) of array 'a'.
//..
//
//  const SampleStruct E4 = { 4 };
//  a.insert(1, E4);              assert( 3 == a.length());
//                                assert(E3.x == a[0].x);
//                                assert(E4.x == a[1].x);
//                                assert(E2.x == a[2].x);
//..
// Next, iterate over the elements in 'a', printing them in increasing order of
// their index positions [0 .. a.length() - 1].
//..
//  std::cout << '[';
//  int len = a.length();
//  for (int i = 0; i < len; ++i) {
//      std::cout << ' ' << a[i].x;
//  }
//  std::cout << " ]" << std::endl;
//..
// which produces the following output on 'stdout':
//..
//      [ 3 4 2 ]
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
// The following snippets of code illustrate how to create and use an array
// using a class that uses a 'bdema_Allocator'.  The class allocates memory
// so cannot be copied with a bitwise copy.  However, it can be moved using
// a bitwise move.
//..
// class SampleClass {
//       bdema_Allocator *d_allocator_p;
//       int x;
//       int * y;
//     public:
//       SampleClass(int value, bdema_Allocator *basicAllocator = 0);
//       SampleClass(const SampleClass& original,
//                   bdema_Allocator *basicAllocator = 0);
//       SampleClass(bdema_Allocator *basicAllocator = 0);
//       ~SampleClass();
//
//       SampleClass& operator=(const SampleClass&);
//
//       friend int operator==(const SampleClass& lhs, const SampleClass& rhs);
//       friend std::ostream& operator<<(std::ostream& os, const SampleClass&);
// };
//
// SampleClass::SampleClass(int value, bdema_Allocator *basicAllocator)
//    : x(value)
//    , d_allocator_p(bdema_Default::allocator(basicAllocator))
// {
//         y = (int *) d_allocator_p->allocate(sizeof(int));
// }
//
// SampleClass::SampleClass(const SampleClass& original,
//                            bdema_Allocator *basicAllocator)
//    : x(original.x)
//    , d_allocator_p(bdema_Default::allocator(basicAllocator))
// {
//         y = (int *) d_allocator_p->allocate(sizeof(int));
// }
//
// SampleClass::SampleClass(bdema_Allocator *basicAllocator)
//    : x(0)
//    , d_allocator_p(bdema_Default::allocator(basicAllocator))
// {
//         y = (int *) d_allocator_p->allocate(sizeof(int));
// }
//
// SampleClass::~SampleClass()
// {
//         d_allocator_p->deallocate(y);
// }
//
// SampleClass& SampleClass::operator=(const SampleClass& item)
// {
//     x = item.x;
//     return *this;
// }
//
// std::ostream& operator<<(std::ostream& os, const SampleClass& rhs)
// {
//         return os << rhs.x;
// }
//
// int operator==(const SampleClass& lhs, const SampleClass& rhs)
// {
//     return lhs.x == rhs.x;
// }
//..
// Define a trait to indicate that the class 'SampleClass' can be moved
// with a bitwise move.
//..
// template <> struct bdemf_IsBitwiseMoveable<SampleClass>
// {
//     enum { Result = 1 };
// };
//..
// Create an empty 'bdea_Array' 'a' to contains
// elements of type 'SampleClass':
//..
//  const SampleClass E1 =  1;
//  const SampleClass E2 =  2;
//
//  bdea_Array<SampleClass> a(&testAllocator);  assert( 0 == a.length());
//..
// Populate the array 'a' with two elements 'E1' and 'E2':
//..
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
//      const SampleClass E3 = 3;
//
//      a[0] = E3;                      assert( 2 == a.length());
//                                      assert(E3 == a[0]);
//                                      assert(E2 == a[1]);
//..
// Then insert a new value in the middle (index position 1) of array 'a'.
//..
//      const SampleClass E4 = 4;
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
//      [ 3 4 2 ]
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_AUTODEALLOCATOR
#include <bdema_autodeallocator.h>
#endif

#ifndef INCLUDED_BDEMA_AUTODELETER
#include <bdema_autodeleter.h>
#endif

#ifndef INCLUDED_BDEMA_AUTODESTRUCTOR
#include <bdema_autodestructor.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

#ifndef INCLUDED_BDEMA_PLACEMENTNEWFUNCTOR
#include <bdema_placementnewfunctor.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULTPLACEMENTNEWFUNCTOR
#include <bdema_defaultplacementnewfunctor.h>
#endif

#ifndef INCLUDED_BDEALG_SCALARPRIMITIVES
#include <bdealg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BDEIMP_BITWISECOPY
#include <bdeimp_bitwisecopy.h>
#endif

#ifndef INCLUDED_BDEMF_ISBITWISE
#include <bdemf_isbitwise.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_CHAR
#include <bdea_array_char.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_DATE
#include <bdea_array_date.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_DATETIME
#include <bdea_array_datetime.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_DOUBLE
#include <bdea_array_double.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_FLOAT
#include <bdea_array_float.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_INT
#include <bdea_array_int.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_INT64
#include <bdea_array_int64.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_SHORT
#include <bdea_array_short.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_STDSTR
#include <bdea_array_stdstr.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_STR
#include <bdea_array_str.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_TIME
#include <bdea_array_time.h>
#endif

#ifndef INCLUDED_BDEA_ARRAY_VOIDPTR
#include <bdea_array_voidptr.h>
#endif

#ifndef INCLUDED_BDES_OBJECTBUFFER
#include <bdes_objectbuffer.h>
#endif

#ifndef INCLUDED_BDES_PLATFORM
#include <bdes_platform.h>
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>        // memmove(), memcpy()
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_OSTREAM
#include <ostream>
#define INCLUDED_OSTREAM
#endif

namespace BloombergLP {

template <class T>
class bdea_Array_Util;

                        // ================
                        // class bdea_Array
                        // ================

template <class T>
class bdea_Array {
    // This class implements an efficient, in-place array of 'T'
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
    // found in the description of 'operator==' for the class.) This container
    // is *exception* *neutral* with no guarantee of rollback: if an exception
    // is thrown during the invocation of a method on a pre-existing instance,
    // the container is left in a valid state, but its value is undefined.  In
    // no event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

    T *d_array_p;              // dynamically allocated array (d_size elements)
    int d_size;                // physical capacity of this array (in elements)
    int d_length;              // logical length of this array (in elements)

    bdema_Allocator *d_allocator_p; // holds (but doesn't own) memory allocator
    bdema_PlacementNewFunctor<T> *d_pnf_p;  // holds allocation functor

    // True if 'T' has the bitwise copyable trait
    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    // True if 'T' has the bitwise moveable trait.  The bitwise copyable
    // trait implies bitwise moveable.
    enum { HasBitwiseMove = bdemf_IsBitwiseCopyable<T>::VALUE
                            || bdemf_IsBitwiseMoveable<T>::VALUE };

  private:
    void appendImp(const T& item);
        // Implements append item when insufficient room in array requiring
        // resizing or bitwise copy not possible.  Used by inline 'append'
        // function.

    void replaceImp(int dstIndex, const T& item);
        // Implements replace item when bitwise copy not possible.  Used by
        // inline 'replace' function.

    void swapImp(int index1, int index2);
        // Implements swap item when bitwise copy not possible.  Used by
        // inline 'swap' function.

    void reserveCapacityImp(int numElements);
        // Note that this function is called only when insufficient capacity
        // exists.  If an exception occurs during the allocation, the array
        // will be left unchanged.

  public:
    // TYPES
    struct Explicit { int d_i; Explicit(int i) : d_i(i) { } ~Explicit() { } };
        // Workaround until the keyword 'explicit' becomes ubiquitous.

    struct InitialCapacity { int d_i;  ~InitialCapacity() { }
                        InitialCapacity(const Explicit& i) : d_i(i.d_i) { } };
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //   bdea_Array x(bdea_Array::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

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
    bdea_Array(bdema_Allocator              *basicAllocator = 0,
               bdema_PlacementNewFunctor<T> *basicFunctor = 0);
    bdea_Array(const Explicit&               initialLength,
               bdema_Allocator              *basicAllocator = 0,
               bdema_PlacementNewFunctor<T> *basicFunctor = 0);
    bdea_Array(const Explicit&               initialLength,
               const T&                      initialValue,
               bdema_Allocator              *basicAllocator = 0,
               bdema_PlacementNewFunctor<T> *basicFunctor = 0);
        // Create an in-place array.  By default, the array is empty.
        // Optionally specify the 'initialLength' of the array.  Array elements
        // are initialized with the specified 'initialValue', or to the default
        // value if 'initialValue' is not specified.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 0 <= initialLength.

    bdea_Array(const InitialCapacity&        numElements,
               bdema_Allocator              *basicAllocator = 0,
               bdema_PlacementNewFunctor<T> *basicFunctor = 0);
        // Create an in-place array with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values without
        // subsequent reallocation.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless 0 <= numElements.

    bdea_Array(const bdea_Array<T>&          original,
               bdema_Allocator              *basicAllocator = 0,
               bdema_PlacementNewFunctor<T> *basicFunctor = 0);
        // Create an in-place array initialized to the value of the specified
        // 'original' array.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdea_Array();
        // Destroy this object.

    // MANIPULATORS
    bdea_Array<T>& operator=(const bdea_Array<T>& rhs);
        // Assign to this array the value of the specified 'rhs' array and
        // return a reference to this modifiable array.  This assignment
        // operator does not depend on the presence of an assignment operator
        // in class 'T'.

    T& operator[](int index);
        // Return a reference to the modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    inline void append(const T& item);
        // Append to the end of this array the value of the specified 'item'.
        // Note that this function is logically equivalent to the following:
        // insert(length(), item).

    void append(const bdea_Array<T>& srcArray);
        // Append to the end of this array the values in the specified
        // 'srcArray'.  Note that this function is logically equivalent to the
        // following: insert(length(), srcArray).

    void append(const bdea_Array<T>& srcArray,
                int                  srcIndex,
                int                  numElements);
        // Append to the end of this array the specified 'numElements' values
        // from the specified 'srcArray', beginning at the specified
        // 'srcIndex'.  The behavior is undefined unless 0 <= srcIndex,
        // 0 <= numElements, and srcIndex + numElements <= srcArray.length().
        // Note that this function is logically equivalent to the following:
        // insert(length(), srcArray, srcIndex, numElements).

    T *data();
        // Return the address of the internal memory representation of the
        // contiguous, modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    void insert(int dstIndex, const T& item);
        // Insert into this array at the specified 'dstIndex' the value of the
        // specified 'item'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by one index position.  The behavior is
        // undefined unless 0 <= dstIndex <= length().

    void insert(int dstIndex, const bdea_Array<T>& srcArray);
        // Insert into this array, beginning the specified 'dstIndex', the
        // values in the specified 'srcArray'.  All values with initial indices
        // at or above 'dstIndex' are shifted up by 'srcArray.length()' index
        // positions.  The behavior is undefined unless
        // 0 <= dstIndex <= length().

    void insert(int                  dstIndex,
                const bdea_Array<T>& srcArray,
                int                  srcIndex,
                int                  numElements);
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
        // or above index + numElements are shifted down by 'numElements'
        // index positions.  The behavior is undefined unless 0 <= index,
        // 0 <= numElements, and index + numElements <= length().

    void removeAll();
        // Remove all elements from this array.  Note that 'length()' is now 0.

    void replace(int dstIndex, const T& item);
        // Replace the value at the specified 'dstIndex' in this array with the
        // value of the specified 'item'.  The behavior is undefined unless
        // 0 <= dstIndex < length().  Note that this function is provided only
        // for consistency with 'bdea_Array' and is logically equivalent to
        // (*this)[dstIndex] = item;

    void replace(int                  dstIndex,
                 const bdea_Array<T>& srcArray,
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

    void setLength(int newLength);
    void setLength(int newLength, const T& initialValue);
        // Set the length of this array to the specified 'newLength'.  If
        // 'newLength' is less than the current length, elements at index
        // positions at or above 'newLength' are removed.  Otherwise any new
        // elements (at or above the current length) are initialized to the
        // specified 'initialValue', or to the default value if 'initialValue'
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
    const T& operator[](int index) const;
        // Return a reference to the non-modifiable element at the specified
        // 'index' position in this array.  The reference will remain valid as
        // long as this array is not destroyed or modified (e.g., via 'insert',
        // 'remove', or 'append').  The behavior is undefined unless
        // 0 <= index < length().

    const T *data() const;
        // Return the address of the internal memory representation of the
        // contiguous, non-modifiable array elements.  The address will remain
        // valid as long as this array is not destroyed or modified (i.e., the
        // current capacity is not exceeded).  The behavior of accessing
        // elements outside the range [ data() .. data() + (length() - 1) ] is
        // undefined.

    int length() const;
        // Return the number of elements in this array.

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
template <class T>
int operator==(const bdea_Array<T>& lhs, const bdea_Array<T>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays have the same value,
    // and 0 otherwise.  Two arrays have the same value if they have the same
    // length and same element value at each respective index position).
    // This operator only be used when the class 'T' defines the operator:
    //    int operator==(const T& lhs, const T& rhs);

template <class T>
inline
int operator!=(const bdea_Array<T>& lhs, const bdea_Array<T>& rhs);
    // Return 1 if the specified 'lhs' and 'rhs' arrays do not have the same
    // value, and 0 otherwise.  Two arrays have do not have the same value if
    // they have different lengths or differ in at least one index position).
    // This operator only be used when the class 'T' defines the operator:
    //    int operator==(const T& lhs, const T& rhs);

template <class T>
inline
std::ostream& operator<<(std::ostream& stream, const bdea_Array<T>& array);
    // Write the specified 'array' to the specified output 'stream' and return
    // a reference to the modifiable 'stream'.  Note that if 'stream' is not
    // valid this operation has no effect.  This function may only be used
    // if class 'T' defines the operator:
    // std::ostream& operator<<(std::ostream& stream, const T&);

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                                // ------------
                                // MANIPULATORS
                                // ------------
template <class T>
inline
T& bdea_Array<T>::operator[](int index)
{
    return d_array_p[index];
}

template <class T>
inline
T *bdea_Array<T>::data()
{
    return d_array_p;
}

template <class T>
inline
void bdea_Array<T>::removeAll()
{
    bdea_Array_Util<T>::cleanup(d_array_p, 0, d_length);

    d_length = 0;
}

template <class T>
inline
void bdea_Array<T>::replace(int dstIndex, const T& item)
{
    if (HasBitwiseCopy) {
        bdeimp_BitwiseCopy<T>::copy(d_array_p + dstIndex, &item);
    }
    else {
        replaceImp(dstIndex, item);
    }
}

template <class T>
inline
void bdea_Array<T>::swap(int index1, int index2)
{
    if (index1 != index2) {
        if (HasBitwiseMove) {
            bdeimp_BitwiseCopy<T>::swap(d_array_p + index1,
                                        d_array_p + index2);
        }
        else {
            swapImp(index1, index2);
        }
    }
}

template <class T>
inline
void bdea_Array<T>::append(const T& item)
{
    // When there is sufficient room in the array and the object supports
    // bitwise copy, perform the operation inline otherwise, not inline.
    if (HasBitwiseCopy && d_length < d_size) {
        bdeimp_BitwiseCopy<T>::copy(&d_array_p[d_length], &item);
        ++d_length;
    }
    else {
        appendImp(item);
    }
}

template <class T>
inline
void bdea_Array<T>::reserveCapacity(int numElements)
{
    if (d_size < numElements) {
        reserveCapacityImp(numElements);
    }
}


                                // ---------
                                // ACCESSORS
                                // ---------

template <class T>
inline
const T& bdea_Array<T>::operator[](int index) const
{
    return d_array_p[index];
}

template <class T>
inline
const T *bdea_Array<T>::data() const
{
    return d_array_p;
}

template <class T>
inline
int bdea_Array<T>::length() const
{
    return d_length;
}
                                // --------------
                                // FREE OPERATORS
                                // --------------

template <class T>
inline
int operator!=(const bdea_Array<T>& lhs, const bdea_Array<T>& rhs)
{
    return !(lhs == rhs);
}

// ===========================================================================
//                      NON-INLINE IMPLEMENTATION
// ===========================================================================

                        // ---------
                        // CONSTANTS
                        // ---------
enum {

    // initial physical capacity (number of elements)
    BDEA_ARRAY_INITIAL_SIZE = 1,

    // multiplicative factor by which to grow 'd_size'
    BDEA_ARRAY_GROW_FACTOR = 2
};

                        // --------------
                        // STATIC METHODS
                        // --------------

inline
static int bdea_Array_nextSize(int size)
    // Return the specified 'size' multiplied by the constant
    // 'BDEA_ARRAY_GROW_FACTOR'.
{
    return size * BDEA_ARRAY_GROW_FACTOR;
}

inline
static int bdea_Array_calculateSufficientSize(int minLength, int size)
    // Geometrically grow the specified current 'size' value while it is less
    // than the specified 'minLength' value.  Return the new size value.  The
    // behavior is undefined unless 1 <= size and 0 <= minLength.  Note that
    // if minLength <= size then 'size' is returned.
{
    // assert(1 <= size);
    // assert(0 <= minLength);

    while (size < minLength) {
        size = bdea_Array_nextSize(size);
    };

    // assert(size >= minLength);
    return size;
}

                        // --------------------------
                        // TEMPLATIZED STATIC METHODS
                        // --------------------------

template <class T>
class bdea_Array_Util {
    // A Namespace for static functions used by the implementation of
    // 'bdea_Array<T>'.  The template parameter is the same as the parameter
    // for 'bdea_Array'.  The static functions have been implemented as
    // a templatized class rather than a set a templatized function to
    // improve portability because some compilers do not implement function
    // templates well.

    friend class bdea_Array<T>;

private:
    static void assign(T *p, const T& item, bdema_Allocator *alloc);
    static void cleanup(T *item);
    static void cleanup(T *array, int index, int length);
    static void cleanupImp(T *array, int index, int length);
    static void reallocate(T **array, int *size, int newSize,
                           int length, bdema_Allocator *basicAllocator);
    static void initializeWithDefaultValue(T *array, int length,
                                           bdema_Allocator *d_allocator_p);
    static void initializeWithValue(T *array, const T& value,
                                    int numElements,
                                    bdema_Allocator *d_allocator_p);
    static void elementInitialize(T *dstArray, int *currentLength,
                                  const T *srcArray, int srcIndex,
                                  int numElements,
                                  bdema_Allocator *d_allocator_p);
    static void elementMove(T *dstArray, int *currentLength,
                            const T *srcArray, int srcIndex,
                            int numElements, bdema_Allocator *d_allocator_p);
    static void construct(T *newArray, const T *existingArray,
                          int length, bdema_Allocator *basicAllocator);
    static void elementCopy(T *dstArray, int *currentLength,
                            const T *srcArray, int srcIndex,
                            int numElements, bdema_Allocator *d_allocator_p);
};

template <class T>
inline void bdea_Array_Util<T>::assign(T               *p,
                                       const T&         item,
                                       bdema_Allocator *alloc)
{
    // The parameter 'T' to an array 'bdea_Array<T>' is not guaranteed to have
    // an assignment operator.  This function simulates this using either
    // bitwise copy  or the destructor and copy constructor of 'T'.  This
    // function is alias safe but is *not* exception neutral.  If an exception
    // is thrown in the constructor, the object at '*p' will be left in an
    // uninitialized state.

    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (p != &item) {
        if (HasBitwiseCopy) {
            bdeimp_BitwiseCopy<T>::copy(p, &item);
        }
        else {
            p -> ~T();

            // Invoke the appropriate copy constructor

            bdealg_ScalarPrimitives::copyConstruct(p, item, alloc);
        }
    }
}

template <class T>
inline void bdea_Array_Util<T>::cleanup(T *item)
    // Invoke the destructor on a single element pointed to by the specified
    // 'item' if 'T' does not have the bitwise copy trait.
{
    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) {
            item->~T();
    }
}

template <class T>
void bdea_Array_Util<T>::cleanupImp(T *array, int index, int length)
    // Invoke the destructor on the specified 'array' of 'T' with the specified
    // 'length' starting at the specified 'index' if 'T' does not have the
    //  bitwise copy trait.
{
    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) {
        T * endp = &array[index + length];
        for (T * d = &array[index]; d < endp; ++d) {
            d -> ~T();
        }
    }
}

template <class T>
inline void bdea_Array_Util<T>::cleanup(T *array, int index, int length)
    // Invoke the destructor on the specified 'array' of 'T' with the specified
    // 'length' starting at the specified 'index' if 'T' does not have the
    //  bitwise copy trait.  This function is invokes the non-inline version
    // only if the destructors are necessary.
{
    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) cleanupImp(array, index, length);
}

template <class T>
void bdea_Array_Util<T>::construct(T               *newArray,
                                   const T         *existingArray,
                                   int              length,
                                   bdema_Allocator *basicAllocator)
{
    bdema_AutoDestructor<T> autoDtor(newArray, 0);

    // Invoke the copy constructor on all the existing items.
    for (int i = 0; i < length; ++i) {
        bdealg_ScalarPrimitives::copyConstruct(newArray + i,
                                               existingArray[i],
                                               basicAllocator);
        ++autoDtor;
    }

    autoDtor.release();
}

template <class T>
inline
void bdea_Array_Util<T>::reallocate(T              **array,
                                    int             *size,
                                    int              newSize,
                                    int              length,
                                    bdema_Allocator *basicAllocator)
    // Reallocate memory in the specified 'array' and update the specified
    // size to the specified 'newSize' using the specified 'basicAllocator'.
    // The specified 'length' number of leading elements are preserved.  If
    // the 'basicAllocator' should throw an exception, this function has no
    // effect.  The behavior is undefined unless 1 <= newSize, 0 <= length,
    // and newSize <= length.
{
    // assert(array);
    // assert(*array);                 // this is not 'allocate'
    // assert(size);
    // assert(1 <= newSize);
    // assert(0 <= length);
    // assert(basicAllocator);
    // assert(length <= *size);        // sanity check
    // assert(length <= newSize);      // ensure class invariant

    enum { HasBitwiseMove = bdemf_IsBitwiseCopyable<T>::VALUE
                            || bdemf_IsBitwiseMoveable<T>::VALUE };

    T *newArray = (T *)
              basicAllocator->allocate(newSize * sizeof **array);
    T *existingArray = *array;

    if (HasBitwiseMove) {
        std::memcpy(newArray, existingArray, length * sizeof(*newArray));
    }
    else {
        // On exception, all items in the new array will be destroyed and
        // the array deallocated.

        bdema_AutoDeallocator<bdema_Allocator> autoDealloc(newArray,
                                                           basicAllocator);

        bdea_Array_Util<T>::
                           construct(newArray, *array, length, basicAllocator);

        autoDealloc.release();
        bdea_Array_Util<T>::cleanup(existingArray, 0, length);
    }

    basicAllocator->deallocate(existingArray);

    *array = newArray;
    *size = newSize;
}

template <class T>
void
bdea_Array_Util<T>::initializeWithDefaultValue(T               *array,
                                               int              length,
                                               bdema_Allocator *d_allocator_p)
    // Initialize the specified 'length' leading elements of the specified
    // 'array' with the default value.  The behavior is undefined unless
    // 0 <= length.  On exception without the bitwise copy trait, the
    // destructor will be run for all elements already initialized.
    // The behavior depends on whether 'T' has the bitwise copy trait.
    // Bitwise copy trait: If the length > 0 invoke the default ctor once
    // and then use 'std::memcpy' for any remaining entries.
    // No bitwise copy trait: Invoke the default ctor 'length' times.
{
    // assert(array);
    // assert(0 <= length);

    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) {
        bdema_AutoDestructor<T> autoDtor(array, 0);

        for (int i = 0; i < length; ++i) {
            bdealg_ScalarPrimitives::defaultConstruct(array + i,d_allocator_p);
            ++autoDtor;
        }
        autoDtor.release();
    }
    else {
        if (length > 0) {
            int initialized = sizeof *array;
            int tocopy = sizeof *array;
            int left = length * sizeof *array;
            bdealg_ScalarPrimitives::defaultConstruct(array, d_allocator_p);

            while ((left = left - tocopy) > 0) {
                tocopy = left;
                if (left > initialized) tocopy = initialized;
                std::memcpy(((char *) array) + initialized, array, tocopy);
                initialized += tocopy;
            }
        }
    }
}

template <class T>
void bdea_Array_Util<T>::initializeWithValue(T               *array,
                                             const T&         value,
                                             int              numElements,
                                             bdema_Allocator *d_allocator_p)
    // The array pointed to by 'array' is initialized to the value 'value'.
    // 'numElements' is the number of values to initialize.  It is possible for
    // the 'T' constructor to throw an exception.  On exception, the destructor
    // will be run for all elements already initialized.  The behavior depends
    // on whether 'T' has the bitwise copy trait.
    // Bitwise copy trait: If the length > 0, bitwise copy 'value' once to the
    // array and then use 'std::memcpy' to propagate the value for any
    // remaining entries.
    // No bitwise copy trait: Invoke the default ctor 'length' times.
{
    // assert(array);
    // assert(0 <= numElements);

    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) {
        // need autodestructor if no bitwise copy trait
        bdema_AutoDestructor<T> autoDtor(array, 0);

        T *endp = array + numElements;
        for (T *d = array; d < endp; ++d) {
            bdealg_ScalarPrimitives::copyConstruct(d, value, d_allocator_p);
            ++autoDtor;
        }
        autoDtor.release();
    }
    else {
        if (numElements > 0) {
            int initialized = sizeof *array;
            int tocopy = sizeof *array;
            int left = numElements * sizeof *array;
            bdeimp_BitwiseCopy<T>::copy(array, &value);

            while ((left = left - tocopy) > 0) {
                tocopy = left;
                if (left > initialized) tocopy = initialized;
                std::memcpy(((char *) array) + initialized, array, tocopy);
                initialized += tocopy;
            }
        }
    }
}

template <class T>
inline
void bdea_Array_Util<T>::elementInitialize(T               *dstArray,
                                           int             *currentLength,
                                           const T         *srcArray,
                                           int              srcIndex,
                                           int              numElements,
                                           bdema_Allocator *d_allocator_p)
    // Initializes new elements in the specified array 'dstArray'
    // as part of ctors and manipulators.  Initialize a contiguous segment of
    // uninitialized 'T' elements using a specified array of elements
    // 'srcArray'.  The specified 'currentLength' is a pointer to the index of
    // the first element to initialize.  The value pointed to by the specified
    // 'currentLength' is incremented by 1 for each value initialized.  If the
    // bitwise copy trait is present, 'std::memcpy' is used for initialization.
    // Otherwise the copy constructor is used.  If an exception occurs in the
    // constructor, 'currentLength' is set appropriately for the number of
    // items initialized.  The behavior is undefined unless currentLength
    // >= 0, srcIndex >=0 and numElements >= 0.
{
    // assert(dstArray);
    // assert(currentLength);
    // assert(*currentLength >= 0);
    // assert(srcArray);
    // assert(srcIndex >= 0);
    // assert(numElements >= 0);

    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (HasBitwiseCopy) {
        std::memcpy(dstArray + (*currentLength), srcArray + srcIndex,
               numElements * sizeof(*dstArray));
        (*currentLength) += numElements;
    }
    else {
        bdea_Array_Util<T>::elementCopy(dstArray, currentLength,
                                        srcArray, srcIndex,
                                        numElements, d_allocator_p);
    }
}

template <class T>
void bdea_Array_Util<T>::elementCopy(T               *dstArray,
                                     int             *currentLength,
                                     const T         *srcArray,
                                     int              srcIndex,
                                     int              numElements,
                                     bdema_Allocator *d_allocator_p)
    // Copies existing elements in the array as part of manipulators.
    // If 'T' is not bitwise copyable, this function has no effect.
    // Initialize the specified 'dstArray', a contiguous segment of
    // uninitialized 'T' elements, using the specified 'srcArray', an array of
    // elements.  The specified 'currentLength' is the index of the first
    // element to initialize.  'currentLength' is incremented by 1 for each
    // value initialized.  The copy constructor is used.  If an
    // exception occurs in the constructor, 'currentLength' is set
    // appropriately for the number of items initialized.
{
    enum { HasBitwiseCopy = bdemf_IsBitwiseCopyable<T>::VALUE };

    if (!HasBitwiseCopy) {
        const T *endp = srcArray + srcIndex + numElements;
        for (const T *s = srcArray + srcIndex;
             s < endp;
             ++*currentLength, ++s) {
            bdealg_ScalarPrimitives::copyConstruct(
                  (T *) (dstArray + *currentLength),
                  *s, d_allocator_p);
        }
    }
}

template <class T>
inline
void bdea_Array_Util<T>::elementMove(T               *dstArray,
                                     int             *currentLength,
                                     const T         *srcArray,
                                     int              srcIndex,
                                     int              numElements,
                                     bdema_Allocator *d_allocator_p)
    // Moves existing elements in the array as part of manipulators.
    // Initialize the specified 'dstArray', a contiguous segment of
    // uninitialized 'T' elements, using the specified 'srcArray', an array of
    // elements.  The specified 'currentLength' is the index of the first
    // element to initialize.  'currentLength' is incremented by 1 for each
    // value initialized.  If the bitwise move trait is present, 'memmove' is
    // used for initialization.  Otherwise the copy constructor is used.  If an
    // exception occurs in the constructor, 'currentLength' is set
    // appropriately for the number of items initialized.
{
    // assert(dstArray);
    // assert(currentLength >= 0);
    // assert(srcArray);
    // assert(srcIndex >= 0);
    // assert(numElements >= 0);

    enum { HasBitwiseMove = bdemf_IsBitwiseCopyable<T>::VALUE
                            || bdemf_IsBitwiseMoveable<T>::VALUE };

    if (HasBitwiseMove) {
        std::memmove(dstArray + *currentLength, srcArray + srcIndex,
               numElements * sizeof(*dstArray));
        *currentLength += numElements;
    }
    else {
        bdea_Array_Util<T>::elementCopy(dstArray, currentLength,
                                        srcArray, srcIndex,
                                        numElements, d_allocator_p);
    }
}

// ===========================================================================
//                      NON-INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------
                        // PRIVATE METHODS
                        // ---------------

template <class T>
inline
void bdea_Array<T>::appendImp(const T& item)
{
    // Implements an append function for the case when there is insufficient
    // memory in the array for an additional item or when the specified object
    // 'item' does not support bitwise copy.  Note that this method is
    // called by the inline 'append' function.

    if (!HasBitwiseCopy && d_length < d_size) {
        // initialize this new item
        bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, &item,
                                     0, 1, d_allocator_p);
    }
    else {

        // assert(d_length == d_size);

        T *tmp = d_array_p;
        int newSize = bdea_Array_nextSize(d_size);
        d_array_p = (T *)
                    d_allocator_p->allocate(newSize * sizeof *d_array_p);
        d_size = newSize;

        // In order to support aliasing, tmp is not deleted until after
        // the item has been appended to the array.  The items in old array
        // 'tmp' will be destroyed and the array released automatically when
        // the function returns or when an exception occurs, thus making this
        // function exception neutral.

        bdema_AutoDeallocator<bdema_Allocator> autoDealloc(tmp, d_allocator_p);
        bdema_AutoDestructor<T> autoDtor(tmp,
                                         HasBitwiseMove ? 0 : d_length);

        int originalLength = d_length;
        d_length = 0;

        bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                               originalLength, d_allocator_p);

        // new item
        bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, &item,
                                     0, 1, d_allocator_p);
    }
}

template <class T>
inline
void bdea_Array<T>::replaceImp(int dstIndex, const T& item)
{
    // Replace the value at the specified 'dstIndex' in this array with the
    // value of the specified 'item'.  The behavior is undefined unless
    // 0 <= dstIndex < length().  This function is only called when the object
    // does not have bitwise copy.  The inline function is used when bitwise
    // copy is possible.

    if (!HasBitwiseCopy) {
        int originalLength = d_length;
        d_length = dstIndex;

        // If an exception is thrown during the assign operation for the
        // element, that element remains uninitialized.  If this occurs, an
        // autodestructor invokes the destructor for all elements starting at
        // the element beyond the element that failed.  The array length is
        // set to the truncated length.

        bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength,
                                         dstIndex - originalLength + 1);

        bdea_Array_Util<T>::assign(&d_array_p[dstIndex], item, d_allocator_p);

        autoDtor.release();
        d_length = originalLength;
    }
}

template <class T>
inline
void bdea_Array<T>::swapImp(int index1, int index2)
{
    // This function is only called when the object does not have bitwise
    // move (and bitwise copy).  The inline function implements the swap when
    // bitwise move is possible.

    if (!HasBitwiseMove) {
        bdes_ObjectBuffer<T> buffer;
        T& temp = buffer.object();

        // Make temporary copy of first element to be swapped.  If an
        // exception is thrown by this constructor, the array is left
        // unchanged.

        bdealg_ScalarPrimitives::copyConstruct(&temp,
                                               d_array_p[index1],
                                               d_allocator_p);

        // Automatically destroy temporary copy on exception or normal exit.

        bdema_AutoDestructor<T> autoDtorTemp(&temp, 1);

        int originalLength = d_length;

        // assign array[index1] = array[index2]
        // If an exception occurs within the constructor on index1, the
        // array will be truncated to a length of index1.

        bdema_AutoDestructor<T> autoDtor1(&d_array_p[index1 + 1],
                                          originalLength - index1 - 1);
        d_length = index1;
        d_array_p[index1].~T();
        bdealg_ScalarPrimitives::copyConstruct(&d_array_p[index1],
                                               d_array_p[index2],
                                               d_allocator_p);

        autoDtor1.release();

        // assign array[index2] = temporary value
        // If an exception occurs within the constructor on index2, the
        // array will be truncated to a length of index2.

        bdema_AutoDestructor<T> autoDtor2(&d_array_p[index2 + 1],
                                          originalLength - index2 - 1);
        d_length = index2;
        d_array_p[index2].~T();
        bdealg_ScalarPrimitives::copyConstruct(&d_array_p[index2],
                                               temp, d_allocator_p);

        autoDtor2.release();

        d_length = originalLength;
    }
}

template <class T>
inline
void bdea_Array<T>::reserveCapacityImp(int numElements)
    // This method is called by the inline reserveCapacity() when the current
    // capacity in insufficient.
{
    // assert(0 <= numElements);

    // The size allocated will be >= numElements

    int newSize = bdea_Array_calculateSufficientSize(numElements, d_size);
    bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, newSize, d_length,
          d_allocator_p);
}

                        // -------------
                        // CLASS METHODS
                        // -------------

template <class T>
inline
int bdea_Array<T>::maxSupportedBdexVersion()
{
    return 1;
}

template <class T>
inline
int bdea_Array<T>::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

                        // --------
                        // CREATORS
                        // --------

template <class T>
inline
bdea_Array<T>::bdea_Array(bdema_Allocator              *basicAllocator,
                          bdema_PlacementNewFunctor<T> *basicFunctor)
: d_size(BDEA_ARRAY_INITIAL_SIZE)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pnf_p(bdema_DefaultPlacementNewFunctor<T>::functor(basicFunctor))
{
    // assert(d_allocator_p);

    d_array_p = (T *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

template <class T>
inline
bdea_Array<T>::bdea_Array(const Explicit&               initialLength,
                          bdema_Allocator              *basicAllocator,
                          bdema_PlacementNewFunctor<T> *basicFunctor)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pnf_p(bdema_DefaultPlacementNewFunctor<T>::functor(basicFunctor))
{
    // assert(0 <= d_length);
    // assert(d_allocator_p);

    d_size = bdea_Array_calculateSufficientSize(d_length,
          BDEA_ARRAY_INITIAL_SIZE);
    // assert(d_length <= d_size);

    d_array_p = (T *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // It is possible for bdea_Array_Util<T>::initializeWithDefaultValue() to
    // throw an exception; if it does, the array is automatically deallocated.
    // bdea_Array_Util<T>::initializeWithDefaultValue() takes care of running
    // the destructor on array elements.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    bdea_Array_Util<T>::initializeWithDefaultValue(d_array_p, d_length,
                               d_allocator_p);

    autoDealloc.release();

}

template <class T>
inline
bdea_Array<T>::bdea_Array(const Explicit&               initialLength,
                          const T&                      initialValue,
                          bdema_Allocator              *basicAllocator,
                          bdema_PlacementNewFunctor<T> *basicFunctor)
: d_length(initialLength.d_i)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pnf_p(bdema_DefaultPlacementNewFunctor<T>::functor(basicFunctor))
{
    // assert(0 <= d_length);
    // assert(d_allocator_p);

    d_size = bdea_Array_calculateSufficientSize(d_length,
          BDEA_ARRAY_INITIAL_SIZE);
    // assert(d_length <= d_size);

    d_array_p = (T *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // It is possible for bdea_Array_Util<T>::initializeWithValue() to throw an
    // exception; if it does, the array is automatically deallocated.
    // bdea_Array_Util<T>::initializeWithValue() takes care of running the
    // destructor on array elements.

    bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                       d_allocator_p);

    bdea_Array_Util<T>::initializeWithValue(d_array_p, initialValue, d_length,
                                   d_allocator_p);

    autoDealloc.release();
}

template <class T>
inline
bdea_Array<T>::bdea_Array(const InitialCapacity&        numElements,
                          bdema_Allocator              *basicAllocator,
                          bdema_PlacementNewFunctor<T> *basicFunctor)
: d_size(numElements.d_i <= 0 ? BDEA_ARRAY_INITIAL_SIZE : numElements.d_i)
, d_length(0)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pnf_p(bdema_DefaultPlacementNewFunctor<T>::functor(basicFunctor))
{
    // assert(1 <= d_size);
    // assert(d_allocator_p);

    d_array_p = (T *)
                d_allocator_p->allocate(d_size * sizeof *d_array_p);
}

template <class T>
inline
bdea_Array<T>::bdea_Array(const bdea_Array<T>&          original,
                          bdema_Allocator              *basicAllocator,
                          bdema_PlacementNewFunctor<T> *basicFunctor)
: d_length(original.d_length)
, d_allocator_p(bdema_Default::allocator(basicAllocator))
, d_pnf_p(bdema_DefaultPlacementNewFunctor<T>::functor(basicFunctor))
{
    // assert(d_allocator_p);

    d_size = bdea_Array_calculateSufficientSize(d_length,
                                                      BDEA_ARRAY_INITIAL_SIZE);

    d_array_p = (T *)d_allocator_p->allocate(d_size * sizeof *d_array_p);

    // The 'T' constructor can throw an exception; if it does, we
    // must run the destructor on elements that have already been initialized
    // followed by a deallocate on the array itself.

    T *origp = original.d_array_p;

    if (!HasBitwiseCopy) {
        bdema_AutoDeallocator<bdema_Allocator> autoDealloc(d_array_p,
                                                           d_allocator_p);
        bdema_AutoDestructor<T> autoDtor(d_array_p, 0);
        for (int i = 0; i < d_length; ++i) {
             bdealg_ScalarPrimitives::copyConstruct(d_array_p + i,
                                                    origp[i], d_allocator_p);
             ++autoDtor;
        }
        autoDtor.release();
        autoDealloc.release();
    }
    else {
        std::memcpy(d_array_p, origp, d_length * sizeof(*d_array_p));
    }

}

template <class T>
inline
bdea_Array<T>::~bdea_Array()
{
    // assert(d_array_p);
    // assert(1 <= d_size);
    // assert(0 <= d_length);
    // assert(d_allocator_p);
    // assert(d_length <= d_size);

    bdea_Array_Util<T>::cleanup(d_array_p, 0, d_length);

    d_allocator_p->deallocate(d_array_p);
}

                        // ------------
                        // MANIPULATORS
                        // ------------
template <class T>
inline
bdea_Array<T>& bdea_Array<T>::operator=(const bdea_Array<T>& rhs)
{
    if (this != &rhs) {
        removeAll();
        int newLength = rhs.d_length;
        if (d_size < newLength) {
            // insufficient memory so reallocate the array.  Don't release
            // the old memory until after allocate succeeds in case of
            // exceptions.

            T *tmp = d_array_p;
            int newSize = bdea_Array_calculateSufficientSize(newLength,
                  d_size);
            d_array_p = (T *)
                        d_allocator_p->allocate(newSize * sizeof *d_array_p);
            // COMMIT
            d_size = newSize;
            d_allocator_p->deallocate(tmp);
        }
        // assert(d_size >= newLength);

        bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                                              rhs.d_array_p, 0,
                                              rhs.d_length, d_allocator_p);
    }
    return *this;
}

template <class T>
inline
void bdea_Array<T>::append(const bdea_Array<T>& srcArray)
{
    int newLength = d_length + srcArray.d_length;
    if (d_size < newLength) {
        int newSize = bdea_Array_calculateSufficientSize(newLength, d_size);
        bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, newSize,
                              d_length, d_allocator_p);
    }

    bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                                          srcArray.d_array_p, 0,
                                          srcArray.d_length, d_allocator_p);
}

template <class T>
inline
void bdea_Array<T>::append(const bdea_Array<T>& srcArray,
                           int                  srcIndex,
                           int                  numElements)
{
    // assert(0 <= srcIndex);
    // assert(0 <= numElements);
    // assert(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;
    if (d_size < newLength) {
        int newSize = bdea_Array_calculateSufficientSize(newLength, d_size);
        bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, newSize,
                              d_length, d_allocator_p);
    }
    // assert(d_size >= newLength);
    bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                                          srcArray.d_array_p, srcIndex,
                                          numElements, d_allocator_p);
}

template <class T>
inline
void bdea_Array<T>::insert(int dstIndex, const T& item)
{
    // assert(0 <= dstIndex);  assert(dstIndex <= d_length);

    if (d_length >= d_size) {  // Need to resize.
        int newSize = bdea_Array_nextSize(d_size);
        T *tmp = d_array_p;
        d_array_p = (T *)
                 d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT

        // tmp is automatically deallocated and the elements destroyed on
        // normal return or exception.  On exception the new array is
        // truncated at the point in the array where the exception occurs.

        bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(tmp,
                                                                d_allocator_p);
        if (HasBitwiseMove) {
            d_size = newSize;
            int currentLength = d_length;

            // Construct left of insertion point followed by new item and
            // finally right of insertion point.  Necessary for exception
            // neutrality.

            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                                   dstIndex, d_allocator_p);

            bdema_AutoDestructor<T> autoDtor2(tmp + dstIndex,
                                              (!HasBitwiseCopy)
                                              ? currentLength - dstIndex : 0);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, &item,
                                         0, 1, d_allocator_p);

            autoDtor2.release();

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp,
                                            dstIndex, currentLength - dstIndex,
                                            d_allocator_p);
        }
        else {
            bdema_AutoDestructor<T> autoDtor1(tmp,
                                              HasBitwiseMove ? 0 : d_length);

            d_size = newSize;
            int currentLength = d_length;

            // Construct left of insertion point followed by new item and
            // finally right of insertion point.  Necessary for exception
            // neutrality.

            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                                            dstIndex, d_allocator_p);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, &item,
                                                  0, 1, d_allocator_p);

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp,
                                            dstIndex, currentLength - dstIndex,
                                            d_allocator_p);
        }

        return;                                                 // RETURN
    }

    // assert(d_length < d_size);  // Enough capacity exists.

    int originalLength = d_length;

    if (!HasBitwiseMove) {
        // Move the elements starting at the end of the array using the copy
        // constructor and destructor, leaving the insertion point
        // uninitialized.  If an exception occurs the destructor is run on the
        // end of the array leaving the array truncated at the insertion point.

        bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength + 1, 0);
        for (; d_length > dstIndex; --d_length) {
            bdealg_ScalarPrimitives::copyConstruct(d_array_p + d_length,
                                                   d_array_p[d_length - 1],
                                                   d_allocator_p);
            d_array_p[d_length - 1].~T();
            --autoDtor;
        }
        autoDtor.release();
    }
    else {
        std::memmove(d_array_p + dstIndex + 1, d_array_p + dstIndex,
                 (d_length - dstIndex) * sizeof(*d_array_p));
    }

    T *here = d_array_p + dstIndex;

    // Check for alias case

    const T * itemPtr = (&item >= here && &item < d_array_p + originalLength)
                  ? (&item + 1) : &item;
    if (HasBitwiseCopy) {
        bdeimp_BitwiseCopy<T>::copy(here, itemPtr);
    }
    else {
        d_length = dstIndex;
        bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength + 1,
                                         dstIndex - originalLength);
        bdealg_ScalarPrimitives::copyConstruct(here, *itemPtr, d_allocator_p);
        autoDtor.release();
    }
    d_length = originalLength + 1;
}

template <class T>
inline
void bdea_Array<T>::insert(int dstIndex, const bdea_Array<T>& srcArray)
{
    // assert(0 <= dstIndex);  assert(dstIndex <= d_length);

    int numElements = srcArray.d_length;
    int newLength = d_length + numElements;
    int originalLength = d_length;

    if (newLength > d_size) {  // Need to resize.
        int newSize = bdea_Array_calculateSufficientSize(newLength, d_size);
        T *tmp = d_array_p;
        const T *src = srcArray.d_array_p;
        d_array_p = (T *)
                 d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT

        bdema_AutoDeallocator<bdema_Allocator> autoDeallocArray(tmp,
                                                                d_allocator_p);

        if (HasBitwiseMove) {
            d_size   = newSize;
            T *there = tmp + dstIndex;
            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                                   dstIndex, d_allocator_p);

            bdema_AutoDestructor<T> autoDtor2(tmp + dstIndex,
                                              (!HasBitwiseCopy)
                                              ? originalLength - dstIndex : 0);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, src, 0,
                              numElements, d_allocator_p);

            autoDtor2.release();

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, there, 0,
                              originalLength - dstIndex, d_allocator_p);
        }
        else {
            bdema_AutoDestructor<T> autoDtor1(tmp, d_length);

            d_size   = newSize;
            T *there = tmp + dstIndex;
            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                                            dstIndex, d_allocator_p);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, src, 0,
                                                  numElements, d_allocator_p);

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, there, 0,
                                            originalLength - dstIndex,
                                            d_allocator_p);
        }

        return;                                                  // RETURN
    }

    if (numElements > 0) {
        // assert(newLength <= d_size);  // Enough capacity exists.

        const T *s = srcArray.d_array_p;

        bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength
           + numElements, HasBitwiseMove ? (dstIndex - originalLength) : 0);

        if (!HasBitwiseMove) {
            // Move the elements starting at the end of the array using the
            // copy constructor and destructor, leaving the insertion point
            // uninitialized.  An exception could occur while initializing the
            // new elements.  In order to be exception neutral, the array must
            // be left in a consistent state.  The autodestructor is run on
            // those elements moved if an exception is thrown.  'd_length' has
            // the current number of contiguous initialized values.

            for (; d_length > dstIndex; --d_length) {
                bdealg_ScalarPrimitives::copyConstruct(
                       d_array_p + d_length + numElements - 1,
                       d_array_p[d_length - 1], d_allocator_p);
                d_array_p[d_length - 1].~T();
                --autoDtor;
            }
        }
        else {
             std::memmove(d_array_p + dstIndex + numElements,
                          d_array_p + dstIndex,
                          (d_length - dstIndex) * sizeof(*d_array_p));
             d_length = dstIndex;
        }

        if (&srcArray == this) {
            // We have an aliasing problem and shifting will affect the source:
            // Unless 'dstIndex' happens to be either 0 or length is 0, then
            // the source is split.  If 'dstIndex' is 0, the entire source has
            // been shifted up by exactly 'numElement' positions.

            int ne2 = numElements - dstIndex;

            // assert(ne2 >= 0);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                              d_array_p, 0, dstIndex, d_allocator_p);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                              d_array_p, numElements + dstIndex,
                              ne2, d_allocator_p);
        }
        else {  // There are no aliasing issues.
            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, s, 0,
                              numElements, d_allocator_p);
        }

        d_length = newLength;
        autoDtor.release();
    }
}

template <class T>
inline
void bdea_Array<T>::insert(int                   dstIndex,
                           const bdea_Array<T>&  srcArray,
                           int                   srcIndex,
                           int                   numElements)
{
    // assert(0 <= dstIndex);  assert(dstIndex <= d_length);
    // assert(0 <= srcIndex);
    // assert(0 <= numElements);
    // assert(srcIndex + numElements <= srcArray.d_length);

    int newLength = d_length + numElements;
    int originalLength = d_length;

    if (newLength > d_size) {  // Need to resize.
        int newSize = bdea_Array_calculateSufficientSize(newLength, d_size);
        T *tmp = d_array_p;
        const T *src = srcArray.d_array_p;
        d_array_p =
            (T *) d_allocator_p->allocate(newSize * sizeof *d_array_p);
        // COMMIT

        // On exception, the destructors for AutoDeallocator and
        // AutoDestructor are called in reverse order.  The order of
        // these statements is significant.

        bdema_AutoDeallocator<bdema_Allocator> autoDealloc(tmp,
                                                           d_allocator_p);

        if (HasBitwiseMove) {
            d_size = newSize;
            T *there = tmp + dstIndex;
            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                    dstIndex, d_allocator_p);

            bdema_AutoDestructor<T> autoDtor2(tmp + dstIndex,
                                              (!HasBitwiseCopy)
                                              ? originalLength - dstIndex : 0);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length,
                                                  src, srcIndex,
                                                  numElements, d_allocator_p);

            autoDtor2.release();

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, there, 0,
                                            originalLength - dstIndex,
                                            d_allocator_p);
        }
        else {
            bdema_AutoDestructor<T> autoDtor1(tmp, d_length);

            d_size = newSize;
            T *there = tmp + dstIndex;
            d_length = 0;

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, tmp, 0,
                                            dstIndex, d_allocator_p);

            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, src,
                                                  srcIndex, numElements,
                                                  d_allocator_p);

            bdea_Array_Util<T>::elementMove(d_array_p, &d_length, there, 0,
                        originalLength - dstIndex, d_allocator_p);
        }

        return;                                                  // RETURN
    }

    if (numElements > 0) {
        const T *s = srcArray.d_array_p + srcIndex;

        // An exception could occur while initializing the new elements.  In
        // order to be exception neutral, the array must be left in a
        // consistent state.  The autodestructor is run on those elements
        // moved.  'd_length' has the current number of contiguous initialized
        // values.

        bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength
           + numElements, HasBitwiseMove ? (dstIndex - originalLength) : 0);

        if (!HasBitwiseMove) {
            for (; d_length > dstIndex; --d_length) {
                bdealg_ScalarPrimitives::copyConstruct(
                       d_array_p + d_length + numElements -1,
                       d_array_p[d_length - 1], d_allocator_p);
                d_array_p[d_length - 1].~T();
                --autoDtor;
            }
        }
        else {
             std::memmove(d_array_p + dstIndex + numElements,
                          d_array_p + dstIndex,
                          (d_length - dstIndex) * sizeof(*d_array_p));
             d_length = dstIndex;
        }

        if (&srcArray == this && srcIndex + numElements > dstIndex) {
            // We have an aliasing problem and shifting will affect the source:
            // If the srcIndex < dstIndex then the source is split; otherwise
            // the entire source has been shifted up by exactly numElement
            // positions.

            if (srcIndex < dstIndex) {  // Alias problem: source split.
                int ne1 = dstIndex - srcIndex;
                const T *s2 = s + numElements + ne1;
                int ne2 = numElements - ne1;
                // assert(ne1 > 0);
                // assert(ne2 > 0);
                bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, s,
                                                      0, ne1, d_allocator_p);
                bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, s2,
                                                      0, ne2, d_allocator_p);
            }
            else {  // Alias problem: source shifted as single unit by
                    // numElements.
                bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, s,
                                                      numElements, numElements,
                                                      d_allocator_p);
            }
        }
        else {  // There are no aliasing issues.
            bdea_Array_Util<T>::elementInitialize(d_array_p, &d_length, s, 0,
                                                  numElements, d_allocator_p);
        }

        d_length = newLength;
        autoDtor.release();
    }
}

template <class T>
inline
void bdea_Array<T>::remove(int index)
{
    // assert(0 <= index);  assert(index < d_length);

    int newLength = d_length - 1;

    if (!HasBitwiseMove) {
        // No bitwise move trait so to move all the remaining elements in the
        // array the destructor and copy constructor of 'T' are invoked.  If
        // an exception is thrown in the copy constructor for an element, that
        // element is left in an uninitialized state.

        T *here = d_array_p + index;
        bdema_AutoDestructor<T> autoDtor(d_array_p + d_length,
                                         index - d_length);

        for (d_length = index; d_length < newLength; ++d_length) {
            ++autoDtor;
            bdea_Array_Util<T>::assign(here, here[1], d_allocator_p);
            ++here;
        }

        // run destructor on final element.

        bdea_Array_Util<T>::cleanup(d_array_p + newLength);
        autoDtor.release();
    }
    else {
        bdea_Array_Util<T>::cleanup(d_array_p + index);
        T *here = d_array_p + index;
        std::memmove(here,
                     here + 1,
                     (d_length - index - 1) * sizeof *d_array_p);
    }

    d_length = newLength;
}

template <class T>
inline
void bdea_Array<T>::remove(int index, int numElements)
{
    // assert(0 <= index);
    // assert(0 <= numElements);
    // assert(index + numElements <= d_length);

    if (!HasBitwiseMove) {
        // To move all the remaining elements in the array the destructor and
        // copy constructor of 'T' are invoked.  If an exception is thrown in
        // the copy constructor for an element, that element is left in an
        // uninitialized state.

        bdema_AutoDestructor<T> autoDtor(d_array_p + d_length,
                                         index - d_length);

        int newLength = d_length - numElements;
        T *here = d_array_p + index;
        for (d_length = index; d_length < newLength; ++d_length) {
            ++autoDtor;
            bdea_Array_Util<T>::assign(here, here[numElements], d_allocator_p);
            ++here;
        }

        // destructor on final elements

        bdea_Array_Util<T>::cleanup(d_array_p, newLength, numElements);
        autoDtor.release();
    }
    else {
        bdea_Array_Util<T>::cleanup(d_array_p, index, numElements);
        T *here = d_array_p + index;
        d_length -= numElements;
        std::memmove(here, here + numElements,
                (d_length - index) * sizeof *d_array_p);
    }

}

template <class T>
inline
void bdea_Array<T>::replace(int                  dstIndex,
                            const bdea_Array<T>& srcArray,
                            int                  srcIndex,
                            int                  numElements)
{
    // assert(0 <= dstIndex);
    // assert(0 <= srcIndex);
    // assert(0 <= numElements);
    // assert(dstIndex + numElements <= d_length);
    // assert(srcIndex + numElements <= srcArray.d_length);
    int originalLength = d_length;

    if (HasBitwiseCopy) {
        // bitwise copy is permitted so perform memmove
        std::memmove(d_array_p + dstIndex, srcArray.d_array_p + srcIndex,
                numElements * sizeof(*d_array_p));
    }
    else {
        // If an exception is thrown during the assign operation for an
        // element, that element remains uninitialized.  If this occurs, an
        // autodestructor invokes the destructor for all elements starting at
        // the element beyond the element that failed.  The array length
        // 'd_length' is set to the truncated length.

        if (&d_array_p[dstIndex] <= &srcArray.d_array_p[srcIndex]) {
            if (&d_array_p[dstIndex] != &srcArray.d_array_p[srcIndex]) {
                // iterate forwards
                const T *s = &srcArray.d_array_p[srcIndex];
                T *endp = &d_array_p[dstIndex + numElements];
                d_length = dstIndex;

                bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength,
                                         dstIndex - originalLength);

                for (T *d = &d_array_p[dstIndex]; d < endp; ++s, ++d) {
                    ++autoDtor;
                    bdea_Array_Util<T>::assign(d, *s, d_allocator_p);
                    ++d_length;
                }

                autoDtor.release();
            }
        }
        else {
            // iterate backwards
            T *endp = &d_array_p[dstIndex];
            const T *s = &srcArray[srcIndex + numElements - 1];

            d_length = dstIndex + numElements - 1;

            bdema_AutoDestructor<T> autoDtor(d_array_p + originalLength,
                    dstIndex + numElements - originalLength);

            for (T *d = &d_array_p[dstIndex + numElements - 1];
                 d >= endp; --s, --d) {
                bdea_Array_Util<T>::assign(d, *s, d_allocator_p);
                --autoDtor;
                --d_length;
            }

            autoDtor.release();
        }

        d_length = originalLength;
    }
}

template <class T>
inline
void bdea_Array<T>::reserveCapacityRaw(int numElements)
{
    // assert(0 <= numElements);

    if (d_size < numElements) {
        bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, numElements,
                              d_length, d_allocator_p);
    }
}

template <class T>
inline
void bdea_Array<T>::setLength(int newLength)
{
    // Exceptions can be thrown during allocation of the new array memory.
    // Exceptions can also be thrown during the initialization of
    // array elements.  If an exception is thrown, the array state is left
    // unchanged.

    // assert(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = bdea_Array_calculateSufficientSize(newLength,
                  d_size);
            bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, newSize,
                                  d_length, d_allocator_p);
        }
        bdea_Array_Util<T>::initializeWithDefaultValue(d_array_p + d_length,
                                   newLength - d_length,
                                   d_allocator_p);
    }
    else {
        if (newLength < d_length) {
             bdea_Array_Util<T>::cleanup(d_array_p, newLength,
                                         d_length - newLength);
        }
    }
    d_length = newLength;
}

template <class T>
inline
void bdea_Array<T>::setLength(int newLength, const T& initialValue)
{
    // Exceptions can be thrown during allocation of the new array memory.
    // Exceptions can also be thrown during the initialization of
    // array elements.  If an exception is thrown, the array state is left
    // unchanged (except that the array size may have been increased).

    // assert(0 <= newLength);

    if (newLength > d_length) {
        if (newLength > d_size) {
            int newSize = bdea_Array_calculateSufficientSize(newLength,
                                                             d_size);
            bdea_Array_Util<T>::reallocate(&d_array_p, &d_size, newSize,
                                           d_length, d_allocator_p);
        }

        // An exception could occur within
        // bdea_Array_Util<T>::initializeWithValue.  However, the function will
        // run the destructor on all array elements already initialized by the
        // function before the exception occurred.

        bdea_Array_Util<T>::initializeWithValue(d_array_p + d_length,
                                                initialValue,
                                                newLength - d_length,
                                                d_allocator_p);
    }
    else {
        if (newLength < d_length) {
            // The array will be shorter so run the destructor on excessive
            // elements.
            bdea_Array_Util<T>::cleanup(d_array_p, newLength,
                                        d_length - newLength);
        }
    }
    d_length = newLength;
}

template <class T>
template <class STREAM>
inline
STREAM& bdea_Array<T>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        int newLength;
        stream.getLength(newLength);
        if (!stream) {
            return stream;                                  // RETURN
        }

        if (newLength < 0) {
            stream.invalidate();
            return stream;                                  // RETURN
        }

        // TBD exception neutrality

        setLength(newLength);

        for (int i = 0; i < newLength; ++i) {
            (*this)[i].streamInRaw(stream, version);
            if (!stream) break;
        }
    }
    return stream;                                              // RETURN
}

template <class T>
template <class STREAM>
inline
STREAM& bdea_Array<T>::streamInRaw(STREAM& stream, int version)
{
    return bdexStreamIn(stream, version);
}

template <class T>
inline
std::ostream& bdea_Array<T>::streamOut(std::ostream& stream) const
{
    stream << '[';
    for (int i = 0; i < length(); ++i) {
        stream << ' ' << (*this)[i];
    }
    return stream << " ]";
}

template <class T>
template <class STREAM>
inline
STREAM& bdea_Array<T>::bdexStreamOut(STREAM& stream, int version) const
{
    stream.putLength(d_length);

    for (int i = 0; i < d_length; ++i) {
        (*this)[i].streamOutRaw(stream, version);
    }

    return stream;
}

template <class T>
template <class STREAM>
inline
STREAM& bdea_Array<T>::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}



                        // ---------
                        // ACCESSORS
                        // ---------




                        // --------------
                        // FREE OPERATORS
                        // --------------

template <class T>
inline
int operator==(const bdea_Array<T>& lhs, const bdea_Array<T>& rhs)
{
    const int len = lhs.length();
    if (len != rhs.length()) {
        return 0;
    }

    for (int i = 0; i < len; ++i) {
        if (!(lhs[i] == rhs[i])) {
            return 0;
        }
    }
    return 1;
}

template <class T>
inline
std::ostream& operator<<(std::ostream& stream, const bdea_Array<T>& rhs)
{
    return rhs.streamOut(stream);
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
