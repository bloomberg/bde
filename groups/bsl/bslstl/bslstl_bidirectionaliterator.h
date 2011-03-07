// bslstl_bidirectionaliterator.h                                     -*-C++-*-
#ifndef INCLUDED_BSLSTL_BIDIRECTIONALITERATOR
#define INCLUDED_BSLSTL_BIDIRECTIONALITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template to create STL-compliant bidirectional iterators.
//
//@CLASSES:
//  bslstl_BidirectionalIterator: bidirectional iterator template
//
//@SEE_ALSO: bslstl_iterator, bslstl_forwarditerator,
//           bslstl_randomaccessiterator
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim),
//         Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides an iterator adaptor that, given an
// implementation class defining a core set of iterator functionality, adapts
// it to provide an STL-compliant bidirectional iterator interface.  The set of
// requirements for a forward iterator is found in "Table 107: Bidirectional
// iterator requirements", under the tag "[bidirectional.iterators]".  (Note
// that this reference is sourced in N3092, a C++0x working paper; the actual
// table number may be different in the actual standard.)
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
//      // ACCESSORS
//      T& operator*() const;
//  };
//
//  template <class T>
//  bool operator==(const my_IteratorImp<T>& lhs,
//                  const my_IteratorImp<T>& rhs);
//..
// simply add the following two 'typedef's to any container class that provides
// 'my_IteratorImp<T>' access, and the container will have STL-compliant
// bidirectional iterators:
//..
//  typedef bslstl_BidirectionalIterator<T, my_IteratorImp<T> > iterator;
//  typedef bslstl_BidirectionalIterator<const T, my_IteratorImp<T> >
//                                                              const_iterator;
//..
// Note that the implementation for 'const_iterator' is 'my_IteratorImp<T>' and
// *not* 'my_IteratorImp<const T>'; rather the 'const' is added to the return
// value of 'operator*' by way of conversion to the first template argument.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_bidirectionaliterator.h> header can't be included directly in \
in BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_FORWARDITERATOR
#include <bslstl_forwarditerator.h>
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

                        //===================================
                        // class bslstl_BidirectionalIterator
                        //===================================

template <typename T, typename ITER_IMP, typename TAG_TYPE =
                                               std::bidirectional_iterator_tag>
class bslstl_BidirectionalIterator
    : public bslstl_ForwardIterator<T, ITER_IMP, TAG_TYPE> {
    // Given an 'ITER_IMP' type that implements a minimal subset of an iterator
    // interface, this template generates a complete iterator that meets all of
    // the requirements of a "bidirectional iterator" in the C++ standard.  If
    // 'T' is 'const'-qualified, then the resulting type is a constant
    // iterator.  'T' shall not be a function or reference type.  'ITER_IMP'
    // must provide public operations so that, for objects 'i' and 'j' of type
    // 'ITER_IMP', the following operations are supported:
    //..
    //     ITER_IMP i;                          default construction
    //     ITER_IMP j(i);                       copy construction
    //     i = j                                assignment
    //     ++i                                  increment to next element
    //     --i                                  decrement to previous element
    //     i == j     // convertible to bool    equality comparison
    //     *i         // convertible to T&      element access (dereference)
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
    typedef std::bidirectional_iterator_tag  iterator_category;

    // CREATORS
    bslstl_BidirectionalIterator();
        // Construct the default value for this iterator type.  All default-
        // constructed 'bslstl_BidirectionalIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They
        // do not have a singular value unless an object of the type specified
        // by the template parameter 'ITER_IMP' has a singular value after
        // value-initialization.

    bslstl_BidirectionalIterator(const ITER_IMP& implementation);
        // Construct a bidirectional iterator having the specified
        // 'implementation' of the parameterized 'ITER_IMP' type.

    //! bslstl_BidirectionalIterator(
    //!                          const bslstl_BidirectionalIterator& original);
        // Construct a bidirectional iterator having the same value as the
        // 'original' iterator.  Note that this method's definition is compiler
        // generated.

    bslstl_BidirectionalIterator(const
                bslstl_BidirectionalIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other);
        // Construct a bidirectional iterator from another (compatible)
        // 'bslstl_BidirectionalIterator' type, e.g., a mutable iterator of
        // the same type.  Note that as this is a template, it has a lower
        // priority in overload resolution than the other constructors.

    //! ~bslstl_BidirectionalIterator();
        // Destroy this iterator.  Note that this method's definition is
        // compiler generated.

    // MANIPULATORS
    //! bslstl_BidirectionalIterator& operator=(
    //!                               const bslstl_BidirectionalIterator& rhs);
        // Copy the value of the specified 'rhs' to this iterator.  Return a
        // reference to this modifiable object.  Note that this method's
        // definition is compiler generated.

    bslstl_BidirectionalIterator& operator++();
        // Increment to the next element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the past-the-end value for an iterator over the
        // underlying sequence.

    bslstl_BidirectionalIterator& operator--();
        // Decrement to the previous element.  Return a reference to this
        // modifiable iterator.  The behavior is undefined if, on entry, this
        // iterator has the same value as an iterator the refers to the start
        // of the underlying sequence.
};

