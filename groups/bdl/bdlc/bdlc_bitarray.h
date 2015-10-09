// bdlc_bitarray.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLC_BITARRAY
#define INCLUDED_BDLC_BITARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a space-efficient, sequential container of boolean values.
//
//@CLASSES:
//   bdlc::BitArray: vector-like, sequential container of boolean values
//
//@DESCRIPTION: This component implements an efficient value-semantic,
// sequential container of boolean values (i.e., 0 or 1) of type 'bool'.  A
// 'BitArray' may be thought of as an arbitrary-precision 'unsigned int'.  This
// metaphor is used to motivate the rich set of "bitwise" operations on
// 'BitArray' objects provided by this component, as well as the notion of
// "zero extension" of a (shorter) bit array during binary operations on bit
// arrays having lengths that are not the same.
//
///Bit-Array-Specific Functionality
///--------------------------------
// In addition to many typical vector-like container methods, this component
// supports "boolean" functionality unique to 'BitArray'.  However, unlike
// other standard container types such as 'bsl::bitset', there is no
// 'operator[](bsl::size_t index)' that returns a reference to a (modifiable)
// boolean element at the specified index position.  This difference is due to
// the densely-packed internal representation of bits within bit arrays:
//..
//  bdlc::BitArray mA(128);
//  assert(0 == mA[13]);             // Ok
//  mA[13] = 'false';                // Error -- 'mA[13]' is not an lvalue.
//  mA.assign(13, 1);                // Ok
//
//  const bdlc::BitArray& A = mA;    // Ok
//  assert(1 == A[13]);              // Ok
//  const bool *bp  = &A[13]         // Error -- 'A[13]' is not an lvalue.
//  const bool  bit = A[13];         // Ok
//..
// Also note that there is no 'data' method returning a contiguous sequence of
// 'bool'.
//
// Finally note that, wherever an argument of non-boolean type -- e.g., the
// literal '5' (of type 'int') -- is used in a 'BitArray' method to specify a
// boolean (bit) value, every non-zero value is automatically converted (via a
// standard conversion) to a 'bool' value 'true', before the method of the
// 'BitArray' is invoked:
//..
//  bdlc::BitArray a(10);
//  assert(0 == a[5]);
//  a.assign(5, 24);            // Ok -- non-boolean value converted to 'true'.
//  assert(1 == a[5]);
//..
//
///Performance and Exception-Safety Guarantees
///-------------------------------------------
// The asymptotic worst-case performance of representative operations is
// characterized using big-O notation, 'O[f(N,M)]', where 'N' and 'M' refer to
// the number of respective bits (i.e., 'length') of arrays 'X' and 'Y',
// respectively.  Here, *Amortized* *Case* complexity, denoted by 'A[f(N)]', is
// defined as the average of 'N' successive invocations, as 'N' gets very
// large.
//..
//                                        Average   Exception-Safety
//  Operation                Worst Case    Case        Guarantee
//  ---------                ----------   -------   ----------------
//  DEFAULT CTOR             O[1]                   No-Throw
//  COPY CTOR(Y)             O[M]                   Exception Safe
//
//  X.DTOR()                 O[1]                   No-Throw
//
//  X.OP=(Y)                 O[M]                   Basic <*>
//  X.insert(index, value)   O[N]                   Basic <*>
//
//  X.reserveCapacity(M)     O[N]                   Strong <*>
//  X.append(value)          O[N]         A[1]      Strong <*>
//
//  X.assign(index, value)   O[1]                   No-Throw
//  X.assign1(value)         O[1]                   No-Throw
//  X.assign0(value)         O[1]                   No-Throw
//
//  X.remove(index)          O[N]                   No-Throw
//  X.assignAll0()           O[N]                   No-Throw
//  X.assignAll1()           O[N]                   No-Throw
//
//  X.length()               O[1]                   No-Throw
//  X.OP[](index)            O[1]                   No-Throw
//
//  X.isAny1                 O[N]                   No-Throw
//  X.isAny0                 O[N]                   No-Throw
//
//  other 'const' methods    O[1] .. O[N]           No-Throw
//
//  OP==(X, Y)               O[min(N, M)]           No-Throw
//  OP!=(X, Y)               O[min(N, M)]           No-Throw
//
//                <*> No-Throw guarantee when capacity is sufficient.
//..
// Note that *all* of the non-creator methods of 'BitArray' provide the
// *No-Throw* guarantee whenever sufficient capacity is already available.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Creating a 'NullableVector' Class
/// - - - - - - - - - - - - - - - - - - - - - -
// An efficient implementation of an arbitrary precision bit sequence container
// has myriad applications.  For example, a 'bdlc::BitArray' can be used
// effectively as a parallel array of flags indicating some special property,
// such as 'isNull', 'isBusinessDay', etc.; its use is especially indicated
// when (1) the number of elements of the primary array can grow large, and (2)
// the individual elements do not have the capacity or capability to store the
// information directly.
//
// As a simple example, we'll implement a (heavily elided) value-semantic
// template class, 'NullableVector<TYPE>', that behaves like a
// 'bsl::vector<TYPE>' but additionally allows storing a nullness flag to
// signify that the corresponding element was not specified.  Elements added to
// a 'NullableVector' are null by default, although there are manipulator
// functions that allow appending a non-null element.  Each null element
// stores the default value for 'TYPE'.
//
// Note that this class has a minimal interface (suitable for illustration
// purpose only) that allows users to either append a (non-null) 'TYPE' value
// or a null value.  A real 'NullableVector' class would support a complete set
// of *value* *semantic* operations, including copy construction, assignment,
// equality comparison, 'ostream' printing, and BDEX serialization.  Also note
// that, for simplicity, exception-neutrality is ignored (some methods are
// clearly not exception-neutral).
//
// First, we define the interface of 'NullableVector':
//..
//  template <class TYPE>
//  class NullableVector {
//      // This class implements a sequential container of elements of the
//      // template parameter 'TYPE'.
//
//      // DATA
//      bsl::vector<TYPE>  d_values;       // data elements
//      bdlc::BitArray     d_nullFlags;    // 'true' indicates i'th element is
//                                         // null
//
//    private:
//      // NOT IMPLEMENTED
//      NullableVector(const NullableVector&);
//      NullableVector& operator=(const NullableVector&);
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(NullableVector,
//                                     bslma::UsesBslmaAllocator);
//
//    public:
//      // CREATORS
//      explicit
//      NullableVector(bsl::size_t       initialLength,
//                     bslma::Allocator *basicAllocator = 0);
//          // Construct a vector having the specified 'initialLength' null
//          // elements.  Optionally specify a 'basicAllocator' used to supply
//          // memory.  If 'basicAllocator' is 0, the currently supplied
//          // default allocator is used.
//
//      // ...
//
//      ~NullableVector();
//          // Destroy this vector.
//
//      // MANIPULATORS
//      void appendNullElement();
//          // Append a null element to this vector.  Note that the appended
//          // element will have the same value as a default constructed 'TYPE'
//          // object.
//
//      void appendElement(const TYPE& value);
//          // Append an element having the specified 'value' to the end of
//          // this vector.
//
//      void makeNonNull(bsl::size_t index);
//          // Make the element at the specified 'index' in this vector
//          // non-null.  The behavior is undefined unless 'index < length()'.
//
//      void makeNull(bsl::size_t index);
//          // Make the element at the specified 'index' in this vector null.
//          // The behavior is undefined unless 'index < length()'.  Note that
//          // the new value of the element will be the default constructed
//          // value for 'TYPE'.
//
//      TYPE& modifiableElement(bsl::size_t index);
//          // Return a reference providing modifiable access to the (valid)
//          // element at the specified 'index' in this vector.  The behavior
//          // is undefined unless 'index < length()'.  Note that if the
//          // element at 'index' is null then the nullness flag is reset and
//          // the returned value is the default constructed value for 'TYPE'.
//
//      void removeElement(bsl::size_t index);
//          // Remove the element at the specified 'index' in this vector.  The
//          // behavior is undefined unless 'index < length()'.
//
//      // ACCESSORS
//      const TYPE& constElement(bsl::size_t index) const;
//          // Return a reference providing non-modifiable access to the
//          // element at the specified 'index' in this vector.  The behavior
//          // is undefined unless 'index < length()'.  Note that if the
//          // element at 'index' is null then the nullness flag is not reset
//          // and the returned value is the default constructed value for
//          // 'TYPE'.
//
//      bool isAnyElementNonNull() const;
//          // Return 'true' if any element in this vector is non-null, and
//          // 'false' otherwise.
//
//      bool isAnyElementNull() const;
//          // Return 'true' if any element in this vector is null, and 'false'
//          // otherwise.
//
//      bool isElementNull(bsl::size_t index) const;
//          // Return 'true' if the element at the specified 'index' in this
//          // vector is null, and 'false' otherwise.  The behavior is
//          // undefined unless 'index < length()'.
//
//      bsl::size_t length() const;
//          // Return the number of elements in this vector.
//
//      bsl::size_t numNullElements() const;
//          // Return the number of null elements in this vector.
//  };
//..
// Then, we implement, in turn, each of the methods declared above:
//..
//                   // --------------------
//                   // class NullableVector
//                   // --------------------
//
//  // CREATORS
//  template <class TYPE>
//  NullableVector<TYPE>::NullableVector(bsl::size_t       initialLength,
//                                       bslma::Allocator *basicAllocator)
//  : d_values(initialLength, TYPE(), basicAllocator)
//  , d_nullFlags(initialLength, true, basicAllocator)
//  {
//  }
//
//  template <class TYPE>
//  NullableVector<TYPE>::~NullableVector()
//  {
//      BSLS_ASSERT(d_values.size() == d_nullFlags.length());
//  }
//
//  // MANIPULATORS
//  template <class TYPE>
//  inline
//  void NullableVector<TYPE>::appendElement(const TYPE& value)
//  {
//      d_values.push_back(value);
//      d_nullFlags.append(false);
//  }
//
//  template <class TYPE>
//  inline
//  void NullableVector<TYPE>::appendNullElement()
//  {
//      d_values.push_back(TYPE());
//      d_nullFlags.append(true);
//  }
//
//  template <class TYPE>
//  inline
//  void NullableVector<TYPE>::makeNonNull(bsl::size_t index)
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      d_nullFlags.assign(index, false);
//  }
//
//  template <class TYPE>
//  inline
//  void NullableVector<TYPE>::makeNull(bsl::size_t index)
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      d_values[index] = TYPE();
//      d_nullFlags.assign(index, true);
//  }
//
//  template <class TYPE>
//  inline
//  TYPE& NullableVector<TYPE>::modifiableElement(bsl::size_t index)
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      d_nullFlags.assign(index, false);
//      return d_values[index];
//  }
//
//  template <class TYPE>
//  inline
//  void NullableVector<TYPE>::removeElement(bsl::size_t index)
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      d_values.erase(d_values.begin() + index);
//      d_nullFlags.remove(index);
//  }
//
//  // ACCESSORS
//  template <class TYPE>
//  inline
//  const TYPE& NullableVector<TYPE>::constElement(bsl::size_t index) const
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      return d_values[index];
//  }
//
//  template <class TYPE>
//  inline
//  bool NullableVector<TYPE>::isAnyElementNonNull() const
//  {
//      return d_nullFlags.isAny0();
//  }
//
//  template <class TYPE>
//  inline
//  bool NullableVector<TYPE>::isAnyElementNull() const
//  {
//      return d_nullFlags.isAny1();
//  }
//
//  template <class TYPE>
//  inline
//  bool NullableVector<TYPE>::isElementNull(bsl::size_t index) const
//  {
//      BSLS_ASSERT_SAFE(index < length());
//
//      return d_nullFlags[index];
//  }
//
//  template <class TYPE>
//  inline
//  bsl::size_t NullableVector<TYPE>::length() const
//  {
//      return d_values.size();
//  }
//
//  template <class TYPE>
//  inline
//  bsl::size_t NullableVector<TYPE>::numNullElements() const
//  {
//      return d_nullFlags.num1();
//  }
//..
// Next, we create an empty 'NullableVector':
//..
//  NullableVector<int>        array(0);
//  const NullableVector<int>& ARRAY       = array;
//  const int                  DEFAULT_INT = 0;
//
//  assert(0       == ARRAY.length());
//  assert(0       == ARRAY.numNullElements());
//  assert(false   == ARRAY.isAnyElementNonNull());
//  assert(false   == ARRAY.isAnyElementNull());
//..
// Then, we append a non-null element to it:
//..
//  array.appendElement(5);
//  assert(1       == ARRAY.length());
//  assert(5       == ARRAY.constElement(0));
//  assert(false   == ARRAY.isElementNull(0));
//  assert(0       == ARRAY.numNullElements());
//  assert(true    == ARRAY.isAnyElementNonNull());
//  assert(false   == ARRAY.isAnyElementNull());
//..
// Next, we append a null element:
//..
//  array.appendNullElement();
//  assert(2           == ARRAY.length());
//  assert(5           == ARRAY.constElement(0));
//  assert(DEFAULT_INT == ARRAY.constElement(1));
//  assert(false       == ARRAY.isElementNull(0));
//  assert(true        == ARRAY.isElementNull(1));
//  assert(1           == ARRAY.numNullElements());
//  assert(true        == ARRAY.isAnyElementNonNull());
//  assert(true        == ARRAY.isAnyElementNull());
//..
// Then, we make the null element non-null:
//..
//  array.makeNonNull(1);
//  assert(2           == ARRAY.length());
//  assert(5           == ARRAY.constElement(0));
//  assert(DEFAULT_INT == ARRAY.constElement(1));
//  assert(false       == ARRAY.isElementNull(0));
//  assert(false       == ARRAY.isElementNull(1));
//  assert(0           == ARRAY.numNullElements());
//  assert(true        == ARRAY.isAnyElementNonNull());
//  assert(false       == ARRAY.isAnyElementNull());
//..
// Next, we make the first element null:
//..
//  array.makeNull(0);
//  assert(2           == ARRAY.length());
//  assert(DEFAULT_INT == ARRAY.constElement(0));
//  assert(DEFAULT_INT == ARRAY.constElement(1));
//  assert(true        == ARRAY.isElementNull(0));
//  assert(false       == ARRAY.isElementNull(1));
//  assert(1           == ARRAY.numNullElements());
//  assert(true        == ARRAY.isAnyElementNonNull());
//  assert(true        == ARRAY.isAnyElementNull());
//..
// Now, we remove the front element:
//..
//  array.removeElement(0);
//  assert(1           == ARRAY.length());
//  assert(DEFAULT_INT == ARRAY.constElement(0));
//  assert(false       == ARRAY.isElementNull(0));
//  assert(0           == ARRAY.numNullElements());
//  assert(true        == ARRAY.isAnyElementNonNull());
//  assert(false       == ARRAY.isAnyElementNull());
//..
// Finally, we remove the last remaining element and observe that the object is
// empty again:
//..
//  array.removeElement(0);
//  assert(0       == ARRAY.length());
//  assert(0       == ARRAY.numNullElements());
//  assert(false   == ARRAY.isAnyElementNonNull());
//  assert(false   == ARRAY.isAnyElementNull());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_BITMASKUTIL
#include <bdlb_bitmaskutil.h>
#endif

#ifndef INCLUDED_BDLB_BITSTRINGUTIL
#include <bdlb_bitstringutil.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
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

namespace BloombergLP {
namespace bdlc {

                                // ==============
                                // class BitArray
                                // ==============

class BitArray {
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

  public:
    // PUBLIC TYPES
    enum { k_BITS_PER_UINT64 = 64 };  // bits used to represent a 'uint64_t'

    // PUBLIC CLASS CONSTANTS
    static const bsl::size_t k_INVALID_INDEX =
                                          bdlb::BitStringUtil::k_INVALID_INDEX;

  private:
    // DATA
    bsl::vector<bsl::uint64_t> d_array;   // array of 64-bit words
    bsl::size_t                d_length;  // number of significant bits in this
                                          // array

    // CLASS DATA
    static const bsl::uint64_t s_one      =                              1;
    static const bsl::uint64_t s_minusOne = ~static_cast<bsl::uint64_t>(0);

    // FRIENDS
    friend bool operator==(const BitArray&, const BitArray&);

  private:
    // PRIVATE CLASS METHODS
    static bsl::size_t arraySize(bsl::size_t numBits);
        // Return the size, in 64-bit words, of the integer array required to
        // store the specified 'numBits'.

    // PRIVATE MANIPULATORS
    bsl::uint64_t *data();
        // Return an address providing modifiable access to the array of
        // 'uint64_t' values managed by this array.

    // PRIVATE ACCESSORS
    const bsl::uint64_t *data() const;
        // Return an address providing non-modifiable access to the array of
        // 'uint64_t' values managed by this array.

  public:
    // CLASS METHODS

                                // Aspects

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit
    BitArray(bslma::Allocator *basicAllocator = 0);
    explicit
    BitArray(bsl::size_t       initialLength,
             bslma::Allocator *basicAllocator = 0);
    BitArray(bsl::size_t       initialLength,
             bool              value,
             bslma::Allocator *basicAllocator = 0);
        // Create an array of binary digits (bits).  By default, the array is
        // empty and has a capacity of 0 bits.  Optionally specify the
        // 'initialLength' (in bits) of the array.  If 'initialLength' is not
        // specified, the default length is 0.  If 'initialLength' is
        // specified, optionally specify the 'value' for each bit in the
        // 'initialLength'.  If 'value' is not specified, the default value for
        // each bit is 'false' (0).  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    BitArray(const BitArray&   original,
             bslma::Allocator *basicAllocator = 0);
        // Create an array of binary digits (bits) having the same value as the
        // specified 'original' array.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~BitArray();
        // Destroy this object.

    // MANIPULATORS
    BitArray& operator=(const BitArray& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference providing modifiable access to this array.

    BitArray& operator&=(const BitArray& rhs);
        // Bitwise AND the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference providing
        // modifiable access to this object.  If 'length() > rhs.length()', the
        // unmatched most-significant bits in this array are set to 0;
        // otherwise, any unmatched most-significant bits in 'rhs' are ignored.
        // Note that this behavior is consistent with zero-extending 'rhs' if
        // needed, but leaving the length of this array unchanged.

    BitArray& operator-=(const BitArray& rhs);
        // Bitwise MINUS the value of the specified 'rhs' array from the value
        // of this array (retaining the results), and return a reference
        // providing modifiable access to this object.  If
        // 'length() > rhs.length()', the unmatched most-significant bits in
        // this array are left unchanged; otherwise, any unmatched
        // most-significant bits in 'rhs' are ignored.  Note that this behavior
        // is consistent with zero-extending 'rhs' if needed, but leaving the
        // length of this array unchanged.  Also note that the logical
        // difference 'A - B' is defined to be 'A & !B'.

    BitArray& operator|=(const BitArray& rhs);
        // Bitwise OR the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference providing
        // modifiable access to this object.  If 'length() > rhs.length()', the
        // unmatched most-significant bits in this array are left unchanged;
        // otherwise, any unmatched most-significant bits in 'rhs' are ignored.
        // Note that this behavior is consistent with zero-extending 'rhs' if
        // needed, but leaving the length of this array unchanged.

    BitArray& operator^=(const BitArray& rhs);
        // Bitwise XOR the value of the specified 'rhs' array with the value of
        // this array (retaining the results), and return a reference providing
        // modifiable access to this object.  If 'length() > rhs.length()', the
        // unmatched most-significant bits in this array are left unchanged;
        // otherwise, any unmatched most-significant bits in 'rhs' are ignored.
        // Note that this behavior is consistent with zero-extending 'rhs' if
        // needed, but leaving the length of this array unchanged.

    BitArray& operator<<=(bsl::size_t numBits);
        // Shift the bits in this array LEFT by the specified 'numBits',
        // filling lower-order bits with zeros (retaining the results), and
        // return a reference providing modifiable access to this object.  The
        // behavior is undefined unless 'numBits <= length()'.  Note that the
        // length of this array is unchanged and the highest-order 'numBits'
        // are discarded.

    BitArray& operator>>=(bsl::size_t numBits);
        // Shift the bits in this array RIGHT by the specified 'numBits',
        // filling higher-order bits with zeros and discarding low-order bits,
        // and return a reference providing modifiable access to this object.
        // The behavior is undefined unless 'numBits <= length()'.  Note that
        // the length of this array is unchanged.

    void andEqual(bsl::size_t index, bool value);
        // AND the bit at the specified 'index' in this array with the
        // specified 'value' (retaining the result).  The behavior is undefined
        // unless 'index < length()'.

    void andEqual(bsl::size_t     dstIndex,
                  const BitArray& srcArray,
                  bsl::size_t     srcIndex,
                  bsl::size_t     numBits);
        // Bitwise AND the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray',
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless 'dstIndex + numBits <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.

    void append(bool value);
        // Append to this array the specified 'value'.  Note that this method
        // has the same behavior as:
        //..
        //  insert(length(), value);
        //..

    void append(bool value, bsl::size_t numBits);
        // Append to this array the specified 'numBits' having the specified
        // 'value'.  Note that this method has the same behavior as:
        //..
        //  insert(length(), value, numBits);
        //..

    void append(const BitArray& srcArray);
        // Append to this array the values from the specified 'srcArray'.  Note
        // that this method has the same behavior as:
        //..
        //  insert(length(), srcArray);
        //..

    void append(const BitArray& srcArray,
                bsl::size_t     srcIndex,
                bsl::size_t     numBits);
        // Append to this array the specified 'numBits' from the specified
        // 'srcArray', beginning at the specified 'srcIndex'.  The behavior is
        // undefined unless 'srcIndex + numBits <= srcArray.length()'.  Note
        // that this method has the same behavior as:
        //..
        //  insert(length(), srcArray, srcIndex, numBits);
        //..

    void assign(bsl::size_t index, bool value);
        // Set the value at the specified 'index' in this array to the
        // specified 'value'.  The behavior is undefined unless
        // 'index < length()'.

    void assign(bsl::size_t index, bool value, bsl::size_t numBits);
        // Set the value of the specified 'numBits' bits starting at the
        // specified 'index' in this array to the specified 'value'.  The
        // behavior is undefined unless 'index + numBits < length()'.

    void assign(bsl::size_t     dstIndex,
                const BitArray& srcArray,
                bsl::size_t     srcIndex,
                bsl::size_t     numBits);
        // Replace the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray'
        // beginning at the specified 'srcIndex'.  The behavior is undefined
        // unless 'dstIndex + numBits <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.  Note that, absent
        // aliasing, this method has the same behavior as, but is more
        // efficient than:
        //..
        //  remove(dstIndex, numBits);
        //  insert(dstIndex, srcArray, srcIndex, numBits);
        //..

    void assign0(bsl::size_t index);
        // Set to 0 the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless 'index < length()'.

    void assign0(bsl::size_t index, bsl::size_t numBits);
        // Set to 0 the specified 'numBits' values in this array, beginning at
        // the specified 'index'.  The behavior is undefined unless
        // 'index + numBits <= length()'.

    void assign1(bsl::size_t index);
        // Set to 1 the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless 'index < length()'.

    void assign1(bsl::size_t index, bsl::size_t numBits);
        // Set to 1 the specified 'numBits' values in this array, beginning at
        // the specified 'index'.  The behavior is undefined unless
        // 'index + numBits <= length()'.

    void assignAll(bool value);
        // Set all bits in this array to the specified 'value'.

    void assignAll0();
        // Set to 0 the value of every bit in this array.

    void assignAll1();
        // Set to 1 the value of every bit in this array.

    void assignBits(bsl::size_t   index,
                    bsl::uint64_t srcBits,
                    bsl::size_t   numBits);
        // Assign the low-order specified 'numBits' from the specified
        // 'srcBits' to this object, starting at the specified 'index'.  The
        // behavior is undefined unless 'numBits <= k_BITS_PER_UINT64' and
        // 'index + numBits <= length()'.

    void insert(bsl::size_t dstIndex, bool value);
        // Insert into this array at the specified 'dstIndex' the specified
        // 'value'.  All values with indices at or above 'dstIndex' in this
        // array are shifted up by one bit position.  The behavior is undefined
        // unless 'dstIndex <= length()'.

    void insert(bsl::size_t dstIndex, bool value, bsl::size_t numBits);
        // Insert into this array at the specified 'dstIndex' the specified
        // 'numBits' having the specified 'value'.  All values with indices at
        // or above 'dstIndex' in this array are shifted up by 'numBits' bit
        // positions.  The behavior is undefined unless 'dstIndex <= length()'.

    void insert(bsl::size_t dstIndex, const BitArray& srcArray);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // values from the specified 'srcArray'.  All values with indices at or
        // above 'dstIndex' in this array are shifted up by 'srcArray.length()'
        // bit positions.  The behavior is undefined unless
        // 'dstIndex <= length()'.

    void insert(bsl::size_t     dstIndex,
                const BitArray& srcArray,
                bsl::size_t     srcIndex,
                bsl::size_t     numBits);
        // Insert into this array, beginning at the specified 'dstIndex', the
        // specified 'numBits' from the specified 'srcArray' beginning at the
        // specified 'srcIndex'.  All values with initial indices at or above
        // 'dstIndex' are shifted up by 'numBits' positions.  The behavior is
        // undefined unless 'dstIndex <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.

    void minusEqual(bsl::size_t index, bool value);
        // MINUS (subtract) from the bit at the specified 'index' in this array
        // the specified 'value' (retaining the result).  The behavior is
        // undefined unless 'index < length()'.  Note that the logical
        // difference 'A - B' is defined to be 'A & !B'.

    void minusEqual(bsl::size_t     dstIndex,
                    const BitArray& srcArray,
                    bsl::size_t     srcIndex,
                    bsl::size_t     numBits);
        // Bitwise MINUS (subtract) from the specified 'numBits' in this array,
        // beginning at the specified 'dstIndex', values from the specified
        // 'srcArray' beginning at the specified 'srcIndex' (retaining the
        // results).  The behavior is undefined unless
        // 'dstIndex + numBits <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.  Note that the logical
        // difference 'A - B' is defined to be 'A & !B'.

    void orEqual(bsl::size_t index, bool value);
        // OR the bit at the specified 'index' in this array with the specified
        // 'value' (retaining the result).  The behavior is undefined unless
        // 'index < length()'.

    void orEqual(bsl::size_t     dstIndex,
                 const BitArray& srcArray,
                 bsl::size_t     srcIndex,
                 bsl::size_t     numBits);
        // Bitwise OR the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray'
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless 'dstIndex + numBits <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.

    void remove(bsl::size_t index);
        // Remove from this array the bit at the specified 'index'.  All values
        // at indices above 'index' in this array are shifted down by one bit
        // position.  The length of this array is reduced by 1.  The behavior
        // is undefined unless 'index < length()'.

    void remove(bsl::size_t index, bsl::size_t numBits);
        // Remove from this array the specified 'numBits', beginning at the
        // specified 'index'.  All values at indices above 'index' in this
        // array are shifted down by 'numBits' positions.  The length of this
        // array is reduced by 'numBits'.  The behavior is undefined unless
        // 'index + numBits <= length()'.

    void removeAll();
        // Remove all of the bits in this array, leaving the length 0, but
        // having no effect on capacity.

    void reserveCapacity(bsl::size_t numBits);
        // Reserve sufficient internal capacity to accommodate a length of at
        // least the specified 'numBits' without reallocation.  If an exception
        // is thrown during this reallocation attempt (i.e., by the memory
        // allocator indicated at construction) the value of this array is
        // guaranteed to be unchanged.

    void rotateLeft(bsl::size_t numBits);
        // Shift the values in this array to the left by the specified
        // 'numBits' positions, with the high-order values "rotating" into the
        // low-order bits.  The behavior is undefined unless
        // 'numBits <= length()'.  Note that the length of this array remains
        // unchanged.

    void rotateRight(bsl::size_t numBits);
        // Shift the values in this array to the right by the specified
        // 'numBits' positions, with the low-order values "rotating" into the
        // high-order bits.  The behavior is undefined unless
        // 'numBits <= length()'.  Note that the length of this array remains
        // unchanged.

    void setLength(bsl::size_t newLength, bool value = false);
        // Set the number of bits in this array to the specified 'newLength'.
        // If 'newLength < length()', bits at index positions at or above
        // 'newLength' are removed; otherwise, any new bits (at or above the
        // current length) are initialized to the optionally specified 'value',
        // or to 0 if 'value' is not specified.

    void swapBits(bsl::size_t index1, bsl::size_t index2);
        // Efficiently exchange the values of the bits at the specified
        // 'index1' and 'index2' indices.  The behavior is undefined unless
        // 'index1 < length()' and 'index2 < length()'.

    void toggle(bsl::size_t index);
        // Complement the value of the bit at the specified 'index' in this
        // array.  The behavior is undefined unless 'index < length()'.

    void toggle(bsl::size_t index, bsl::size_t numBits);
        // Complement the values of each of the specified 'numBits' in this
        // array, beginning at the specified 'index'.  The behavior is
        // undefined unless 'index + numBits <= length()'.

    void toggleAll();
        // Complement the value of every bit in this array.  Note that the
        // behavior is analogous to applying the '~' operator to an object of
        // fundamental type 'unsigned int'.

    void xorEqual(bsl::size_t index, bool value);
        // XOR the bit at the specified 'index' in this array with the
        // specified 'value' (retaining the result).  The behavior is undefined
        // unless 'index < length()'.

    void xorEqual(bsl::size_t     dstIndex,
                  const BitArray& srcArray,
                  bsl::size_t     srcIndex,
                  bsl::size_t     numBits);
        // Bitwise XOR the specified 'numBits' in this array, beginning at the
        // specified 'dstIndex', with values from the specified 'srcArray'
        // beginning at the specified 'srcIndex' (retaining the results).  The
        // behavior is undefined unless 'dstIndex + numBits <= length()' and
        // 'srcIndex + numBits <= srcArray.length()'.

                                // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void swap(BitArray& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bool operator[](bsl::size_t index) const;
        // Return the value of the bit at the specified 'index' in this array.
        // The behavior is undefined unless 'index < length()'.

    bsl::uint64_t bits(bsl::size_t index, bsl::size_t numBits) const;
        // Return the specified 'numBits' beginning at the specified 'index' in
        // this array as the low-order bits of the returned value.  The
        // behavior is undefined unless
        // 'numBits <= sizeof(uint64_t) * CHAR_BIT' and
        // 'index + numBits <= length()'.

    bsl::size_t find0AtMaxIndex(bsl::size_t begin = 0,
                                bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the index of the most-significant 0 bit in this array in the
        // range optionally specified by 'begin' and 'end', and
        // 'k_INVALID_INDEX' otherwise.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

    bsl::size_t find0AtMinIndex(bsl::size_t begin = 0,
                                bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the index of the least-significant 0 bit in this array in the
        // range optionally specified by 'begin' and 'end', and
        // 'k_INVALID_INDEX' otherwise.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

    bsl::size_t find1AtMaxIndex(bsl::size_t begin = 0,
                                bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the index of the most-significant 1 bit in this array in the
        // range optionally specified by 'begin' and 'end', and
        // 'k_INVALID_INDEX' otherwise.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

    bsl::size_t find1AtMinIndex(bsl::size_t begin = 0,
                                bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the index of the least-significant 1 bit in this array in the
        // range optionally specified by 'begin' and 'end', and
        // 'k_INVALID_INDEX' otherwise.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

    bool isAny0() const;
        // Return 'true' if the value of any bit in this array is 0, and
        // 'false' otherwise.

    bool isAny1() const;
        // Return 'true' if the value of any bit in this array is 1, and
        // 'false' otherwise.

    bool isEmpty() const;
        // Return 'true' if the length of this bit array is 0, and 'false'
        // otherwise.

    bsl::size_t length() const;
        // Return the number of bits in this array.

    bsl::size_t num0(bsl::size_t begin = 0,
                     bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the number of bits in the range optionally specified by
        // 'begin' and 'end' having a value of 0.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

    bsl::size_t num1(bsl::size_t begin = 0,
                     bsl::size_t end   = k_INVALID_INDEX) const;
        // Return the number of bits in the range optionally specified by
        // 'begin' and 'end' having a value of 1.  The range is
        // '[begin .. effectiveEnd)', where 'effectiveEnd == length()' if 'end'
        // is not specified and 'effectiveEnd == end' otherwise.  The behavior
        // is undefined unless 'begin <= effectiveEnd <= length()'.

                                // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

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

};

// FREE OPERATORS
bool operator==(const BitArray& lhs, const BitArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays have the same
    // value, and 'false' otherwise.  Two arrays have the same value if they
    // have the same length, and corresponding bits at each bit position have
    // the same value.

bool operator!=(const BitArray& lhs, const BitArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays do not have the
    // same value, and 'false' otherwise.  Two arrays do not have the same
    // value if they do not have the same length, or there is at least one
    // valid index position at which corresponding bits do not have the same
    // value.

BitArray operator~(const BitArray& array);
    // Return the bitwise complement ("toggle") of the specified 'array'.

BitArray operator&(const BitArray& lhs, const BitArray& rhs);
    // Return the value that is the bitwise AND of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order bits set to
    // 0.  Note that this behavior is consistent with zero-extending the
    // shorter array.

BitArray operator-(const BitArray& lhs, const BitArray& rhs);
    // Return the value that is the bitwise MINUS of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order 'lhs' bits
    // copied unchanged, and any unmatched high-order 'rhs' bits set to 0.
    // Note that this behavior is consistent with zero-extending the shorter
    // array.

BitArray operator|(const BitArray& lhs, const BitArray& rhs);
    // Return the value that is the bitwise OR of the specified 'lhs' and 'rhs'
    // arrays.  The length of the resulting bit array will be the maximum of
    // that of 'lhs' and 'rhs', with any unmatched high-order bits copied
    // unchanged.  Note that this behavior is consistent with zero-extending
    // the shorter array.

BitArray operator^(const BitArray& lhs, const BitArray& rhs);
    // Return the value that is the bitwise XOR of the specified 'lhs' and
    // 'rhs' arrays.  The length of the resulting bit array will be the maximum
    // of that of 'lhs' and 'rhs', with any unmatched high-order bits copied
    // unchanged.  Note that this behavior is consistent with zero-extending
    // the shorter array.

BitArray operator<<(const BitArray& array, bsl::size_t numBits);
    // Return the value of the specified 'array' left-shifted by the specified
    // 'numBits' positions, having filled the lower-index positions with zeros.
    // The behavior is undefined unless 'numBits <= array.length()'.  Note that
    // the length of the result equals the length of the original array, and
    // that the highest-order 'numBits' are discarded in the result.

BitArray operator>>(const BitArray& array, bsl::size_t numBits);
    // Return the value of the specified 'array' right-shifted by the specified
    // 'numBits' positions, having filled the higher-index positions with
    // zeros.  The behavior is undefined unless
    // 'numBits <= array.length()'.  Note that the length of the result equals
    // the length of the original array, and that the lowest-order 'numBits'
    // are discarded in the result.

bsl::ostream& operator<<(bsl::ostream& stream, const BitArray& rhs);
    // Format the bits in the specified 'rhs' bit array to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.

// FREE FUNCTIONS
void swap(BitArray& a, BitArray& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                                // --------------
                                // class BitArray
                                // --------------

// PRIVATE CLASS METHODS
inline
bsl::size_t BitArray::arraySize(bsl::size_t numBits)
{
    // Note that we ensure that the capacity of 'd_array' is at least 1 at all
    // times.  This way we know that 'd_array.front()' is valid.

    const bsl::size_t ret = (numBits + k_BITS_PER_UINT64 - 1) /
                                                             k_BITS_PER_UINT64;
    return ret ? ret : 1;
}

// PRIVATE MANIPULATORS
inline
bsl::uint64_t *BitArray::data()
{
    BSLS_ASSERT_SAFE(!d_array.empty());

    return d_array.data();
}

// PRIVATE ACCESSORS
inline
const bsl::uint64_t *BitArray::data() const
{
    BSLS_ASSERT_SAFE(!d_array.empty());

    return d_array.data();
}

// CLASS METHODS

                                // Aspects

inline
int BitArray::maxSupportedBdexVersion(int)
{
    return 1;
}

// MANIPULATORS
inline
BitArray& BitArray::operator=(const BitArray& rhs)
{
    if (this != &rhs) {
        // The allocator used by the temporary copy must be the same as the
        // allocator of this object.

        BitArray(rhs, allocator()).swap(*this);
    }

    return *this;
}

inline
BitArray& BitArray::operator&=(const BitArray& rhs)
{
    if (this != &rhs) {
        bdlb::BitStringUtil::andEqual(data(),
                                      0,
                                      rhs.data(),
                                      0,
                                      bsl::min(d_length, rhs.d_length));
        if (d_length > rhs.d_length) {
            assign0(rhs.d_length, d_length - rhs.d_length);
        }
    }

    return *this;
}

inline
BitArray& BitArray::operator-=(const BitArray& rhs)
{
    if (this != &rhs) {
        bdlb::BitStringUtil::minusEqual(data(),
                                        0,
                                        rhs.data(),
                                        0,
                                        bsl::min(d_length, rhs.d_length));
    }
    else {
        assignAll0();
    }

    return *this;
}

inline
BitArray& BitArray::operator|=(const BitArray& rhs)
{
    if (this != &rhs) {
        bdlb::BitStringUtil::orEqual(data(),
                                     0,
                                     rhs.data(),
                                     0,
                                     bsl::min(d_length, rhs.d_length));
    }

    return *this;
}

inline
BitArray& BitArray::operator^=(const BitArray& rhs)
{
    if (this != &rhs) {
        bdlb::BitStringUtil::xorEqual(data(),
                                      0,
                                      rhs.data(),
                                      0,
                                      bsl::min(d_length, rhs.d_length));;
    }
    else {
        assignAll0();
    }

    return *this;
}

inline
BitArray& BitArray::operator>>=(bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const bsl::size_t remBits = d_length - numBits;

            bdlb::BitStringUtil::copyRaw(data(), 0, data(), numBits, remBits);
            assign0(remBits, numBits);
        }
        else {
            assignAll0();
        }
    }

    return *this;
}

inline
BitArray& BitArray::operator<<=(bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(numBits <= d_length);

    if (numBits) {
        if (d_length > numBits) {
            const bsl::size_t remBits = d_length - numBits;

            bdlb::BitStringUtil::copy(data(), numBits, data(), 0, remBits);
            assign0(0, numBits);
        }
        else {
            assignAll0();
        }
    }

    return *this;
}

inline
void BitArray::andEqual(bsl::size_t index, bool value)
{
    BSLS_ASSERT_SAFE(index < d_length);

    if (!value) {
        assign0(index);
    }
}

inline
void BitArray::andEqual(bsl::size_t     dstIndex,
                        const BitArray& srcArray,
                        bsl::size_t     srcIndex,
                        bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(dstIndex + numBits <= d_length);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitStringUtil::andEqual(data(),
                                  dstIndex,
                                  srcArray.data(),
                                  srcIndex,
                                  numBits);
}

inline
void BitArray::append(bool value)
{
    if (d_length && 0 == d_length % k_BITS_PER_UINT64) {
        d_array.push_back(value);
    }
    else if (value) {
        bdlb::BitStringUtil::assign1(data(), d_length);
    }
    ++d_length;
}

inline
void BitArray::append(bool value, bsl::size_t numBits)
{
    insert(d_length, value, numBits);
}

inline
void BitArray::append(const BitArray& srcArray)
{
    insert(d_length, srcArray, 0, srcArray.d_length);
}

inline
void BitArray::append(const BitArray& srcArray,
                      bsl::size_t     srcIndex,
                      bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    insert(d_length, srcArray, srcIndex, numBits);
}

inline
void BitArray::assign(bsl::size_t index, bool value)
{
    BSLS_ASSERT_SAFE(index < d_length);

    bdlb::BitStringUtil::assign(data(), index, value);
}

inline
void BitArray::assign(bsl::size_t index, bool value, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    bdlb::BitStringUtil::assign(data(), index, value, numBits);
}

inline
void BitArray::assign(bsl::size_t     dstIndex,
                      const BitArray& srcArray,
                      bsl::size_t     srcIndex,
                      bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(dstIndex + numBits <= d_length);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    if (&srcArray == this) {
        // Might be overlapping copy.

        bdlb::BitStringUtil::copy(data(),
                                  dstIndex,
                                  srcArray.data(),
                                  srcIndex,
                                  numBits);
    }
    else {
        // Definitely not overlapping copy.

        bdlb::BitStringUtil::copyRaw(data(),
                                     dstIndex,
                                     srcArray.data(),
                                     srcIndex,
                                     numBits);
    }
}

inline
void BitArray::assign0(bsl::size_t index)
{
    BSLS_ASSERT_SAFE(index < d_length);

    bdlb::BitStringUtil::assign0(data(), index);
}

inline
void BitArray::assign0(bsl::size_t index, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    bdlb::BitStringUtil::assign0(data(), index, numBits);
}

inline
void BitArray::assign1(bsl::size_t index)
{
    BSLS_ASSERT_SAFE(index < d_length);

    bdlb::BitStringUtil::assign1(data(), index);
}

inline
void BitArray::assign1(bsl::size_t index, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    bdlb::BitStringUtil::assign1(data(), index, numBits);
}

inline
void BitArray::assignAll(bool value)
{
    if (value) {
        assignAll1();
    }
    else {
        assignAll0();
    }
}

inline
void BitArray::assignAll0()
{
    bdlb::BitStringUtil::assign0(data(), 0, d_length);
}

inline
void BitArray::assignAll1()
{
    bdlb::BitStringUtil::assign1(data(), 0, d_length);
}

inline
void BitArray::assignBits(bsl::size_t   index,
                          bsl::uint64_t srcBits,
                          bsl::size_t   numBits)
{
    BSLS_ASSERT_SAFE(        numBits <= k_BITS_PER_UINT64);
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    bdlb::BitStringUtil::assignBits(data(), index, srcBits, numBits);
}

inline
void BitArray::insert(bsl::size_t dstIndex, const BitArray& srcArray)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_length);

    insert(dstIndex, srcArray, 0, srcArray.d_length);
}

inline
void BitArray::insert(bsl::size_t dstIndex, bool value)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_length);

    setLength(d_length + 1);
    bdlb::BitStringUtil::insert(data(), d_length - 1, dstIndex, value, 1);
}

inline
void BitArray::insert(bsl::size_t dstIndex, bool value, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_length);

    setLength(d_length + numBits);
    bdlb::BitStringUtil::insert(data(),
                                d_length - numBits,
                                dstIndex,
                                value,
                                numBits);
}

inline
void BitArray::minusEqual(bsl::size_t index, bool value)
{
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        assign0(index);
    }
}

inline
void BitArray::minusEqual(bsl::size_t     dstIndex,
                          const BitArray& srcArray,
                          bsl::size_t     srcIndex,
                          bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(dstIndex + numBits <= d_length);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitStringUtil::minusEqual(data(),
                                    dstIndex,
                                    srcArray.data(),
                                    srcIndex,
                                    numBits);
}

inline
void BitArray::orEqual(bsl::size_t index, bool value)
{
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        assign1(index);
    }
}

inline
void BitArray::orEqual(bsl::size_t     dstIndex,
                       const BitArray& srcArray,
                       bsl::size_t     srcIndex,
                       bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(dstIndex + numBits <= d_length);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitStringUtil::orEqual(data(),
                                 dstIndex,
                                 srcArray.data(),
                                 srcIndex,
                                 numBits);
}

inline
void BitArray::remove(bsl::size_t index)
{
    BSLS_ASSERT_SAFE(index < d_length);

    remove(index, 1);
}

inline
void BitArray::remove(bsl::size_t index, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    bdlb::BitStringUtil::remove(data(), d_length, index, numBits);
    setLength(d_length - numBits);
}

inline
void BitArray::removeAll()
{
    d_array.clear();
    d_array.resize(1);
    d_length = 0;
}

inline
void BitArray::reserveCapacity(bsl::size_t numBits)
{
    d_array.reserve(arraySize(numBits));
}

inline
void BitArray::swapBits(bsl::size_t index1, bsl::size_t index2)
{
    BSLS_ASSERT_SAFE(index1 < d_length);
    BSLS_ASSERT_SAFE(index2 < d_length);

    if (index1 != index2) {
        const bool tmp = (*this)[index1];
        assign(index1,   (*this)[index2]);
        assign(index2, tmp);
    }
}

inline
void BitArray::toggle(bsl::size_t index)
{
    BSLS_ASSERT_SAFE(index < d_length);

    const bsl::size_t idx =                       index  / k_BITS_PER_UINT64;
    const int         pos = static_cast<unsigned>(index) % k_BITS_PER_UINT64;

    d_array[idx] ^= (s_one << pos);
}

inline
void BitArray::toggle(bsl::size_t index, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    // 'index' and 'numBits' non-negative checked by 'BitStringUtil'.

    bdlb::BitStringUtil::toggle(data(), index, numBits);
}

inline
void BitArray::toggleAll()
{
    toggle(0, d_length);
}

inline
void BitArray::xorEqual(bsl::size_t index, bool value)
{
    BSLS_ASSERT_SAFE(index < d_length);

    if (value) {
        toggle(index);
    }
}

inline
void BitArray::xorEqual(bsl::size_t     dstIndex,
                        const BitArray& srcArray,
                        bsl::size_t     srcIndex,
                        bsl::size_t     numBits)
{
    BSLS_ASSERT_SAFE(dstIndex + numBits <= d_length);
    BSLS_ASSERT_SAFE(srcIndex + numBits <= srcArray.d_length);

    bdlb::BitStringUtil::xorEqual(data(),
                                  dstIndex,
                                  srcArray.data(),
                                  srcIndex,
                                  numBits);
}

                                // Aspects

template <class STREAM>
STREAM& BitArray::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            int newLength;
            stream.getLength(newLength);
            if (!stream) {
                return stream;                                        // RETURN
            }

            if (0 == newLength) {
                removeAll();

                return stream;                                        // RETURN
            }

            const bsl::size_t len = arraySize(newLength);
            removeAll();
            d_array.resize(len);

            // 'getArrayUint64' will throw if there is bad input, so to prevent
            // invariants tests in the bit array destructor from failing, we
            // must make 'd_length' consistent with 'd_array.size()' before
            // that happens.

            d_length = newLength;

            stream.getArrayUint64(
                       reinterpret_cast<bsls::Types::Uint64 *>(d_array.data()),
                       static_cast<int>(len));
            if (!stream) {
                removeAll();
                return stream;                                        // RETURN
            }

            // Test for corrupted data.

            const int rem = static_cast<unsigned>(d_length) %
                                                             k_BITS_PER_UINT64;
            if (rem) {
                const bsl::uint64_t mask = (s_one << rem) - 1;
                if (d_array.back() & ~mask) {
                    // Correct invalid bit array and invalidate stream.  This
                    // is fastest way to valid, arbitrary state.

                    d_array.back() &= mask;
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
            }
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void BitArray::swap(BitArray& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_array,  &other.d_array);
    bslalg::SwapUtil::swap(&d_length, &other.d_length);
}

// ACCESSORS
inline
bool BitArray::operator[](bsl::size_t index) const
{
    BSLS_ASSERT_SAFE(index < d_length);

    return bdlb::BitStringUtil::bit(data(), index);
}

inline
bsl::uint64_t BitArray::bits(bsl::size_t index, bsl::size_t numBits) const
{
    BSLS_ASSERT_SAFE(index + numBits <= d_length);

    return bdlb::BitStringUtil::bits(data(), index, numBits);
}

inline
bsl::size_t BitArray::find0AtMaxIndex(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::find0AtMaxIndex(data(), begin, end);
}

inline
bsl::size_t BitArray::find0AtMinIndex(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::find0AtMinIndex(data(), begin, end);
}

inline
bsl::size_t BitArray::find1AtMaxIndex(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::find1AtMaxIndex(data(), begin, end);
}

inline
bsl::size_t BitArray::find1AtMinIndex(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::find1AtMinIndex(data(), begin, end);
}

inline
bool BitArray::isAny0() const
{
    return bdlb::BitStringUtil::isAny0(data(), 0, d_length);
}

inline
bool BitArray::isAny1() const
{
    return bdlb::BitStringUtil::isAny1(data(), 0, d_length);
}

inline
bool BitArray::isEmpty() const
{
    return 0 == d_length;
}

inline
bsl::size_t BitArray::length() const
{
    return d_length;
}

inline
bsl::size_t BitArray::num0(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::num0(data(), begin, end - begin);
}

inline
bsl::size_t BitArray::num1(bsl::size_t begin, bsl::size_t end) const
{
    if (k_INVALID_INDEX == end) {
        end = d_length;
    }
    BSLS_ASSERT_SAFE(begin <= end);
    BSLS_ASSERT_SAFE(         end <= d_length);

    return bdlb::BitStringUtil::num1(data(), begin, end - begin);
}

                                // Aspects

inline
bslma::Allocator *BitArray::allocator() const
{
    return d_array.get_allocator().mechanism();
}

template <class STREAM>
STREAM& BitArray::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        BSLS_ASSERT_SAFE(d_length <= INT_MAX);

        stream.putLength(static_cast<int>(d_length));
        if (0 != d_length) {
            stream.putArrayUint64(
                 reinterpret_cast<const bsls::Types::Uint64 *>(d_array.data()),
                 static_cast<int>(d_array.size()));
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}


// FREE OPERATORS
inline
bool operator==(const BitArray& lhs, const BitArray& rhs)
{
    if (lhs.d_length != rhs.d_length) {
        return false;                                                 // RETURN
    }

    return bdlb::BitStringUtil::areEqual(lhs.data(),
                                         rhs.data(),
                                         lhs.d_length);
}

inline
bool operator!=(const BitArray& lhs, const BitArray& rhs)
{
    return !(lhs == rhs);
}

inline
BitArray operator~(const BitArray& array)
{
    BitArray tmp(array);
    tmp.toggleAll();
    return tmp;
}

inline
BitArray operator&(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp &= rhs;
    return tmp;
}

inline
BitArray operator|(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp |= rhs;
    return tmp;
}

inline
BitArray operator^(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp ^= rhs;
    return tmp;
}

inline
BitArray operator-(const BitArray& lhs, const BitArray& rhs)
{
    BitArray tmp(lhs);
    tmp -= rhs;
    return tmp;
}

inline
BitArray operator<<(const BitArray& array, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(numBits <= array.length());

    BitArray tmp(array);
    tmp <<= numBits;
    return tmp;
}

inline
BitArray operator>>(const BitArray& array, bsl::size_t numBits)
{
    BSLS_ASSERT_SAFE(numBits <= array.length());

    BitArray tmp(array);
    tmp >>= numBits;
    return tmp;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const BitArray& rhs)
{
    return rhs.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(BitArray& a, BitArray& b)
{
    a.swap(b);
}

}  // close package namespace

namespace bslmf {

template <>
struct IsBitwiseMoveable<bdlc::BitArray> :
                        public IsBitwiseMoveable<bsl::vector<bsl::uint64_t> > {
    // This template specialization for 'IsBitwiseMoveable' indicates that
    // 'BitArray' is a bitwise moveable type if 'vector<uint64_t>' is a bitwise
    // moveable type.
};

}  // close namespace bslmf

namespace bslma {

template <>
struct UsesBslmaAllocator<bdlc::BitArray> : bsl::true_type {
    // This template specialization for 'UsesBslmaAllocator' indicates that
    // 'BitArray' uses 'bslma::Allocator'.
};

}  // close namespace bslma

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
