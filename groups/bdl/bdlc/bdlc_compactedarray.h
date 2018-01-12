// bdlc_compactedarray.h                                              -*-C++-*-
#ifndef INCLUDED_BDLC_COMPACTEDARRAY
#define INCLUDED_BDLC_COMPACTEDARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compacted array of user-defined objects.
//
//@CLASSES:
//  bdlc::CompactedArray: compacted array of user-defined objects
//  bdlc::CompactedArrayConstIterator: bidirectional 'const_iterator'
//
//@DESCRIPTION: This component provides a space-effient value-semantic array,
// 'bdlc::CompactedArray', and an associated iterator,
// 'bdlc::CompactedArrayConstIterator', that provides non-modifiable access to
// its elements.  The interface of this class provides the user with
// functionality similar to a 'bsl::vector<T>'.  The implementation is designed
// to reduce dynamic memory usage by removing data duplication at the expense
// of an additional indirection to obtain the stored data by use of the
// flyweight design pattern and requiring 'operator<' to be defined for the
// stored type.  The array supports primitive operations (e.g., insertion,
// look-up, removal) as well as a complete set of value-semantic operations;
// however, direct reference to individual elements is not available.  Users
// can access the value of individual elements by calling the indexing operator
// or via iterators.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Storing Daily Schedules'
/// - - - - - - - - - - - - - - - - - -
// Suppose we are creating a sequence of daily schedules for an employee.
// Most Mondays (Tuesdays, Wednesdays, etc.) will have the same schedule,
// although some may differ.  Instead of storing this data in a
// 'bsl::vector<my_DailySchedule>', we can use
// 'bdlc::CompactedArray<my_DailySchedule>' to efficiently store this data.
//
// First, we declare and define a 'my_DailySchedule' class.  This class is not
// overly relevant to the example and is elided for the sake of compactness
//..
//                          // ================
//                          // my_DailySchedule
//                          // ================
//
//  class my_DailySchedule {
//      // A value-semantic class the provides a daily schedule and consumes a
//      // significant amount of memory.
//
//      int d_initialLocationId;
//
//      // ...
//
//      // FRIENDS
//      friend bool operator<(const my_DailySchedule&,
//                            const my_DailySchedule&);
//
//    public:
//      // CREATORS
//      my_DailySchedule(int initialLocationId);
//          // Create a 'my_DailySchedule' object having the specified
//          // 'initialLocationId'.
//
//      // ...
//
//  };
//
//  bool operator<(const my_DailySchedule& lhs, const my_DailySchedule& rhs);
//      // Return 'true' if the specified 'lhs' is lexicographically less than
//      // the specified 'rhs' object, and 'false' otherwise.
//
//                           // ----------------
//                           // my_DailySchedule
//                           // ----------------
//
//  // CREATORS
//  inline
//  my_DailySchedule::my_DailySchedule(int initialLocationId)
//  : d_initialLocationId(initialLocationId)
//  {
//  }
//
//  bool operator<(const my_DailySchedule& lhs, const my_DailySchedule& rhs)
//  {
//      if (lhs.d_initialLocationId < rhs.d_initialLocationId) {
//          return true;                                              // RETURN
//      }
//
//      // ...
//
//      return false;
//  }
//..
// Then, we create our schedule, which is a vector of 'my_DailySchedule' where
// the index is the date offset (from an arbitrary epoch measured in days).
//..
//  bdlc::CompactedArray<my_DailySchedule> schedule;
//..
// Now, we create some daily schedules and append them to the schedule.
//..
//  my_DailySchedule evenDays(0);
//  my_DailySchedule oddDays(1);
//
//  // Population of the 'my_DailySchedule' is elided.
//
//  schedule.push_back(evenDays);
//  schedule.push_back(oddDays);
//  schedule.push_back(evenDays);
//  schedule.push_back(oddDays);
//  schedule.push_back(evenDays);
//..
// Finally, we verify that the storage is compacted.
//..
//  assert(5 == schedule.length());
//  assert(2 == schedule.uniqueLength());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLC_PACKEDINTARRAY
#include <bdlc_packedintarray.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_BSLIM_PRINTER
#include <bslim_printer.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace bdlc {

// FORWARD DECLARATIONS
template <class TYPE> class CompactedArray;

template <class TYPE> class CompactedArrayConstIterator;

template <class TYPE> CompactedArrayConstIterator<TYPE>
                           operator++(CompactedArrayConstIterator<TYPE>&, int);

template <class TYPE> CompactedArrayConstIterator<TYPE>
                           operator--(CompactedArrayConstIterator<TYPE>&, int);

