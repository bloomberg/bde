// bslstl_randomaccessiterator.h                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_RANDOMACCESSITERATOR
#define INCLUDED_BSLSTL_RANDOMACCESSITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template to create STL-compliant random access iterators.
//
//@CLASSES:
//  bslstl_RandomAccessIterator: random-access iterator template
//
//@SEE_ALSO: bslstl_iterator, bslstl_forwarditerator,
//           bslstl_bidirectionaliterator
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim),
//         Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides an iterator adaptor that, given an
// implementation class defining a core set of iterator functionality, adapts
// it to provide an STL-compliant random access iterator interface.  The set of
// requirements for a random access iterator is found in "Table 108: Random
// access iterator requirements", under the tag "[random.access.iterators]".
// (Note that this reference is sourced in N3092, a C++0x working paper; the
// actual table number may be different in the actual standard.)
//
///Usage
///-----
// Given the following "iterator-like" implementation class:
//..
//  template <class T>
//  class my_IteratorImp {
//    public:
//      // CREATORS
//      my_IteratorImp();
//      my_IteratorImp(const my_IteratorImp&);
//      ~my_IteratorImp();
//
//      // An additional value-constructor should be supplied that can be
//      // called by the unspecified container type, providing access to the
//      // container's internal data structure that is to be iterated over.
//      // This would typically be called by 'begin' and 'end'.
//
//      // MANIPULATORS
//      my_IteratorImp& operator=(const my_IteratorImp&);
//
//      void operator++();
//      void operator--();
//
//      void operator+=(int);
//
//      // ACCESSORS
//      T& operator*() const;
//  };
//
//  template <class T>
//  bool operator==(const my_IteratorImp<T>& lhs,
//                  const my_IteratorImp<T>& rhs);
//
//  template <class T>
//  std::ptrdiff_t operator-(const my_IteratorImp<T>& lhs,
//                           const my_IteratorImp<T>& rhs);
//
//  template <class T>
//  bool operator<(const my_IteratorImp<T>& lhs,
//                 const my_IteratorImp<T>& rhs);
//..
// simply add the following two 'typedef's to any container class that provides
// 'my_IteratorImp<T>' access, and the container will have STL-compliant random
// access iterators:
//..
//  typedef bslstl_RandomAccessIterator<T, my_IteratorImp<T> > iterator;
//  typedef bslstl_RandomAccessIterator<const T, my_IteratorImp<T> >
//                                                              const_iterator;
//..
// Note that the implementation for 'const_iterator' is 'my_IteratorImp<T>' and
// *not* 'my_IteratorImp<const T>'; rather the 'const' is added to the return
// value of 'operator*' by way of conversion to the first template argument.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#include <bslstl_bidirectionaliterator.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // 'ptrdiff_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

                        //==================================
                        // class bslstl_RandomAccessIterator
                        //==================================

template <typename T, typename ITER_IMP, typename TAG_TYPE =
                                               std::random_access_iterator_tag>
