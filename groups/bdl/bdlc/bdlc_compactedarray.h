// bdlc_compactedarray.h                                              -*-C++-*-
#ifndef INCLUDED_BDLC_COMPACTEDARRAY
#define INCLUDED_BDLC_COMPACTEDARRAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compacted array of 'const' user-defined objects.
//
//@CLASSES:
//  bdlc::CompactedArray: compacted array of user-defined objects
//
//@DESCRIPTION: This component provides a space-efficient value-semantic array,
// 'bdlc::CompactedArray', and an associated iterator,
// 'bdlc::CompactedArray::const_iterator', that provides non-modifiable access
// to its elements.  The interface of this class provides the user with
// functionality similar to a 'bsl::vector<T>'.  The implementation is designed
// to reduce dynamic memory usage by (1) removing data duplication at the
// expense of an additional indirection to obtain the stored objects (using the
// flyweight design pattern) and (2) requiring 'operator<' to be defined for
// the type of the stored objects.  The array supports primitive operations
// (e.g., insertion, look-up, removal), as well as a complete set of
// value-semantic operations; however, modifiable reference to individual
// elements is not available.  Users can access the (non-modifiable) value of
// individual elements by calling the indexing operator or via iterators.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Storing Daily Schedules'
/// - - - - - - - - - - - - - - - - - -
// Suppose we are creating a sequence of daily schedules for an employee.  Most
// Mondays (and Tuesdays, Wednesdays, etc.) will have the same schedule,
// although some may differ.  Instead of storing this data in a
// 'bsl::vector<my_DailySchedule>', we can use
// 'bdlc::CompactedArray<my_DailySchedule>' to efficiently store this data.
//
// First, we declare and define a 'my_DailySchedule' class.  This class is not
// overly relevant to the example and is elided for the sake of brevity:
//..
//                          // ================
//                          // my_DailySchedule
//                          // ================
//
//  class my_DailySchedule {
//      // A value-semantic class that provides a daily schedule and consumes a
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
//      my_DailySchedule(int               initialLocationId,
//                       bslma::Allocator *basicAllocator = 0);
//          // Create a 'my_DailySchedule' object having the specified
//          // 'initialLocationId'.  Optionally specify a 'basicAllocator' used
//          // to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
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
//  my_DailySchedule::my_DailySchedule(int               initialLocationId,
//                                     bslma::Allocator *basicAllocator)
//  : d_initialLocationId(initialLocationId)
//  {
//      (void)basicAllocator;  // suppress unused variable compiler warning
//
//      // ...
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
// Then, we create our schedule, which is an array of 'my_DailySchedule' where
// the index of each element is the date offset (from an arbitrary epoch
// measured in days).
//..
//  bdlc::CompactedArray<my_DailySchedule> schedule;
//..
// Now, we create some daily schedules and append them to the 'schedule':
//..
//  my_DailySchedule evenDays(0);
//  my_DailySchedule oddDays(1);
//
//  // Population of the 'my_DailySchedule' objects is elided.
//
//  schedule.push_back(evenDays);
//  schedule.push_back(oddDays);
//  schedule.push_back(evenDays);
//  schedule.push_back(oddDays);
//  schedule.push_back(evenDays);
//..
// Finally, we verify that the storage is compacted:
//..
//  assert(5 == schedule.length());
//  assert(2 == schedule.uniqueLength());
//..

#include <bdlscm_version.h>

#include <bdlc_packedintarray.h>

#include <bslalg_swaputil.h>

#include <bslh_hash.h>

#include <bslim_printer.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdlc {

// FORWARD DECLARATIONS
template <class TYPE> class CompactedArray;

template <class TYPE> class CompactedArray_ConstIterator;

template <class TYPE> CompactedArray_ConstIterator<TYPE>
                          operator++(CompactedArray_ConstIterator<TYPE>&, int);

template <class TYPE> CompactedArray_ConstIterator<TYPE>
                          operator--(CompactedArray_ConstIterator<TYPE>&, int);

template <class TYPE>
CompactedArray_ConstIterator<TYPE> operator+(
                                     const CompactedArray_ConstIterator<TYPE>&,
                                     bsl::ptrdiff_t);

template <class TYPE>
CompactedArray_ConstIterator<TYPE> operator-(
                                     const CompactedArray_ConstIterator<TYPE>&,
                                     bsl::ptrdiff_t);

