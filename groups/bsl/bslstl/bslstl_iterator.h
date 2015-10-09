// bslstl_iterator.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_ITERATOR
#define INCLUDED_BSLSTL_ITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide basic iterator traits, adaptors, and utilities.
//
//@CLASSES:
//  bsl::iterator_traits: information about iterator associated types
//  bsl::reverse_iterator: bring in 'std::reverse_iterator'
//  bsl::distance: global function to calculate iterator distance
//
//@SEE_ALSO: bslstl_forwarditerator, bslstl_bidirectionaliterator,
//           bslstl_randomaccessiterator, C++ Standard
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_iterator.h>' directly.  This component provides the facilities of the
// iterators library from the C++ Standard, including iterator primitives
// (24.4), iterator adaptors (24.5), and stream iterators (24.6).
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Using Iterators to Traverse a Container
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use the 'bsl::iterator' and 'bsl::reverse_iterator'
// to traverse an iterable container type.
//
// Suppose that we have an iterable container template type 'MyFixedSizeArray'.
// An instantiation of 'MyFixedSizeArray' represents an array having fixed
// number of elements, which is a parameter passed to the class constructor
// during construction.  A traversal of 'MyFixedSizeArray' can be accomplished
// using basic iterators (pointers) as well as reverse iterators.
//
// First, we create a elided definition of the template container class,
// 'MyFixedSizeArray', which provides mutable and constant iterators of
// template type 'bsl::iterator' and 'reverse_iterator':
//..
//  template <class VALUE, int SIZE>
//  class MyFixedSizeArray
//      // This is a container that contains a fixed number of elements.  The
//      // number of elements is specified upon construction and can not be
//      // changed afterwards.
//  {
//      // DATA
//      VALUE  d_array[SIZE];  // storage of the container
//
//    public:
//      // PUBLIC TYPES
//      typedef VALUE value_type;
//..
// Here, we define mutable and constant iterators and reverse iterators:
//..
//      typedef VALUE                                  *iterator;
//      typedef VALUE const                            *const_iterator;
//      typedef bsl::reverse_iterator<iterator>         reverse_iterator;
//      typedef bsl::reverse_iterator<const_iterator>   const_reverse_iterator;
//
//      // CREATORS
//      //! MyFixedSizeArray() = default;
//          // Create a 'MyFixedSizeArray' object having the parameterized
//          // 'SIZE' elements of the parameterized type 'VALUE'.
//
//      //! MyFixedSizeArray(const MyFixedSizeArray& original) = default;
//          // Create a 'MyFixedSizeArray' object having same number of
//          // elements as that of the specified 'rhs', and the same value of
//          // each element as that of corresponding element in 'rhs'.
//
//      //! ~MyFixedSizeArray() = default;
//          // Destroy this object.
//..
// Now, we define the 'begin' and 'end' methods to return basic iterators
// ('VALUE*' and 'const VALUE*'), and the 'rbegin' and 'rend' methods to return
// reverse iterators ('bsl::reverse_iterator<VALUE*>' and
// 'bsl::reverse_iterator<const VALUE*>) type:
//..
//      // MANIPULATORS
//      iterator begin();
//          // Return the basic iterator providing modifiable access to the
//          // first valid element of this object.
//
//      iterator end();
//          // Return the basic iterator providing modifiable access to the
//          // position one after the last valid element of this object.
//
//      reverse_iterator rbegin();
//          // Return the reverse iterator providing modifiable access to the
//          // last valid element of this object.
//
//      reverse_iterator rend();
//          // Return the reverse iterator providing modifiable access to the
//          // position one before the first valid element of this object.
//
//      VALUE& operator[](int i);
//          // Return the reference providing modifiable access of the
//          // specified 'i'th element of this object.
//
//      // ACCESSORS
//      const_iterator begin() const;
//          // Return the basic iterator providing non-modifiable access to the
//          // first valid element of this object.
//
//      const_iterator end() const;
//          // Return the basic iterator providing non-modifiable access to the
//          // position one after the last valid element of this object.
//
//      const_reverse_iterator rbegin() const;
//          // Return the reverse iterator providing non-modifiable access to
//          // the last valid element of this object.
//
//      const_reverse_iterator rend() const;
//          // Return the reverse iterator providing non-modifiable access to
//          // the position one before the first valid element of this object.
//
//      int size() const;
//          // Return the number of elements contained in this object.
//
//      const VALUE& operator[](int i) const;
//          // Return the reference providing non-modifiable access of the
//          // specified 'i'th element of this object.
//  };
//
//  // ...
//..
// Then, we create a 'MyFixedSizeArray' and initialize its elements:
//..
//  // Create a fixed array having five elements.
//
//  MyFixedSizeArray<int, 5> fixedArray;
//
//  // Initialize the values of each element in the fixed array.
//
//  for (int i = 0; i < fixedArray.size(); ++i) {
//      fixedArray[i] = i + 1;
//  }
//..
// Now, we generate reverse iterators using the 'rbegin' and 'rend' methods of
// the fixed array object:
//..
//  MyFixedSizeArray<int, 5>::reverse_iterator rstart  = fixedArray.rbegin();
//  MyFixedSizeArray<int, 5>::reverse_iterator rfinish = fixedArray.rend();
//..
// Finally, we traverse the fixed array again in reverse order using the two
// generated reverse iterators:
//..
//  printf("Traverse array using reverse iterator:\n");
//  while (rstart != rfinish) {
//      printf("\tElement: %d\n", *rstart);
//      ++rstart;
//  }
//..
// The preceding loop produces the following output on 'stdout':
//..
//  Traverse array using reverse iterator:
//       Element: 5
//       Element: 4
//       Element: 3
//       Element: 2
//       Element: 1
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_iterator.h> instead of <bslstl_iterator.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace bsl {

// 24.3 primitives
using native_std::input_iterator_tag;
using native_std::output_iterator_tag;
using native_std::forward_iterator_tag;
using native_std::bidirectional_iterator_tag;
using native_std::random_access_iterator_tag;
using native_std::iterator;

// 24.3.4 iterator operations
using native_std::advance;

// 24.3.4 predefined iterators
using native_std::back_insert_iterator;
using native_std::back_inserter;
using native_std::front_insert_iterator;
using native_std::front_inserter;
using native_std::insert_iterator;
using native_std::inserter;

// 24.5 stream iterators
using native_std::istream_iterator;
using native_std::ostream_iterator;
using native_std::istreambuf_iterator;
using native_std::ostreambuf_iterator;

#if defined(BSLS_PLATFORM_CMP_SUN) && !defined(BDE_BUILD_TARGET_STLPORT)

// Sun does not provide 'std::iterator_traits' at all.  We will provide our own
// in namespace 'bsl'.

                        // =========================
                        // class bsl::IteratorTraits
                        // =========================

template <class ITER>
struct iterator_traits {
    // This 'struct' will provide access to iterator traits.

    // TYPES
    typedef typename ITER::iterator_category iterator_category;
    typedef typename ITER::value_type        value_type;
    typedef typename ITER::difference_type   difference_type;
    typedef typename ITER::pointer           pointer;
    typedef typename ITER::reference         reference;
};

// SPECIALIZATIONS
template <class TYPE>
struct iterator_traits<const TYPE *> {
    // This specialization of 'iterator_traits' will match pointer types to a
    // parameterized non-modifiable 'TYPE'.

    // TYPES
    typedef std::random_access_iterator_tag iterator_category;
    typedef TYPE                            value_type;
    typedef std::ptrdiff_t                  difference_type;
    typedef const TYPE*                     pointer;
    typedef const TYPE&                     reference;
};

template <class TYPE>
struct iterator_traits<TYPE *> {
    // This specialization of 'iterator_traits' will match pointer types to a
    // parameterized modifiable 'TYPE'.

    // TYPES
    typedef std::random_access_iterator_tag iterator_category;
    typedef TYPE                            value_type;
    typedef std::ptrdiff_t                  difference_type;
    typedef TYPE*                           pointer;
    typedef TYPE&                           reference;
};

// Working around a sun compiler bug where 'std::reverse_iterator' takes 6
// (with 3 default) template arguments instead of 1, which is not standard
// compliant.  Inherit from 'std::reverse_iterator'.  For reference, the
// signature of sun's 'std::reverse_iterator' is:
//..
//  template <class Iterator,
//            class Category,
//            class T,
//            class Reference = T &,
//            class Pointer = T *,
//            class Distance = ptrdiff_t>
//  class reverse_iterator;
//..

                        // ===========================
                        // class bsl::reverse_iterator
                        // ===========================

template <class ITER>
class reverse_iterator :
    public native_std::reverse_iterator<
                             ITER,
                             typename iterator_traits<ITER>::iterator_category,
                             typename iterator_traits<ITER>::value_type,
                             typename iterator_traits<ITER>::reference,
                             typename iterator_traits<ITER>::pointer> {
    // This class provides a template iterator adaptor that iterates from the
    // end of the sequence defined by the (template parameter) type 'ITER' to
    // the beginning of that sequence.  The type 'ITER' shall meet all the
    // requirements of a bidirectional iterator [24.2.6].  The element sequence
    // generated in this reversed iteration is referred as "reverse iteration
    // sequence" in the following class level documentation.  The fundamental
    // relation between a reverse iterator and its corresponding iterator 'i'
    // of type 'ITER' is established by the identity
    // '&*(reverse_iterator(i)) == &*(i - 1)'.  This template meets the
    // requirement of reverse iterator adaptor defined in C++11 standard
    // [24.5.1].

    // PRIVATE TYPES
    typedef native_std::reverse_iterator<
                 ITER,
                 typename iterator_traits<ITER>::iterator_category,
                 typename iterator_traits<ITER>::value_type,
                 typename iterator_traits<ITER>::reference,
                 typename iterator_traits<ITER>::pointer>                 Base;

  public:
    // For convenience:

    typedef typename reverse_iterator::difference_type difference_type;

    // CREATORS
    reverse_iterator();
        // Create the default value for this reverse iterator.  The
        // default-constructed reverse iterator does not have a singular value
        // unless an object of the type specified by the template parameter
        // 'ITER' has a singular value after default construction.

    explicit reverse_iterator(ITER base);
        // Create a reverse iterator using the specified 'base' of the
        // (template parameter) type 'ITER'.

    template <class OTHER_ITER>
    reverse_iterator(const reverse_iterator<OTHER_ITER>& original);
        // Create a reverse iterator having the same value as the specified
        // 'original'.

    // MANIPULATORS
    reverse_iterator& operator++();
        // Increment to the next element in the reverse iteration sequence and
        // return a reference providing modifiable access to this reverse
        // iterator.  The behavior is undefined if, on entry, this reverse
        // iterator has the past-the-end value for a reverse iterator over the
        // underlying sequence.

    reverse_iterator  operator++(int);
        // Increment to the next element in the reverse iteration sequence and
        // return a reverse iterator having the pre-increment value of this
        // reverse iterator.  The behavior is undefined if, on entry, this
        // reverse iterator has the past-the-end value for a reverse iterator
        // over the underlying sequence.

    reverse_iterator& operator+=(difference_type n);
        // Increment by the specified 'n' number of elements in the reverse
        // iteration sequence and return a reference providing modifiable
        // access to this reverse iterator.  The behavior is undefined unless
        // this reverse iterator, after incrementing by 'n', is within the
        // bounds of the underlying sequence.  Note that the (template
        // parameter) type 'ITER' shall meet the requirements of a random
        // access iterator.

    reverse_iterator& operator--();
        // Decrement to the previous element in the reverse iteration sequence
        // and return a reference providing modifiable access to this reverse
        // iterator.  The behavior is undefined if, on entry, this reverse
        // iterator has the same value as a reverse iterator to the start of
        // the underlying sequence.

    reverse_iterator  operator--(int);
        // Decrement to the previous element in the reverse iteration sequence
        // and return a reverse iterator having the pre-decrement value of this
        // reverse iterator.  The behavior is undefined if, on entry, this
        // reverse iterator has the same value as a reverse iterator to the
        // start of the underlying sequence.

    reverse_iterator& operator-=(difference_type n);
        // Decrement by the specified 'n' number of elements in the reverse
        // iteration sequence and return a reference providing modifiable
        // access to this reverse iterator.  The behavior is undefined unless
        // this reverse iterator, after decrementing by 'n', is within the
        // bounds of the underlying sequence.  Note that the (template
        // parameter) type 'ITER' shall meet the requirements of a random
        // access iterator.

    // ACCESSORS
    reverse_iterator operator+(difference_type n) const;
        // Return a reverse iterator having the same value as that of
        // incrementing this reverse iterator by the specified 'n' number of
        // elements in the reverse iteration sequence.  The behavior is
        // undefined unless this reverse iterator, if increments by 'n', would
        // be within the bounds of the underlying sequence.  Note that the
        // (template parameter) type 'ITER' shall meet the requirements of a
        // random access iterator.

    reverse_iterator operator-(difference_type n) const;
        // Return a reverse iterator having the same value as that of
        // decrementing this reverse iterator by the specified 'n' number of
        // elements in the reverse iteration sequence.  The behavior is
        // undefined unless this reverse iterator, if decrements by 'n', would
        // be within the bounds of the underlying sequence.  Note that the
        // (template parameter) type 'ITER' shall meet the requirements of a
        // random access iterator.
};

// FREE OPERATORS
template <class ITER>
inline
bool operator==(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator has the same value
    // as the specified 'rhs' reverse iterator, and 'false' otherwise.  Two
    // reverse iterators have the same value if they refer to the same element,
    // or both have the past-the-end value for a reverse iterator over the
    // underlying reverse iteration sequence.  The behavior is undefined unless
    // both reverse iterators refer to the same underlying sequence.

template <class ITER1, class ITER2>
inline
bool operator==(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator of the (template
    // parameter) type 'ITER1' has the same value as the specified 'rhs'
    // reverse iterator of the (template parameter) type 'ITER2', and 'false'
    // otherwise.  Two reverse iterators have the same value if they refer to
    // the same element, or both have the past-the-end value for a reverse
    // iterator over the underlying reverse iteration sequence.  The behavior
    // is undefined unless both reverse iterators refer to the same underlying
    // sequence.

template <class ITER>
inline
bool operator!=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator does not have the
    // same value as the specified 'rhs' reverse iterator, and 'false'
    // otherwise.  Two reverse iterators do not have the same value if (1) they
    // do not refer to the same element and (2) both do not have the
    // past-the-end value for a reverse iterator over the underlying reverse
    // iteration sequence.  The behavior is undefined unless both reverse
    // iterators refer to the same underlying sequence.

template <class ITER1, class ITER2>
inline
bool operator!=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs);
    // Return 'true' if the specified 'lhs' reverse iterator of the (template
    // parameter) type 'ITER1' does not have the same value as the specified
    // 'rhs' reverse iterator of the (template parameter) type 'ITER2', and
    // 'false' otherwise.  Two reverse iterators do not have the same value if
    // (1) they do not refer to the same element and (2) both do not have the
    // past-the-end value for a reverse iterator over the underlying reverse
    // iteration sequence.  The behavior is undefined unless both reverse
    // iterators refer to the same underlying sequence.

template <class ITER>
inline
bool operator<(const reverse_iterator<ITER>& lhs,
               const reverse_iterator<ITER>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator refers to an
    // element before the specified 'rhs' reverse iterator in the reverse
    // iteration sequence, or (2) 'rhs' (and not 'lhs') has the past-the-end
    // value for a reverse iterator over this sequence, and 'false' otherwise.
    // The behavior is undefined unless both reverse iterators refer to the
    // same underlying sequence.  Note that the (template parameter) type
    // 'ITER' shall meet the requirements of random access iterator.

template <class ITER1, class ITER2>
inline
bool operator<(const reverse_iterator<ITER1>& lhs,
               const reverse_iterator<ITER2>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator of the
    // (template parameter) type 'ITER1' refers to an element before the
    // specified 'rhs' reverse iterator of the (template parameter) type
    // 'ITER2' in the reverse iteration sequence, or (2) 'rhs' (and not 'lhs')
    // has the past-the-end value for a reverse iterator over this sequence,
    // and 'false' otherwise.  The behavior is undefined unless both reverse
    // iterators refer to the same underlying sequence.  Note that both 'ITER1'
    // and 'ITER2' shall meet the requirements of random access iterator.

template <class ITER>
inline
bool operator>(const reverse_iterator<ITER>& lhs,
               const reverse_iterator<ITER>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator refers to an
    // element after the specified 'rhs' reverse iterator in the reverse
    // iteration sequence, or (2) 'lhs' (and not 'rhs') has the past-the-front
    // value of an reverse iterator over this sequence, and 'false' otherwise.
    // The behavior is undefined unless both reverse iterators refer to the
    // same underlying sequence.  Note that the (template parameter) type
    // 'ITER' shall meet the requirements of random access iterator.

template <class ITER1, class ITER2>
inline
bool operator>(const reverse_iterator<ITER1>& lhs,
               const reverse_iterator<ITER2>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator of the
    // (template parameter) type 'ITER1' refers to an element after the
    // specified 'rhs' reverse iterator of the (template parameter) type
    // 'ITER2' in the reverse iteration sequence, or (2) 'lhs' (and not 'rhs')
    // has the past-the-front value of an reverse iterator over this sequence,
    // and 'false' otherwise.  The behavior is undefined unless both reverse
    // iterators refer to the same underlying sequence.  Note that both 'ITER1'
    // and 'ITER2' shall meet the requirements of random access iterator.

template <class ITER>
inline
bool operator<=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator has the same
    // value as the specified 'rhs' reverse iterator, or (2) 'lhs' refers to an
    // element before 'rhs' in the reverse iteration sequence, or (3) 'rhs' has
    // the past-the-end value for a reverse iterator over this sequence, and
    // 'false' otherwise.  The behavior is undefined unless both reverse
    // iterators refer to the same underlying sequence.  Note that the
    // (template parameter) type 'ITER' shall meet the requirements of a random
    // access iterator.

template <class ITER1, class ITER2>
inline
bool operator<=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator of the
    // (template parameter) type 'ITER1' has the same value as the specified
    // 'rhs' reverse iterator of the (template parameter) type 'ITER2', or (2)
    // 'lhs' refers to an element before 'rhs' in the reverse iteration
    // sequence, or (3) 'rhs' has the past-the-end value for a reverse iterator
    // over this sequence, and 'false' otherwise.  The behavior is undefined
    // unless both reverse iterators refer to the same underlying sequence.
    // Note that both 'ITER1' and 'ITER2' shall meet the requirements of a
    // random access iterator.

template <class ITER>
inline
bool operator>=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator has the same
    // value as the specified 'rhs' reverse iterator, or (2) 'lhs' has the
    // past-the-end value for a reverse iterator over the underlying reverse
    // iteration sequence, or (3) 'lhs' refers to an element after 'rhs' in
    // this sequence, and 'false' otherwise.  The behavior is undefined unless
    // both reverse iterators refer to the same underlying sequence.  Note that
    // the (template parameter) type 'ITER' shall meet the requirements of
    // random access iterator.

template <class ITER1, class ITER2>
inline
bool operator>=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs);
    // Return 'true' if (1) the specified 'lhs' reverse iterator of the
    // (template parameter) type 'ITER1' has the same value as the specified
    // 'rhs' reverse iterator of the (template parameter) type 'ITER2', or (2)
    // 'lhs' has the past-the-end value for a reverse iterator over the
    // underlying reverse iteration sequence, or (3) 'lhs' refers to an element
    // after 'rhs' in this sequence, and 'false' otherwise.  The behavior is
    // undefined unless both reverse iterators refer to the same underlying
    // sequence.  Note that both type 'ITER1' and type 'ITER2' shall meet the
    // requirements of random access iterator.

template <class ITER>
inline
typename reverse_iterator<ITER>::difference_type
operator-(const reverse_iterator<ITER>& lhs,
          const reverse_iterator<ITER>& rhs);
    // Return the distance from the specified 'rhs' reverse iterator to the
    // specified 'lhs' reverse iterator.  The behavior is undefined unless
    // 'lhs' and 'rhs' are reverse iterators into the same underlying sequence.
    // Note that the (template parameter) type 'ITER' shall meet the
    // requirements of random access iterator.  Also note that the result might
    // be negative.

template <class ITER, class DIFF_TYPE>
inline
reverse_iterator<ITER>
operator+(DIFF_TYPE n, const reverse_iterator<ITER>& rhs);
    // Return a reverse iterator to the element at the specified 'n' positions
    // past the specified 'rhs' reverse iterator.  The behavior is undefined
    // unless 'rhs', after incrementing by 'n', is within the bounds of the
    // underlying sequence.  Note that the (template parameter) type 'ITER'
    // shall meet the requirements of random access iterator.

                        // ==========================
                        // struct IteratorDistanceImp
                        // ==========================

struct IteratorDistanceImp {
    // This utility class provides a namespace for functions that operate on
    // iterators.

    template <class FWD_ITER, class DIFFERENCE_TYPE>
    static void getDistance(DIFFERENCE_TYPE *ret,
                            FWD_ITER         start,
                            FWD_ITER         finish,
                            input_iterator_tag);
        // Return in the specified '*ret' the distance from the specified
        // 'start' iterator to the specified 'finish' iterator.  The behavior
        // is undefined unless 'start' and 'finish' both have the
        // 'input_iterator_tag' into the same underlying sequence, and 'start'
        // is before 'finish' in that sequence.

    template <class FWD_ITER, class DIFFERENCE_TYPE>
    static void getDistance(DIFFERENCE_TYPE *ret,
                            FWD_ITER         start,
                            FWD_ITER         finish,
                            forward_iterator_tag);
        // Return in the specified '*ret' the distance from the specified
        // 'start' iterator to the specified 'finish' iterator.  The behavior
        // is undefined unless 'start' and 'finish' both have the
        // 'forward_iterator_tag' into the same underlying sequence, and
        // 'start' is before 'finish' in that sequence.

    template <class RANDOM_ITER, class DIFFERENCE_TYPE>
    static void getDistance(DIFFERENCE_TYPE *ret,
                            RANDOM_ITER      start,
                            RANDOM_ITER      finish,
                            random_access_iterator_tag);
        // Return in the specified '*ret' the distance from the specified
        // 'start' iterator to the specified 'finish' iterator.  The behavior
        // is undefined unless 'start' and 'finish' both have the
        // 'random_access_iterator_tag' into the same underlying sequence.
        // Note that the result might be negative.
};

template <class ITER>
typename iterator_traits<ITER>::difference_type
distance(ITER start, ITER finish);
    // Return the distance from the specified 'start' iterator to the specified
    // 'finish' iterator.  The behavior is undefined unless 'start' and
    // 'finish' are both into the same underlying sequence, and 'start' is
    // before 'finish' in that sequence.  Note that the (template parameter)
    // type 'ITER' shall meet the requirements of input iterator.
#else

// Just use the native version
using native_std::reverse_iterator;
using native_std::iterator_traits;
using native_std::distance;

#endif

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class bsl::reverse_iterator
                        // ---------------------------

#if defined(BSLS_PLATFORM_CMP_SUN) && !defined(BDE_BUILD_TARGET_STLPORT)

// CREATORS
template <class ITER>
inline
reverse_iterator<ITER>::reverse_iterator()
: Base()
{
}

template <class ITER>
inline
reverse_iterator<ITER>::reverse_iterator(ITER base)
: Base(base)
{
}

template <class ITER>
template <class OTHER_ITER>
inline
reverse_iterator<ITER>::reverse_iterator(
                                  const reverse_iterator<OTHER_ITER>& original)
: Base(original.base())
{
}

// MANIPULATORS
template <class ITER>
inline
reverse_iterator<ITER>&
reverse_iterator<ITER>::operator++()
{
    Base::operator++();
    return *this;
}

template <class ITER>
inline
reverse_iterator<ITER>
reverse_iterator<ITER>::operator++(int)
{
    const reverse_iterator tmp(*this);
    this->operator++();
    return tmp;
}

template <class ITER>
inline
reverse_iterator<ITER>&
reverse_iterator<ITER>::operator+=(difference_type n)
{
    Base::operator+=(n);
    return *this;
}

template <class ITER>
inline
reverse_iterator<ITER>&
reverse_iterator<ITER>::operator--()
{
    Base::operator--();
    return *this;
}

template <class ITER>
inline
reverse_iterator<ITER>
reverse_iterator<ITER>::operator--(int)
{
    reverse_iterator tmp(*this);
    this->operator--();
    return tmp;
}

template <class ITER>
inline
reverse_iterator<ITER>&
reverse_iterator<ITER>::operator-=(difference_type n)
{
    Base::operator-=(n);
    return *this;
}

// ACCESSORS
template <class ITER>
inline
reverse_iterator<ITER>
reverse_iterator<ITER>::operator+(difference_type n) const
{
    reverse_iterator tmp(*this);
    tmp += n;
    return tmp;
}

template <class ITER>
inline
reverse_iterator<ITER>
reverse_iterator<ITER>::operator-(difference_type n) const
{
    reverse_iterator tmp(*this);
    tmp -= n;
    return tmp;
}

// FREE OPERATORS
template <class ITER>
inline
bool operator==(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs)
{
    typedef native_std::reverse_iterator<
                 ITER,
                 typename iterator_traits<ITER>::iterator_category,
                 typename iterator_traits<ITER>::value_type,
                 typename iterator_traits<ITER>::reference,
                 typename iterator_traits<ITER>::pointer>                 Base;

    return std::operator==(static_cast<const Base&>(lhs),
                           static_cast<const Base&>(rhs));
}

template <class ITER1, class ITER2>
inline
bool operator==(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs)
{
    // this is to compare reverse_iterator with const_reverse_iterator

    return lhs.base() == rhs.base();
}

template <class ITER>
inline
bool operator!=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs)
{
    return ! (lhs == rhs);
}

template <class ITER1, class ITER2>
inline
bool operator!=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs)
{
    // this is to compare reverse_iterator with const_reverse_iterator

    return ! (lhs == rhs);
}

template <class ITER>
inline
bool operator<(const reverse_iterator<ITER>& lhs,
               const reverse_iterator<ITER>& rhs)
{
    typedef native_std::reverse_iterator<
                 ITER,
                 typename iterator_traits<ITER>::iterator_category,
                 typename iterator_traits<ITER>::value_type,
                 typename iterator_traits<ITER>::reference,
                 typename iterator_traits<ITER>::pointer>                 Base;

    return std::operator<(static_cast<const Base&>(lhs),
                          static_cast<const Base&>(rhs));
}

template <class ITER1, class ITER2>
inline
bool operator<(const reverse_iterator<ITER1>& lhs,
               const reverse_iterator<ITER2>& rhs)
{
    // this is to compare reverse_iterator with const_reverse_iterator

    return lhs.base() < rhs.base();
}

template <class ITER>
inline
bool operator>(const reverse_iterator<ITER>& lhs,
               const reverse_iterator<ITER>& rhs)
{
    return rhs < lhs;
}

template <class ITER1, class ITER2>
inline
bool operator>(const reverse_iterator<ITER1>& lhs,
               const reverse_iterator<ITER2>& rhs)
{
    return rhs < lhs;
}

template <class ITER>
inline
bool operator<=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs)
{
    return !(rhs < lhs);
}

template <class ITER1, class ITER2>
inline
bool operator<=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs)
{
    return !(rhs < lhs);
}

template <class ITER>
inline
bool operator>=(const reverse_iterator<ITER>& lhs,
                const reverse_iterator<ITER>& rhs)
{
    return !(lhs < rhs);
}

template <class ITER1, class ITER2>
inline
bool operator>=(const reverse_iterator<ITER1>& lhs,
                const reverse_iterator<ITER2>& rhs)
{
    return !(lhs < rhs);
}

template <class ITER>
inline
typename reverse_iterator<ITER>::difference_type
operator-(const reverse_iterator<ITER>& lhs,
          const reverse_iterator<ITER>& rhs)
{
    typedef native_std::reverse_iterator<
                 ITER,
                 typename iterator_traits<ITER>::iterator_category,
                 typename iterator_traits<ITER>::value_type,
                 typename iterator_traits<ITER>::reference,
                 typename iterator_traits<ITER>::pointer>                 Base;

    return std::operator-(static_cast<const Base&>(lhs),
                          static_cast<const Base&>(rhs));
}

template <class ITER, class DIFF_TYPE>
inline
reverse_iterator<ITER>
operator+(DIFF_TYPE n, const reverse_iterator<ITER>& rhs)
{
    return rhs.operator+(n);
}

                         // --------------------------
                         // struct IteratorDistanceImp
                         // --------------------------

template <class FWD_ITER, class DIFFERENCE_TYPE>
void IteratorDistanceImp::getDistance(DIFFERENCE_TYPE *ret,
                                      FWD_ITER         start,
                                      FWD_ITER         finish,
                                      input_iterator_tag)
{
    DIFFERENCE_TYPE count = 0;
    for ( ; start != finish; ++start) {
        ++count;
    }

    *ret = count;
}

template <class FWD_ITER, class DIFFERENCE_TYPE>
void IteratorDistanceImp::getDistance(DIFFERENCE_TYPE *ret,
                                      FWD_ITER         start,
                                      FWD_ITER         finish,
                                      forward_iterator_tag)
{
    DIFFERENCE_TYPE count = 0;
    for ( ; start != finish; ++start) {
        ++count;
    }

    *ret = count;
}

template <class RANDOM_ITER, class DIFFERENCE_TYPE>
inline
void IteratorDistanceImp::getDistance(DIFFERENCE_TYPE *ret,
                                      RANDOM_ITER      start,
                                      RANDOM_ITER      finish,
                                      random_access_iterator_tag)
{
    *ret = DIFFERENCE_TYPE(finish - start);
}

template <class ITER>
inline
typename iterator_traits<ITER>::difference_type
distance(ITER start, ITER finish)
{
    typedef typename bsl::iterator_traits<ITER>::iterator_category tag;

    typename iterator_traits<ITER>::difference_type ret;
    IteratorDistanceImp::getDistance(&ret, start, finish, tag());
    return ret;
}

#endif  // BSLS_PLATFORM_CMP_SUN && !BDE_BUILD_TARGET_STLPORT

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
