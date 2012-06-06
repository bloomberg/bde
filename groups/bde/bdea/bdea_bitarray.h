// bdea_bitarray.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEA_BITARRAY
#define INCLUDED_BDEA_BITARRAY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a space-efficient, sequence container of boolean values.
//
//@CLASSES:
//   bdea_BitArray: vector-like, sequential container of boolean values
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component implements an efficient value-semantic,
// sequential (non-templated) container of bit (binary digit) values (i.e., '0'
// or '1') of type 'bool'.  A 'bdea_BitArray' may be thought of as an
// arbitrary-precision 'unsigned int'.  This metaphor is used to motivate the
// rich set of "bitwise" operations on 'bdea_BitArray' objects provided by this
// component, as well as the notion of "zero extension" of a (shorter) bit
// array during binary operations on bit arrays having lengths that are not the
// same.
//
///Bit-Array-Specific Functionality
///--------------------------------
// In addition to many typical vector-like container methods, this component
// supports "boolean" functionality unique to 'bdea_BitArray' (note that 'LSB'
// is the bit at index position 0).  However, unlike other standard container
// types such as 'bsl::bitset', there is no 'operator[](int index)' that
// returns a reference to a (modifiable) boolean element at the specified index
// position.  This difference is due to the densely-packed internal
// representation of bits in this array:
//..
//  bdea_BitArray mA(128);
//  assert(0 == mA[13]);         // Ok
//  mA[13] = 'false';            // Error -- 'mA[13]' is not an lvalue.
//  mA.replace(13, 1);           // Ok
//
//  const bdea_BitArray& A = a;  // Ok
//  assert(1 == A[13]);          // Ok
//  const bool *bp  = &A[13]     // Error -- 'A[23]' is not an lvalue.
//  const bool  bit = A[13];     // Ok
//..
// Also note that there is no 'data' method returning a contiguous sequence of
// 'bool'.
//
// Finally note that, wherever an argument of non-boolean type -- e.g., the
// literal '5' (of type 'int') -- is used in a 'bdea_BitArray' method to
// specify a boolean (BIT) value, every non-zero value is automatically
// converted (via a standard conversion) to a 'bool' value 'true', before the
// method of the 'bdea_BitArray' is invoked:
//..
//  bdea_BitArray a(10);
//  assert(0 == a[5]);
//  a.replace(5, 24);           // Ok -- non-boolean value converted to 'true'.
//  assert(1 == a[5]);
//..
//
///Performance and Exception-Safety Guarantees
///-------------------------------------------
// The asymptotic worst-case performance of representative operations is
// characterized using big-oh notation, 'O[f(N,M)]', where 'N' and 'M' refer to
// the number of respective bits (i.e., 'length') of each array.  Here,
// *Average* *Case* complexity, denoted by 'A[f(N)]', means the *amortized*
// cost, which is defined as the asymptotic average cost of 'N' successive
// invocations.
//..
//                                        Average   Exception-Safety
//  Operation                Worst Case    Case        Guarantee
//  ---------                ----------   -------   ----------------
//  DEFAULT CTOR             O[1]                   No-Throw
//  COPY CTOR(N)             O[N]                   Exception Safe
//
//  N.DTOR()                 O[1]                   No-Throw
//
//  N.OP=(M)                 O[M]                   Basic <*>
//  N.insert(index, value)   O[N]                   Basic <*>
//
//  N.reserveCapacity(M)     O[N]                   Strong <*>
//  N.append(value)          O[N]         A[1]      Strong <*>
//
//  N.replace(index, value)  O[1]                   No-Throw
//  N.set1(value)            O[1]                   No-Throw
//  N.set0(value)            O[1]                   No-Throw
//
//  N.remove(index)          O[N]                   No-Throw
//  N.setAll0()              O[N]                   No-Throw
//  N.setAll1()              O[N]                   No-Throw
//
//  N.length()               O[1]                   No-Throw
//  N.OP[](index)            O[1]                   No-Throw
//
//  N.isAnySet               O[N]                   No-Throw
//  N.isAnyUnset             O[N]                   No-Throw
//
//  other const methods      O[1] .. O[N]           No-Throw
//
//  OP==(N, M)               O[min(N, M)]           No-Throw
//  OP!=(N, M)               O[min(N, M)]           No-Throw
//
//                                  <*> No-Throw guarantee when
//                                            capacity is sufficient.
//..
// Note that *all* of the non-creator methods of 'bdea_BitArray' provide the
// *No-Throw* guarantee whenever sufficient capacity is already available.
//
///Usage
///-----
// An efficient implementation of an arbitrary precision bit sequence container
// has myriad applications.  For example, a 'bdea_BitArray' can be used
// effectively as a parallel array of flags indicating some special property,
// such as, 'isNull', 'isBusinessDay', etc.; its use is especially indicated
// when (1) the number of elements of the primary array can grow large, and
// (2) the individual elements do not have the capacity or capability to store
// the information directly.
//
// As a simple example, we'll implement a (heavily elided) value-semantic
// template class, 'NullableVector<TYPE>', that behaves like a
// 'bsl::vector<TYPE>' but additionally allows storing a nullness flag to
// signify that the corresponding element was not specified.  Elements added to
// a 'NullableVector' are null by default, although there are manipulator
// functions that allow appending a non-null element.  Each null element stores
// the default value for 'TYPE'.
//
// Note that this class has a minimal interface (suitable for illustration
// purposes only) that allows users to either append a (non-null) 'TYPE' value
// or a null value.  A real 'NullableVector' class would support a complete set
// of *value* *semantic* operations, including copy construction, assignment,
// equality comparison, 'ostream' printing, and 'bdex' serialization.
//
// We start by defining the interface of 'NullableVector':
//..
//  template <class TYPE>
//  class NullableVector {
//      // This class implements a sequential container of elements of the
//      // parameterized 'TYPE'.
//
//      // DATA
//      bsl::vector<TYPE>  d_values;       // data elements
//      bdea_BitArray      d_nullFlags;    // 'true' indicates i'th element is
//                                         // null
//      bslma_Allocator   *d_allocator_p;  // held, but not owned
//
//      // NOT IMPLEMENTED
//      NullableVector(const NullableVector& original);
//      NullableVector& operator=(const NullableVector& rhs);
//
//    public:
//      // CREATORS
//      // ...
//      NullableVector(int initialLength, bslma_Allocator *basicAllocator = 0);
//          // Construct a vector having the specified 'initialLength' null
//          // elements.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently supplied
//          // default allocator is used.  The behavior is undefined unless
//          // '0 <= initialLength'.
//
//      // ...
//
//      ~NullableVector();
//          // Destroy this vector.
//
//      // MANIPULATORS
//      TYPE& modifiableElement(int index);
//          // Return a reference to the (valid) modifiable element at the
//          // specified 'index' in this vector.  The behavior is undefined
//          // unless '0 <= index < length()'.  Note that if the element at
//          // 'index' is null then the nullness flag is reset and the returned
//          // value is the default constructed 'TYPE' object.
//
//      void appendNullElement();
//          // Append a null element to this vector.  Note that the appended
//          // element will have the same value as a default constructed 'TYPE'
//          // object.
//
//      void appendElement(const TYPE& value);
//          // Append an element having the specified 'value' to the end of
//          // this vector.
//
//      void makeNonNull(int index);
//          // Make the element at the specified 'index' in this vector
//          // non-null.  The behavior is undefined unless
//          // '0 <= index < length()'.
//
//      void makeNull(int index);
//          // Make the element at the specified 'index' in this vector null.
//          // The behavior is undefined unless '0 <= index < length()'.  Note
//          // that the new value of the element will be the default
//          // constructed value for 'TYPE'.
//
//      void removeElement(int index);
//          // Remove the element at the specified 'index' in this vector.  The
//          // behavior is undefined unless '0 <= index < length()'.
//
//      // ACCESSORS
//      const TYPE& constElement(int index) const;
//          // Return a reference to the non-modifiable element at the
//          // specified 'index' in this vector.  The behavior is undefined
//          // unless '0 <= index < length()'.  Note that if the element at
//          // 'index' is null then the nullness flag is not reset and the
//          // returned value is the default constructed 'TYPE' object.
//
//      bool isElementNull(int index) const;
//          // Return 'true' if the element at the specified 'index' in this
//          // vector is null, and 'false' otherwise.  The behavior is
//          // undefined unless '0 <= index < length()'.
//
//      int length() const;
//          // Return the number of elements in this vector.
//
//      int numNullElements() const;
//          // Return the number of null elements in this vector.
//
//      bool isAnyElementNonNull() const;
//          // Return 'true' if any element in this vector is non-null, and
//          // 'false' otherwise.
//
//      bool isAnyElementNull() const;
//          // Return 'true' if any element in this vector is null, and 'false'
//          // otherwise.
//  };
//..
// Next we implement, in turn, each of the methods declared above.  Note also
// that, since the example class is a template, all methods would be
// implemented within the header file, regardless of whether or not they are
// (or should be) declared 'inline':
//..
//                      // --------------------
//                      // class NullableVector
//                      // --------------------
//
//      // CREATORS
//      template <typename TYPE>
//      NullableVector<TYPE>::NullableVector(int              initialLength,
//                                           bslma_Allocator *basicAllocator)
//      : d_values(initialLength, TYPE(), basicAllocator)
//      , d_nullFlags(initialLength, true, basicAllocator)
//      , d_allocator_p(bslma_Default::allocator(basicAllocator))
//      {
//      }
//
//      template <typename TYPE>
//      NullableVector<TYPE>::~NullableVector()
//      {
//          BSLS_ASSERT(d_values.size() == d_nullFlags.length());
//      }
//
//      // MANIPULATORS
//      template <typename TYPE>
//      inline
//      TYPE& NullableVector<TYPE>::modifiableElement(int index)
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          d_nullFlags.set(index, false);
//          return d_values[index];
//      }
//
//      template <typename TYPE>
//      inline
//      void NullableVector<TYPE>::appendElement(const TYPE& value)
//      {
//          d_values.push_back(value);
//          d_nullFlags.append(false);
//      }
//
//      template <typename TYPE>
//      inline
//      void NullableVector<TYPE>::appendNullElement()
//      {
//          d_values.push_back(TYPE());
//          d_nullFlags.append(true);
//      }
//
//      template <typename TYPE>
//      inline
//      void NullableVector<TYPE>::removeElement(int index)
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          d_values.erase(d_values.begin() + index);
//          d_nullFlags.remove(index);
//      }
//
//      template <typename TYPE>
//      inline
//      void NullableVector::makeNonNull(int index)
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          d_nullFlags.set(index, false);
//      }
//
//      template <typename TYPE>
//      inline
//      void NullableVector::makeNull(int index)
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          d_values[index] = TYPE();
//          d_nullFlags.set(index, true);
//      }
//
//      // ACCESSORS
//      template <typename TYPE>
//      inline
//      int NullableVector::length() const
//      {
//          return d_values.size();
//      }
//
//      template <typename TYPE>
//      inline
//      const TYPE& NullableVector::constElement(int index) const
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          return d_values[index];
//      }
//
//      template <typename TYPE>
//      inline
//      bool NullableVector::isElementNull(int index) const
//      {
//          BSLS_ASSERT_SAFE(0 <= index && index < length());
//          return d_nullFlags[index];
//      }
//
//      template <typename TYPE>
//      inline
//      int NullableVector<TYPE>::numNullElements() const
//      {
//          return d_nullFlags.numSet1();
//      }
//
//      template <typename TYPE>
//      inline
//      bool NullableVector<TYPE>::isAnyElementNonNull() const
//      {
//          return d_nullFlags.isAnySet0();
//      }
//
//      template <typename TYPE>
//      inline
//      bool NullableVector<TYPE>::isAnyElementNull() const
//      {
//          return d_nullFlags.isAnySet1();
//      }
//..
// Clients of 'NullableVector' can then use this class as follows:
//..
//       NullableVector<int> array(0);
//       const NullableVector<int>& ARRAY = array;
//       const int DEFAULT_INT = 0;
//       assert(0       == ARRAY.length());
//       assert(0       == ARRAY.numNullElements());
//       assert(false   == ARRAY.isAnyElementNonNull());
//       assert(false   == ARRAY.isAnyElementNull());
//
//       array.appendElement(5);
//       assert(1       == ARRAY.length());
//       assert(5       == ARRAY.constElement(0));
//       assert(false   == ARRAY.isElementNull(0));
//       assert(0       == ARRAY.numNullElements());
//       assert(true    == ARRAY.isAnyElementNonNull());
//       assert(false   == ARRAY.isAnyElementNull());
//
//       array.appendNullElement();
//       assert(2           == ARRAY.length());
//       assert(5           == ARRAY.constElement(0));
//       assert(DEFAULT_INT == ARRAY.constElement(1));
//       assert(false       == ARRAY.isElementNull(0));
//       assert(true        == ARRAY.isElementNull(1));
//       assert(1           == ARRAY.numNullElements());
//       assert(true        == ARRAY.isAnyElementNonNull());
//       assert(true        == ARRAY.isAnyElementNull());
//
//       array.makeNonNull(1);
//       assert(2           == ARRAY.length());
//       assert(5           == ARRAY.constElement(0));
//       assert(DEFAULT_INT == ARRAY.constElement(1));
//       assert(false       == ARRAY.isElementNull(0));
//       assert(false       == ARRAY.isElementNull(1));
//       assert(0           == ARRAY.numNullElements());
//       assert(true        == ARRAY.isAnyElementNonNull());
//       assert(false       == ARRAY.isAnyElementNull());
//
//       array.makeNull(0);
//       assert(2           == ARRAY.length());
//       assert(DEFAULT_INT == ARRAY.constElement(0));
//       assert(DEFAULT_INT == ARRAY.constElement(1));
//       assert(true        == ARRAY.isElementNull(0));
//       assert(false       == ARRAY.isElementNull(1));
//       assert(1           == ARRAY.numNullElements());
//       assert(true        == ARRAY.isAnyElementNonNull());
//       assert(true        == ARRAY.isAnyElementNull());
//
//       array.removeElement(0);
//       assert(1           == ARRAY.length());
//       assert(DEFAULT_INT == ARRAY.constElement(0));
//       assert(false       == ARRAY.isElementNull(0));
//       assert(0           == ARRAY.numNullElements());
//       assert(true        == ARRAY.isAnyElementNonNull());
//       assert(false       == ARRAY.isAnyElementNull());
//
//       array.removeElement(0);
//       assert(0       == ARRAY.length());
//       assert(0       == ARRAY.numNullElements());
//       assert(false   == ARRAY.isAnyElementNonNull());
//       assert(false   == ARRAY.isAnyElementNull());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDES_BITUTIL
#include <bdes_bitutil.h>
#endif