class bslstl_RandomAccessIterator
    : public bslstl_BidirectionalIterator<T,ITER_IMP, TAG_TYPE> {
    // Given an 'ITER_IMP' type that implements a minimal subset of an iterator
    // interface, this template generates a complete iterator that meets all of
    // the requirements of a "random-access iterator" in the C++ standard.  If
    // 'T' is const-qualified, then the resulting type is a const iterator.
    // 'T' shall not be a function or reference type.  'ITER_IMP' must provide
    // public operations so that, for objects 'i' and 'j' of type 'ITER_IMP'
    // and 'n' of an integral type, the following operations are supported:
    //..
    //     ITER_IMP i;                          default construction
    //     ITER_IMP j(i);                       copy construction
    //     i = j                                assignment
    //     ++i                                  increment to next element
    //     --i                                  decrement to previous element
    //     i += n                               increment by n elements
    //     j - i   // convertible to ptrdiff_t  distance from i to j
    //     i == j  // convertible to bool       equality comparison
    //     i < j   // convertible to bool       less-than comparison
    //     *i      // convertible to T&         element access (dereference)
    //..

    // PRIVATE TYPES
    typedef typename bslmf_RemoveCvq<T>::Type UnCvqT;  // value type without
                                                       // 'const' and
                                                       // 'volatile'
                                                       // qualifications
  public:
    // TYPES
    typedef T                                value_type;
    typedef std::ptrdiff_t                   difference_type;
    typedef T                               *pointer;
    typedef T&                               reference;
    typedef std::random_access_iterator_tag  iterator_category;

    // CREATORS
    bslstl_RandomAccessIterator();
        // Construct the default value for this iterator type.  All default-
        // constructed 'bslstl_RandomAccessIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They do
        // not have a singular value unless an object of the type specified by
        // the template parameter 'ITER_IMP' has a singular value after
        // value-initialization.

    bslstl_RandomAccessIterator(const ITER_IMP& implementation);
        // Construct a random access iterator having the specified
        // 'implementation' of the parameterized 'ITER_IMP' type.

    //! bslstl_RandomAccessIterator(
    //!                           const bslstl_RandomAccessIterator& original);
    //  // Create a 'bslstl_RandomAccessIterator' having the same value as the
    //  // 'original' iterator.  Note that this method's definition is compiler
    //  // generated.

    bslstl_RandomAccessIterator(
           const bslstl_RandomAccessIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other);
        // Construct a random access iterator from another (compatible)
        // 'bslstl_RandomAccessIterator' type, e.g., a mutable iterator of the
        // same type.  Note that since this is a template, it has a lower
        // priority in overload resolution than the other constructors.

    //! ~bslstl_RandomAccessIterator();
    //  // Destroy this iterator.  Note that this method's definition is
    //  // compiler generated.

    // MANIPULATORS
    //! bslstl_RandomAccessIterator& operator=(
    //!                                const bslstl_RandomAccessIterator& rhs);
    //  // Copy the value of the specified 'rhs' to this iterator.  Return a
    //  // reference to this modifiable iterator.  Note that this method's
    //  // definition is compiler generated.

    bslstl_RandomAccessIterator& operator++();
        // Increment to the next element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the past-the-end value for an iterator over the
        // underlying sequence.

    bslstl_RandomAccessIterator& operator--();
        // Decrement to the previous element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the same value as an iterator to the start of the
        // underlying sequence.

    bslstl_RandomAccessIterator& operator+=(difference_type offset);
        // Increment by the specified 'offset' number of elements.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the iterator, after incrementing by 'offset', is within the
        // bounds of the underlying sequence.

    bslstl_RandomAccessIterator& operator-=(difference_type offset);
        // Decrement by the specified 'offset' number of elements.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the iterator, after decrementing by 'offset', is within the
        // bounds of the underlying sequence.

    // ACCESSORS
    T& operator[](difference_type index) const;
        // Return a reference to the element at the specified 'index' positions
        // past the current one.  The behavior is undefined unless the
        // referenced position lies within the underlying sequence.  Note that
        // 'index' may be negative.
};

