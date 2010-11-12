// bdeut_nullinputiterator.h                  -*-C++-*-
#ifndef INCLUDED_BDEUT_NULLINPUTITERATOR
#define INCLUDED_BDEUT_NULLINPUTITERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a pure input iterator for an empty range.
//
//@CLASSES:
//  bdeut_NullInputIterator: parameterized null input iterator
//
//@SEE_ALSO: bdeut_nulloutputiterator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This components provides at templated iterator type,
// 'bdeut_NullInputIterator', with the following attributes:
//..
//  o Exactly meets the requirements for an input iterator according to the
//    C++ Standard (C++98, Section 24.1.1 [lib.input.iterators]).
//  o For a given type, 'T', all objects of type 'bdeut_NullInputIterator<T>'
//    compare equal.  Thus, any pair of such iterators constitute an
//    empty range.
//  o Dereferencing or incrementing the iterator is undefined behavior, since
//    every iterator is logically at the end of its valid range.
//..
// This iterator type is typically used to test that a function or template
// class compiles when instantiated with a pure input iterator.
//
///Usage
///-----
// The following test function is designed to traverse an input iterator range
// and sum the elements.
//..
//  template <class IN_ITER>
//  typename bsl::iterator_traits<IN_ITER>::value_type
//  sum(IN_ITER first, IN_ITER last)
//  {
//      typename bsl::iterator_traits<IN_ITER>::value_type total = 0;
//      while (first != last) {
//          total += *first++;
//      }
//      return total;
//  }
//..
// The following program uses 'sum' to compute the sum of elements in an
// array.  Then it uses 'sum' again, this time instantiated with
// 'bdeut_NullInputIterator'.  The result is zero because
// 'bdeut_NullInputIterator' is always at the end.  The point is to prove that
// 'sum' compiles when instantiated with pure input iterators.
//..
//  int main()
//  {
//      static const int myArray[6] = { 2, 3, 5, 7, 11, 0 };
//
//      // Compute the sum using random-access iterators (pointers).
//      int r1 = sum(&myArray[0], &myArray[5]);
//      assert(28 == r1);
//
//      // Now test that it compiles using pure input iterators:
//      typedef bdeut_NullInputIterator<unsigned> iterType;
//      unsigned r2 = sum(iterType(), iterType());
//      assert(0 == r2);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif


namespace BloombergLP {

                        // =============================
                        // class bdeut_NullInputIterator
                        // =============================

template <typename TYPE>
class bdeut_NullInputIterator :
    public bsl::iterator<bsl::input_iterator_tag, TYPE> {
    // Provide an input iterator that iterates over an empty sequence.
    // All null iterators compare equal.  Since the iteration sequence is
    // empty, incrementing or dereferencing this iterator yields undefined
    // behavior.

  public:
    // CREATORS
    bdeut_NullInputIterator();
        // Creates a null input iterator.

    bdeut_NullInputIterator(const bdeut_NullInputIterator& original);
        // Construct a copy of 'original'.

    ~bdeut_NullInputIterator();
        // Destroy this object.

    // MANIPULATORS
    bdeut_NullInputIterator& operator=(const bdeut_NullInputIterator& rhs);
        // Assignment operator.  Does nothing and returns '*this'.

    bdeut_NullInputIterator& operator++();
    bdeut_NullInputIterator& operator++(int);
        // Increment the iterator.  Since the only value for this iterator is
        // the "end" value, incrementing it always produces undefined
        // behavior.  A loop traversing a pair of null input iterators will
        // execute zero times, but may still contain an (unused) calls to
        // these operators.  Thus, these operators can be instantiated at
        // compile-time, even though they must never be called at run-time.

    // ACCESSORS
    TYPE* operator->() const;
    TYPE operator*() const;
        // Dereference the iterator.  Since the only value for this iterator
        // is the "end" value, dereferencing it always produces undefined
        // behavior.  A loop traversing a pair of null input iterators will
        // execute zero times, but may still contain an (unused) calls to
        // these operators.  Thus, these operators can be instantiated at
        // compile-time, even though they must never be called at run-time.
};

// FREE OPERATORS
template <typename TYPE>
inline
bool operator==(const bdeut_NullInputIterator<TYPE>& lhs,
                const bdeut_NullInputIterator<TYPE>& rhs);
    // Return 'true'.

template <typename TYPE>
inline
bool operator!=(const bdeut_NullInputIterator<TYPE>& lhs,
                const bdeut_NullInputIterator<TYPE>& rhs);
    // Return 'false'.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>::bdeut_NullInputIterator()
{
}

template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>::bdeut_NullInputIterator(
                                                const bdeut_NullInputIterator&)
{
}

template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>::~bdeut_NullInputIterator()
{
}

// MANIPULATORS
template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>&
bdeut_NullInputIterator<TYPE>::operator=(const bdeut_NullInputIterator&)
{
    return *this;
}

template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>& bdeut_NullInputIterator<TYPE>::operator++()
{
    return *this;
}

template<class TYPE>
inline
bdeut_NullInputIterator<TYPE>& bdeut_NullInputIterator<TYPE>::operator++(int)
{
    return *this;
}

// ACCESSORS
template<class TYPE>
inline
TYPE *bdeut_NullInputIterator<TYPE>::operator->() const
{
    return 0;
}

template<class TYPE>
inline
TYPE bdeut_NullInputIterator<TYPE>::operator*() const
{
    return *this->operator->();
}

// FREE OPERATORS
template <typename TYPE>
inline
bool operator==(const bdeut_NullInputIterator<TYPE>&,
                const bdeut_NullInputIterator<TYPE>&)
{
    return true;
}

template <typename TYPE>
inline
bool operator!=(const bdeut_NullInputIterator<TYPE>&,
                const bdeut_NullInputIterator<TYPE>&)
{
    return false;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