#ifndef INCLUDED_BDEU_BITSTRINGUTIL
#include <bdeu_bitstringutil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // ===================
                        // class bdea_BitArray
                        // ===================

class bdea_BitArray {
    // This class implements an efficient, value-semantic array of boolean
    // (a.k.a. bit, i.e., binary digit) values stored in contiguous memory.
    // The physical capacity of this array may grow, but never shrinks.
    // Capacity may be reserved initially via a constructor, or at any time
    // thereafter by using the 'reserveCapacity' method; otherwise, capacity
    // will be increased automatically as needed.  Note that capacity is not a
    // *salient* attribute of this object, and, as such, does not contribute to
    // overall value.  Also note that this class provides an implicit no-throw
    // guarantee for all methods (including manipulators) that do not attempt
    // to alter capacity.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two objects have the same value can be
    // found in the description of the homogeneous (free) 'operator==' for this
    // class.)  This class is *exception* *safe*, but provides no general
    // guarantee of rollback: If an exception is thrown during the invocation
    // of a method on a pre-existing object, the object will be left in a
    // coherent state, but (unless otherwise stated) its value is not defined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) for the same operation
    // is supported in all cases.

  public:
    // CONSTANTS
    enum {
        BDEA_BITS_PER_BYTE = CHAR_BIT,
        BDEA_BYTES_PER_INT = sizeof(int),
        BDEA_BITS_PER_INT  = BDEA_BYTES_PER_INT * BDEA_BITS_PER_BYTE

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
        , BITS_PER_BYTE = BDEA_BITS_PER_BYTE
        , BYTES_PER_INT = BDEA_BYTES_PER_INT
        , BITS_PER_INT  = BDEA_BITS_PER_INT
#endif

    };