// FREE OPERATORS
template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator==(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator has the same value as the
    // specified 'rhs' iterator, and 'false' otherwise.  Two iterators have the
    // same value if they refer to the same element, or both have the past-the-
    // end value for the underlying sequence.  The behavior is undefined unless
    // both iterators refer to the same underlying sequence.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator!=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator does not have the same
    // value as the specified 'rhs' iterator, and 'false' otherwise.  Two
    // iterators do not have the same value if (1) they do not refer to the
    // same element and (2) both do not have the past-the-end iterator value
    // for the underlying sequence.  The behavior is undefined unless both
    // iterators refer to the same underlying sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator++(bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Increment the specified 'iter' to next element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has the
    // past-the-end value for an iterator of the underlying sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator--(bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Decrement the specified 'iter' to previous element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has the
    // same value as an iterator to the start of the underlying sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                          rhs);
    // Return an iterator to the element at the specified 'rhs' positions
    // past the specified 'lhs'.  The behavior is undefined unless 'lhs',
    // after incrementing by 'rhs', is within the bounds of the underlying
    // sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(std::ptrdiff_t                                          lhs,
          const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& rhs);
    // Return an iterator to the element at the specified 'lhs' positions
    // past the specified 'rhs'.  The behavior is undefined unless 'rhs',
    // after incrementing by 'lhs', is within the bounds of the underlying
    // sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator-(const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                          rhs);
    // Return an iterator to the element at the specified 'rhs' positions
    // before the specified 'lhs'.  The behavior is undefined unless 'lhs',
    // after decrementing by 'rhs', is within the bounds of the underlying
    // sequence.  Note that this function is logically equivalent to:
    //..
    //  iter + (-rhs)
    //..

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
std::ptrdiff_t
operator-(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
          const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return the distance from the specified 'rhs' iterator to the specified
    // 'lhs' iterator.  The behavior is undefined unless 'lhs' and 'rhs' are
    // iterators into the same underlying sequence.  Note that the result might
    // be negative.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator<(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator refers to an element
    // before the specified 'rhs' iterator in the iteration sequence, or (2)
    // 'rhs' (and not 'lhs') has the past-the-end value for an iterator over
    // this sequence, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' are iterators into the same underlying sequence.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator>(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator refers to an element
    // after the specified 'rhs' iterator in the iteration sequence, or (2)
    // 'lhs' (and not 'rhs') has the past-the-end value for an iterator over
    // this sequence, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' are iterators into the same underlying sequence.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator<=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator has the same value as
    // the specified 'rhs' iterator, or (2) 'lhs' refers to an element before
    // 'rhs' in the iteration sequence, or (3) 'rhs' has the past-the-end value
    // for an iterator over this sequence, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs' and 'rhs' are iterators into the same
    // underlying sequence.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator>=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if (1) the specified 'lhs' iterator has the same value as
    // the specified 'rhs' iterator, or (2) 'lhs' has the past-the-end value
    // for an iterator over this sequence, or (3) 'lhs' refers to an element
    // after 'rhs' in the iteration sequence, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' are iterators into the same
    // underlying sequence.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    //----------------------------------
                    // class bslstl_RandomAccessIterator
                    //----------------------------------

// CREATORS
template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::bslstl_RandomAccessIterator()
{
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::
bslstl_RandomAccessIterator(const ITER_IMP& implementation)
: bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>(implementation)
{
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::
bslstl_RandomAccessIterator(
            const bslstl_RandomAccessIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other)
: bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>(other.imp())
{
}

// MANIPULATORS
template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator++()
{
    ++this->imp();
    return *this;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator--()
{
    --this->imp();
    return *this;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator+=(
                                                        difference_type offset)
{
    this->imp() += offset;
    return *this;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator-=(
                                                        difference_type offset)
{
    this->imp() += -offset;
    return *this;
}

// ACCESSORS
template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
T& bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>::operator[](
                                                   difference_type index) const
{
    bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(*this);
    tmp += index;
    return *tmp;
}

// FREE OPERATORS
template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator==(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() == rhs.imp();
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator!=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator++(bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    ++iter;
    return tmp;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator--(bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    --iter;
    return tmp;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                          rhs)
{
    bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE> result(lhs);
    result += rhs;
    return result;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator+(std::ptrdiff_t                                          lhs,
          const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& rhs)
{
    return rhs + lhs;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>
operator-(const bslstl_RandomAccessIterator<T,ITER_IMP,TAG_TYPE>& lhs,
          std::ptrdiff_t                                          rhs)
{
    return lhs + -rhs;
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
std::ptrdiff_t operator-(
                  const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                  const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() - rhs.imp();
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator<(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() < rhs.imp();
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator>(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
               const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return rhs < lhs;
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator<=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(rhs < lhs);
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator>=(const bslstl_RandomAccessIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_RandomAccessIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs < rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