template <class TYPE>
bool operator==(const CompactedArrayConstIterator<TYPE>&,
                const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator!=(const CompactedArrayConstIterator<TYPE>&,
                const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bsl::ptrdiff_t operator-(const CompactedArrayConstIterator<TYPE>&,
                         const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator<(const CompactedArrayConstIterator<TYPE>&,
               const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator<=(const CompactedArrayConstIterator<TYPE>&,
                const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator>(const CompactedArrayConstIterator<TYPE>&,
               const CompactedArrayConstIterator<TYPE>&);

template <class TYPE>
bool operator>=(const CompactedArrayConstIterator<TYPE>&,
                const CompactedArrayConstIterator<TYPE>&);

                    // ==================================
                    // struct CompactedArray_CountedValue
                    // ==================================

template <class TYPE>
struct CompactedArray_CountedValue {
    // This 'struct' represents a reference counted value.

    // PUBLIC DATA
    TYPE        d_value;
    bsl::size_t d_count;

    // CREATORS
    CompactedArray_CountedValue(const TYPE& value, bsl::size_t count)
        : d_value(value)
        , d_count(count)
    {
    }
};

// FREE OPERATORS
template <class TYPE>
bool operator==(const CompactedArray_CountedValue<TYPE>& lhs,
                const CompactedArray_CountedValue<TYPE>& rhs) {
    return lhs.d_value == rhs.d_value;
}

template <class TYPE>
bool operator!=(const CompactedArray_CountedValue<TYPE>& lhs,
                const CompactedArray_CountedValue<TYPE>& rhs) {
    return lhs.d_value != rhs.d_value;
}

template <class TYPE>
bool operator<(const CompactedArray_CountedValue<TYPE>& lhs, const TYPE& rhs) {
    return lhs.d_value < rhs;
}

template <class TYPE>
bool operator<(const TYPE& lhs, const CompactedArray_CountedValue<TYPE>& rhs) {
    return lhs < rhs.d_value;
}

                    // =================================
                    // class CompactedArrayConstIterator
                    // =================================

template <class TYPE>
class CompactedArrayConstIterator {
    // This unconstrained (value-semantic) class represents a random access
    // iterator providing non-modifiable access to the elements of a
    // 'CompactedArray'.  This class provides all functionality of a random
    // access iterator, as defined by the standard, but is *not* compatible
    // with most standard methods requiring a bidirectional const_iterator.
    //
    // This class does not perform any bounds checking.  The returned iterator,
    // 'it', referencing an element within a 'CompactedArray', 'array', remains
    // valid while '0 <= it - array.begin() < array.length()'.

    // DATA
    const CompactedArray<TYPE> *d_array_p;  // A pointer to the
                                            // 'CompactedArray' into which this
                                            // iterator references.

    bsl::size_t                 d_index;    // The index of the referenced
                                            // value within the array.

    // FRIENDS
    friend class CompactedArray<TYPE>;

    friend CompactedArrayConstIterator
                               operator++<>(CompactedArrayConstIterator&, int);

    friend CompactedArrayConstIterator
                               operator--<>(CompactedArrayConstIterator&, int);

    friend bool operator==<>(const CompactedArrayConstIterator&,
                             const CompactedArrayConstIterator&);

    friend bool operator!=<>(const CompactedArrayConstIterator&,
                             const CompactedArrayConstIterator&);

    friend bsl::ptrdiff_t operator-<>(const CompactedArrayConstIterator&,
                                      const CompactedArrayConstIterator&);

    friend bool operator< <>(const CompactedArrayConstIterator&,
                             const CompactedArrayConstIterator&);

    friend bool operator<=<>(const CompactedArrayConstIterator&,
                             const CompactedArrayConstIterator&);

    friend bool operator><>(const CompactedArrayConstIterator&,
                            const CompactedArrayConstIterator&);

    friend bool operator>=<>(const CompactedArrayConstIterator&,
                             const CompactedArrayConstIterator&);

  public:
    // PUBLIC TYPES

    // The following typedefs define the traits for this iterator to make it
    // compatible with standard functions.

    typedef bsl::ptrdiff_t difference_type;  // The type used for the distance
                                             // between two iterators.

    typedef bsl::size_t    size_type;        // The type used for any function
                                             // requiring a length (i.e,
                                             // index).

    typedef TYPE           value_type;       // The type for all returns of
                                             // element values.

    typedef TYPE *         pointer;          // The type of an arbitrary
                                             // pointer into the array.

    typedef TYPE&          reference;        // The type for all returns of
                                             // element references.

    typedef std::random_access_iterator_tag  iterator_category;
                                             // This is a random access
                                             // iterator.

  private:
    // PRIVATE CREATORS
    CompactedArrayConstIterator(const CompactedArray<TYPE> *array,
                                bsl::size_t                 index);
        // Create a 'CompactedArrayConstIterator' object with a pointer to the
        // specified 'array' and the specified 'index'.  The behavior is
        // undefined unless 'index <= array->length()'.

  public:
    // CREATORS
    CompactedArrayConstIterator();
        // Create a default 'CompactedArrayConstIterator'.  Note that the use
        // of most methods - as indicated in their documentation - upon this
        // iterator will result in undefined behavior.

    CompactedArrayConstIterator(const CompactedArrayConstIterator& original);
        // Create a 'CompactedArrayConstIterator' having the same value as the
        // specified 'original' one.

    //! ~CompactedArrayConstIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    CompactedArrayConstIterator&
                             operator=(const CompactedArrayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this iterator.

    CompactedArrayConstIterator& operator++();
        // Advance this iterator to refer to the next element in the referenced
        // array and return a reference to this iterator *after* the
        // advancement.  The returned iterator, 'it', referencing an element
        // within a 'CompactedArray', 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry,
        // 'CompactedArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    CompactedArrayConstIterator& operator--();
        // Decrement this iterator to refer to the previous element in the
        // referenced array and return a reference to this iterator *after* the
        // decrementation.  The returned iterator, 'it', referencing an element
        // within a 'CompactedArray', 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry, '0 < *this - array.begin()'.

    CompactedArrayConstIterator& operator+=(bsl::ptrdiff_t offset);
        // Advance this iterator by the specified 'offset' from the element
        // referenced to this iterator.  The returned iterator, 'it',
        // referencing an element within a 'CompactedArray', 'array', remains
        // valid as long as '0 <= it - array.begin() <= array.length()'.  The
        // behavior is undefined unless
        // 'CompactedArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() + offset <= array.length()'.

    CompactedArrayConstIterator& operator-=(bsl::ptrdiff_t offset);
        // Decrement this iterator by the specified 'offset' from the element
        // referenced to this iterator.  The returned iterator, 'it',
        // referencing an element within a 'CompactedArray', 'array', remains
        // valid as long as '0 <= it - array.begin() <= array.length()'.  The
        // behavior is undefined unless
        // 'CompactedArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() - offset <= array.length()'.

    // ACCESSORS
    const TYPE& operator*() const;
        // Return the element value referenced by this iterator.  The behavior
        // is undefined unless for this iterator, referencing an element within
        // a 'CompactedArray' 'array',
        // 'CompactedArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    const TYPE& operator->() const;
        // Return the element value referenced by this iterator.  The behavior
        // is undefined unless for this iterator, referencing an element within
        // a 'CompactedArray' 'array',
        // 'CompactedArrayConstInterator() != *this' and
        // '*this - array.begin() < array.length()'.

    const TYPE& operator[](bsl::ptrdiff_t offset) const;
        // Return the element that is the specified 'offset' from the element
        // reference by this array.  The behavior is undefined unless for this
        // iterator, referencing an element within a 'CompactedArray' 'array',
        // 'CompactedArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() + offset < array.length()'.

    CompactedArrayConstIterator operator+(bsl::ptrdiff_t offset) const;
        // Return an iterator referencing the location at the specified
        // 'offset' from the element referenced by this iterator.  The returned
        // iterator, 'it', referencing an element within a 'CompactedArray',
        // 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless
        // '0 <= *this - array.begin() + offset <= array.length()'.

    CompactedArrayConstIterator operator-(bsl::ptrdiff_t offset) const;
        // Return an iterator referencing the location at the specified
        // 'offset' from the element referenced by this iterator.  The returned
        // iterator, 'it', referencing an element within a 'CompactedArray',
        // 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless 'CompactedArrayConstInterator() != *this' and
        // '0 <= *this - array.begin() - offset <= array.length()'.
};

// FREE FUNCTIONS
template <class TYPE>
CompactedArrayConstIterator<TYPE>
                      operator++(CompactedArrayConstIterator<TYPE>& iter, int);
    // Advance the specified iterator 'iter' to refer to the next element in
    // the referenced array, and return an iterator referring to the original
    // element (*before* the advancement).  The returned iterator, 'it',
    // referencing an element within a 'CompactedArray', 'array', remains valid
    // as long as '0 <= it - array.begin() <= array.length()'.  The behavior is
    // undefined unless, on entry, 'CompactedArrayConstInterator() != iter' and
    // 'iter - array.begin() < array.length()'.

template <class TYPE>
CompactedArrayConstIterator<TYPE>
                      operator--(CompactedArrayConstIterator<TYPE>& iter, int);
    // Decrement the specified iterator 'iter' to refer to the previous element
    // in the referenced array, and return an iterator referring to the
    // original element (*before* the decrementation).  The returned iterator,
    // 'it', referencing an element within a 'CompactedArray', 'array', remains
    // valid as long as '0 <= it - array.begin() <= array.length()'.  The
    // behavior is undefined unless, on entry,
    // 'CompactedArrayConstInterator() != iter' and '0 < iter - array.begin()'.

template <class TYPE>
bool operator==(const CompactedArrayConstIterator<TYPE>& lhs,
                const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two 'CompactedArrayConstIterator'
    // iterators have the same value if they refer to the same array, and have
    // the same index.

template <class TYPE>
bool operator!=(const CompactedArrayConstIterator<TYPE>& lhs,
                const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value and 'false' otherwise.  Two 'CompactedArrayConstIterator'
    // iterators do not have the same value if they do not refer to the same
    // array, or do not have the same index.

template <class TYPE>
bsl::ptrdiff_t operator-(const CompactedArrayConstIterator<TYPE>& lhs,
                         const CompactedArrayConstIterator<TYPE>& rhs);
    // Return the number of elements between specified 'lhs' and 'rhs'.  The
    // behavior is undefined unless 'lhs' and 'rhs' reference the same array.

template <class TYPE>
bool operator<(const CompactedArrayConstIterator<TYPE>& lhs,
               const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', 'false' otherwise.  An iterator has a value less than another if
    // its index is less the other's index.  The behavior is undefined unless
    // 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator<=(const CompactedArrayConstIterator<TYPE>& lhs,
                const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal to
    // the specified 'rhs, 'false' otherwise.  An iterator has a value less
    // than or equal to another if its index is less or equal the other's
    // index.  The behavior is undefined unless 'lhs' and 'rhs' refer to the
    // same array.

template <class TYPE>
bool operator>(const CompactedArrayConstIterator<TYPE>& lhs,
               const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater than the
    // specified 'rhs', 'false' otherwise.  An iterator has a value greater
    // than another if its index is greater the other's index.  The behavior is
    // undefined unless 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator>=(const CompactedArrayConstIterator<TYPE>& lhs,
                const CompactedArrayConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater or equal than
    // the specified 'rhs', 'false' otherwise.  An iterator has a value greater
    // than or equal to another if its index is greater the other's index.  The
    // behavior is undefined unless 'lhs' and 'rhs' refer to the same array.

                           // ====================
                           // class CompactedArray
                           // ====================

template <class TYPE>
class CompactedArray {
    // PRIVATE TYPES
    typedef bsl::vector<CompactedArray_CountedValue<TYPE> > Data;

    // DATA
    Data                        d_data;
    PackedIntArray<bsl::size_t> d_index;

    // PRIVATE MANIPULATORS
    void erase(bsl::size_t index);

    bsl::size_t increment(const TYPE& value, bsl::size_t count = 1);

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for all returns of element values.

    typedef CompactedArrayConstIterator<TYPE> const_iterator;

    // CREATORS
    explicit CompactedArray(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'CompactedArray'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    explicit CompactedArray(bsl::size_t       numElements,
                            const TYPE&       value = TYPE(),
                            bslma::Allocator *basicAllocator = 0);
        // Create a 'CompactedArray' having the specified 'numElements'.
        // Optionally specify a 'value' to which each element will be set.  If
        // value is not specified, 'TYPE()' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    CompactedArray(const CompactedArray&  obj,
                   bslma::Allocator      *basicAllocator);
        // Create a 'CompactedArray' having the same value as the specified
        // 'original' one.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~CompactedArray();
        // Destroy this object

    // MANIPULATORS
    CompactedArray& operator=(const CompactedArray& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference providing modifiable access to this array.

    void append(const TYPE& value);
        // Append an element having the specified 'value' to the end of this
        // array.

    void append(const CompactedArray& srcArray);
        // Append the sequence of values represented by the specified
        // 'srcArray' to the end of this array.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void append(const CompactedArray& srcArray,
                bsl::size_t           srcIndex,
                bsl::size_t           numElements);
        // Append the sequence of values of the specified 'numElements'
        // starting at the specified 'srcIndex' in the specified 'srcArray' to
        // the end of this array.  The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    void insert(bsl::size_t dstIndex, const TYPE& value);
        // Insert into this array, at the specified 'dstIndex', an element of
        // specified 'value', shifting any elements originally at or above
        // 'dstIndex' up by one.  The behavior is undefined unless
        // 'dstIndex <= length()'.

    const_iterator insert(const_iterator dst, const TYPE& value);
        // Insert into this array, at the specified 'dst', an element having
        // the specified 'value', shifting any elements originally at or above
        // 'dst' up by one.  Return an iterator to the newly inserted element.

    void insert(bsl::size_t dstIndex, const CompactedArray& srcArray);
        // Insert into this array, at the specified 'dstIndex', the sequence of
        // values represented by the specified 'srcArray', shifting any
        // elements originally at or above 'dstIndex' up by 'srcArray.length()'
        // indices higher.  The behavior is undefined unless
        // 'dstIndex <= length()'.  Note that if this array and 'srcArray' are
        // the same, the behavior is as if a copy of 'srcArray' were passed.

    void insert(bsl::size_t           dstIndex,
                const CompactedArray& srcArray,
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

    void push_back(const TYPE& value);
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
        // Remove all the elements from this array.

    void replace(bsl::size_t dstIndex, const TYPE& value);
        // Change the value of the element at the specified 'dstIndex' in this
        // array to the specified 'value'.  The behavior is undefined unless
        // 'dstIndex < length()'.

    void replace(bsl::size_t           dstIndex,
                 const CompactedArray& srcArray,
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
        // 'numElements', assuming the number of unique elements within this
        // array does not increase.  This method has no effect if the current
        // capacity meets or exceeds the required capacity.  The behavior is
        // undefined unless 'false == isEmpty() || 0 == numElements'.

    void reserveCapacity(bsl::size_t numElements,
                         bsl::size_t numUniqueElements);
        // Make the capacity of this array at least the specified
        // 'numElements', assuming the number of unique elements in this array
        // does not exceed the greater of the specified 'numUniqueElements' and
        // 'uniqueLength()'.  This method has no effect if the current capacity
        // meets or exceeds the required capacity.  The behavior is undefined
        // unless 'numUniqueElements <= numElements' and
        // '0 < numUniqueElements || 0 == numElements'.

    void resize(bsl::size_t numElements);
        // Set the length of this array to the specified 'numElements'.  If
        // 'numElements > length()', the added elements are initialized to
        // 'TYPE()'.

    void swap(CompactedArray& other);
        // Efficiently exchange the value of this array with the value of the
        // specified 'other' array.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // array was created with the same allocator as 'other'.

    // ACCESSORS
    const TYPE& operator[](bsl::size_t index) const;
        // Return the value of the element at the specified 'index'.  The
        // behavior is undefined unless 'index < length()'.

    bslma::Allocator *allocator() const;
        // Return the allocator used by this array to supply memory.

    const TYPE& back() const;
        // Return the value of the element at the back of this array.  The
        // behavior is undefined unless '0 < length()'.  Note that this
        // function is logically equivalent to:
        //..
        //    operator[](length() - 1)
        //..

    const_iterator begin() const;
        // Return an iterator referring to the first element in this array (or
        // the past-the-end iterator if this array is empty).  This reference
        // remains valid as long as this array exists.

    bsl::size_t capacity() const;
        // Return the number of elements this array can hold, without
        // reallocation, assuming the number of unique elements within this
        // array does not increase.

    const_iterator end() const;
        // Return an iterator referring to one element beyond the last element
        // in this array.  This reference remains valid as long as this array
        // exists, and length does not decrease.

    const TYPE& front() const;
        // Return the value of the element at the front of this array.  The
        // behavior is undefined unless '0 < length()'.  Note that this
        // function is logically equivalent to:
        //..
        //    operator[](0)
        //..

    bool isEmpty() const;
        // Return 'true' if there are no elements in this array, and 'false'
        // otherwise.

    bool isEqual(const CompactedArray& other) const;
        // Return 'true' if this and the specified 'other' array have the same
        // value, and 'false' otherwise.  Two 'CompactedArray' arrays have the
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

    const TYPE& uniqueElement(bsl::size_t index) const;
        // Return the value of the element at the specified 'index' within the
        // sorted sequence of unique element values in this object.  The
        // behavior is undefined unless 'index < uniqueLength()'.

    bsl::size_t uniqueLength() const;
        // Return number of unique elements in this array.
};

// FREE OPERATORS
template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const CompactedArray<TYPE>& array);

template <class TYPE>
bool operator==(const CompactedArray<TYPE>& lhs,
                const CompactedArray<TYPE>& rhs);

template <class TYPE>
bool operator!=(const CompactedArray<TYPE>& lhs,
                const CompactedArray<TYPE>& rhs);

// FREE FUNCTIONS
template <class TYPE>
void swap(CompactedArray<TYPE>& a, CompactedArray<TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' arrays.
    // This method provides the no-throw exception-safety guarantee.  This
    // method invalidates previously-obtained iterators and references.  The
    // behavior is undefined unless both arrays were created with the same
    // allocator.

// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const CompactedArray<TYPE>& input);
    // Pass the specified 'input' to the specified 'hashAlg'

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================
                     // ---------------------------------
                     // class CompactedArrayConstIterator
                     // ---------------------------------

// PRIVATE CREATORS
template <class TYPE>
CompactedArrayConstIterator<TYPE>::CompactedArrayConstIterator(
                                             const CompactedArray<TYPE> *array,
                                             bsl::size_t                 index)
: d_array_p(array)
, d_index(index)
{
    BSLS_ASSERT_SAFE(d_index <= d_array_p->length());
}

// CREATORS
template <class TYPE>
CompactedArrayConstIterator<TYPE>::CompactedArrayConstIterator()
: d_array_p(0)
, d_index(0)
{
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>::CompactedArrayConstIterator(
                                   const CompactedArrayConstIterator& original)
: d_array_p(original.d_array_p)
, d_index(original.d_index)
{
}

// MANIPULATORS
template <class TYPE>
CompactedArrayConstIterator<TYPE>& CompactedArrayConstIterator<TYPE>::
                              operator=(const CompactedArrayConstIterator& rhs)
{
    d_array_p = rhs.d_array_p;
    d_index   = rhs.d_index;
    return *this;
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>&
                                CompactedArrayConstIterator<TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    ++d_index;
    return *this;
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>&
                                CompactedArrayConstIterator<TYPE>::operator--()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(0 < d_index);

    --d_index;
    return *this;
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>&
           CompactedArrayConstIterator<TYPE>::operator+=(bsl::ptrdiff_t offset)
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
CompactedArrayConstIterator<TYPE>&
           CompactedArrayConstIterator<TYPE>::operator-=(bsl::ptrdiff_t offset)
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
const TYPE& CompactedArrayConstIterator<TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return (*d_array_p)[d_index];
}

template <class TYPE>
const TYPE& CompactedArrayConstIterator<TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return (*d_array_p)[d_index];
}

template <class TYPE>
const TYPE& CompactedArrayConstIterator<TYPE>::
                                        operator[](bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset < d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index > bsl::size_t(offset));

    return (*d_array_p)[d_index + offset];
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>
      CompactedArrayConstIterator<TYPE>::operator+(bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index >= bsl::size_t(offset));

    return CompactedArrayConstIterator<TYPE>(d_array_p, d_index + offset);
}

template <class TYPE>
CompactedArrayConstIterator<TYPE>
      CompactedArrayConstIterator<TYPE>::operator-(bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index - offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(   0 >= offset || d_index >= bsl::size_t(offset));
    BSLS_ASSERT_SAFE(   0 <= offset
                     || d_array_p->length() - d_index >= bsl::size_t(-offset));

    return CompactedArrayConstIterator<TYPE>(d_array_p, d_index - offset);
}

}  // close package namespace

// FREE FUNCTIONS
template <class TYPE>
bdlc::CompactedArrayConstIterator<TYPE> bdlc::operator++(
                                       CompactedArrayConstIterator<TYPE>& iter,
                                       int)
{
    BSLS_ASSERT_SAFE(iter.d_array_p);
    BSLS_ASSERT_SAFE(iter.d_index < iter.d_array_p->length());

    const CompactedArrayConstIterator<TYPE> curr = iter;
    ++iter;
    return curr;
}

template <class TYPE>
bdlc::CompactedArrayConstIterator<TYPE> bdlc::operator--(
                                       CompactedArrayConstIterator<TYPE>& iter,
                                       int)
{
    BSLS_ASSERT_SAFE(iter.d_array_p);
    BSLS_ASSERT_SAFE(iter.d_index > 0);

    const CompactedArrayConstIterator<TYPE> curr = iter;
    --iter;
    return curr;
}

template <class TYPE>
bool bdlc::operator==(const CompactedArrayConstIterator<TYPE>& lhs,
                      const CompactedArrayConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p == rhs.d_array_p && lhs.d_index == rhs.d_index;
}

template <class TYPE>
bool bdlc::operator!=(const CompactedArrayConstIterator<TYPE>& lhs,
                      const CompactedArrayConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p != rhs.d_array_p || lhs.d_index != rhs.d_index;
}

template <class TYPE>
bsl::ptrdiff_t bdlc::operator-(const CompactedArrayConstIterator<TYPE>& lhs,
                               const CompactedArrayConstIterator<TYPE>& rhs)
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
bool bdlc::operator<(const CompactedArrayConstIterator<TYPE>& lhs,
                     const CompactedArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index < rhs.d_index;
}

template <class TYPE>
bool bdlc::operator<=(const CompactedArrayConstIterator<TYPE>& lhs,
                      const CompactedArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index <= rhs.d_index;
}

template <class TYPE>
bool bdlc::operator>(const CompactedArrayConstIterator<TYPE>& lhs,
                     const CompactedArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index > rhs.d_index;
}

template <class TYPE>
bool bdlc::operator>=(const CompactedArrayConstIterator<TYPE>& lhs,
                      const CompactedArrayConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index >= rhs.d_index;
}

namespace bdlc {

                           // --------------------
                           // class CompactedArray
                           // --------------------

// PRIVATE MANIPULATORS
template <class TYPE>
void CompactedArray<TYPE>::erase(bsl::size_t index)
{
    for (bsl::size_t i = 0; i < d_index.length(); ++i) {
        if (d_index[i] > index) {
            d_index.replace(i, d_index[i] - 1);
        }
    }

    d_data.erase(d_data.begin() + index);
}

template <class TYPE>
bsl::size_t CompactedArray<TYPE>::increment(const TYPE& value,
                                            bsl::size_t count)
{
    bsl::size_t index;

    typename Data::iterator iter = bsl::lower_bound(d_data.begin(),
                                                    d_data.end(),
                                                    value);

    if (iter == d_data.end()) {
        index = d_data.size();
        d_data.emplace_back(value, count);
    }
    else if (value < iter->d_value) {
        index = iter - d_data.begin();

        d_data.insert(iter, CompactedArray_CountedValue<TYPE>(value, count));

        for (bsl::size_t i = 0; i < d_index.length(); ++i) {
            if (d_index[i] >= index) {
                d_index.replace(i, d_index[i] + 1);
            }
        }
    }
    else {
        index = iter - d_data.begin();

        iter->d_count += count;
    }

    return index;
}

// CREATORS
template <class TYPE>
CompactedArray<TYPE>::CompactedArray(bslma::Allocator *basicAllocator)
: d_data(basicAllocator)
, d_index(basicAllocator)
{
}

template <class TYPE>
CompactedArray<TYPE>::CompactedArray(bsl::size_t       numElements,
                                     const TYPE&       value,
                                     bslma::Allocator *basicAllocator)
: d_data(basicAllocator)
, d_index(basicAllocator)
{
    if (numElements) {
        d_index.reserveCapacity(numElements, 1);

        d_data.emplace_back(value, numElements);
        d_index.resize(numElements);
    }
}

template <class TYPE>
CompactedArray<TYPE>::CompactedArray(
                                   const CompactedArray<TYPE>&  obj,
                                   bslma::Allocator            *basicAllocator)
: d_data(obj.d_data, basicAllocator)
, d_index(obj.d_index, basicAllocator)
{
}

template <class TYPE>
CompactedArray<TYPE>::~CompactedArray()
{
}

// MANIPULATORS
template <class TYPE>
CompactedArray<TYPE>& CompactedArray<TYPE>::operator=(
                                               const CompactedArray<TYPE>& rhs)
{
    if (this != &rhs) {
        d_index.reserveCapacity(rhs.length(), rhs.uniqueLength());
        d_data = rhs.d_data;
        d_index = rhs.d_index;
    }

    return *this;
}

template <class TYPE>
void CompactedArray<TYPE>::append(const TYPE& value)
{
    d_index.reserveCapacity(d_index.length() + 1, d_data.size() + 1);

    d_index.push_back(increment(value));
}

template <class TYPE>
void CompactedArray<TYPE>::append(const CompactedArray& srcArray)
{
    if (&srcArray != this) {
        d_index.reserveCapacity(d_index.length() + srcArray.d_index.length(),
                                d_data.size()    + srcArray.d_data.size());

        for (bsl::size_t i = 0; i < srcArray.length(); ++i) {
            d_index.push_back(increment(srcArray[i]));
        }
    }
    else {
        d_index.reserveCapacity(d_index.length() * 2);

        for (bsl::size_t i = 0; i < d_data.size(); ++i) {
            d_data[i].d_count *= 2;
        }

        d_index.append(d_index);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::append(const CompactedArray& srcArray,
                                  bsl::size_t           srcIndex,
                                  bsl::size_t           numElements)
{
    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    if (&srcArray != this) {
        d_index.reserveCapacity(d_index.length() + numElements,
                                d_data.size()    + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_index.push_back(increment(srcArray[srcIndex + i]));
        }
    }
    else {
        d_index.reserveCapacity(d_index.length() + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_data[d_index[srcIndex + i]].d_count += 1;
        }

        d_index.append(d_index, srcIndex, numElements);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::insert(bsl::size_t dstIndex, const TYPE& value)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_index.length());

    d_index.reserveCapacity(d_index.length() + 1, d_data.size() + 1);

    d_index.insert(dstIndex, increment(value));
}

template <class TYPE>
typename CompactedArray<TYPE>::const_iterator
            CompactedArray<TYPE>::insert(const_iterator dst, const TYPE& value)
{
    insert(dst.d_index, value);
    return dst;
}

template <class TYPE>
void CompactedArray<TYPE>::insert(bsl::size_t           dstIndex,
                                  const CompactedArray& srcArray)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_index.length());

    if (&srcArray != this) {
        d_index.reserveCapacity(d_index.length() + srcArray.d_index.length(),
                                d_data.size()    + srcArray.d_data.size());

        for (bsl::size_t i = 0; i < srcArray.length(); ++i) {
            d_index.insert(dstIndex + i, increment(srcArray[i]));
        }
    }
    else {
        d_index.reserveCapacity(d_index.length() * 2);

        for (bsl::size_t i = 0; i < d_data.size(); ++i) {
            d_data[i].d_count *= 2;
        }

        d_index.insert(dstIndex, d_index);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::insert(bsl::size_t           dstIndex,
                                  const CompactedArray& srcArray,
                                  bsl::size_t           srcIndex,
                                  bsl::size_t           numElements)
{
    BSLS_ASSERT_SAFE(dstIndex <= d_index.length());

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    if (&srcArray != this) {
        d_index.reserveCapacity(d_index.length() + numElements,
                                d_data.size()    + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_index.insert(dstIndex + i, increment(srcArray[srcIndex + i]));
        }
    }
    else {
        d_index.reserveCapacity(d_index.length() + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_data[d_index[srcIndex + i]].d_count += 1;
        }

        d_index.insert(dstIndex, d_index, srcIndex, numElements);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::pop_back()
{
    bsl::size_t                        dataIndex = d_index.back();
    CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

    d_index.pop_back();

    if (0 == --dataValue.d_count) {
        erase(dataIndex);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::push_back(const TYPE& value)
{
    append(value);
}

template <class TYPE>
void CompactedArray<TYPE>::remove(bsl::size_t dstIndex)
{
    BSLS_ASSERT_SAFE(dstIndex < d_index.length());

    remove(dstIndex, 1);
}

template <class TYPE>
void CompactedArray<TYPE>::remove(bsl::size_t dstIndex,
                                  bsl::size_t numElements)
{
    // Assert 'dstIndex + numElements <= d_index.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= d_index.length());
    BSLS_ASSERT_SAFE(dstIndex    <= d_index.length() - numElements);

    bsl::size_t endIndex = dstIndex + numElements;
    for (bsl::size_t i = dstIndex; i < endIndex; ++i) {
        bsl::size_t                        dataIndex = d_index[i];
        CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

        if (0 == --dataValue.d_count) {
            erase(dataIndex);
        }
    }

    d_index.remove(dstIndex, numElements);
}

template <class TYPE>
typename CompactedArray<TYPE>::const_iterator
                          CompactedArray<TYPE>::remove(const_iterator dstFirst,
                                                       const_iterator dstLast)
{
    BSLS_ASSERT_SAFE(dstFirst <= dstLast);

    remove(dstFirst.d_index, dstLast.d_index - dstFirst.d_index);
    return dstFirst;
}

template <class TYPE>
void CompactedArray<TYPE>::removeAll()
{
    d_data.clear();
    d_index.removeAll();
}

template <class TYPE>
void CompactedArray<TYPE>::replace(bsl::size_t dstIndex, const TYPE& value)
{
    BSLS_ASSERT_SAFE(dstIndex < length());

    d_index.reserveCapacity(d_index.length(), d_data.size() + 1);

    bsl::size_t                        newDataIndex = increment(value);
    bsl::size_t                        dataIndex    = d_index[dstIndex];
    CompactedArray_CountedValue<TYPE>& dataValue    = d_data[dataIndex];

    if (0 == --dataValue.d_count) {
        erase(dataIndex);
        if (dataIndex <= newDataIndex) {
            --newDataIndex;
        }
    }

    d_index.replace(dstIndex, newDataIndex);
}

template <class TYPE>
void CompactedArray<TYPE>::replace(bsl::size_t           dstIndex,
                                   const CompactedArray& srcArray,
                                   bsl::size_t           srcIndex,
                                   bsl::size_t           numElements)
{
    // Assert 'dstIndex + numElements <= length()' without risk of overflow.
    BSLS_ASSERT_SAFE(numElements <= length());
    BSLS_ASSERT_SAFE(dstIndex    <= length() - numElements);

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(numElements <= srcArray.length());
    BSLS_ASSERT_SAFE(srcIndex    <= srcArray.length() - numElements);

    if (&srcArray != this) {
        d_index.reserveCapacity(d_index.length(), d_data.size() + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            bsl::size_t newDataIndex = increment(srcArray[srcIndex + i]);
            bsl::size_t dataIndex    = d_index[dstIndex + i];

            CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

            if (0 == --dataValue.d_count) {
                erase(dataIndex);
                if (dataIndex <= newDataIndex) {
                    --newDataIndex;
                }
            }

            d_index.replace(dstIndex + i, newDataIndex);
        }
    }
    else {
        bsl::size_t endIndex;

        endIndex = srcIndex + numElements;
        for (bsl::size_t i = srcIndex; i < endIndex; ++i) {
            ++d_data[d_index[i]].d_count;
        }

        endIndex = dstIndex + numElements;
        for (bsl::size_t i = dstIndex; i < endIndex; ++i) {
            bsl::size_t                        dataIndex = d_index[i];
            CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

            if (0 == --dataValue.d_count) {
                erase(dataIndex);
            }
        }

        d_index.replace(dstIndex, d_index, srcIndex, numElements);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::reserveCapacity(bsl::size_t numElements)
{
    BSLS_ASSERT_SAFE(false == isEmpty() || 0 == numElements);

    if (0 < numElements) {
        d_index.reserveCapacity(numElements, d_data.size() - 1);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::reserveCapacity(bsl::size_t numElements,
                                           bsl::size_t numUniqueElements)
{
    BSLS_ASSERT_SAFE(numUniqueElements <= numElements);
    BSLS_ASSERT_SAFE(0 < numUniqueElements || 0 == numElements);

    if (0 < numElements) {
        d_data.reserve(numUniqueElements);
        if (d_data.size() > numUniqueElements) {
            numUniqueElements = d_data.size();
        }
        d_index.reserveCapacity(numElements, numUniqueElements - 1);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::resize(bsl::size_t numElements)
{
    if (d_index.length() < numElements) {
        d_index.reserveCapacity(numElements, d_data.size() + 1);

        bsl::size_t count = numElements - d_index.length();
        bsl::size_t index = increment(TYPE(), count);

        for (bsl::size_t i = 0; i < count; ++i) {
            d_index.push_back(index);
        }
    }
    else {
        bsl::size_t count = d_index.length() - numElements;

        for (bsl::size_t i = 0; i < count; ++i) {
            pop_back();
        }
    }
}

template <class TYPE>
void CompactedArray<TYPE>::swap(CompactedArray<TYPE>& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_data,  &other.d_data);
    bslalg::SwapUtil::swap(&d_index, &other.d_index);
}

// ACCESSORS
template <class TYPE>
const TYPE& CompactedArray<TYPE>::operator[](bsl::size_t index) const
{
    return d_data[d_index[index]].d_value;
}

template <class TYPE>
bslma::Allocator *CompactedArray<TYPE>::allocator() const
{
    return d_index.allocator();
}

template <class TYPE>
const TYPE& CompactedArray<TYPE>::back() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return operator[](length() - 1);
}

template <class TYPE>
typename CompactedArray<TYPE>::const_iterator
                                            CompactedArray<TYPE>::begin() const
{
    return const_iterator(this, 0);
}

template <class TYPE>
bsl::size_t CompactedArray<TYPE>::capacity() const
{
    return d_index.isEmpty() ? 0 : d_index.capacity();
}

template <class TYPE>
typename CompactedArray<TYPE>::const_iterator CompactedArray<TYPE>::end() const
{
    return const_iterator(this, d_index.length());
}

template <class TYPE>
const TYPE& CompactedArray<TYPE>::front() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return operator[](0);
}

template <class TYPE>
bool CompactedArray<TYPE>::isEmpty() const {
    return 0 == length();
}

template <class TYPE>
bool CompactedArray<TYPE>::isEqual(const CompactedArray& other) const
{
    return d_index == other.d_index && d_data == other.d_data;
}

template <class TYPE>
bsl::size_t CompactedArray<TYPE>::length() const
{
    return d_index.length();
}

template <class TYPE>
bsl::ostream& CompactedArray<TYPE>::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    for (bsl::size_t i = 0; i < d_index.length(); ++i) {
        printer.printValue(d_data[d_index[i]].d_value);
    }
    printer.end();

    return stream;
}

template <class TYPE>
const TYPE& CompactedArray<TYPE>::uniqueElement(bsl::size_t index) const
{
    return d_data[index].d_value;
}

template <class TYPE>
bsl::size_t CompactedArray<TYPE>::uniqueLength() const
{
    return d_data.size();
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
bsl::ostream& bdlc::operator<<(bsl::ostream&               stream,
                               const CompactedArray<TYPE>& array)
{
    return array.print(stream, 0, -1);
}

template <class TYPE>
bool bdlc::operator==(const CompactedArray<TYPE>& lhs,
                      const CompactedArray<TYPE>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class TYPE>
bool bdlc::operator!=(const CompactedArray<TYPE>& lhs,
                      const CompactedArray<TYPE>& rhs)
{
    return !lhs.isEqual(rhs);
}

// FREE FUNCTIONS
template <class TYPE>
void bdlc::swap(CompactedArray<TYPE>& a, CompactedArray<TYPE>& b)
{
    BSLS_ASSERT_SAFE(a.allocator() == b.allocator());

    a.swap(b);
}

// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
inline
void bdlc::hashAppend(HASHALG& hashAlg, const CompactedArray<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    typedef typename CompactedArray<TYPE>::const_iterator ci_t;
    hashAppend(hashAlg, input.length());
    for (ci_t b = input.begin(), e = input.end(); b != e; ++b) {
        hashAppend(hashAlg, *b);
    }
}

}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator<bdlc::CompactedArray<TYPE> > : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