  private:
    // DATA
    bsl::vector<int> d_array;   // array of integers
    int              d_length;  // number of significant bits in this array

    // FRIENDS
    friend bool operator==(const bdea_BitArray&, const bdea_BitArray&);

    // PRIVATE CLASS METHODS
    static int arraySize(int numBits);
        // Return the size of the integer array required to store the specified
        // 'numBits'.  The behavior is undefined unless '0 <= numBits'.

    // PRIVATE ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the allocator with which this object was constructed.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdea_BitArray,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    struct InitialCapacity {
        // This 'struct' enable uniform use of an optional integral constructor
        // argument to specify the initial internal capacity (in bits).  For
        // example:
        //..
        //  bdea_BitArray x(bdea_BitArray::InitialCapacity(8));
        //..
        // defines a 'bdea_BitArray' object 'x' with an initial capacity of 8
        // (bits), but a logical length of 0 (bits).

        // DATA
        int d_i;

        // CREATORS
        explicit InitialCapacity(int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit bdea_BitArray(bslma_Allocator *basicAllocator = 0);
    explicit bdea_BitArray(int              initialLength,
                           bslma_Allocator *basicAllocator = 0);
    bdea_BitArray(int              initialLength,
                  bool             value,
                  bslma_Allocator *basicAllocator = 0);
    explicit bdea_BitArray(const InitialCapacity&  numBits,
                           bslma_Allocator        *basicAllocator = 0);
    bdea_BitArray(const InitialCapacity&  numBits,
                  int                     initialLength,
                  bslma_Allocator        *basicAllocator = 0);
    bdea_BitArray(const InitialCapacity&  numBits,
                  int                     initialLength,
                  bool                    value,
                  bslma_Allocator        *basicAllocator = 0);
        // Create an array of binary digits (bits).  By default, the array is
        // empty and has a capacity of 0 bits.  Optionally reserve an
        // 'InitialCapacity' that will accommodate a length of up to the
        // specified 'numBits' without subsequent reallocation.  If no initial
        // capacity is specified, capacity will be increased on demand (as
        // appropriate).  Optionally specify the (non-negative) 'initialLength'
        // (in bits) of the array.  If 'initialLength' is not specified, the
        // default length is 0.  If 'initialLength' is specified, optionally
        // specify the 'value' for each bit in the 'initialLength'.  If 'value'
        // is not specified, the default value for each bit is 'false' (0).
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'numBits.d_i >= 0' and
        // 'initialLength >= 0'.

    bdea_BitArray(const bdea_BitArray&  original,
                  bslma_Allocator      *basicAllocator = 0);
        // Create an array of binary digits (bits) having the same value as the
        // specified 'original' array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~bdea_BitArray();
        // Destroy this object.

    // MANIPULATORS
    bdea_BitArray& operator=(const bdea_BitArray& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference to this modifiable array.

    bdea_BitArray& operator&=(const bdea_BitArray& rhs);
        // Bitwise AND the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference to this
        // modifiable array.  If 'length() > rhs.length()', the unmatched
        // most-significant bits in this array are set to 0; otherwise, any
        // unmatched most-significant bits in 'rhs' are ignored.  Note that
        // this behavior is consistent with zero-extending 'rhs' if needed, but
        // leaving the length of this array unchanged.

    bdea_BitArray& operator|=(const bdea_BitArray& rhs);
        // Bitwise OR the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference to this
        // modifiable array.  If 'length() > rhs.length()', the unmatched
        // most-significant bits in this array are left unchanged; otherwise,
        // any unmatched most-significant bits in 'rhs' are ignored.  Note that
        // this behavior is consistent with zero-extending 'rhs' if needed, but
        // leaving the length of this array unchanged.

    bdea_BitArray& operator^=(const bdea_BitArray& rhs);
        // Bitwise XOR the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference to this
        // modifiable array.  If 'length() > rhs.length()', the unmatched
        // most-significant bits in this array are left unchanged; otherwise,
        // any unmatched most-significant bits in 'rhs' are ignored.  Note that
        // this behavior is consistent with zero-extending 'rhs' if needed, but
        // leaving the length of this array unchanged.

    bdea_BitArray& operator-=(const bdea_BitArray& rhs);
        // Bitwise MINUS the value of the specified 'rhs' array from the value
        // of this array (retaining the results), and return a reference to
        // this modifiable array.  If 'length() > rhs.length()', the unmatched
        // most-significant bits in this array are left unchanged; otherwise,
        // any unmatched most-significant bits in 'rhs' are ignored.  Note that
        // this behavior is consistent with zero-extending 'rhs' if needed, but
        // leaving the length of this array unchanged.  Also note that the
        // logical difference 'A - B' is defined to be 'A & !B'.

    bdea_BitArray& operator<<=(int numBits);
        // Shift the bits in this array LEFT by the specified 'numBits',
        // filling lower-order bits with zeros (retaining the results), and
        // return a reference to this modifiable array.  The behavior is
        // undefined unless '0 <= numBits <= length()'.  Note that the length
        // of this array is unchanged and the highest-order 'numBits' are
        // discarded.

    bdea_BitArray& operator>>=(int numBits);
        // Shift the bits in this array RIGHT by the specified 'numBits',
        // filling higher-order bits with zeros (retaining the results), and
        // return a reference to this modifiable array.  The behavior is
        // undefined unless '0 <= numBits <= length()'.  Note that the length
        // of this array is unchanged and the lowest-order 'numBits' are
        // discarded.

    void andEqual(int index, bool value);
        // AND the bit at the specified 'index' in this array with the
        // specified 'value' (retaining the result).  The behavior is undefined
        // unless '0 <= index < length()'.

    void andEqual(int                  dstIndex,
                  const bdea_BitArray& srcArray,
                  int                  srcIndex,
                  int                  numBits);
        // Bitwise AND the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray',
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstIndex + numBits <= length()', and
        // 'srcIndex + numBits <= srcArray.length()'.

    void append(bool value);
        // Append to this array the specified 'value'.  Note that this method
        // has the same behavior as:
        //..
        //  insert(length(), value);
        //..

    void append(bool value, int numBits);
        // Append to this array the specified 'numBits' having the specified
        // 'value'.  The behavior is undefined unless '0 <= numBits'.  Note
        // that this method has the same behavior as:
        //..
        //  insert(length(), value, numBits);
        //..

    void append(const bdea_BitArray& srcArray);
        // Append to this array the values from the specified 'srcArray'.  Note
        // that this method has the same behavior as:
        //..
        //  insert(length(), srcArray);
        //..

    void append(const bdea_BitArray& srcArray, int srcIndex, int numBits);
        // Append to this array the specified 'numBits' from the specified
        // 'srcArray', beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless '0 <= srcIndex < srcArray.length()',
        // '0 <= numBits', and 'srcIndex + numBits <= srcArray.length()'.  Note
        // that this method has the same behavior as:
        //..
        //  insert(length(), srcArray, srcIndex, numBits);
        //..

    void insert(int dstIndex, bool value);
        // Insert into this array at the specified 'dstIndex' the specified
        // 'value'.  All values with indices at or above 'dstIndex' in this
        // array are shifted up by one bit position.  The behavior is undefined
        // unless '0 <= dstIndex <= length()'.

    void insert(int dstIndex, bool value, int numBits);
        // Insert into this array at the specified 'dstIndex' the specified
        // 'numBits' having the specified 'value'.  All values with indices at
        // or above 'dstIndex' in this array are shifted up by 'numBits' bit
        // positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and '0 <= numBits'.

    void insert(int dstIndex, const bdea_BitArray& srcArray);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // values from the specified 'srcArray'.  All values with indices at or
        // above 'dstIndex' in this array are shifted up by 'srcArray.length()'
        // bit positions.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insert(int                  dstIndex,
                const bdea_BitArray& srcArray,
                int                  srcIndex,
                int                  numBits);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numBits' from the specified 'srcArray', beginning at the
        // specified 'srcIndex'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by 'numBits' positions.  The behavior is
        // undefined unless '0 <= dstIndex <= length()', '0 <= srcIndex',
        // '0 <= numBits', and 'srcIndex + numBits <= srcArray.length()'.

    void minusEqual(int index, bool value);
        // MINUS (subtract) from the bit at the specified 'index' in this array
        // the specified 'value' (retaining the result).  The behavior is
        // undefined unless '0 <= index < length()'.  Note that the logical
        // difference 'A - B' is defined to be 'A & !B'.

    void minusEqual(int                  dstIndex,
                    const bdea_BitArray& srcArray,
                    int                  srcIndex,
                    int                  numBits);
        // Bitwise MINUS (subtract from) the specified 'numBits' in this array,
        // beginning at the specified 'dstIndex', values from the specified
        // 'srcArray', beginning at the specified 'srcIndex' (retaining the
        // results).  The behavior is undefined unless '0 <= dstIndex',
        // '0 <= srcIndex', '0 <= numBits', 'dstIndex + numBits <= length()',
        // and 'srcIndex + numBits <= srcArray.length()'.  Note that the
        // logical difference 'A - B' is defined to be 'A & !B'.

    void orEqual(int index, bool value);
        // OR the bit at the specified 'index' in this array with the specified
        // 'value' (retaining the result).  The behavior is undefined unless
        // '0 <= index < length()'.

    void orEqual(int                  dstIndex,
                 const bdea_BitArray& srcArray,
                 int                  srcIndex,
                 int                  numBits);
        // Bitwise OR the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray',
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstIndex + numBits <= length()', and
        // 'srcIndex + numBits <= srcArray.length()'.

    void remove(int index);
        // Remove from this array the bit at the specified 'index'.  All values
        // at indices above 'index' in this array are shifted down by one bit
        // position.  The behavior is undefined unless '0 <= index < length()'.

    void remove(int index, int numBits);
        // Remove from this array the specified 'numBits', beginning at the
        // specified 'index'.  All values at indices above 'index' in this
        // array are shifted down by 'numBits' positions.  The behavior is
        // undefined unless '0 <= index', '0 <= numBits', and
        // 'index + numBits <= length()'.

    void removeAll();
        // Remove all of the bits in this array, leaving the length zero, but
        // having no effect on capacity.

    void replace(int index, bool value);
        // Replace the value at the specified 'index' in this array with the
        // specified 'value'.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that this method has the same
        // behavior as:
        //..
        //  set(index, value);
        //..

    void replace(int                  dstIndex,
                 const bdea_BitArray& srcArray,
                 int                  srcIndex,
                 int                  numBits);
        // Replace the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex' with values from the specified 'srcArray'
        // beginning at the specified 'srcIndex'.  The behavior is undefined
        // unless '0 <= dstIndex', '0 <= srcIndex', '0 <= numBits',
        // 'dstIndex + numBits <= length()', and
        // 'srcIndex + numBits <= srcArray.length()'.  Note that, absent
        // aliasing, this method has the same behavior as, but is more
        // efficient than:
        //..
        //  remove(index, numBits);
        //  insert(dstIndex, srcArray, srcIndex, numBits);
        //..

    void reserveCapacity(int numBits);
        // Reserve sufficient internal capacity to accommodate a length of at
        // least the specified 'numBits' without reallocation (but otherwise
        // need have no effect).  If an exception is thrown during this
        // reallocation attempt (i.e., by the memory allocator indicated at
        // construction) the value of this array is guaranteed to be unchanged.
        // The behavior is undefined unless '0 <= numBits'.

    void reserveCapacityRaw(int numBits);
        // Reserve exactly enough *additional* capacity to accommodate the
        // specified 'numBits' without reallocation.  The behavior is undefined
        // unless '0 <= numBits'.  Note that if 'numBits <= length()', this
        // operation is guaranteed to have no effect.
        //
        // WARNING: Inappropriate use of this method, such as repeatedly
        // calling:
        //..
        //  void f(bdea_BitArray a, bool v)
        //  {
        //      reserveCapacityRaw(a.length() + 1);
        //      a.append(v);
        //  }
        //..
        // may lead to poor (e.g., quadratic) runtime behavior.

    void rotateLeft(int numBits);
        // Shift the values in this array to the left by the specified
        // 'numBits' positions, with the high-order values "rotating" into the
        // low-order ones.  The behavior is undefined unless
        // '0 <= numBits <= length()'.  Note that the length of this array
        // remains unchanged.

    void rotateRight(int numBits);
        // Shift the values in this array to the right by the specified
        // 'numBits' positions, with the low-order values "rotating" into the
        // high-order ones.  The behavior is undefined unless
        // '0 <= numBits <= length()'.  Note that the length of this array
        // remains unchanged.

    void set(int index, bool value);
        // Set the value at the specified 'index' in this array to the
        // specified 'value'.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that this method has the same
        // behavior as:
        //..
        //   replace(index, value);
        //..

    void set0(int index);
        // Set to 0 the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless '0 <= index < length()'.

    void set0(int index, int numBits);
        // Set to 0 the specified 'numBits' values in this array, beginning at
        // the specified 'index'.  The behavior is undefined unless
        // '0 <= index < length()', '0 <= numBits', and
        // 'index + numBits <= length()'.

    void set1(int index);
        // Set to 1 the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless '0 <= index < length()'.

    void set1(int index, int numBits);
        // Set to 1 the specified 'numBits' values in this array, beginning at
        // the specified 'index'.  The behavior is undefined unless
        // '0 <= index < length()', '0 <= numBits', and
        // 'index + numBits <= length()'.

    void setAll0();
        // Set to 0 the value of every bit in this array.

    void setAll1();
        // Set to 1 the value of every bit in this array.

    void setLength(int newLength);
    void setLength(int newLength, bool value);
        // Set the number of bits in this array to the specified 'newLength'.
        // If 'newLength < length()', bits at index positions at or above
        // 'newLength' are removed; otherwise, any new bits (at or above the
        // current length) are initialized to the optionally specified 'value',
        // or to 0 if 'value' is not specified.  The behavior is undefined
        // unless '0 <= newLength'.

    void setLengthRaw(int newLength);
        // Set the number of bits in this array to the specified 'newLength'.
        // If 'newLength < length()', bits at index positions at or above
        // 'newLength' are removed; otherwise, any new bits (at or above the
        // current length) remain _uninitialized_.  The behavior is undefined
        // unless '0 <= newLength'.

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

    void swap(bdea_BitArray& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee.  The
        // behavior is undefined if the two objects being swapped have
        // non-equal allocators.

    void swap(int index1, int index2);
        // Efficiently exchange the values of the bits at the specified
        // 'index1' and 'index2' indices.  The behavior is undefined unless
        // '0 <= index1 < length()' and '0 <= index2 < length()'.

    void toggle(int index);
        // Complement the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless '0 <= index < length()'.

    void toggle(int index, int numBits);
        // Complement the values of each of the specified 'numBits' in this
        // array, beginning at the specified 'index'.  The behavior is
        // undefined unless '0 <= index < length()', '0 <= numBits', and
        // 'index + numBits <= length()'.

    void toggleAll();
        // Complement the value of every bit in this array.  Note that the
        // behavior is analogous to applying the '~' operator to an object of
        // fundamental type 'int'.

    void xorEqual(int index, bool value);
        // XOR the bit at the specified 'index' in this array with the
        // specified 'value' (retaining the result).  The behavior is undefined
        // unless '0 <= index < length()'.

    void xorEqual(int                  dstIndex,
                  const bdea_BitArray& srcArray,
                  int                  srcIndex,
                  int                  numBits);
        // Bitwise XOR the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray',
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numBits', 'dstIndex + numBits <= length()', and
        // 'srcIndex + numBits <= srcArray.length()'.

    // ACCESSORS
    bool operator[](int index) const;
        // Return the value of the bit at the specified 'index' in this array.
        // The behavior is undefined unless '0 <= index < length()'.

    int find0AtLargestIndex() const;
        // Return the index of the most-significant 0 bit in this array, if
        // such a bit exists, and a negative value otherwise.

    int find0AtLargestIndexGE(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find0AtLargestIndexGT(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.

    int find0AtLargestIndexLE(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find0AtLargestIndexLT(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.

    int find0AtSmallestIndex() const;
        // Return the index of the least-significant 0 bit in this array, if
        // such a bit exists, and a negative value otherwise.

    int find0AtSmallestIndexGE(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find0AtSmallestIndexGT(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.

    int find0AtSmallestIndexLE(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find0AtSmallestIndexLT(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.

    int find1AtLargestIndex() const;
        // Return the index of the most-significant 1 bit in this array, if
        // such a bit exists, and a negative value otherwise.

    int find1AtLargestIndexGE(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find1AtLargestIndexGT(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.

    int find1AtLargestIndexLE(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is less than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find1AtLargestIndexLT(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.

    int find1AtSmallestIndex() const;
        // Return the index of the least-significant 1 bit in this array, if
        // such a bit exists, and a negative value otherwise.

    int find1AtSmallestIndexGE(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find1AtSmallestIndexGT(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.

    int find1AtSmallestIndexLE(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.

    int find1AtSmallestIndexLT(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.

    bool isAnySet1() const;
        // Return 'true' if the value of any bit in this array is 1, and
        // 'false' otherwise.

    bool isAnySet0() const;
        // Return 'true' if the value of any bit in this array is 0, and
        // 'false' otherwise.

    int length() const;
        // Return the number of bit positions in this array.

    int numSet0() const;
        // Return the number of bits in this array having a value of 0.

    int numSet1() const;
        // Return the number of bits in this array having a value of 1.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // optionally specified indentation 'level' and return a reference to
        // the modifiable 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  Each line is indented by
        // the absolute value of 'level * spacesPerLevel'.  If 'level' is
        // negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, suppress line breaks and format the
        // entire output on one line.  If 'stream' is initially invalid, this
        // operation has no effect.  Note that a trailing newline is provided
        // in multiline mode only.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object to the specified output 'stream'
        // using the specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    int findBit0AtLargestIndex() const;
        // Return the index of the most-significant 0 bit in this array, if
        // such a bit exists, and a negative value otherwise.
        //
        // DEPRECATED: Use 'find0AtLargestIndex' instead.

    int findBit0AtLargestIndexGE(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGE' instead.

    int findBit0AtLargestIndexGT(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexGT' instead.

    int findBit0AtLargestIndexLE(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLE' instead.

    int findBit0AtLargestIndexLT(int index) const;
        // Return the index of the most-significant 0 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtLargestIndexLT' instead.

    int findBit0AtSmallestIndex() const;
        // Return the index of the least-significant 0 bit in this array, if
        // such a bit exists, and a negative value otherwise.
        //
        // DEPRECATED: Use 'find0AtSmallestIndex' instead.

    int findBit0AtSmallestIndexGE(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGE' instead.

    int findBit0AtSmallestIndexGT(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexGT' instead.

    int findBit0AtSmallestIndexLE(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLE' instead.

    int findBit0AtSmallestIndexLT(int index) const;
        // Return the index of the least-significant 0 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find0AtSmallestIndexLT' instead.

    int findBit1AtLargestIndex() const;
        // Return the index of the most-significant 1 bit in this array, if
        // such a bit exists, and a negative value otherwise.
        //
        // DEPRECATED: Use 'find1AtLargestIndex' instead.

    int findBit1AtLargestIndexGE(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGE' instead.

    int findBit1AtLargestIndexGT(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexGT' instead.

    int findBit1AtLargestIndexLE(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLE' instead.

    int findBit1AtLargestIndexLT(int index) const;
        // Return the index of the most-significant 1 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtLargestIndexLT' instead.

    int findBit1AtSmallestIndex() const;
        // Return the index of the least-significant 1 bit in this array, if
        // such a bit exists, and a negative value otherwise.
        //
        // DEPRECATED: Use 'find1AtSmallestIndex' instead.

    int findBit1AtSmallestIndexGE(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is greater than or equal to the specified 'index', if such
        // a bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGE' instead.

    int findBit1AtSmallestIndexGT(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is greater than the specified 'index', if such a bit
        // exists, and a negative value otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexGT' instead.

    int findBit1AtSmallestIndexLE(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is less than or equal to the specified 'index', if such a
        // bit exists, and a negative value otherwise.  The behavior is
        // undefined unless '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLE' instead.

    int findBit1AtSmallestIndexLT(int index) const;
        // Return the index of the least-significant 1 bit in this array whose
        // position is less than the specified 'index', if such a bit exists,
        // and a negative value otherwise.  The behavior is undefined unless
        // '0 <= index < length()'.
        //
        // DEPRECATED: Use 'find1AtSmallestIndexLT' instead.

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

#endif
};

// FREE OPERATORS
bool operator==(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays have the same
    // value, and 'false' otherwise.  Two arrays have the same value if they
    // have the same length, and corresponding bits at each bit position have
    // the same value.

bool operator!=(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays do not have the
    // same value, and 'false' otherwise.  Two arrays do not have the same
    // value if they do have the same length, or there is at least one valid
    // index position at which corresponding bits do not have the same value.

bdea_BitArray operator~(const bdea_BitArray& array);
    // Return the bitwise complement ("toggle") of the specified 'array'.

bdea_BitArray operator<<(const bdea_BitArray& array, int numBits);
    // Return the value of the specified 'array' left-shifted by the specified
    // 'numBits' positions, having filled the lower-index positions with zeros.
    // The behavior is undefined unless '0 <= numBits <= array.length()'.  Note
    // that the length of this array remains unchanged, and that the
    // highest-order 'numBits' are discarded.

bdea_BitArray operator>>(const bdea_BitArray& array, int numBits);
    // Return the value of the specified 'array' right-shifted by the specified
    // 'numBits' positions, having filled the higher-index positions with
    // zeros.  The behavior is undefined unless
    // '0 <= numBits <= array.length()'.  Note that the length of this array
    // remains unchanged, and that the lowest-order 'numBits' are discarded.

bdea_BitArray operator&(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return the value that is the bitwise AND of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order bits set to
    // 0.  Note that this behavior is consistent with zero-extending the
    // shorter array.

bdea_BitArray operator|(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return the value that is the bitwise OR of the specified 'lhs' and 'rhs'
    // arrays.  The length of the resulting bit array will be the maximum of
    // that of 'lhs' and 'rhs', with any unmatched high-order bits copied
    // unchanged.  Note that this behavior is consistent with zero-extending
    // the shorter array.

bdea_BitArray operator^(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return the value that is the bitwise XOR of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order bits copied
    // unchanged.  Note that this behavior is consistent with zero-extending
    // the shorter array.

bdea_BitArray operator-(const bdea_BitArray& lhs, const bdea_BitArray& rhs);
    // Return the value that is the bitwise MINUS of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order 'lhs' bits
    // copied unchanged, and any unmatched high-order 'rhs' bits set to 0.
    // Note that this behavior is consistent with zero-extending the shorter
    // array.

bsl::ostream& operator<<(bsl::ostream& stream, const bdea_BitArray& rhs);
    // Format the bits in the specified 'rhs' bit array to the specified output
    // 'stream' in a single-line format, and return a reference to the
    // modifiable 'stream'.

// FREE FUNCTIONS
void swap(bdea_BitArray& a, bdea_BitArray& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee.  The behavior is undefined if the two
    // objects being swapped have non-equal allocators.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------
                        // class bdea_BitArray
                        // -------------------

// PRIVATE CLASS METHODS
inline
int bdea_BitArray::arraySize(int numBits)
{
    BSLS_ASSERT_SAFE(0 <= numBits);

    return (numBits + BDEA_BITS_PER_INT - 1) / BDEA_BITS_PER_INT;
}

// PRIVATE ACCESSORS
inline
bslma_Allocator *bdea_BitArray::allocator() const
{
    return d_array.get_allocator().mechanism();
}

// CLASS METHODS
inline
int bdea_BitArray::maxSupportedBdexVersion()
{
    return 1;
}

// MANIPULATORS
inline
void bdea_BitArray::set0(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    bdeu_BitstringUtil::set(&d_array.front(), index, false);
}

inline
void bdea_BitArray::set1(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    bdeu_BitstringUtil::set(&d_array.front(), index, true);
}

inline
void bdea_BitArray::andEqual(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    if (!value) {
        set0(index);
    }
}

inline
void bdea_BitArray::orEqual(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        set1(index);
    }
}

inline
void bdea_BitArray::xorEqual(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        const int pos = index % BDEA_BITS_PER_INT;
        const int idx = index / BDEA_BITS_PER_INT;
        const int bit = 1 << pos;
        d_array[idx] ^= bit;
    }
}

inline
void bdea_BitArray::minusEqual(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        set0(index);
    }
}

inline
void bdea_BitArray::set(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    bdeu_BitstringUtil::set(&d_array.front(), index, value);
}

inline
void bdea_BitArray::append(bool value)
{
    // Note that '++d_length' cannot be factored out of the following.  In
    // particular, 'd_length' must be incremented prior to calling 'set' to
    // satisfy that function's pre-conditions.

    if (0 == d_length % BDEA_BITS_PER_INT) {
        d_array.push_back(!!value);
        ++d_length;
    }
    else {
        set(d_length++, value);
    }
}

inline
void bdea_BitArray::append(bool value, int numBits)
{
    BSLS_ASSERT_SAFE(0 <= numBits);

    insert(length(), value, numBits);
}

inline
void bdea_BitArray::removeAll()
{
    d_length = 0;
    d_array.clear();
}

inline
void bdea_BitArray::replace(int index, bool value)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    set(index, value);
}

inline
void bdea_BitArray::reserveCapacity(int numBits)
{
    BSLS_ASSERT_SAFE(0 <= numBits);

    d_array.reserve(arraySize(numBits));
}

inline
void bdea_BitArray::reserveCapacityRaw(int numBits)
{
    BSLS_ASSERT_SAFE(0 <= numBits);

    d_array.reserve(arraySize(numBits));
}

inline
void bdea_BitArray::toggle(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    const int pos = index % BDEA_BITS_PER_INT;
    const int idx = index / BDEA_BITS_PER_INT;
    d_array[idx] ^= (1 << pos);
}

template <class STREAM>
STREAM& bdea_BitArray::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int newLength;
            stream.getLength(newLength);
            if (!stream) {
                return stream;                                        // RETURN
            }

            const int len = (arraySize(newLength));
            d_array.clear();
            d_length = 0;
            d_array.reserve(len);

            if (4 == sizeof(int)) {
                const int lenmm = len - 1;
                for (int i = 0; i < lenmm; i += 2) {
                    int tmp, tmp2;
                    stream.getInt32(tmp2);
                    if (!stream) {
                        d_array.clear();
                        d_length = 0;
                        return stream;                                // RETURN
                    }
                    stream.getInt32(tmp);
                    if (!stream) {
                        d_array.clear();
                        d_length = 0;
                        return stream;                                // RETURN
                    }
                    d_array.push_back(tmp);
                    d_array.push_back(tmp2);
                    d_length += 64;
                }
                if (len % 2) {
                    int tmp;
                    stream.getInt32(tmp);
                    if (!stream) {
                        d_array.clear();
                        d_length = 0;
                        return stream;                                // RETURN
                    }
                    stream.getInt32(tmp);
                    if (!stream) {
                        d_array.clear();
                        d_length = 0;
                        return stream;                                // RETURN
                    }
                    d_array.push_back(tmp);
                    d_length += 32;
                }
            }
            else {
                for (int i = 0; i < len; ++i) {
                    bsls_Types::Int64 tmp;
                    stream.getInt64(tmp);
                    if (!stream) {
                        return stream;                                // RETURN
                    }
                    d_array.push_back((int)tmp);
                    d_length += 64;
                }
            }

            d_length = newLength;

            // Test for corrupted data.

            if (d_length % BDEA_BITS_PER_INT
             && d_array[d_array.size() - 1] >>
                                              (d_length % BDEA_BITS_PER_INT)) {

                // Correct invalid bit array and invalidate stream.  This is
                // fastest way to valid, arbitrary state.

                const int pos = d_length % bdea_BitArray::BDEA_BITS_PER_INT;
                if (pos) {
                    bdes_BitUtil::replaceZero(
                                       &d_array[d_array.size() - 1],
                                       pos,
                                       bdea_BitArray::BDEA_BITS_PER_INT - pos);
                }
                stream.invalidate();
                return stream;                                        // RETURN
            }

          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
bool bdea_BitArray::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(index < d_length);

    return bdeu_BitstringUtil::get(&d_array.front(), index);
}

inline
int bdea_BitArray::length() const
{
    return d_length;
}

template <class STREAM>
STREAM& bdea_BitArray::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putLength(d_length);
        const int len = static_cast<int>(d_array.size());
        if (4 == sizeof(int)) {
            const int lenmm = len - 1;
            for (int i = 0; i < lenmm; i += 2) {
                stream.putInt32(d_array[i + 1]);
                stream.putInt32(d_array[i]);
            }
            if (len % 2) {
                stream.putInt32(0);
                stream.putInt32(d_array[lenmm]);
            }
        }
        else {
          for (int i = 0; i < len; ++i) {
              stream.putInt64(d_array[i]);
          }
        }
      } break;
    }
    return stream;
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

inline
int bdea_BitArray::findBit0AtLargestIndex() const
{
    return find0AtLargestIndex();
}

inline
int bdea_BitArray::findBit0AtLargestIndexGE(int index) const
{
    return find0AtLargestIndexGE(index);
}

inline
int bdea_BitArray::findBit0AtLargestIndexGT(int index) const
{
    return find0AtLargestIndexGT(index);
}

inline
int bdea_BitArray::findBit0AtLargestIndexLE(int index) const
{
    return find0AtLargestIndexLE(index);
}

inline
int bdea_BitArray::findBit0AtLargestIndexLT(int index) const
{
    return find0AtLargestIndexLT(index);
}

inline
int bdea_BitArray::findBit0AtSmallestIndex() const
{
    return find0AtSmallestIndex();
}

inline
int bdea_BitArray::findBit0AtSmallestIndexGE(int index) const
{
    return find0AtSmallestIndexGE(index);
}

inline
int bdea_BitArray::findBit0AtSmallestIndexGT(int index) const
{
    return find0AtSmallestIndexGT(index);
}

inline
int bdea_BitArray::findBit0AtSmallestIndexLE(int index) const
{
    return find0AtSmallestIndexLE(index);
}

inline
int bdea_BitArray::findBit0AtSmallestIndexLT(int index) const
{
    return find0AtSmallestIndexLT(index);
}

inline
int bdea_BitArray::findBit1AtLargestIndex() const
{
    return find1AtLargestIndex();
}

inline
int bdea_BitArray::findBit1AtLargestIndexGE(int index) const
{
    return find1AtLargestIndexGE(index);
}

inline
int bdea_BitArray::findBit1AtLargestIndexGT(int index) const
{
    return find1AtLargestIndexGT(index);
}

inline
int bdea_BitArray::findBit1AtLargestIndexLE(int index) const
{
    return find1AtLargestIndexLE(index);
}

inline
int bdea_BitArray::findBit1AtLargestIndexLT(int index) const
{
    return find1AtLargestIndexLT(index);
}

inline
int bdea_BitArray::findBit1AtSmallestIndex() const
{
    return find1AtSmallestIndex();
}

inline
int bdea_BitArray::findBit1AtSmallestIndexGE(int index) const
{
    return find1AtSmallestIndexGE(index);
}

inline
int bdea_BitArray::findBit1AtSmallestIndexGT(int index) const
{
    return find1AtSmallestIndexGT(index);
}

inline
int bdea_BitArray::findBit1AtSmallestIndexLE(int index) const
{
    return find1AtSmallestIndexLE(index);
}

inline
int bdea_BitArray::findBit1AtSmallestIndexLT(int index) const
{
    return find1AtSmallestIndexLT(index);
}

inline
int bdea_BitArray::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

#endif

// FREE OPERATORS
inline
bool operator!=(const bdea_BitArray& lhs, const bdea_BitArray& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdea_BitArray& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(bdea_BitArray& a, bdea_BitArray& b)
{
    a.swap(b);
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