template <class TYPE>
bsl::ptrdiff_t operator-(const CompactedArray_ConstIterator<TYPE>&,
                         const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator==(const CompactedArray_ConstIterator<TYPE>&,
                const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator!=(const CompactedArray_ConstIterator<TYPE>&,
                const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator<(const CompactedArray_ConstIterator<TYPE>&,
               const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator<=(const CompactedArray_ConstIterator<TYPE>&,
                const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator>(const CompactedArray_ConstIterator<TYPE>&,
               const CompactedArray_ConstIterator<TYPE>&);

template <class TYPE>
bool operator>=(const CompactedArray_ConstIterator<TYPE>&,
                const CompactedArray_ConstIterator<TYPE>&);

                  // =====================================
                  // class CompactedArray_RemoveAllProctor
                  // =====================================

template <class TYPE>
class CompactedArray_RemoveAllProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically invokes 'removeAll' on a
    // 'CompactedArray' upon destruction.

    // DATA
    CompactedArray<TYPE> *d_array_p;  // managed array

    // NOT IMPLEMENTED
    CompactedArray_RemoveAllProctor();
    CompactedArray_RemoveAllProctor(const CompactedArray_RemoveAllProctor&);
    CompactedArray_RemoveAllProctor& operator=(
                                       const CompactedArray_RemoveAllProctor&);

  public:
    // CREATORS
    CompactedArray_RemoveAllProctor(CompactedArray<TYPE> *array);
        // Create a 'removeAll' proctor that conditionally manages the
        // specified 'array' (if non-zero).

    ~CompactedArray_RemoveAllProctor();
        // Destroy this object and, if 'release' has not been invoked, invoke
        // the managed array's 'removeAll' method.

    // MANIPULATORS
    void release();
        // Release from management the array currently managed by this proctor.
        // If no array, this method has no effect.
};

                    // ==================================
                    // struct CompactedArray_CountedValue
                    // ==================================

template <class TYPE>
struct CompactedArray_CountedValue {
    // This 'struct' represents a reference-counted value.  Note that
    // comparison of 'd_count' is intentionally omitted from the free
    // equality-comparison operators of this class.

    // PUBLIC DATA
    bsls::ObjectBuffer<TYPE> d_value;  // footprint of stored object
    bsl::size_t              d_count;  // reference count of the stored object

    // CREATORS
    CompactedArray_CountedValue(const TYPE&       value,
                                bsl::size_t       count,
                                bslma::Allocator *basicAllocator);
        // Create a 'CompactedArray_CountedValue' having the specified 'value'
        // and reference 'count'.  The specified 'basicAllocator' is used to
        // supply memory.  The behavior is undefined unless
        // '0 != basicAllocator'.

    CompactedArray_CountedValue(
                     const CompactedArray_CountedValue<TYPE>&  original,
                     bslma::Allocator                         *basicAllocator);
        // Create a 'CompactedArray_CountedValue' having the same underlying
        // object value and reference count as the specified 'original' object.
        // The specified 'basicAllocator' is used to supply memory.  The
        // behavior is undefined unless '0 != basicAllocator'.

    ~CompactedArray_CountedValue();
        // Destroy this object.

    // MANIPULATORS
    CompactedArray_CountedValue& operator=(
                                 const CompactedArray_CountedValue<TYPE>& rhs);
        // Assign to this object the underlying object value and reference
        // count of the specified 'rhs' object, and return a reference
        // providing modifiable access to this object.
};

// FREE OPERATORS
template <class TYPE>
bool operator==(const CompactedArray_CountedValue<TYPE>& lhs,
                const CompactedArray_CountedValue<TYPE>& rhs);
    // Return 'true' if the underlying object value of the specified 'lhs' is
    // the same as the underlying object value of the specified 'rhs', and
    // 'false' otherwise.  Note that the reference counts are intentionally
    // ignored.

template <class TYPE>
bool operator!=(const CompactedArray_CountedValue<TYPE>& lhs,
                const CompactedArray_CountedValue<TYPE>& rhs);
    // Return 'true' if the underlying object value of the specified 'lhs' is
    // not the same as the underlying object value of the specified 'rhs', and
    // 'false' otherwise.  Note that the reference counts are intentionally
    // ignored.

template <class TYPE>
bool operator<(const CompactedArray_CountedValue<TYPE>& lhs, const TYPE& rhs);
    // Return 'true' if the underlying object value of the specified 'lhs' is
    // less than the value of the specified 'rhs', and 'false' otherwise.  Note
    // that the reference count is intentionally ignored.

template <class TYPE>
bool operator<(const TYPE& lhs, const CompactedArray_CountedValue<TYPE>& rhs);
    // Return 'true' if the value of the specified 'lhs' is less than the
    // underlying object value of the specified 'rhs', and 'false' otherwise.
    // Note that the reference count is intentionally ignored.

                    // ==================================
                    // class CompactedArray_ConstIterator
                    // ==================================

template <class TYPE>
class CompactedArray_ConstIterator {
    // This value-semantic class represents a random access iterator providing
    // non-modifiable access to the elements of a 'CompactedArray'.  This class
    // provides all functionality of a random access iterator, as defined by
    // the standard, but is *not* compatible with most standard methods
    // requiring a bidirectional 'const_iterator'.
    //
    // This class does not perform any bounds checking.  Any iterator, 'it',
    // referencing a 'CompactedArray' 'array', remains valid while
    // '0 <= it - array.begin() < array.length()'.

    // DATA
    const CompactedArray<TYPE> *d_array_p;  // 'CompactedArray' referenced by
                                            // this iterator, or 0 if default
                                            // value

    bsl::size_t                 d_index;    // index of the referenced element,
                                            // or one past the end of
                                            // 'd_array_p'

    // FRIENDS
    friend class CompactedArray<TYPE>;

    friend CompactedArray_ConstIterator
                              operator++<>(CompactedArray_ConstIterator&, int);

    friend CompactedArray_ConstIterator
                              operator--<>(CompactedArray_ConstIterator&, int);

    friend bool operator==<>(const CompactedArray_ConstIterator&,
                             const CompactedArray_ConstIterator&);

    friend bool operator!=<>(const CompactedArray_ConstIterator&,
                             const CompactedArray_ConstIterator&);

    friend CompactedArray_ConstIterator<TYPE> operator+<>(
                                     const CompactedArray_ConstIterator<TYPE>&,
                                     bsl::ptrdiff_t);

    friend CompactedArray_ConstIterator<TYPE> operator-<>(
                                     const CompactedArray_ConstIterator<TYPE>&,
                                     bsl::ptrdiff_t);

    friend bsl::ptrdiff_t operator-<>(const CompactedArray_ConstIterator&,
                                      const CompactedArray_ConstIterator&);

    friend bool operator< <>(const CompactedArray_ConstIterator&,
                             const CompactedArray_ConstIterator&);

    friend bool operator<=<>(const CompactedArray_ConstIterator&,
                             const CompactedArray_ConstIterator&);

    friend bool operator><>(const CompactedArray_ConstIterator&,
                            const CompactedArray_ConstIterator&);

    friend bool operator>=<>(const CompactedArray_ConstIterator&,
                             const CompactedArray_ConstIterator&);

  public:
    // PUBLIC TYPES

    // The following 'typedef's define the traits for this iterator to make it
    // compatible with standard functions.

    typedef bsl::ptrdiff_t  difference_type;  // The type used for the distance
                                              // between two iterators.

    typedef bsl::size_t     size_type;        // The type used for any function
                                              // requiring a length (i.e,
                                              // index).

    typedef TYPE            value_type;       // The type for elements.

    typedef TYPE           *pointer;          // The type of an arbitrary
                                              // pointer into the array.

    typedef TYPE&           reference;        // The type for element
                                              // references.

    typedef std::random_access_iterator_tag iterator_category;
                                              // This is a random access
                                              // iterator.

  private:
    // PRIVATE CREATORS
    CompactedArray_ConstIterator(const CompactedArray<TYPE> *array,
                                 bsl::size_t                 index);
        // Create a 'CompactedArray_ConstIterator' object that refers to the
        // element at the specified 'index' in the specified 'array', or the
        // past-the-end iterator for 'array' if 'index == array->length()'.
        // The behavior is undefined unless 'index <= array->length()'.

  public:
    // CREATORS
    CompactedArray_ConstIterator();
        // Create a default 'CompactedArray_ConstIterator'.  Note that the
        // behavior of most methods is undefined when used on a
        // default-constructed iterator.

    CompactedArray_ConstIterator(const CompactedArray_ConstIterator& original);
        // Create a 'CompactedArray_ConstIterator' having the same value as the
        // specified 'original' object.

    //! ~CompactedArray_ConstIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    CompactedArray_ConstIterator&
                             operator=(
                                      const CompactedArray_ConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this iterator.

    CompactedArray_ConstIterator& operator+=(bsl::ptrdiff_t offset);
        // Advance this iterator by the specified 'offset' from the location
        // referenced by this iterator, and return a reference providing
        // modifiable access to this iterator.  The returned iterator, 'it',
        // referencing a 'CompactedArray' 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless 'CompactedArray_ConstIterator() != *this' and
        // '0 <= *this - array.begin() + offset <= array.length()'.

    CompactedArray_ConstIterator& operator-=(bsl::ptrdiff_t offset);
        // Decrement this iterator by the specified 'offset' from the location
        // referenced by this iterator, and return a reference providing
        // modifiable access to this iterator.  The returned iterator, 'it',
        // referencing a 'CompactedArray' 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless 'CompactedArray_ConstIterator() != *this' and
        // '0 <= *this - array.begin() - offset <= array.length()'.

    CompactedArray_ConstIterator& operator++();
        // Advance this iterator to refer to the next location in the
        // referenced array, and return a reference to this iterator *after*
        // the advancement.  The returned iterator, 'it', referencing a
        // 'CompactedArray' 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry,
        // 'CompactedArray_ConstIterator() != *this' and
        // '*this - array.begin() < array.length()'.

    CompactedArray_ConstIterator& operator--();
        // Decrement this iterator to refer to the previous location in the
        // referenced array, and return a reference to this iterator *after*
        // the decrementation.  The returned iterator, 'it', referencing a
        // 'CompactedArray' 'array', remains valid as long as
        // '0 <= it - array.begin() <= array.length()'.  The behavior is
        // undefined unless, on entry,
        // 'CompactedArray_ConstIterator() != *this' and
        // '0 < *this - array.begin()'.

    // ACCESSORS
    const TYPE& operator*() const;
        // Return a 'const' reference to the element referenced by this
        // iterator.  The behavior is undefined unless for this iterator,
        // referencing a 'CompactedArray' 'array',
        // 'CompactedArray_ConstIterator() != *this' and
        // '*this - array.begin() < array.length()'.

    const TYPE& operator->() const;
        // Return a 'const' reference to the element referenced by this
        // iterator.  The behavior is undefined unless for this iterator,
        // referencing a 'CompactedArray' 'array',
        // 'CompactedArray_ConstIterator() != *this' and
        // '*this - array.begin() < array.length()'.

    const TYPE& operator[](bsl::ptrdiff_t offset) const;
        // Return a 'const' reference to the element at the specified 'offset'
        // from the location referenced by this iterator.  The behavior is
        // undefined unless for this iterator, referencing a 'CompactedArray'
        // 'array', 'CompactedArray_ConstIterator() != *this' and
        // '0 <= *this - array.begin() + offset < array.length()'.
};

// FREE OPERATORS
template <class TYPE>
CompactedArray_ConstIterator<TYPE>
                       operator++(CompactedArray_ConstIterator<TYPE>& iterator,
                                  int);
    // Advance the specified 'iterator' to refer to the next location in the
    // referenced array, and return an iterator referring to the original
    // location (*before* the advancement).  The returned iterator, 'it',
    // referencing a 'CompactedArray' 'array', remains valid as long as
    // '0 <= it - array.begin() <= array.length()'.  The behavior is undefined
    // unless, on entry, 'CompactedArray_ConstIterator() != iterator' and
    // 'iterator - array.begin() < array.length()'.

template <class TYPE>
CompactedArray_ConstIterator<TYPE>
                       operator--(CompactedArray_ConstIterator<TYPE>& iterator,
                                  int);
    // Decrement the specified 'iterator' to refer to the previous location in
    // the referenced array, and return an iterator referring to the original
    // location (*before* the decrementation).  The returned iterator, 'it',
    // referencing a 'CompactedArray' 'array', remains valid as long as
    // '0 <= it - array.begin() <= array.length()'.  The behavior is undefined
    // unless, on entry, 'CompactedArray_ConstIterator() != iterator' and
    // '0 < iterator - array.begin()'.

template <class TYPE>
CompactedArray_ConstIterator<TYPE> operator+(
                           const CompactedArray_ConstIterator<TYPE>& iterator,
                           bsl::ptrdiff_t                            offset);
template <class TYPE>
CompactedArray_ConstIterator<TYPE> operator+(
                           bsl::ptrdiff_t                            offset,
                           const CompactedArray_ConstIterator<TYPE>& iterator);
    // Return an iterator referencing the location at the specified 'offset'
    // from the location referenced by the specified 'iterator'.  The returned
    // iterator, 'it', referencing a 'CompactedArray' 'array', remains valid as
    // long as '0 <= it - array.begin() <= array.length()'.  The behavior is
    // undefined unless 'CompactedArray_ConstIterator() != iterator' and
    // '0 <= iterator - array.begin() + offset <= array.length()'.

template <class TYPE>
CompactedArray_ConstIterator<TYPE> operator-(
                            const CompactedArray_ConstIterator<TYPE>& iterator,
                            bsl::ptrdiff_t                            offset);
    // Return an iterator referencing the location at the specified 'offset'
    // from the location referenced by the specified 'iterator'.  The returned
    // iterator, 'it', referencing a 'CompactedArray' 'array', remains valid as
    // long as '0 <= it - array.begin() <= array.length()'.  The behavior is
    // undefined unless 'CompactedArray_ConstIterator() != iterator' and
    // '0 <= iterator - array.begin() - offset <= array.length()'.

template <class TYPE>
bsl::ptrdiff_t operator-(const CompactedArray_ConstIterator<TYPE>& lhs,
                         const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return the number of elements between the specified 'lhs' and 'rhs' as a
    // signed value.  The behavior is undefined unless 'lhs' and 'rhs'
    // reference the same array.  Note that the return value is positive when a
    // positive number of 'rhs++' invocations would result in 'lhs == rhs', and
    // negative when a positive number of 'rhs--' invocations would result in
    // 'lhs == rhs'.

template <class TYPE>
bool operator==(const CompactedArray_ConstIterator<TYPE>& lhs,
                const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two 'CompactedArray_ConstIterator'
    // iterators have the same value if they both have the default value, or
    // neither has the default value and they reference the same location in
    // the same array.

template <class TYPE>
bool operator!=(const CompactedArray_ConstIterator<TYPE>& lhs,
                const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value, and 'false' otherwise.  Two 'CompactedArray_ConstIterator'
    // iterators do not have the same value if one has the default value and
    // the other does not, or neither has the default value and they do not
    // reference the same location in the same array.

template <class TYPE>
bool operator<(const CompactedArray_ConstIterator<TYPE>& lhs,
               const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Iterator 'lhs' has a value less than
    // iterator 'rhs' if '0 < rhs - lhs' (see 'operator-').  The behavior is
    // undefined unless 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator<=(const CompactedArray_ConstIterator<TYPE>& lhs,
                const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value less than or equal to
    // the specified 'rhs, and 'false' otherwise.  Iterator 'lhs' has a value
    // less than or equal to iterator 'rhs' if '0 <= rhs - lhs' (see
    // 'operator-').  The behavior is undefined unless 'lhs' and 'rhs' refer to
    // the same array.

template <class TYPE>
bool operator>(const CompactedArray_ConstIterator<TYPE>& lhs,
               const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater than the
    // specified 'rhs', and 'false' otherwise.  Iterator 'lhs' has a value
    // greater than iterator 'rhs' if '0 > rhs - lhs' (see 'operator-').  The
    // behavior is undefined unless 'lhs' and 'rhs' refer to the same array.

template <class TYPE>
bool operator>=(const CompactedArray_ConstIterator<TYPE>& lhs,
                const CompactedArray_ConstIterator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' has a value greater or equal than
    // the specified 'rhs', and 'false' otherwise.  Iterator 'lhs' has a value
    // greater than or equal to iterator 'rhs' if '0 >= rhs - lhs' (see
    // 'operator-').  The behavior is undefined unless 'lhs' and 'rhs' refer to
    // the same array.

                           // ====================
                           // class CompactedArray
                           // ====================

template <class TYPE>
class CompactedArray {
    // This space-efficient, value-semantic array class represents a sequence
    // of 'TYPE' elements.  The interface provides functionality similar to a
    // 'vector<TYPE>', however, modifiable references to individual elements
    // are not provided.  This class provides accessors that return iterators
    // that provide non-modifiable access to its elements.  The returned
    // iterators, unlike those returned by a 'vector<TYPE>', are *not*
    // invalidated upon reallocation.

    // PRIVATE TYPES
    typedef bsl::vector<CompactedArray_CountedValue<TYPE> > Data;

    // DATA
    Data                        d_data;   // sorted vector of reference-counted
                                          // unique objects

    PackedIntArray<bsl::size_t> d_index;  // array of indices into 'd_data'

    // PRIVATE MANIPULATORS
    void erase(bsl::size_t index);
        // Remove the element in 'd_data' at the specified 'index'.  Update the
        // 'd_index' values to account for this removal.  The behavior is
        // undefined unless 'index < uniqueLength()'.

    bsl::size_t increment(const TYPE& value, bsl::size_t count = 1);
        // Find the element in 'd_data' equal to the specified 'value',
        // increment the element's reference count by the specified 'count',
        // and return the element's index.  If the 'value' is not in 'd_data',
        // insert the element so as to retain sorted order in 'd_data', assign
        // 'count' as the new element's reference count, and return the
        // inserted element's index.  The behavior is undefined unless
        // '0 < count'.

  public:
    // PUBLIC TYPES
    typedef TYPE value_type;  // The type for elements.

    typedef CompactedArray_ConstIterator<TYPE> const_iterator;

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
        // 'value' is not specified, 'TYPE()' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    CompactedArray(const CompactedArray&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a 'CompactedArray' having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~CompactedArray();
        // Destroy this object

    // MANIPULATORS
    CompactedArray& operator=(const CompactedArray& rhs);
        // Assign to this array the value of the specified 'rhs' array, and
        // return a reference providing modifiable access to this array.

    void append(const TYPE& value);
        // Append to this array an element having the specified 'value'.  Note
        // that this method is logically equivalent to:
        //..
        //  push_back(value);
        //..

    void append(const CompactedArray& srcArray);
        // Append to this array the elements from the specified 'srcArray'.
        // Note that if this array and 'srcArray' are the same, the behavior is
        // as if a copy of 'srcArray' were passed.

    void append(const CompactedArray& srcArray,
                bsl::size_t           srcIndex,
                bsl::size_t           numElements);
        // Append to this array the specified 'numElements' starting at the
        // specified 'srcIndex' in the specified 'srcArray'.  The behavior is
        // undefined unless 'srcIndex + numElements <= srcArray.length()'.
        // Note that if this array and 'srcArray' are the same, the behavior is
        // as if a copy of 'srcArray' were passed.

    void insert(bsl::size_t dstIndex, const TYPE& value);
        // Insert into this array, at the specified 'dstIndex', an element
        // having the specified 'value', shifting any elements originally at or
        // above 'dstIndex' up by one index position.  The behavior is
        // undefined unless 'dstIndex <= length()'.

    const_iterator insert(const_iterator dst, const TYPE& value);
        // Insert into this array, at the specified 'dst', an element having
        // the specified 'value', shifting any elements originally at or above
        // 'dst' up by one index position.  Return an iterator to the newly
        // inserted element.  The behavior is undefined unless 'dst' is an
        // iterator over this array.

    void insert(bsl::size_t dstIndex, const CompactedArray& srcArray);
        // Insert into this array, at the specified 'dstIndex', the elements
        // from the specified 'srcArray', shifting any elements originally at
        // or above 'dstIndex' up by 'srcArray.length()' index positions.  The
        // behavior is undefined unless 'dstIndex <= length()'.  Note that if
        // this array and 'srcArray' are the same, the behavior is as if a copy
        // of 'srcArray' were passed.

    void insert(bsl::size_t           dstIndex,
                const CompactedArray& srcArray,
                bsl::size_t           srcIndex,
                bsl::size_t           numElements);
        // Insert into this array, at the specified 'dstIndex', the specified
        // 'numElements' starting at the specified 'srcIndex' in the specified
        // 'srcArray'.  Elements having an index greater than or equal to
        // 'dstIndex' before the insertion are shifted up by 'numElements'
        // index positions.  The behavior is undefined unless
        // 'dstIndex <= length()' and
        // 'srcIndex + numElements <= srcArray.length()'.  Note that if this
        // array and 'srcArray' are the same, the behavior is as if a copy of
        // 'srcArray' were passed.

    void pop_back();
        // Remove the last element from this array.  The behavior is undefined
        // unless '0 < length()'.

    void push_back(const TYPE& value);
        // Append to this array an element having the specified 'value'.

    void remove(bsl::size_t dstIndex);
        // Remove from this array the element at the specified 'dstIndex'.
        // Each element having an index greater than 'dstIndex' before the
        // removal is shifted down by one index position.  The behavior is
        // undefined unless 'dstIndex < length()'.

    void remove(bsl::size_t dstIndex, bsl::size_t numElements);
        // Remove from this array the specified 'numElements' starting at the
        // specified 'dstIndex'.  Each element having an index greater than or
        // equal to 'dstIndex + numElements' before the removal is shifted down
        // by 'numElements' index positions.  The behavior is undefined unless
        // 'dstIndex + numElements <= length()'.

    const_iterator remove(const_iterator dstFirst, const_iterator dstLast);
        // Remove from this array the elements starting at the specified
        // 'dstFirst' iterator up to, but not including, the specified
        // 'dstLast' iterator.  Each element at or above 'dstLast' before the
        // removal is shifted down by 'dstLast - dstFirst' index positions.
        // Return an iterator to the new position of the element that was
        // referred to by 'dstLast', or 'end()' if 'dstLast == end()'.  The
        // behavior is undefined unless 'dstFirst' and 'dstLast' are iterators
        // over this array, and 'dstFirst <= dstLast'.

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
        // Change the values of the specified 'numElements' starting at the
        // specified 'dstIndex' in this array to those of the 'numElements'
        // starting at the specified 'srcIndex' in the specified 'srcArray'.
        // The behavior is undefined unless
        // 'srcIndex + numElements <= srcArray.length()' and
        // 'dstIndex + numElements <= length()'.  Note that if this array and
        // 'srcArray' are the same, the behavior is as if a copy of 'srcArray'
        // were passed.

    void reserveCapacity(bsl::size_t numElements);
        // Make the capacity of this array at least the specified
        // 'numElements', assuming the number of unique elements within this
        // array does not increase.  This method has no effect if the current
        // capacity meets or exceeds the required capacity.  The behavior is
        // undefined unless 'false == isEmpty() || 0 == numElements'.  Note
        // that the assumption of not increasing the number of unique elements
        // implies the need for the narrow contract.

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
        // Return a 'const' reference to the element at the specified 'index'
        // in this array.  The behavior is undefined unless 'index < length()'.

    bslma::Allocator *allocator() const;
        // Return the allocator used by this array to supply memory.

    const TYPE& back() const;
        // Return a 'const' reference to the element at the back of this array.
        // The behavior is undefined unless '0 < length()'.  Note that this
        // method is logically equivalent to:
        //..
        //    operator[](length() - 1)
        //..

    const_iterator begin() const;
        // Return an iterator referring to the first element in this array, or
        // the past-the-end iterator if this array is empty.  The iterator
        // remains valid as long as this array exists.

    bsl::size_t capacity() const;
        // Return the number of elements this array can hold, without
        // reallocation, assuming the number of unique elements within this
        // array does not increase.

    const_iterator end() const;
        // Return the past-the-end iterator for this array.  The iterator
        // remains valid as long as this array exists, and its length does not
        // decrease.

    const TYPE& front() const;
        // Return a 'const' reference to the element at the front of this
        // array.  The behavior is undefined unless '0 < length()'.  Note that
        // this method is logically equivalent to:
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
        // Return the number of elements in this array.

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
        // Return a 'const' reference to the element at the specified 'index'
        // within the sorted sequence of unique element values in this object.
        // The behavior is undefined unless 'index < uniqueLength()'.  Note
        // that 'uniqueElement(index)' and 'operator[](index)' can return
        // different objects.

    bsl::size_t uniqueLength() const;
        // Return the number of unique elements in this array.
};

// FREE OPERATORS
template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const CompactedArray<TYPE>& array);
    // Write the value of the specified 'array' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.

template <class TYPE>
bool operator==(const CompactedArray<TYPE>& lhs,
                const CompactedArray<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays have the same
    // value, and 'false' otherwise.  Two 'CompactedArray' arrays have the same
    // value if they have the same length, and all corresponding elements
    // (those at the same indices) have the same value.

template <class TYPE>
bool operator!=(const CompactedArray<TYPE>& lhs,
                const CompactedArray<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' arrays do not have the
    // same value, and 'false' otherwise.  Two 'CompactedArray' arrays do not
    // have the same value if they do not have the same length, or if any
    // corresponding elements (those at the same indices) do not have the same
    // value.

// FREE FUNCTIONS
template <class TYPE>
void swap(CompactedArray<TYPE>& a, CompactedArray<TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' arrays.
    // This method provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless both arrays were created with the same
    // allocator.

// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
void hashAppend(HASHALG& hashAlg, const CompactedArray<TYPE>& input);
    // Pass the specified 'input' to the specified 'hashAlg'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                  // -------------------------------------
                  // class CompactedArray_RemoveAllProctor
                  // -------------------------------------

// CREATORS
template <class TYPE>
inline
CompactedArray_RemoveAllProctor<TYPE>::CompactedArray_RemoveAllProctor(
                                                   CompactedArray<TYPE> *array)
: d_array_p(array)
{
}

template <class TYPE>
inline
CompactedArray_RemoveAllProctor<TYPE>::~CompactedArray_RemoveAllProctor()
{
    if (d_array_p) {
        d_array_p->removeAll();
    }
}

// MANIPULATORS
template <class TYPE>
inline
void CompactedArray_RemoveAllProctor<TYPE>::release()
{
    d_array_p = 0;
}

                    // ----------------------------------
                    // struct CompactedArray_CountedValue
                    // ----------------------------------

// CREATORS
template <class TYPE>
inline
CompactedArray_CountedValue<TYPE>::CompactedArray_CountedValue(
                                              const TYPE&       value,
                                              bsl::size_t       count,
                                              bslma::Allocator *basicAllocator)
: d_count(count)
{
    BSLS_ASSERT_SAFE(basicAllocator);

    bslma::ConstructionUtil::construct(d_value.address(),
                                       basicAllocator,
                                       value);
}

template <class TYPE>
inline
CompactedArray_CountedValue<TYPE>::CompactedArray_CountedValue(
                      const CompactedArray_CountedValue<TYPE>&  original,
                      bslma::Allocator                         *basicAllocator)
: d_count(original.d_count)
{
    BSLS_ASSERT_SAFE(basicAllocator);

    bslma::ConstructionUtil::construct(d_value.address(),
                                       basicAllocator,
                                       original.d_value.object());
}

template <class TYPE>
inline
CompactedArray_CountedValue<TYPE>::~CompactedArray_CountedValue()
{
    d_value.object().~TYPE();
}

// MANIPULATORS
template <class TYPE>
inline
CompactedArray_CountedValue<TYPE>&
        CompactedArray_CountedValue<TYPE>::operator=(
                                  const CompactedArray_CountedValue<TYPE>& rhs)
{
    d_value.object() = rhs.d_value.object();
    d_count          = rhs.d_count;

    return *this;
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bool bdlc::operator==(const CompactedArray_CountedValue<TYPE>& lhs,
                      const CompactedArray_CountedValue<TYPE>& rhs)
{
    return lhs.d_value.object() == rhs.d_value.object();
}

template <class TYPE>
inline
bool bdlc::operator!=(const CompactedArray_CountedValue<TYPE>& lhs,
                      const CompactedArray_CountedValue<TYPE>& rhs)
{
    return lhs.d_value.object() != rhs.d_value.object();
}

template <class TYPE>
inline
bool bdlc::operator<(const CompactedArray_CountedValue<TYPE>& lhs,
                     const TYPE&                              rhs)
{
    return lhs.d_value.object() < rhs;
}

template <class TYPE>
inline
bool bdlc::operator<(const TYPE&                              lhs,
                     const CompactedArray_CountedValue<TYPE>& rhs)
{
    return lhs < rhs.d_value.object();
}

namespace bdlc {

                    // ----------------------------------
                    // class CompactedArray_ConstIterator
                    // ----------------------------------

// PRIVATE CREATORS
template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>::CompactedArray_ConstIterator(
                                             const CompactedArray<TYPE> *array,
                                             bsl::size_t                 index)
: d_array_p(array)
, d_index(index)
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index <= d_array_p->length());
}

// CREATORS
template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>::CompactedArray_ConstIterator()
: d_array_p(0)
, d_index(0)
{
}

template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>::CompactedArray_ConstIterator(
                                  const CompactedArray_ConstIterator& original)
: d_array_p(original.d_array_p)
, d_index(original.d_index)
{
}

// MANIPULATORS
template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>& CompactedArray_ConstIterator<TYPE>::
                             operator=(const CompactedArray_ConstIterator& rhs)
{
    d_array_p = rhs.d_array_p;
    d_index   = rhs.d_index;
    return *this;
}

template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>&
          CompactedArray_ConstIterator<TYPE>::operator+=(bsl::ptrdiff_t offset)
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset <= d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(   0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index >= bsl::size_t(offset));

    d_index += offset;
    return *this;
}

template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>&
          CompactedArray_ConstIterator<TYPE>::operator-=(bsl::ptrdiff_t offset)
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

template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>&
                               CompactedArray_ConstIterator<TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    ++d_index;
    return *this;
}

template <class TYPE>
inline
CompactedArray_ConstIterator<TYPE>&
                               CompactedArray_ConstIterator<TYPE>::operator--()
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(0 < d_index);

    --d_index;
    return *this;
}

// ACCESSORS
template <class TYPE>
inline
const TYPE& CompactedArray_ConstIterator<TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return (*d_array_p)[d_index];
}

template <class TYPE>
inline
const TYPE& CompactedArray_ConstIterator<TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(d_array_p);
    BSLS_ASSERT_SAFE(d_index < d_array_p->length());

    return (*d_array_p)[d_index];
}

template <class TYPE>
inline
const TYPE& CompactedArray_ConstIterator<TYPE>::
                                        operator[](bsl::ptrdiff_t offset) const
{
    BSLS_ASSERT_SAFE(d_array_p);

    // Assert '0 <= d_index + offset < d_array_p->length()' without risk of
    // overflow.
    BSLS_ASSERT_SAFE(   0 <= offset || d_index >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || d_array_p->length() - d_index > bsl::size_t(offset));

    return (*d_array_p)[d_index + offset];
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bdlc::CompactedArray_ConstIterator<TYPE> bdlc::operator++(
                                  CompactedArray_ConstIterator<TYPE>& iterator,
                                  int)
{
    BSLS_ASSERT_SAFE(iterator.d_array_p);
    BSLS_ASSERT_SAFE(iterator.d_index < iterator.d_array_p->length());

    const CompactedArray_ConstIterator<TYPE> curr = iterator;
    ++iterator;
    return curr;
}

template <class TYPE>
inline
bdlc::CompactedArray_ConstIterator<TYPE> bdlc::operator--(
                                  CompactedArray_ConstIterator<TYPE>& iterator,
                                  int)
{
    BSLS_ASSERT_SAFE(iterator.d_array_p);
    BSLS_ASSERT_SAFE(iterator.d_index > 0);

    const CompactedArray_ConstIterator<TYPE> curr = iterator;
    --iterator;
    return curr;
}

template <class TYPE>
inline
bdlc::CompactedArray_ConstIterator<TYPE> bdlc::operator+(
                            const CompactedArray_ConstIterator<TYPE>& iterator,
                            bsl::ptrdiff_t                            offset)
{
    BSLS_ASSERT_SAFE(iterator.d_array_p);

    // Assert '0 <= iterator.d_index + offset <= iterator.d_array_p->length()'
    // without risk of overflow.
    BSLS_ASSERT_SAFE(   0 <= offset
                     || iterator.d_index              >= bsl::size_t(-offset));
    BSLS_ASSERT_SAFE(   0 >= offset
                     || iterator.d_array_p->length() - iterator.d_index
                                                      >= bsl::size_t( offset));

    return CompactedArray_ConstIterator<TYPE>(iterator.d_array_p,
                                              iterator.d_index + offset);
}

template <class TYPE>
inline
bdlc::CompactedArray_ConstIterator<TYPE> bdlc::operator+(
                            bsl::ptrdiff_t                            offset,
                            const CompactedArray_ConstIterator<TYPE>& iterator)
{
    return iterator + offset;
}

template <class TYPE>
inline
bdlc::CompactedArray_ConstIterator<TYPE> bdlc::operator-(
                            const CompactedArray_ConstIterator<TYPE>& iterator,
                            bsl::ptrdiff_t                            offset)
{
    BSLS_ASSERT_SAFE(iterator.d_array_p);

    // Assert '0 <= iterator.d_index - offset <= iterator.d_array_p->length()'
    // without risk of overflow.
    BSLS_ASSERT_SAFE(   0 >= offset
                     || iterator.d_index              >= bsl::size_t( offset));
    BSLS_ASSERT_SAFE(   0 <= offset
                     || iterator.d_array_p->length() - iterator.d_index
                                                      >= bsl::size_t(-offset));

    return CompactedArray_ConstIterator<TYPE>(iterator.d_array_p,
                                              iterator.d_index - offset);
}

template <class TYPE>
inline
bsl::ptrdiff_t bdlc::operator-(const CompactedArray_ConstIterator<TYPE>& lhs,
                               const CompactedArray_ConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p);
    BSLS_ASSERT_SAFE(rhs.d_array_p);
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
bool bdlc::operator==(const CompactedArray_ConstIterator<TYPE>& lhs,
                      const CompactedArray_ConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p == rhs.d_array_p && lhs.d_index == rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator!=(const CompactedArray_ConstIterator<TYPE>& lhs,
                      const CompactedArray_ConstIterator<TYPE>& rhs)
{
    return lhs.d_array_p != rhs.d_array_p || lhs.d_index != rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator<(const CompactedArray_ConstIterator<TYPE>& lhs,
                     const CompactedArray_ConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p);
    BSLS_ASSERT_SAFE(rhs.d_array_p);
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index < rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator<=(const CompactedArray_ConstIterator<TYPE>& lhs,
                      const CompactedArray_ConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p);
    BSLS_ASSERT_SAFE(rhs.d_array_p);
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index <= rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator>(const CompactedArray_ConstIterator<TYPE>& lhs,
                     const CompactedArray_ConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p);
    BSLS_ASSERT_SAFE(rhs.d_array_p);
    BSLS_ASSERT_SAFE(lhs.d_array_p == rhs.d_array_p);

    return lhs.d_index > rhs.d_index;
}

template <class TYPE>
inline
bool bdlc::operator>=(const CompactedArray_ConstIterator<TYPE>& lhs,
                      const CompactedArray_ConstIterator<TYPE>& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_array_p);
    BSLS_ASSERT_SAFE(rhs.d_array_p);
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
    BSLS_ASSERT(index < uniqueLength());

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
    BSLS_ASSERT(0 < count);

    bsl::size_t index;

    typename Data::iterator iter = bsl::lower_bound(d_data.begin(),
                                                    d_data.end(),
                                                    value);

    if (iter == d_data.end()) {
        index = d_data.size();
        d_data.emplace_back(value, count);
    }
    else if (value < iter->d_value.object()) {
        index = iter - d_data.begin();

        d_data.insert(iter,
                      CompactedArray_CountedValue<TYPE>(
                                          value,
                                          count,
                                          d_data.get_allocator().mechanism()));

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
                                   const CompactedArray<TYPE>&  original,
                                   bslma::Allocator            *basicAllocator)
: d_data(original.d_data, basicAllocator)
, d_index(original.d_index, basicAllocator)
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
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(rhs.length(), rhs.uniqueLength());
        d_data  = rhs.d_data;
        d_index = rhs.d_index;

        proctor.release();
    }

    return *this;
}

template <class TYPE>
void CompactedArray<TYPE>::append(const TYPE& value)
{
    CompactedArray_RemoveAllProctor<TYPE> proctor(this);

    d_index.reserveCapacity(d_index.length() + 1, d_data.size() + 1);

    d_index.push_back(increment(value));

    proctor.release();
}

template <class TYPE>
void CompactedArray<TYPE>::append(const CompactedArray& srcArray)
{
    if (&srcArray != this) {
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(d_index.length() + srcArray.d_index.length(),
                                d_data.size()    + srcArray.d_data.size());

        for (bsl::size_t i = 0; i < srcArray.length(); ++i) {
            d_index.push_back(increment(srcArray[i]));
        }

        proctor.release();
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
    BSLS_ASSERT(numElements <= srcArray.length());
    BSLS_ASSERT(srcIndex    <= srcArray.length() - numElements);

    if (&srcArray != this) {
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(d_index.length() + numElements,
                                d_data.size()    + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_index.push_back(increment(srcArray[srcIndex + i]));
        }

        proctor.release();
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
    BSLS_ASSERT(dstIndex <= d_index.length());

    CompactedArray_RemoveAllProctor<TYPE> proctor(this);

    d_index.reserveCapacity(d_index.length() + 1, d_data.size() + 1);

    d_index.insert(dstIndex, increment(value));

    proctor.release();
}

template <class TYPE>
inline
typename CompactedArray<TYPE>::const_iterator
            CompactedArray<TYPE>::insert(const_iterator dst, const TYPE& value)
{
    BSLS_ASSERT_SAFE(this == dst.d_array_p);

    insert(dst.d_index, value);
    return dst;
}

template <class TYPE>
void CompactedArray<TYPE>::insert(bsl::size_t           dstIndex,
                                  const CompactedArray& srcArray)
{
    BSLS_ASSERT(dstIndex <= d_index.length());

    if (&srcArray != this) {
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(d_index.length() + srcArray.d_index.length(),
                                d_data.size()    + srcArray.d_data.size());

        for (bsl::size_t i = 0; i < srcArray.length(); ++i) {
            d_index.insert(dstIndex + i, increment(srcArray[i]));
        }

        proctor.release();
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
    BSLS_ASSERT(dstIndex <= d_index.length());

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT(numElements <= srcArray.length());
    BSLS_ASSERT(srcIndex    <= srcArray.length() - numElements);

    if (&srcArray != this) {
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(d_index.length() + numElements,
                                d_data.size()    + numElements);

        for (bsl::size_t i = 0; i < numElements; ++i) {
            d_index.insert(dstIndex + i, increment(srcArray[srcIndex + i]));
        }

        proctor.release();
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
    BSLS_ASSERT(!isEmpty());

    bsl::size_t                        dataIndex = d_index.back();
    CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

    d_index.pop_back();

    if (0 == --dataValue.d_count) {
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        erase(dataIndex);

        proctor.release();
    }
}

template <class TYPE>
inline
void CompactedArray<TYPE>::push_back(const TYPE& value)
{
    append(value);
}

template <class TYPE>
inline
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
    BSLS_ASSERT(numElements <= d_index.length());
    BSLS_ASSERT(dstIndex    <= d_index.length() - numElements);

    CompactedArray_RemoveAllProctor<TYPE> proctor(this);

    bsl::size_t endIndex = dstIndex + numElements;
    for (bsl::size_t i = dstIndex; i < endIndex; ++i) {
        bsl::size_t                        dataIndex = d_index[i];
        CompactedArray_CountedValue<TYPE>& dataValue = d_data[dataIndex];

        if (0 == --dataValue.d_count) {
            erase(dataIndex);
        }
    }

    d_index.remove(dstIndex, numElements);

    proctor.release();
}

template <class TYPE>
inline
typename CompactedArray<TYPE>::const_iterator
                          CompactedArray<TYPE>::remove(const_iterator dstFirst,
                                                       const_iterator dstLast)
{
    BSLS_ASSERT_SAFE(this     == dstFirst.d_array_p);
    BSLS_ASSERT_SAFE(this     == dstLast.d_array_p);
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
    BSLS_ASSERT(dstIndex < length());

    CompactedArray_RemoveAllProctor<TYPE> proctor(this);

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

    proctor.release();
}

template <class TYPE>
void CompactedArray<TYPE>::replace(bsl::size_t           dstIndex,
                                   const CompactedArray& srcArray,
                                   bsl::size_t           srcIndex,
                                   bsl::size_t           numElements)
{
    // Assert 'dstIndex + numElements <= length()' without risk of overflow.
    BSLS_ASSERT(numElements <= length());
    BSLS_ASSERT(dstIndex    <= length() - numElements);

    // Assert 'srcIndex + numElements <= srcArray.length()' without risk of
    // overflow.
    BSLS_ASSERT(numElements <= srcArray.length());
    BSLS_ASSERT(srcIndex    <= srcArray.length() - numElements);

    CompactedArray_RemoveAllProctor<TYPE> proctor(this);

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

    proctor.release();
}

template <class TYPE>
void CompactedArray<TYPE>::reserveCapacity(bsl::size_t numElements)
{
    BSLS_ASSERT(false == isEmpty() || 0 == numElements);

    if (0 < numElements) {
        d_index.reserveCapacity(numElements, d_data.size() - 1);
    }
}

template <class TYPE>
void CompactedArray<TYPE>::reserveCapacity(bsl::size_t numElements,
                                           bsl::size_t numUniqueElements)
{
    BSLS_ASSERT(numUniqueElements <= numElements);
    BSLS_ASSERT(0 < numUniqueElements || 0 == numElements);

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
        CompactedArray_RemoveAllProctor<TYPE> proctor(this);

        d_index.reserveCapacity(numElements, d_data.size() + 1);

        bsl::size_t count = numElements - d_index.length();
        bsl::size_t index = increment(TYPE(), count);

        for (bsl::size_t i = 0; i < count; ++i) {
            d_index.push_back(index);
        }

        proctor.release();
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
    BSLS_ASSERT(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_data,  &other.d_data);
    bslalg::SwapUtil::swap(&d_index, &other.d_index);
}

// ACCESSORS
template <class TYPE>
inline
const TYPE& CompactedArray<TYPE>::operator[](bsl::size_t index) const
{
    BSLS_ASSERT_SAFE(index < length());

    return d_data[d_index[index]].d_value.object();
}

template <class TYPE>
inline
bslma::Allocator *CompactedArray<TYPE>::allocator() const
{
    return d_index.allocator();
}

template <class TYPE>
inline
const TYPE& CompactedArray<TYPE>::back() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return operator[](length() - 1);
}

template <class TYPE>
inline
typename CompactedArray<TYPE>::const_iterator
                                            CompactedArray<TYPE>::begin() const
{
    return const_iterator(this, 0);
}

template <class TYPE>
inline
bsl::size_t CompactedArray<TYPE>::capacity() const
{
    return d_index.isEmpty() ? 0 : d_index.capacity();
}

template <class TYPE>
inline
typename CompactedArray<TYPE>::const_iterator CompactedArray<TYPE>::end() const
{
    return const_iterator(this, d_index.length());
}

template <class TYPE>
inline
const TYPE& CompactedArray<TYPE>::front() const
{
    BSLS_ASSERT_SAFE(0 < length());

    return operator[](0);
}

template <class TYPE>
inline
bool CompactedArray<TYPE>::isEmpty() const
{
    return 0 == length();
}

template <class TYPE>
inline
bool CompactedArray<TYPE>::isEqual(const CompactedArray& other) const
{
    return d_index == other.d_index && d_data == other.d_data;
}

template <class TYPE>
inline
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
        printer.printValue(d_data[d_index[i]].d_value.object());
    }
    printer.end();

    return stream;
}

template <class TYPE>
inline
const TYPE& CompactedArray<TYPE>::uniqueElement(bsl::size_t index) const
{
    BSLS_ASSERT_SAFE(index < uniqueLength());

    return d_data[index].d_value.object();
}

template <class TYPE>
inline
bsl::size_t CompactedArray<TYPE>::uniqueLength() const
{
    return d_data.size();
}

}  // close package namespace

// FREE OPERATORS
template <class TYPE>
inline
bsl::ostream& bdlc::operator<<(bsl::ostream&               stream,
                               const CompactedArray<TYPE>& array)
{
    return array.print(stream, 0, -1);
}

template <class TYPE>
inline
bool bdlc::operator==(const CompactedArray<TYPE>& lhs,
                      const CompactedArray<TYPE>& rhs)
{
    return lhs.isEqual(rhs);
}

template <class TYPE>
inline
bool bdlc::operator!=(const CompactedArray<TYPE>& lhs,
                      const CompactedArray<TYPE>& rhs)
{
    return !lhs.isEqual(rhs);
}

// FREE FUNCTIONS
template <class TYPE>
inline
void bdlc::swap(CompactedArray<TYPE>& a, CompactedArray<TYPE>& b)
{
    BSLS_ASSERT_SAFE(a.allocator() == b.allocator());

    a.swap(b);
}

// HASH SPECIALIZATIONS
template <class HASHALG, class TYPE>
void bdlc::hashAppend(HASHALG& hashAlg, const CompactedArray<TYPE>& input)
{
    using ::BloombergLP::bslh::hashAppend;
    typedef typename CompactedArray<TYPE>::const_iterator citer;
    hashAppend(hashAlg, input.length());
    for (citer b = input.begin(), e = input.end(); b != e; ++b) {
        hashAppend(hashAlg, *b);
    }
}

}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator<bdlc::CompactedArray_CountedValue<TYPE> >
                                                           : bsl::true_type {};

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