// FREE OPERATORS
template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator==(const bslstl_BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator has the same value as the
    // specified 'rhs' iterator, and 'false' otherwise.  Two iterators have the
    // same value if they refer to the same element, or both have the past-the-
    // end value for the underlying sequence.  The behavior is undefined unless
    // both iterators refer to the same underlying sequence.

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
bool operator!=(const bslstl_BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' iterator does not have the same
    // value as the specified 'rhs' iterator, and 'false' otherwise.  Two
    // iterators do not have the same value if (1) they do not refer to the
    // same element and (2) both do not have the past-the-end iterator value
    // for the underlying sequence.  The behavior is undefined unless both
    // iterators refer to the same underlying sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator++(bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Increment the specified 'iter' to the next element.  Return the previous
    // value of 'iter'.  The behavior is undefined if, on entry, 'iter' has
    // the past-the-end value for an iterator of the underlying sequence.

template <typename T, typename ITER_IMP, typename TAG_TYPE>
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator--(bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int);
    // Decrement the specified 'iter' to the previous element.  Return the
    // previous value of 'iter'.  The behavior is undefined if, on entry,
    // 'iter' has the same value as an iterator to the start of the underlying
    // sequence.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                //-----------------------------------
                // class bslstl_BidirectionalIterator
                //-----------------------------------

// CREATORS
template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::
bslstl_BidirectionalIterator()
: bslstl_ForwardIterator<T,ITER_IMP,TAG_TYPE>()
{
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::
bslstl_BidirectionalIterator(const ITER_IMP& implementation)
: bslstl_ForwardIterator<T,ITER_IMP,TAG_TYPE>(implementation)
{
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::
bslstl_BidirectionalIterator(
           const bslstl_BidirectionalIterator<UnCvqT,ITER_IMP,TAG_TYPE>& other)
: bslstl_ForwardIterator<T,ITER_IMP,TAG_TYPE>(other.imp())
{
}

// MANIPULATORS
template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::operator++()
{
    ++this->imp();
    return *this;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>&
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>::operator--()
{
    --this->imp();
    return *this;
}

// FREE OPERATORS
template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator==(const bslstl_BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() == rhs.imp();
}

template <typename T1, typename T2, typename ITER_IMP, typename TAG_TYPE>
inline
bool operator!=(const bslstl_BidirectionalIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const bslstl_BidirectionalIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs == rhs);
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator++(bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    ++iter;
    return tmp;
}

template <typename T, typename ITER_IMP, typename TAG_TYPE>
inline
bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>
operator--(bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    bslstl_BidirectionalIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    --iter;
    return tmp;
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
