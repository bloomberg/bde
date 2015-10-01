// bdlc_packedintarray.h                                              -*-C++-*-
#ifndef INCLUDED_BDLC_PACKEDINTARRAY
#define INCLUDED_BDLC_PACKEDINTARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an extensible, packed array of integral values.
//
//@CLASSES:
//  bdlc::PackedIntArray: packed array of integral values
//  bdlc::PackedIntArrayConstIterator: bidirectional 'const_iterator'
//
//@DESCRIPTION: This component provides a space-efficient value-semantic array
// class template, 'bdlc::PackedIntArray', and an associated iterator,
// 'bdlc::PackedIntArrayConstIterator', that provides non-modifiable access to
// its elements.  The interface of this class provides the user with
// functionality similar to a 'bsl::vector<int>'.  The implementation is
// designed to reduce dynamic memory usage by storing its contents differently
// according to the magnitude of values placed within it.  The user need not be
// concerned with the internal representation of the data.  The array supports
// primitive operations (e.g., insertion, look-up, removal) as well as a
// complete set of value-semantic operations; however, direct reference to
// individual elements is not available.  Users can access the value of
// individual elements by calling the indexing operator or via iterators.  Note
// that iterators are *not* invalidated if an array object reallocates memory.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Temperature Map'
/// - - - - - - - - - - - - - -
// There exist many applications in which the range of 'int' data that a
// container will hold is not known at design time.  This means in order to
// build a robust component one must default to 'bsl::vector<int>', which for
// many applications is excessive in its usage of space.
//
// Suppose we are creating a map of temperatures for every city in the United
// States for every day.  This represents a large body of data, most of which
// is easily representable in a 'signed char', and in only rare situations is a
// 'short' required.
//
// To be able to represent all possible values for all areas and times,
// including extremes like Death Valley, a traditional implementation would
// require use of a 'vector<short>' for each day for each area.  This is
// excessive for all but the most extreme values, and therefore wasteful for
// this map as a whole.
//
// We can use 'bdlc::PackedIntArray' to efficiently store this data.
//
// First, we declare and define a 'my_Date' class.  This class is very similar
// to 'bdlt::Date', and therefore is elided for the sake of compactness.
//..
//                              // =======
//                              // my_Date
//                              // =======
//  class my_Date {
//      // A (value-semantic) attribute class that provides a very simple date.
//      signed char d_day;    // the day
//      signed char d_month;  // the month
//      int         d_year;   // the year
//
//      // FRIENDS
//      friend bool operator<(const my_Date&, const my_Date&);
//
//    public:
//      // CREATORS
//      explicit my_Date(int         year  = 1,
//                       signed char month = 1,
//                       signed char day   = 1);
//          // Create a 'my_Date' object having the optionally specified 'day',
//          // 'month', and 'year'. Each, if unspecified, will default to 1.
//  };
//
//  bool operator<(const my_Date& lhs, const my_Date& rhs);
//      // Return 'true' if the specified 'lhs' represents an earlier date than
//      // the specified 'rhs' object, and 'false' otherwise.
//
//                          // -------
//                          // my_Date
//                          // -------
//  // CREATORS
//  inline
//  my_Date::my_Date(int year, signed char month , signed char day)
//  : d_day(day)
//  , d_month(month)
//  , d_year(year)
//  {
//  }
//
//  bool operator<(const my_Date& lhs, const my_Date& rhs)
//  {
//      return 10000 * lhs.d_year + 100 * lhs.d_month + lhs.d_day <
//             10000 * rhs.d_year + 100 * rhs.d_month + rhs.d_day;
//  }
//..
// Then, we create our TemperatureMap, which is a map of dates to a map of zip
// codes to a 'PackedIntArray' of temperatures.  Each 'PackedIntArray' has
// entries for each temperature from 12 A.M, to 11 P.M for each city in each
// zip code.  Notice that we use a 'PackedIntArray' to hold the data
// compactly.
//..
//  bsl::map<my_Date, bsl::map<int, bdlc::PackedIntArray<int> > >
//                                                              temperatureMap;
//..
// Next, we add data to the map (provided by the National Weather Service) for
// a normal case, and the extreme.
//..
//  bdlc::PackedIntArray<int>& nyc
//                         = temperatureMap[my_Date(2013, 9, 06)][10023];
//  bdlc::PackedIntArray<int>& dValley
//                         = temperatureMap[my_Date(1913, 7, 10)][92328];
//
//  int nycTemperatures[24]  = {60,  58, 57,  56, 55,  54,  54,  55,
//                              56,  59, 61,  64, 66,  67,  69,  69,
//                              70,  70, 68,  67, 65,  63,  61,  60};
//
//  int deathValleyTemps[24] = {65,  55, 50, 47,  62,  75,  77,  89,
//                              91,  92, 95, 110, 113, 121, 134, 126,
//                              113, 99, 96, 84,  79,  81,  73,  69};
//..
// Then, since the size of the data set is known at design time, as well as
// extreme values for the areas, we can use the 'reserveCapacity()' method to
// give the container hints about the data to come.
//..
//  nyc.reserveCapacity(    24, 50, 108);
//  dValley.reserveCapacity(24, 45, 134);
//..
// Now, we add the data to the respective containers.
//..
//  for (bsl::size_t i= 0; i < 24; ++i) {
//      nyc.append(nycTemperatures[i]);
//      dValley.append(deathValleyTemps[i])
//  }
//..
// Finally, notice that in order to represent these values in a
// 'PackedIntArray', it required '24 * sizeof(signed char)' bytes (24 on most
// systems) of dynamic memory for 'nyc', which represents the normal case for
// this data.  A 'vector<short>' would require '24 * sizeof(short)' bytes (48
// on most systems) of dynamic memory to represent the same data.
//..
//  assert(static_cast<int>(sizeof(signed char)) == nyc.bytesPerElement());
//  assert(                                   24 == nyc.length());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
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

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_CSTDINT
#include <bsl_cstdint.h>
#endif


namespace BloombergLP {
namespace bdlc {

// FORWARD DECLARATIONS
template <class TYPE> class PackedIntArray;

template <class TYPE> class PackedIntArrayConstIterator;

template <class TYPE> PackedIntArrayConstIterator<TYPE>
                           operator++(PackedIntArrayConstIterator<TYPE>&, int);

template <class TYPE> PackedIntArrayConstIterator<TYPE>
                           operator--(PackedIntArrayConstIterator<TYPE>&, int);

template <class TYPE>
bool operator==(const PackedIntArrayConstIterator<TYPE>&,
                const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator!=(const PackedIntArrayConstIterator<TYPE>&,
                const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bsl::ptrdiff_t operator-(const PackedIntArrayConstIterator<TYPE>&,
                         const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator<(const PackedIntArrayConstIterator<TYPE>&,
               const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator<=(const PackedIntArrayConstIterator<TYPE>&,
                const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator>(const PackedIntArrayConstIterator<TYPE>&,
               const PackedIntArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator>=(const PackedIntArrayConstIterator<TYPE>&,
                const PackedIntArrayConstIterator<TYPE>&);

                      // ===============================
                      // struct PackedIntArrayImp_Signed
                      // ===============================

struct PackedIntArrayImp_Signed {
    // This 'struct' provides a namespace for types and methods used to
    // implement a space-efficient value-semantic array class representing a
    // sequence of 'TYPE' elements; 'TYPE' must be convertable to either a
    // 'bsl::int64_t'.  Specifically, it defines the types used to store the
    // array's data, methods needed to externalize and unexternalize the array,
    // and a method to determine the storage size to use for a given value.

    // PUBLIC TYPES
    typedef  bsl::int8_t OneByteStorageType;
    typedef bsl::int16_t TwoByteStorageType;
    typedef bsl::int32_t FourByteStorageType;
    typedef bsl::int64_t EightByteStorageType;

    // CLASS METHODS
    template <class STREAM>
    static void bdexGet8(STREAM& stream, bsl::int8_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet16(STREAM& stream, bsl::int16_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet32(STREAM& stream, bsl::int32_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet64(STREAM& stream, bsl::int64_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut8(STREAM& stream, bsl::int8_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut16(STREAM& stream, bsl::int16_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut32(STREAM& stream, bsl::int32_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut64(STREAM& stream, bsl::int64_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    static int requiredBytesPerElement(EightByteStorageType value);
        // Return the required number of bytes to store the specified 'value'.
};

                     // =================================
                     // struct PackedIntArrayImp_Unsigned
                     // =================================

struct PackedIntArrayImp_Unsigned {
    // This 'struct' provides a namespace for types and methods used to
    // implement a space-efficient value-semantic array class representing a
    // sequence of 'TYPE' elements; 'TYPE' must be convertable to either a
    // 'bsl::uint64_t'.  Specifically, it defines the types used to store the
    // array's data, methods needed to externalize and unexternalize the array,
    // and a method to determine the storage size to use for a given value.

    // PUBLIC TYPES
    typedef  bsl::uint8_t OneByteStorageType;
    typedef bsl::uint16_t TwoByteStorageType;
    typedef bsl::uint32_t FourByteStorageType;
    typedef bsl::uint64_t EightByteStorageType;

    // CLASS METHODS
    template <class STREAM>
    static void bdexGet8(STREAM& stream, bsl::uint8_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet16(STREAM& stream, bsl::uint16_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet32(STREAM& stream, bsl::uint32_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexGet64(STREAM& stream, bsl::uint64_t& variable);
        // Read from the specified 'stream' the specified 'variable' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut8(STREAM& stream, bsl::uint8_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut16(STREAM& stream, bsl::uint16_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut32(STREAM& stream, bsl::uint32_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    template <class STREAM>
    static void bdexPut64(STREAM& stream, bsl::uint64_t value);
        // Write to the specified 'stream' the specified 'value' as per the
        // requirements of the BDEX protocol.

    static int requiredBytesPerElement(EightByteStorageType value);
        // Return the required number of bytes to store the specified 'value'.
};

                          // =======================
                          // class PackedIntArrayImp
                          // =======================

template <class STORAGE>
class PackedIntArrayImp {
    // This space-efficient value-semantic array class represents a sequence of
    // 'STORAGE::EightByteStorageType' elements;
    // 'STORAGE::EightByteStorageType' must be convertable to either a signed
    // or unsigned 64-bit integer using 'static_cast'.  The interface provides
    // functionality similar to a 'vector<int>' however references to
    // individual elements are not provided.

  public:
    // PUBLIC TYPES
    typedef typename STORAGE::EightByteStorageType ElementType;

    // CLASS DATA
    static const bsl::size_t k_MAX_CAPACITY = 0x7fffffff;  // maximum capacity
                                                           // in bytes

  private:
    // DATA
    void             *d_storage_p;        // allocated memory

    bsl::size_t       d_length;           // length of the array

    int               d_bytesPerElement;  // number of bytes used to store each
                                          // element

    bsl::size_t       d_capacityInBytes;  // capacity of the array

    bslma::Allocator *d_allocator_p;      // allocator used for all memory

    // PRIVATE CLASS METHODS
    static bsl::size_t nextCapacityGE(bsl::size_t minValue, bsl::size_t value);
        // Return the next valid number of bytes of capacity that is at least
        // the specified 'minValue', starting from the specified 'value'.

    // PRIVATE MANIPULATORS
    void expandImp(int         requiredBytesPerElement,
                   bsl::size_t requiredCapacityInBytes);
        // Make the capacity of this array at least the specified
        // 'requiredCapacityInBytes' and increase the bytes used to store an
        // element to the specified 'requiredBytesPerElement'.  The behavior is
        // undefined unless 'requiredBytesPerElement > bytesPerElement()'.

    void replaceImp(bsl::size_t dstIndex, ElementType value);
        // Change the value of the element at the specified 'dstIndex' in this
        // array to the specified 'value'.  The behavior is undefined unless
        // 'dstIndex < length()' and the required bytes to store the 'value' is
        // less than or equal to 'bytesPerElement()'.

    void replaceImp(void        *dst,
                    bsl::size_t  dstIndex,
                    int          dstBytesPerElement,
                    void        *src,
                    bsl::size_t  srcIndex,
                    int          srcBytesPerElement,
                    bsl::size_t  numElements);
        // Change the values of the specified 'numElements' elements in the
        // specified 'dst' array beginning at the specified 'dstIndex' with the
        // specified 'dstBytesPerElement' to those of the 'numElements' values
        // in the specified 'src' array beginning at the specified 'srcIndex'
        // with the specified 'srcBytesPerElement'.  The behavior is undefined
        // unless the source array has sufficient values,
        // 'dstIndex + numElements <= length()',
        // 'srcBytesPerElement != dstBytesPerElement', and either the memory
        // ranges do not overlap or: 'dst == src' and 'dstIndex >= srcIndex'
        // and 'dstBytesPerElement > srcBytesPerElement'.

    // PRIVATE ACCESSORS
    char *address() const;
        // Return the address of the storage as a 'char *'.

    bool isEqualImp(const PackedIntArrayImp& other) const;
        // Return 'true' if this and the specified 'other' array have the same
        // value, and 'false' otherwise.  Two 'PackedIntArrayImp' arrays have
        // the same value if they have the same length, and all corresponding
        // elements (those at the same indices) have the same value.  The
        // behavior is undefined unless 'length() == other.length()' and
        // 'bytesPerElement() != other.bytesPerElement()'.

    int requiredBytesPerElement(bsl::size_t index,
                                bsl::size_t numElements) const;
        // Return the required number of bytes to store the specified
        // 'numValues' values of this array starting at the specified 'index'.
        // The behavior is undefined unless 'index + numElements <= length()'.

  public:
    // CLASS METHODS
    static int maxSupportedBdexVersion(int serializationVersion);
        // Return the 'version' to be used with the 'bdexStreamOut' method
        // corresponding to the specified 'serializationVersion'.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    // CREATORS
    explicit PackedIntArrayImp(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'PackedIntArrayImp'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit PackedIntArrayImp(bsl::size_t       numElements,
                               ElementType       value = 0,
                               bslma::Allocator *basicAllocator = 0);
        // Create a 'PackedIntArrayImp' having the specified 'numElements'.
        // Optionally specify a 'value' to which each element will be set.  If
        // value is not specified, 0 is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    PackedIntArrayImp(const PackedIntArrayImp&  original,
                      bslma::Allocator         *basicAllocator = 0);
        // Create a 'PackedIntArrayImp' having the same value as the specified
        // 'original' one.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~PackedIntArrayImp();
        // Destroy this object

    // MANIPULATORS
    PackedIntArrayImp& operator=(const PackedIntArrayImp& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference providing modifiable access to this array.

    void append(ElementType value);
        // Append an element having the specified 'value' to the end of this
        // array.

    void append(const PackedIntArrayImp& srcArray);
        // Append the sequence of values represented by the specified
        // 'srcArray' to the end of this array.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void append(const PackedIntArrayImp& srcArray,
                bsl::size_t              srcIndex,
                bsl::size_t              numElements);
        // Append the sequence of values of the specified 'numElements'
        // starting at the specified 'srcIndex' in the specified 'srcArray' to
        // the end of this array.  The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void insert(bsl::size_t dstIndex, ElementType value);
        // Insert into this array, at the specified 'dstIndex', an element of
        // specified 'value', shifting any elements originally at or above
        // 'dstIndex' up by one.  The behavior is undefined unless
        // 'dstIndex <= length()'.

    void insert(bsl::size_t dstIndex, const PackedIntArrayImp& srcArray);
        // Insert into this array, at the specified 'dstIndex', the sequence of
        // values represented by the specified 'srcArray', shifting any
        // elements originally at or above 'dstIndex' up by 'srcArray.length()'
        // indices higher.  The behavior is undefined unless
        // 'dstIndex <= length()'.  Note that if this array and 'srcArray' are
        // the same, the behavior is as if a copy of 'srcArray' were passed.

    void insert(bsl::size_t              dstIndex,
                const PackedIntArrayImp& srcArray,
                bsl::size_t              srcIndex,
                bsl::size_t              numElements);
        // Insert into this array, at the specified 'dstIndex', the specified
        // 'numElements' values in the specified 'srcArray' starting at the
        // specified 'srcIndex'.  Elements greater than or equal to 'dstIndex'
        // are shifted up 'numElements' positions.  The behavior is undefined
        // unless 'dstIndex <= length()' and
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    void pop_back();
        // Remove the last element from this array.  The behavior is undefined
        // unless '0 < length()' .

    void push_back(ElementType value);
        // Append an element having the specified 'value' to the end of this
        // array.

    void remove(bsl::size_t dstIndex);
        // Remove from this array the element at the specified 'dstIndex'.
        // Each element having an index greater than 'dstIndex' before the
        // removal is shifted down by one index position.  The behavior is
        // undefined unless 'dstIndex < length()' .

    void remove(bsl::size_t dstIndex, bsl::size_t numElements);
        // Remove from this array, starting at the specified 'dstIndex', the
        // specified 'numElements'.  Shift the elements of this array that are
        // at 'dstIndex + numElements' or above to 'numElements' indices lower.
        // The behavior is undefined unless
        // 'dstIndex + numElements <= length()'.

    void removeAll();
        // Remove all the elements from this array and set the storage required
        // per element to one byte.

    void replace(bsl::size_t dstIndex, ElementType value);
        // Change the value of the element at the specified 'dstIndex' in this
        // array to the specified 'value'.  The behavior is undefined unless
        // 'dstIndex < length()'.

    void replace(bsl::size_t              dstIndex,
                 const PackedIntArrayImp& srcArray,
                 bsl::size_t              srcIndex,
                 bsl::size_t              numElements);
        // Change the values of the specified 'numElements' elements in this
        // array beginning at the specified 'dstIndex' to those of the
        // 'numElements' values in the specified 'srcArray' beginning at the
        // specified 'srcIndex'.  The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()' and
        // 'dstIndex + numElements <= length()'.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void reserveCapacityImp(bsl::size_t requiredCapacityInBytes);
        // Make the capacity of this array at least the specified
        // 'requiredCapacityInBytes'.  This method has no effect if the
        // current capacity meets or exceeds the required capacity.

    void reserveCapacity(bsl::size_t numElements);
        // Make the capacity of this array at least the specified
        // 'numElements' assuming the current 'bytesPerElement()'.  This
        // method has no effect if the current capacity meets or exceeds the
        // required capacity.

    void reserveCapacity(bsl::size_t numElements, ElementType maxValue);
        // Make the capacity of this array at least the specified
        // 'numElements'.  The specified 'maxValue' denotes the maximum element
        // value that will be subsequently added to this array.  After this
        // call 'numElements' having values in the range '[0, maxValue]' are
        // guaranteed to not cause a reallocation.  This method has no effect
        // if the current capacity meets or exceeds the required capacity.
        // The behavior is undefined unless '0 <= maxValue'.

    void reserveCapacity(bsl::size_t numElements,
                         ElementType minValue,
                         ElementType maxValue);
        // Make the capacity of this array at least the specified
        // 'numElements'.  The specified 'minValue' and 'maxValue' denote,
        // respectively, the minimum and maximum elements values that will be
        // subsequently added to this array.  After this call 'numElements'
        // having values in the range '[minValue, maxValue]' are guaranteed to
        // not cause a reallocation.  This method has no effect if the current
        // capacity meets or exceeds the required capacity.  The behavior is
        // undefined unless 'minValue <= maxValue'.

    void resize(bsl::size_t numElements);
        // Set the length of this array to the specified 'numElements'.  If
        // 'numElements > length()', the added elements are initialized to 0.

    void swap(PackedIntArrayImp& other);
        // Efficiently exchange the value of this array with the value of the
        // specified 'other' array.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // array was created with the same allocator as 'other'.

    // ACCESSORS
    ElementType operator[](bsl::size_t index) const;
        // Return the value of the element at the specified 'index'.  The
        // behavior is undefined unless 'index < length()'.

    bslma::Allocator *allocator() const;
        // Return the allocator used by this array to supply memory.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    int bytesPerElement() const;
        // Return the number of bytes currently used to store each element in
        // this array.

    bsl::size_t capacity() const;
        // Return the number of elements this array can hold in terms of the
        // current data type used to store its elements.

    bool isEmpty() const;
        // Return 'true' if there are no elements in this array, and 'false'
        // otherwise.

    bool isEqual(const PackedIntArrayImp& other) const;
        // Return 'true' if this and the specified 'other' array have the same
        // value, and 'false' otherwise.  Two 'PackedIntArrayImp' arrays have
        // the same value if they have the same length, and all corresponding
        // elements (those at the same indices) have the same value.

    bsl::size_t length() const;
        // Return number of elements in this array.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this array to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested arrays.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested arrays.  If 'level' is negative, format the entire
        // output on one line, suppressing all but the initial indentation (as
        // governed by 'level').  If 'stream' is not valid on entry, this
        // operation has no effect.  Note that the format is not fully
        // specified, and can change without notice.
};


                        // ============================
                        // struct PackedIntArrayImpType
                        // ============================

template <class TYPE>
struct PackedIntArrayImpType {
    // This meta-function selects
    // 'PackedIntArrayImp<PackedIntArrayImp_Unsigned>' if 'TYPE' should be
    // stored as an unsigned integer, and
    // 'PackedIntArrayImp<PackedIntArrayImp_Signed>' otherwise.

    typedef typename bslmf::If<   bsl::is_same<TYPE, unsigned char>::value
                               || bsl::is_same<TYPE, unsigned short>::value
                               || bsl::is_same<TYPE, unsigned int>::value
                               || bsl::is_same<TYPE, unsigned long int>::value
                               || bsl::is_same<TYPE,
                                               bsls::Types::Uint64>::value
                               || bsl::is_same<TYPE, bsl::uint8_t>::value
                               || bsl::is_same<TYPE, bsl::uint16_t>::value
                               || bsl::is_same<TYPE, bsl::uint32_t>::value
                               || bsl::is_same<TYPE, bsl::uint64_t>::value,
                               PackedIntArrayImp<PackedIntArrayImp_Unsigned>,
                               PackedIntArrayImp<PackedIntArrayImp_Signed> >
                                                                ::Type Type;
};

                     // =================================
                     // class PackedIntArrayConstIterator
                     // =================================

template <class TYPE>
class PackedIntArrayConstIterator {
    // This unconstrained (value-semantic) class represents a random access
    // iterator providing non-modifiable access to the elements of a
    // 'PackedIntArray'.  This class provides all functionality of a random
    // access iterator, as defined by the standard, but is *not* compatible
    // with most standard methods requiring a bidirectional const_iterator.
    //
    // This class does not perform any bounds checking.  The returned iterator,
    // 'it', referencing an element within a 'PackedIntArray', 'array', remains
    // valid while '0 <= it - array.begin() < array.length()'.

    // PRIVATE TYPES
    typedef typename PackedIntArrayImpType<TYPE>::Type ImpType;

    // DATA
    const ImpType *d_array_p; // A pointer to the 'PackedIntArrayImp' into
                              // which this iterator references.

    bsl::size_t    d_index;   // The index of the referenced value within the
                              // array.

    // FRIENDS
    friend class PackedIntArray<TYPE>;

    friend PackedIntArrayConstIterator
                               operator++<>(PackedIntArrayConstIterator&, int);

    friend PackedIntArrayConstIterator
                               operator--<>(PackedIntArrayConstIterator&, int);

    friend bool operator==<>(const PackedIntArrayConstIterator&,
                             const PackedIntArrayConstIterator&);

    friend bool operator!=<>(const PackedIntArrayConstIterator&,
                             const PackedIntArrayConstIterator&);

    friend bsl::ptrdiff_t operator-<>(const PackedIntArrayConstIterator&,
                                      const PackedIntArrayConstIterator&);

    friend bool operator< <>(const PackedIntArrayConstIterator&,
                             const PackedIntArrayConstIterator&);

    friend bool operator<=<>(const PackedIntArrayConstIterator&,
                             const PackedIntArrayConstIterator&);

    friend bool operator><>(const PackedIntArrayConstIterator&,
                            const PackedIntArrayConstIterator&);

    friend bool operator>=<>(const PackedIntArrayConstIterator&,
                             const PackedIntArrayConstIterator&);

  public:
    // PUBLIC TYPES

    // The following typedefs define the traits for this iterator to make it
    // compatible with standard functions.

    typedef bsl::ptrdiff_t                   difference_type;   // The type
                                                                // used for the
                                                                // distance
                                                                // between two
                                                                // iterators.

    typedef bsl::size_t                      size_type;         // The type
                                                                // used for any
                                                                // function
                                                                // requiring a
                                                                // length (i.e,
                                                                // index).

    typedef TYPE                             value_type;        // The type for
                                                                // all returns
                                                                // of element
                                                                // values.

    typedef void *                           pointer;           // The type of
                                                                // an arbitrary
                                                                // pointer into
                                                                // the array.

    typedef TYPE&                            reference;         // The type for
                                                                // all returns
                                                                // of element
                                                                // references.

  private:
    // PRIVATE CREATORS
    PackedIntArrayConstIterator(const ImpType *array, bsl::size_t index);
        // Create a 'PackedIntArrayConstIterator' object with a pointer to the
        // specified 'array' and the specified 'index'.  The behavior is
        // undefined unless 'index <= array->length()'.

  public:
    // CREATORS
    PackedIntArrayConstIterator();
        // Create a default 'PackedIntArrayConstIterator'.  Note that the use
        // of most methods - as indicated in their documentation - upon this
        // iterator will result in undefined behavior.

    PackedIntArrayConstIterator(const PackedIntArrayConstIterator& original);
        // Create a 'PackedIntArrayConstIterator' having the same value as the
        // specified 'original' one.

    //! ~PackedIntArrayConstIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    PackedIntArrayConstIterator&
                             operator=(const PackedIntArrayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this iterator.

    PackedIntArrayConstIterator& operator++();
        // Advance this iterator to refer to the next element in the referenced
        // array and return a reference to this iterator *after* the
        // advancement.  The returned iterator, 'it', referencing an element
        // within a 'PackedIntArray', 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry,
        // 'PackedIntArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    PackedIntArrayConstIterator& operator--();
        // Decrement this iterator to refer to the previous element in the
        // referenced array and return a reference to this iterator *after* the
        // decrementation.  The returned iterator, 'it', referencing an element
        // within a 'PackedIntArray', 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry, '0 < *this - array.begin()'.

    PackedIntArrayConstIterator& operator+=(bsl::ptrdiff_t offset);
        // Advance this iterator by the specified 'offset' from the element
        // referenced to this iterator.  The returned iterator, 'it',
        // referencing an element within a 'PackedIntArray', 'array', remains
        // valid as long as '0 <= it - array.begin() <= array.length()'.  The
        // behavior is undefined unless
        // 'PackedIntArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() + offset <= array.length()'.

    PackedIntArrayConstIterator& operator-=(bsl::ptrdiff_t offset);
        // Decrement this iterator by the specified 'offset' from the element
        // referenced to this iterator.  The returned iterator, 'it',
        // referencing an element within a 'PackedIntArray', 'array', remains
        // valid as long as '0 <= it - array.begin() <= array.length()'.  The
        // behavior is undefined unless
        // 'PackedIntArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() - offset <= array.length()'.

    // ACCESSORS
    TYPE operator*() const;
        // Return the element value referenced by this iterator.  The behavior
        // is undefined unless for this iterator, referencing an element within
        // a 'PackedIntArray' 'array',
        // 'PackedIntArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    TYPE operator->() const;
        // Return the element value referenced by this iterator.  The behavior
        // is undefined unless for this iterator, referencing an element within
        // a 'PackedIntArray' 'array',
        // 'PackedIntArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    TYPE operator[](bsl::ptrdiff_t offset) const;
        // Return the element that is the specified 'offset' from the element
        // reference by this array.  The behavior is undefined unless for this
        // iterator, referencing an element within a 'PackedIntArray' 'array',
        // 'PackedIntArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() + offset < array.length()'.

    PackedIntArrayConstIterator operator+(bsl::ptrdiff_t offset) const;
        // Return an iterator referencing the location at the specified
        // 'offset' from the element referenced by this iterator.  The returned
        // iterator, 'it', referencing an element within a 'PackedIntArray',
        // 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless
        // '0 <= *this - array.begin() + offset <= array.length()'.

    PackedIntArrayConstIterator operator-(bsl::ptrdiff_t offset) const;
        // Return an iterator referencing the location at the specified
        // 'offset' from the element referenced by this iterator.  The returned
        // iterator, 'it', referencing an element within a 'PackedIntArray',
        // 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless 'PackedIntArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() - offset <= array.length()'.
};

// FREE FUNCTIONS
template <class TYPE>
PackedIntArrayConstIterator<TYPE>
                      operator++(PackedIntArrayConstIterator<TYPE>& iter, int);
    // Advance the specified iterator 'iter' to refer to the next element in
    // the referenced array, and return an iterator referring to the original
    // element (*before* the advancement).  The returned iterator, 'it',
    // referencing an element within a 'PackedIntArray', 'array', remains valid
    // as long as '0 <= it - array.begin() <= array.length()'.  The behavior is
    // undefined unless, on entry, 'PackedIntArrayConstInterator() != iter' and
    // 'iter - array.begin() < array.length()'.

template <class TYPE>
PackedIntArrayConstIterator<TYPE>
                      operator--(PackedIntArrayConstIterator<TYPE>& iter, int);
    // Decrement the specified iterator 'iter' to refer to the previous element
    // in the referenced array, and return an iterator referring to the
    // original element (*before* the decrementation).  The returned iterator,
    // 'it', referencing an element within a 'PackedIntArray', 'array', remains
    // valid as long as '0 <= it - array.begin() <= array.length()'.  The
    // behavior is undefined unless, on entry,
    // 'PackedIntArrayConstInterator() != iter' and '0 < iter - array.begin()'.

template <class TYPE>
bool operator==(const PackedIntArrayConstIterator<TYPE>& lhs,
                const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two 'PackedIntArrayConstIterator'
    // iterators have the same value if they refer to the same array, and have
    // the same index.

template <class TYPE>
bool operator!=(const PackedIntArrayConstIterator<TYPE>& lhs,
                const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value and 'false' otherwise.  Two 'PackedIntArrayConstIterator'
    // iterators do not have the same value if they do not refer to the same
    // array, or do not have the same index.

template <class TYPE>
bsl::ptrdiff_t operator-(const PackedIntArrayConstIterator<TYPE>& lhs,
                         const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return the number of elements between specified 'lhs' and 'rhs'.  The
    // behavior is undefined unless 'lhs' and 'rhs' reference the same array.

template <class TYPE>
bool operator<(const PackedIntArrayConstIterator<TYPE>& lhs,
               const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', 'false' otherwise.  An iterator has a value less than another if
    // its index is less the other's index.  The behavior is undefined unless
    // 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator<=(const PackedIntArrayConstIterator<TYPE>& lhs,
                const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal to
    // the specified 'rhs, 'false' otherwise.  An iterator has a value less
    // than or equal to another if its index is less or equal the other's
    // index.  The behavior is undefined unless 'lhs' and 'rhs' refer to the
    // same array.

template <class TYPE>
bool operator>(const PackedIntArrayConstIterator<TYPE>& lhs,
               const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater than the
    // specified 'rhs', 'false' otherwise.  An iterator has a value greater
    // than another if its index is greater the other's index.  The behavior is
    // undefined unless 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator>=(const PackedIntArrayConstIterator<TYPE>& lhs,
                const PackedIntArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater or equal than
    // the specified 'rhs', 'false' otherwise.  An iterator has a value greater
    // than or equal to another if its index is greater the other's index.  The
    // behavior is undefined unless 'lhs' and 'rhs' refer to the same array.

                            // ====================
                            // class PackedIntArray
                            // ====================

template <class TYPE>
class PackedIntArray {
    // This space-efficient value-semantic array class represents a sequence of
    // 'TYPE' elements; 'TYPE' must be convertable to either a signed or
    // unsigned 64-bit integer using 'static_cast'.  The interface provides
    // functionality similar to a 'vector<int>' however references to
    // individual elements are not provided.  This class provides accessors
    // that return iterators that provide non-modifiable access to its
    // elements.  The returned iterators, unlike those returned by a
    // 'vector<int>' are *not* invalidated upon reallocation.

    // PRIVATE TYPES
    typedef typename PackedIntArrayImpType<TYPE>::Type ImpType;

    // PRIVATE CLASS DATA
    static const bsl::size_t k_MAX_BYTES_PER_ELEMENT = 8;

    // DATA
    ImpType d_imp;  // Implementation of either a signed or unsigned 64-bit
                    // integer packed array.

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for all returns of element values.

    typedef PackedIntArrayConstIterator<TYPE> const_iterator;

    // CLASS METHODS
    static int maxSupportedBdexVersion(int serializationVersion);
        // Return the 'version' to be used with the 'bdexStreamOut' method
        // corresponding to the specified 'serializationVersion'.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    // CREATORS
    explicit PackedIntArray(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'PackedIntArray'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit PackedIntArray(bsl::size_t       numElements,
                            TYPE              value = 0,
                            bslma::Allocator *basicAllocator = 0);
        // Create a 'PackedIntArray' having the specified 'numElements'.
        // Optionally specify a 'value' to which each element will be set.  If
        // value is not specified, 0 is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    PackedIntArray(const PackedIntArray&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a 'PackedIntArray' having the same value as the specified
        // 'original' one.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~PackedIntArray();
        // Destroy this object

    // MANIPULATORS
    PackedIntArray& operator=(const PackedIntArray& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference providing modifiable access to this array.

    void append(TYPE value);
        // Append an element having the specified 'value' to the end of this
        // array.

    void append(const PackedIntArray& srcArray);
        // Append the sequence of values represented by the specified
        // 'srcArray' to the end of this array.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void append(const PackedIntArray& srcArray,
                bsl::size_t           srcIndex,
                bsl::size_t           numElements);
        // Append the sequence of values of the specified 'numElements'
        // starting at the specified 'srcIndex' in the specified 'srcArray' to
        // the end of this array.  The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    void insert(bsl::size_t dstIndex, TYPE value);
        // Insert into this array, at the specified 'dstIndex', an element
        // having the specified 'value', shifting any elements originally at
        // or above 'dstIndex' up by one.  The behavior is undefined unless
        // 'dstIndex <= length()'.

    const_iterator insert(const_iterator dst, TYPE value);
        // Insert into this array, at the specified 'dst', an element having
        // the specified 'value', shifting any elements originally at or above
        // 'dst' up by one.  Return an iterator to the newly inserted element.

    void insert(bsl::size_t dstIndex, const PackedIntArray& srcArray);
        // Insert into this array, at the specified 'dstIndex', the sequence of
        // values represented by the specified 'srcArray', shifting any
        // elements originally at or above 'dstIndex' up by 'srcArray.length()'
        // indices higher.  The behavior is undefined unless
        // 'dstIndex <= length()'.  Note that if this array and 'srcArray' are
        // the same, the behavior is as if a copy of 'srcArray' were passed.

    void insert(bsl::size_t           dstIndex,
                const PackedIntArray& srcArray,
                bsl::size_t           srcIndex,
                bsl::size_t           numElements);
        // Insert into this array, at the specified 'dstIndex', the specified
        // 'numElements' values in the specified 'srcArray' starting at the
        // specified 'srcIndex'.  Elements greater than or equal to 'dstIndex'
        // are shifted up 'numElements' positions.  The behavior is undefined
        // unless 'dstIndex <= length()' and
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    void pop_back();
        // Remove the last element from this array.  The behavior is undefined
        // unless '0 < length()' .

    void push_back(TYPE value);
        // Append an element having the specified 'value' to the end of this
        // array.

    void remove(bsl::size_t dstIndex);
        // Remove from this array the element at the specified 'dstIndex'.
        // Each element having an index greater than 'dstIndex' before the
        // removal is shifted down by one index position.  The behavior is
        // undefined unless 'dstIndex < length()' .

    void remove(bsl::size_t dstIndex, bsl::size_t numElements);
        // Remove from this array, starting at the specified 'dstIndex', the
        // specified 'numElements', shifting the elements of this array that
        // are at 'dstIndex + numElements' or above to 'numElements' indices
        // lower.  The behavior is undefined unless
        // 'dstIndex + numElements <= length()'.

    const_iterator remove(const_iterator dstFirst, const_iterator dstLast);
        // Remove from this array the elements starting from the specified
        // 'dstFirst' up to, but not including, the specified 'dstLast',
        // shifting the elements of this array that are at or above 'dstLast'
        // to 'dstLast - dstFirst' indices lower.  Return an iterator to the
        // new position of the element that was referred to by 'dstLast' or
        // 'end()' if 'dstLast == end()'.  The behavior is undefined unless
        // 'dstFirst <= dstLast'.

    void removeAll();
        // Remove all the elements from this array and set the storage required
        // per element to one byte.

    void replace(bsl::size_t dstIndex, TYPE value);
        // Change the value of the element at the specified 'dstIndex' in this
        // array to the specified 'value'.  The behavior is undefined unless
        // 'dstIndex < length()'.

    void replace(bsl::size_t           dstIndex,
                 const PackedIntArray& srcArray,
                 bsl::size_t           srcIndex,
                 bsl::size_t           numElements);
        // Change the values of the specified 'numElements' elements in this
        // array beginning at the specified 'dstIndex' to those of the
        // 'numElements' values in the specified 'srcArray' beginning at the
        // specified 'srcIndex'.  The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()' and
        // 'dstIndex + numElements <= length()'.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void reserveCapacity(bsl::size_t numElements);
        // Make the capacity of this array at least the specified
        // 'numElements'.  This method has no effect if the current capacity
        // meets or exceeds the required capacity.

    void reserveCapacity(bsl::size_t numElements, TYPE maxValue);
        // Make the capacity of this array at least the specified
        // 'numElements'.  The specified 'maxValue' denotes the maximum element
        // value that will be subsequently added to this array.  After this
        // call 'numElements' having values in the range '[0, maxValue]' are
        // guaranteed to not cause a reallocation.  This method has no effect
        // if the current capacity meets or exceeds the required capacity.
        // The behavior is undefined unless '0 <= maxValue'.

    void reserveCapacity(bsl::size_t numElements,
                         TYPE        minValue,
                         TYPE        maxValue);
        // Make the capacity of this array at least the specified
        // 'numElements'.  The specified 'minValue' and 'maxValue' denote,
        // respectively, the minimum and maximum elements values that will be
        // subsequently added to this array.  After this call 'numElements'
        // having values in the range '[minValue, maxValue]' are guaranteed to
        // not cause a reallocation.  This method has no effect if the current
        // capacity meets or exceeds the required capacity.  The behavior is
        // undefined unless 'minValue <= maxValue'.

    void resize(bsl::size_t numElements);
        // Set the length of this array to the specified 'numElements'.  If
        // 'numElements > length()', the added elements are initialized to 0.

    void swap(PackedIntArray& other);
        // Efficiently exchange the value of this array with the value of the
        // specified 'other' array.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // array was created with the same allocator as 'other'.

    // ACCESSORS
    TYPE operator[](bsl::size_t index) const;
        // Return the value of the element at the specified 'index'.  The
        // behavior is undefined unless 'index < length()'.

    bslma::Allocator *allocator() const;
        // Return the allocator used by this array to supply memory.

    TYPE back() const;
        // Return the value of the element at the back of this array.  The
        // behavior is undefined unless '0 < length()'.  Note that this
        // function is logically equivalent to:
        //..
        //    operator[](length() - 1)
        //..

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to 'stream'.  If
        // 'stream' is initially invalid, this operation has no effect.  If
        // 'version' is not supported, 'stream' is invalidated but otherwise
        // unmodified.  Note that 'version' is not written to 'stream'.  See
        // the 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    const_iterator begin() const;
        // Return an iterator referring to the first element in this array (or
        // the past-the-end iterator if this array is empty).  This reference
        // remains valid as long as this array exists.

    int bytesPerElement() const;
        // Return the number of bytes currently used to store each element in
        // this array.

    bsl::size_t capacity() const;
        // Return the number of elements this array can hold in terms of the
        // current data type used to store its elements.

    const_iterator end() const;
        // Return an iterator referring to one element beyond the last element
        // in this array.  This reference remains valid as long as this array
        // exists, and length does not decrease.

    TYPE front() const;
        // Return the value of the element at the front of this array.  The
        // behavior is undefined unless '0 < length()'.  Note that this
        // function is logically equivalent to:
        //..
        //    operator[](0)
        //..

    bool isEmpty() const;
        // Return 'true' if there are no elements in this array, and 'false'
        // otherwise.

    bool isEqual(const PackedIntArray& other) const;
        // Return 'true' if this and the specified 'other' array have the same
        // value, and 'false' otherwise.  Two 'PackedIntArray' arrays have the
        // same value if they have the same length, and all corresponding
        // elements (those at the same indices) have the same value.

    bsl::size_t length() const;
        // Return number of elements in this array.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this array to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested arrays.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested arrays.  If 'level' is negative, format the entire
        // output on one line, suppressing all but the initial indentation (as
        // governed by 'level').  If 'stream' is not valid on entry, this
        // operation has no effect.  Note that the format is not fully
        // specified, and can change without notice.
};

// FREE OPERATORS
template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const PackedIntArray<TYPE>& array);
    // Write the value of the specified 'array' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.

template <class TYPE>
bool operator==(const PackedIntArray<TYPE>& lhs,
                const PackedIntArray<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays have the same
    // value, and 'false' otherwise.  Two 'PackedIntArray' arrays have the same
    // value if they have the same length, and all corresponding elements
    // (those at the same indices) have the same value.

template <class TYPE>
bool operator!=(const PackedIntArray<TYPE>& lhs,
                const PackedIntArray<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays do not have the
    // same value, and 'false' otherwise.  Two 'PackedIntArray' arrays do not
    // have the same value if they do not have the same length, or if any
    // corresponding elements (those at the same indices) do not have the same
    // value.

// FREE FUNCTIONS
template <class TYPE>
void swap(PackedIntArray<TYPE>& a, PackedIntArray<TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' arrays.
    // This method provides the no-throw exception-safety guarantee.  This
    // method invalidates previously-obtained iterators and references.  The
    // behavior is undefined unless both arrays were created with the same
    // allocator.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // struct PackedIntArrayImp_Signed
                      // -------------------------------

template <class STREAM>
void PackedIntArrayImp_Signed::bdexGet8(STREAM& stream, bsl::int8_t& variable)
{
    char v;
    stream.getInt8(v);
    variable = static_cast<bsl::int8_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexGet16(STREAM& stream,
                                         bsl::int16_t& variable)
{
    short v;
    stream.getInt16(v);
    variable = static_cast<bsl::int16_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexGet32(STREAM& stream,
                                         bsl::int32_t& variable)
{
    int v;
    stream.getInt32(v);
    variable = static_cast<bsl::int32_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexGet64(STREAM& stream,
                                         bsl::int64_t& variable)
{
    bsls::Types::Int64 v;
    stream.getInt64(v);
    variable = static_cast<bsl::int64_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexPut8(STREAM& stream, bsl::int8_t value)
{
    stream.putInt8(static_cast<int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexPut16(STREAM& stream, bsl::int16_t value)
{
    stream.putInt16(static_cast<int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexPut32(STREAM& stream, bsl::int32_t value)
{
    stream.putInt32(static_cast<int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Signed::bdexPut64(STREAM& stream, bsl::int64_t value)
{
    stream.putInt64(static_cast<bsls::Types::Int64>(value));
}

                     // ---------------------------------
                     // struct PackedIntArrayImp_Unsigned
                     // ---------------------------------

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexGet8(STREAM& stream,
                                          bsl::uint8_t& variable)
{
    unsigned char v;
    stream.getUint8(v);
    variable = static_cast<bsl::uint8_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexGet16(STREAM& stream,
                                           bsl::uint16_t& variable)
{
    unsigned short v;
    stream.getUint16(v);
    variable = static_cast<bsl::uint16_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexGet32(STREAM& stream,
                                           bsl::uint32_t& variable)
{
    unsigned int v;
    stream.getUint32(v);
    variable = static_cast<bsl::uint32_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexGet64(STREAM& stream,
                                           bsl::uint64_t& variable)
{
    bsls::Types::Uint64 v;
    stream.getUint64(v);
    variable = static_cast<bsl::uint64_t>(v);
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexPut8(STREAM& stream, bsl::uint8_t value)
{
    stream.putUint8(static_cast<unsigned int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexPut16(STREAM& stream, bsl::uint16_t value)
{
    stream.putUint16(static_cast<unsigned int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexPut32(STREAM& stream, bsl::uint32_t value)
{
    stream.putUint32(static_cast<unsigned int>(value));
}

template <class STREAM>
void PackedIntArrayImp_Unsigned::bdexPut64(STREAM& stream, bsl::uint64_t value)
{
    stream.putUint64(static_cast<bsls::Types::Uint64>(value));
}

                          // ------------------------
                          // struct PackedIntArrayImp
                          // ------------------------

// PRIVATE CLASS METHODS
template <class STORAGE>
inline
bsl::size_t PackedIntArrayImp<STORAGE>::nextCapacityGE(bsl::size_t minValue,
                                                       bsl::size_t value)
{
    BSLS_ASSERT_SAFE(minValue <= k_MAX_CAPACITY);

    static const bsl::size_t k_TOP_CAPACITY = k_MAX_CAPACITY / 3 * 2 - 3;

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(minValue >= k_TOP_CAPACITY)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return minValue;                                              // RETURN
    }

    while (value < minValue) {
        value += (value + 3) / 2;
    }

    return value;
}

// PRIVATE ACCESSORS
template <class STORAGE>
inline
char *PackedIntArrayImp<STORAGE>::address() const
{
    return static_cast<char *>(d_storage_p);
}

// CLASS METHODS
template <class STORAGE>
inline
int PackedIntArrayImp<STORAGE>::maxSupportedBdexVersion(int)
{
    return 1;
}

// MANIPULATORS
template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::
                             append(const PackedIntArrayImp<STORAGE>& srcArray)
{
    append(srcArray, 0, srcArray.d_length);
}

template <class STORAGE>
template <class STREAM>
inline
STREAM& PackedIntArrayImp<STORAGE>::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            int tmpBytesPerElement;
            {
                char v;
                stream.getInt8(v);
                tmpBytesPerElement = static_cast<int>(v);
            }
            if (   1 != tmpBytesPerElement
                && 2 != tmpBytesPerElement
                && 4 != tmpBytesPerElement
                && 8 != tmpBytesPerElement) {
                stream.invalidate();
            }
            else {
                bsl::size_t tmpLength;
                {
                    int v;
                    stream.getLength(v);
                    tmpLength = static_cast<bsl::size_t>(v);
                }
                if (stream) {
                    bsl::size_t numBytes = tmpBytesPerElement * tmpLength;
                    if (numBytes > d_capacityInBytes) {
                        // Compute next capacity level.

                        bsl::size_t requiredCapacityInBytes =
                                   nextCapacityGE(numBytes, d_capacityInBytes);

                        // Allocate new memory.
                        void *dst =
                              d_allocator_p->allocate(requiredCapacityInBytes);

                        // Deallocate original memory.

                        d_allocator_p->deallocate(d_storage_p);

                        // Update storage and capacity.

                        d_storage_p = dst;
                        d_capacityInBytes = requiredCapacityInBytes;
                    }

                    // Update bytes per element and length.

                    d_bytesPerElement = tmpBytesPerElement;
                    d_length = tmpLength;

                    // Populate the data from the stream.

                    switch (d_bytesPerElement) {
                      case 1: {
                        typename STORAGE::OneByteStorageType *s =
                            static_cast<typename STORAGE::OneByteStorageType *>
                                                                 (d_storage_p);
                        for (bsl::size_t i = 0; i < d_length; ++i) {
                            STORAGE::bdexGet8(stream, s[i]);
                        }
                      } break;
                      case 2: {
                        typename STORAGE::TwoByteStorageType *s =
                            static_cast<typename STORAGE::TwoByteStorageType *>
                                                                 (d_storage_p);
                        for (bsl::size_t i = 0; i < d_length; ++i) {
                            STORAGE::bdexGet16(stream, s[i]);
                        }
                      } break;
                      case 4: {
                        typename STORAGE::FourByteStorageType *s =
                           static_cast<typename STORAGE::FourByteStorageType *>
                                                                 (d_storage_p);
                        for (bsl::size_t i = 0; i < d_length; ++i) {
                            STORAGE::bdexGet32(stream, s[i]);
                        }
                      } break;
                      case 8: {
                        typename STORAGE::EightByteStorageType *s =
                          static_cast<typename STORAGE::EightByteStorageType *>
                                                                 (d_storage_p);
                        for (bsl::size_t i = 0; i < d_length; ++i) {
                            STORAGE::bdexGet64(stream, s[i]);
                        }
                      } break;
                    }
                }
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::insert(
                                    bsl::size_t                       dstIndex,
                                    const PackedIntArrayImp<STORAGE>& srcArray)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_length);

    insert(dstIndex, srcArray, 0, srcArray.length());
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::pop_back()
{
    BSLS_ASSERT_SAFE(0 < d_length);

    --d_length;
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::push_back(ElementType value)
{
    append(value);
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::remove(bsl::size_t dstIndex)
{
    BSLS_ASSERT_SAFE(dstIndex < d_length);

    remove(dstIndex, 1);
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::remove(bsl::size_t dstIndex,
                                        bsl::size_t numElements)
{
    // Assert 'dstIndex + numElements <= d_length' without risk of overflow.
    BSLS_ASSERT_SAFE(numElements <= d_length);
    BSLS_ASSERT_SAFE(dstIndex    <= d_length - numElements);

    d_length -= numElements;

    bsl::memmove(address() + dstIndex * d_bytesPerElement,
                 address() + (dstIndex + numElements) * d_bytesPerElement,
                 (d_length - dstIndex) * d_bytesPerElement);
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::removeAll()
{
    d_length = 0;
    d_bytesPerElement = 1;
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::reserveCapacity(bsl::size_t numElements)
{
    // Test for potential overflow.
    BSLS_ASSERT_SAFE(k_MAX_CAPACITY / d_bytesPerElement >= numElements);

    size_t requiredCapacityInBytes = d_bytesPerElement * numElements;
    if (requiredCapacityInBytes > d_capacityInBytes) {
        reserveCapacityImp(requiredCapacityInBytes);
    }
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::reserveCapacity(bsl::size_t numElements,
                                                 ElementType maxValue)
{
    BSLS_ASSERT_SAFE(0 <= maxValue);

    int requiredBytesPerElement = d_bytesPerElement;

    int rbpe = STORAGE::requiredBytesPerElement(maxValue);
    if (rbpe > requiredBytesPerElement) {
        requiredBytesPerElement = rbpe;
    }

    // Test for potential overflow.
    BSLS_ASSERT_SAFE(k_MAX_CAPACITY / requiredBytesPerElement >= numElements);

    size_t requiredCapacityInBytes = requiredBytesPerElement * numElements;

    if (requiredCapacityInBytes > d_capacityInBytes) {
        reserveCapacityImp(requiredCapacityInBytes);
    }
}

template <>
inline
void PackedIntArrayImp<PackedIntArrayImp_Unsigned>::
                                       reserveCapacity(bsl::size_t numElements,
                                                       ElementType maxValue)
{
    int requiredBytesPerElement = d_bytesPerElement;

    int rbpe = PackedIntArrayImp_Unsigned::requiredBytesPerElement(maxValue);
    if (rbpe > requiredBytesPerElement) {
        requiredBytesPerElement = rbpe;
    }

    // Test for potential overflow.
    BSLS_ASSERT_SAFE(k_MAX_CAPACITY / requiredBytesPerElement >= numElements);

    size_t requiredCapacityInBytes = requiredBytesPerElement * numElements;

    if (requiredCapacityInBytes > d_capacityInBytes) {
        reserveCapacityImp(requiredCapacityInBytes);
    }
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::reserveCapacity(bsl::size_t numElements,
                                                 ElementType minValue,
                                                 ElementType maxValue)
{
    BSLS_ASSERT_SAFE(minValue <= maxValue);

    int requiredBytesPerElement = d_bytesPerElement;

    int rbpe = STORAGE::requiredBytesPerElement(maxValue);
    if (rbpe > requiredBytesPerElement) {
        requiredBytesPerElement = rbpe;
    }

    rbpe = STORAGE::requiredBytesPerElement(minValue);
    if (rbpe > requiredBytesPerElement) {
        requiredBytesPerElement = rbpe;
    }

    // Test for potential overflow.
    BSLS_ASSERT_SAFE(k_MAX_CAPACITY / requiredBytesPerElement >= numElements);

    size_t requiredCapacityInBytes = requiredBytesPerElement * numElements;

    if (requiredCapacityInBytes > d_capacityInBytes) {
        reserveCapacityImp(requiredCapacityInBytes);
    }
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::resize(bsl::size_t numElements)
{
    if (numElements > d_length) {
        reserveCapacity(numElements);
        bsl::memset(address() + d_length * d_bytesPerElement,
                    0,
                    (numElements - d_length) * d_bytesPerElement);
    }
    d_length = numElements;
}

template <class STORAGE>
inline
void PackedIntArrayImp<STORAGE>::swap(PackedIntArrayImp<STORAGE>& other)
{
    BSLS_ASSERT_SAFE(d_allocator_p == other.d_allocator_p);

    bslalg::SwapUtil::swap(&d_storage_p,        &other.d_storage_p);
    bslalg::SwapUtil::swap(&d_length,           &other.d_length);
    bslalg::SwapUtil::swap(&d_bytesPerElement,  &other.d_bytesPerElement);
    bslalg::SwapUtil::swap(&d_capacityInBytes,  &other.d_capacityInBytes);
}

// ACCESSORS
template <class STORAGE>
inline
bslma::Allocator *PackedIntArrayImp<STORAGE>::allocator() const
{
    return d_allocator_p;
}

template <class STORAGE>
template <class STREAM>
inline
STREAM& PackedIntArrayImp<STORAGE>::bdexStreamOut(STREAM& stream,
                                                  int     version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            stream.putInt8(d_bytesPerElement);
            stream.putLength(static_cast<int>(d_length));
            switch (d_bytesPerElement) {
              case 1: {
                typename STORAGE::OneByteStorageType *s =
                            static_cast<typename STORAGE::OneByteStorageType *>
                                                                 (d_storage_p);
                for (bsl::size_t i = 0; i < d_length; ++i) {
                    STORAGE::bdexPut8(stream, s[i]);
                }
              } break;
              case 2: {
                typename STORAGE::TwoByteStorageType *s =
                            static_cast<typename STORAGE::TwoByteStorageType *>
                                                                 (d_storage_p);
                for (bsl::size_t i = 0; i < d_length; ++i) {
                    STORAGE::bdexPut16(stream, s[i]);
                }
              } break;
              case 4: {
                typename STORAGE::FourByteStorageType *s =
                           static_cast<typename STORAGE::FourByteStorageType *>
                                                                 (d_storage_p);
                for (bsl::size_t i = 0; i < d_length; ++i) {
                    STORAGE::bdexPut32(stream, s[i]);
                }
              } break;
              case 8: {
                typename STORAGE::EightByteStorageType *s =
                          static_cast<typename STORAGE::EightByteStorageType *>
                                                                 (d_storage_p);
                for (bsl::size_t i = 0; i < d_length; ++i) {
                    STORAGE::bdexPut64(stream, s[i]);
                }
              } break;
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

template <class STORAGE>
inline
int PackedIntArrayImp<STORAGE>::bytesPerElement() const {
    return d_bytesPerElement;
}

template <class STORAGE>
inline
bsl::size_t PackedIntArrayImp<STORAGE>::capacity() const {
    return d_capacityInBytes / d_bytesPerElement;
}

template <class STORAGE>
inline
bool PackedIntArrayImp<STORAGE>::isEmpty() const {
    return 0 == d_length;
}

template <class STORAGE>
inline
bool PackedIntArrayImp<STORAGE>::isEqual(
                                 const PackedIntArrayImp<STORAGE>& other) const
{
    if (d_length == other.d_length) {
        if (d_bytesPerElement == other.d_bytesPerElement) {
            return 0 == bsl::memcmp(d_storage_p,
                                    other.d_storage_p,
                                    d_length * d_bytesPerElement);    // RETURN
        }
        else {
            return isEqualImp(other);                                 // RETURN
        }
    }
    return false;
}

template <class STORAGE>
inline
bsl::size_t PackedIntArrayImp<STORAGE>::length() const {
    return d_length;
}

                     // ---------------------------------
                     // class PackedIntArrayConstIterator
                     // ---------------------------------

// PRIVATE CREATORS
template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>::PackedIntArrayConstIterator(
                                                          const ImpType *array,
                                                          bsl::size_t    index)
: d_array_p(array)
, d_index(index)
{
    BSLS_ASSERT_SAFE(d_index <= d_array_p->length());
}

// CREATORS
template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>::PackedIntArrayConstIterator()
: d_array_p(0)
, d_index(0)
{
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>::PackedIntArrayConstIterator(
                                   const PackedIntArrayConstIterator& original)
: d_array_p(original.d_array_p)
, d_index(original.d_index)
{
}

// MANIPULATORS
template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>& PackedIntArrayConstIterator<TYPE>::
                              operator=(const PackedIntArrayConstIterator& rhs)
{
    d_array_p = rhs.d_array_p;
    d_index   = rhs.d_index;
    return *this;
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>&
                                PackedIntArrayConstIterator<TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    ++d_index;
    return *this;
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>&
                                PackedIntArrayConstIterator<TYPE>::operator--()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(0 < d_index);

    --d_index;
    return *this;
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>&
           PackedIntArrayConstIterator<TYPE>::operator+=(bsl::ptrdiff_t offset)
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index >= bsl::size_t(offset));

    d_index += offset;
    return *this;
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>&
           PackedIntArrayConstIterator<TYPE>::operator-=(bsl::ptrdiff_t offset)
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index - offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(   0 >= offset || d_index >= bsl::size_t(offset));
    BSLS_ASSERT_SAFE(   0 <= offset
                     || d_array_p->length() - d_index >= bsl::size_t(-offset));

    d_index -= offset;
    return *this;
}

// ACCESSORS
template <class TYPE>
inline
TYPE PackedIntArrayConstIterator<TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return static_cast<TYPE>((*d_array_p)[d_index]);
}

template <class TYPE>
inline
TYPE PackedIntArrayConstIterator<TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return *(*this);
}

template <class TYPE>
inline
TYPE PackedIntArrayConstIterator<TYPE>::operator[](bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset < d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index > bsl::size_t(offset));

    return static_cast<TYPE>((*d_array_p)[d_index + offset]);
}

template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>
      PackedIntArrayConstIterator<TYPE>::operator+(bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index >= bsl::size_t(offset));

    return PackedIntArrayConstIterator<TYPE>(d_array_p, d_index + offset);
}


template <class TYPE>
inline
PackedIntArrayConstIterator<TYPE>
      PackedIntArrayConstIterator<TYPE>::operator-(bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index - offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(   0 >= offset || d_index >= bsl::size_t(offset));
    BSLS_ASSERT_SAFE(   0 <= offset
                     || d_array_p->length() - d_index >= bsl::size_t(-offset));

    return PackedIntArrayConstIterator<TYPE>(d_array_p, d_index - offset);
}

}  // close package namespace

// FREE FUNCTIONS
template <class TYPE>
inline
bdlc::PackedIntArrayConstIterator<TYPE> bdlc::operator++(
                                       PackedIntArrayConstIterator<TYPE>& iter,
                                       int)
{
    BSLS_ASSERT_SAFE(iter.d_array_p);
    BSLS_ASSERT_SAFE(iter.d_index < iter.d_array_p->length());

    const PackedIntArrayConstIterator<TYPE> curr = iter;
    ++iter;
    return curr;
}

template <class TYPE>
inline
bdlc::PackedIntArrayConstIterator<TYPE> bdlc::operator--(
                                       PackedIntArrayConstIterator<TYPE>& iter,
                                       int)
{
    BSLS_ASSERT_SAFE(iter.d_array_p);
    BSLS_ASSERT_SAFE(iter.d_index > 0);

    const PackedIntArrayConstIterator<TYPE> curr = iter;
    --iter;
    return curr;
}

template <class TYPE>
inline
bool bdlc::operator==(const PackedIntArrayConstIterator<TYPE>& lhs,
                      const PackedIntArrayConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p == rhs.d_array_p && lhs.d_index == rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator!=(const PackedIntArrayConstIterator<TYPE>& lhs,
                      const PackedIntArrayConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p != rhs.d_array_p || lhs.d_index != rhs.d_index;
}

template <class TYPE>
inline
bsl::ptrdiff_t bdlc::operator-(const PackedIntArrayConstIterator<TYPE>& lhs,
                               const PackedIntArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    BSLS_ASSERT_SAFE(
          lhs.d_index >= rhs.d_index
        ? lhs.d_index - rhs.d_index <=
                        bsl::size_t(bsl::numeric_limits<bsl::ptrdiff_t>::max())
        : rhs.d_index - lhs.d_index <=
                      bsl::size_t(bsl::numeric_limits<bsl::ptrdiff_t>::min()));

    return static_cast<bsl::ptrdiff_t>(lhs.d_index - rhs.d_index);
}

template <class TYPE>
inline
bool bdlc::operator<(const PackedIntArrayConstIterator<TYPE>& lhs,
                     const PackedIntArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index < rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator<=(const PackedIntArrayConstIterator<TYPE>& lhs,
                      const PackedIntArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index <= rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator>(const PackedIntArrayConstIterator<TYPE>& lhs,
                     const PackedIntArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index > rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator>=(const PackedIntArrayConstIterator<TYPE>& lhs,
                      const PackedIntArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index >= rhs.d_index;
}

namespace bdlc {

                    // --------------------
                    // class PackedIntArray
                    // --------------------

// CLASS METHODS
template <class TYPE>
inline
int PackedIntArray<TYPE>::maxSupportedBdexVersion(int serializationVersion)
{
    return ImpType::maxSupportedBdexVersion(serializationVersion);
}

// CREATORS
template <class TYPE>
inline
PackedIntArray<TYPE>::PackedIntArray(bslma::Allocator *basicAllocator)
: d_imp(basicAllocator)
{
}

template <class TYPE>
inline
PackedIntArray<TYPE>::PackedIntArray(bsl::size_t       numElements,
                                     TYPE              value,
                                     bslma::Allocator *basicAllocator)
: d_imp(numElements,
        static_cast<typename ImpType::ElementType>(value),
        basicAllocator)
{
}

template <class TYPE>
inline
PackedIntArray<TYPE>::PackedIntArray(
                                   const PackedIntArray<TYPE>&  original,
                                   bslma::Allocator            *basicAllocator)
: d_imp(original.d_imp, basicAllocator)
{
}

template <class TYPE>
inline
PackedIntArray<TYPE>::~PackedIntArray()
{
}

// MANIPULATORS
template <class TYPE>
inline
PackedIntArray<TYPE>& PackedIntArray<TYPE>::operator=(
                                               const PackedIntArray<TYPE>& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::append(TYPE value)
{
    d_imp.append(static_cast<typename ImpType::ElementType>(value));
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::append(const PackedIntArray<TYPE>& srcArray)
{
    d_imp.append(srcArray.d_imp);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::append(const PackedIntArray<TYPE>& srcArray,
                                  bsl::size_t                 srcIndex,
                                  bsl::size_t                 numElements)
{
    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    d_imp.append(srcArray.d_imp, srcIndex, numElements);
}

template <class TYPE>
template <class STREAM>
inline
STREAM& PackedIntArray<TYPE>::bdexStreamIn(STREAM& stream, int version)
{
    return d_imp.bdexStreamIn(stream, version);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::insert(bsl::size_t dstIndex, TYPE value)
{
    BSLS_ASSERT_SAFE(dstIndex <= length());

    d_imp.insert(dstIndex, static_cast<typename ImpType::ElementType>(value));
}

template <class TYPE>
inline
typename PackedIntArray<TYPE>::const_iterator
                   PackedIntArray<TYPE>::insert(const_iterator dst, TYPE value)
{
    insert(dst.d_index, value);
    return dst;
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::insert(bsl::size_t                 dstIndex,
                                  const PackedIntArray<TYPE>& srcArray)
{
    BSLS_ASSERT_SAFE(dstIndex <= length());

    d_imp.insert(dstIndex, srcArray.d_imp);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::insert(bsl::size_t                 dstIndex,
                                  const PackedIntArray<TYPE>& srcArray,
                                  bsl::size_t                 srcIndex,
                                  bsl::size_t                 numElements)
{
    BSLS_ASSERT_SAFE(dstIndex <= length());

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    d_imp.insert(dstIndex, srcArray.d_imp, srcIndex, numElements);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::pop_back()
{
    BSLS_ASSERT_SAFE(0 < length());

    d_imp.pop_back();
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::push_back(TYPE value)
{
    d_imp.push_back(static_cast<typename ImpType::ElementType>(value));
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::remove(bsl::size_t dstIndex)
{
    BSLS_ASSERT(dstIndex < length());

    d_imp.remove(dstIndex);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::remove(bsl::size_t dstIndex,
                                  bsl::size_t numElements)
{
    // Assert 'dstIndex + numElements <= length()' without risk of overflow.
    BSLS_ASSERT_SAFE(numElements <= length());
    BSLS_ASSERT_SAFE(dstIndex    <= length() - numElements);

    d_imp.remove(dstIndex, numElements);
}

template <class TYPE>
inline
typename PackedIntArray<TYPE>::const_iterator
  PackedIntArray<TYPE>::remove(const_iterator dstFirst, const_iterator dstLast)
{
    BSLS_ASSERT_SAFE(dstFirst <= dstLast);

    remove(dstFirst.d_index, dstLast.d_index - dstFirst.d_index);
    return dstFirst;
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::removeAll()
{
    d_imp.removeAll();
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::replace(bsl::size_t dstIndex, TYPE value)
{
    BSLS_ASSERT_SAFE(dstIndex < length());

    d_imp.replace(dstIndex, static_cast<typename ImpType::ElementType>(value));
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::replace(bsl::size_t                 dstIndex,
                                   const PackedIntArray<TYPE>& srcArray,
                                   bsl::size_t                 srcIndex,
                                   bsl::size_t                 numElements)
{
    // Assert 'dstIndex + numElements <= length()' without risk of overflow.
    BSLS_ASSERT_SAFE(numElements <= length());
    BSLS_ASSERT_SAFE(dstIndex    <= length() - numElements);

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    d_imp.replace(dstIndex, srcArray.d_imp, srcIndex, numElements);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::reserveCapacity(bsl::size_t numElements)
{
    // Test for potential overflow.
    BSLS_ASSERT_SAFE(
             ImpType::k_MAX_CAPACITY / k_MAX_BYTES_PER_ELEMENT >= numElements);

    d_imp.reserveCapacityImp(numElements * k_MAX_BYTES_PER_ELEMENT);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::reserveCapacity(bsl::size_t numElements,
                                           TYPE        maxValue)
{
    // To avoid a compiler warning, asserting '0 <= maxValue' is omitted; the
    // test is performed in 'd_imp.reserveCapacity'.

    d_imp.reserveCapacity(numElements, maxValue);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::reserveCapacity(bsl::size_t numElements,
                                           TYPE        minValue,
                                           TYPE        maxValue)
{
    BSLS_ASSERT_SAFE(minValue <= maxValue);

    d_imp.reserveCapacity(numElements, minValue, maxValue);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::resize(bsl::size_t numElements)
{
    d_imp.resize(numElements);
}

template <class TYPE>
inline
void PackedIntArray<TYPE>::swap(PackedIntArray<TYPE>& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_imp.swap(other.d_imp);
}

// ACCESSORS
template <class TYPE>
inline
TYPE PackedIntArray<TYPE>::operator[](bsl::size_t index) const
{
    BSLS_ASSERT_SAFE(index < length());

    return static_cast<TYPE>(d_imp[index]);
}

template <class TYPE>
inline
bslma::Allocator *PackedIntArray<TYPE>::allocator() const
{
    return d_imp.allocator();
}

template <class TYPE>
inline
TYPE PackedIntArray<TYPE>::back() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return static_cast<TYPE>(d_imp[length() - 1]);
}

template <class TYPE>
template <class STREAM>
inline
STREAM& PackedIntArray<TYPE>::bdexStreamOut(STREAM& stream, int version) const
{
    return d_imp.bdexStreamOut(stream, version);
}

template <class TYPE>
inline
typename PackedIntArray<TYPE>::const_iterator
                                            PackedIntArray<TYPE>::begin() const
{
    return const_iterator(&d_imp, 0);
}

template <class TYPE>
inline
int PackedIntArray<TYPE>::bytesPerElement() const
{
    return d_imp.bytesPerElement();
}

template <class TYPE>
inline
bsl::size_t PackedIntArray<TYPE>::capacity() const
{
    return d_imp.capacity();
}

template <class TYPE>
inline
typename PackedIntArray<TYPE>::const_iterator PackedIntArray<TYPE>::end() const
{
    return const_iterator(&d_imp, d_imp.length());
}

template <class TYPE>
inline
TYPE PackedIntArray<TYPE>::front() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return static_cast<TYPE>(d_imp[0]);
}

template <class TYPE>
inline
bool PackedIntArray<TYPE>::isEmpty() const
{
    return d_imp.isEmpty();
}

template <class TYPE>
inline
bool PackedIntArray<TYPE>::isEqual(const PackedIntArray<TYPE>& other) const
{
    return d_imp.isEqual(other.d_imp);
}

template <class TYPE>
inline
bsl::size_t PackedIntArray<TYPE>::length() const
{
    return d_imp.length();
}

template <class TYPE>
bsl::ostream& PackedIntArray<TYPE>::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    return d_imp.print(stream, level, spacesPerLevel);
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bsl::ostream& bdlc::operator<<(bsl::ostream&               stream,
                               const PackedIntArray<TYPE>& array)
{
    return array.print(stream);
}

template <class TYPE>
inline
bool bdlc::operator==(const PackedIntArray<TYPE>& lhs,
                      const PackedIntArray<TYPE>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class TYPE>
inline
bool bdlc::operator!=(const PackedIntArray<TYPE>& lhs,
                      const PackedIntArray<TYPE>& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlc::swap(PackedIntArray<TYPE>& a, PackedIntArray<TYPE>& b)
{
    BSLS_ASSERT_SAFE(a.allocator() == b.allocator());

    a.swap(b);
}

}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <class STORAGE>
struct UsesBslmaAllocator<bdlc::PackedIntArrayImp<STORAGE> >
                                                           : bsl::true_type {};

template <class TYPE>
struct UsesBslmaAllocator<bdlc::PackedIntArray<TYPE> > : bsl::true_type {};

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
