// bdeut_nulloutputiterator.h                  -*-C++-*-
#ifndef INCLUDED_BDEUT_NULLOUTPUTITERATOR
#define INCLUDED_BDEUT_NULLOUTPUTITERATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a bit-bucket for functions that take output iterators.
//
//@CLASSES:
//  bdeut_NullOutputIterator: null output iterator template
//
//@SEE_ALSO: bdeut_nullinputiterator
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a templated iterator type,
// 'bdeut_NullOutputIterator', with the following attributes:
//..
//  - Meets exactly the requirements for an output iterator according to the
//    C++ Standard (C++98, Section 24.1.2 [lib.output.iterators]).
//  - Dereferencing an iterator and assigning tot he result is a no-op.
//  - Incrementing an iterator is a no-op.
//..
// This iterator type is typically used to call functions purely for their
// side-effects, discarding the normal output.  It is also useful for testing
// whether a template function will compile when presented with a pure output
// iterator.
//
///Usage
///-----
// The following test function is designed to traverse an input iterator range
// and sum the elements.  A running total is written to the output iterator
// and the final sum is returned.
//..
//  template <class IN_ITER, class OUT_ITER>
//  typename bsl::iterator_traits<OUT_ITER>::value_type
//  runningSum(IN_ITER first, IN_ITER last, OUT_ITER output)
//  {
//      typename bsl::iterator_traits<OUT_ITER>::value_type total = 0;
//      while (first != last) {
//          total += *first++;
//          *output++ = total;
//      }
//      return total;
//  }
//..
// The following program uses 'bdeut_NullOutputIterator' to compute the
// sum of a elements in an array while discarding the running total.
//..
//  int main()
//  {
//      static const int myArray[5] = { 2, 3, 5, 7, 11 };
//      int sum = runningSum(myArray, myArray + 5,
//                           bdeut_NullOutputIterator<int>());
//      assert(28 == sum);
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

                        // ==============================
                        // class bdeut_NullOutputIterator
                        // ==============================

template <class TYPE>
class bdeut_NullOutputIterator :
    public bsl::iterator<bsl::output_iterator_tag, TYPE> {
    // Provide an output iterator that throws elements into the bit bucket.
    // Dereferencing this iterator and assigning to the result is a no-op.

  public:
    // TYPES
    class AssignmentProxy {
        // Provide an object that can appear on the left side of an assignment
        // from 'TYPE'.  The operation AssignmentProxy() = TYPE() is valid and
        // has no effect.  (i.e., the result goes to the bit bucket.)
      public:
        void operator=(const TYPE&) { }
            // Does nothing.
    };

    // CREATORS
    bdeut_NullOutputIterator();
        // Creates a null output iterator.

    bdeut_NullOutputIterator(const bdeut_NullOutputIterator& original);
        // Construct a copy of 'original'.

    ~bdeut_NullOutputIterator();
        // Destroy this object.

    // MANIPULATORS
    bdeut_NullOutputIterator& operator=(const bdeut_NullOutputIterator& rhs);
        // Assignment operator.  Does nothing and returns '*this'.

    AssignmentProxy operator*();
        // Return an object which can appear on the left-hand side of
        // an assignment from 'TYPE'.  The assignment itself is a no-op.  This
        // function is non-const in accordance with the input iterator
        // requirements, even though '*this' is not modified.

    bdeut_NullOutputIterator& operator++();
        // Does nothing and returns '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.

    bdeut_NullOutputIterator& operator++(int);
        // Does nothing and returns '*this'.  This function is non-const in
        // accordance with the input iterator requirements, even though
        // '*this' is not modified.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS
template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>::bdeut_NullOutputIterator()
{
}

template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>::
                      bdeut_NullOutputIterator(const bdeut_NullOutputIterator&)
{
}

template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>::~bdeut_NullOutputIterator()
{
}

// MANIPULATORS
template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>&
bdeut_NullOutputIterator<TYPE>::operator=(const bdeut_NullOutputIterator&)
{
    return *this;
}

template <class TYPE>
inline
typename bdeut_NullOutputIterator<TYPE>::AssignmentProxy
bdeut_NullOutputIterator<TYPE>::operator*()
{
    return AssignmentProxy();
}

template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>& bdeut_NullOutputIterator<TYPE>::operator++()
{
    return *this;
}

template <class TYPE>
inline
bdeut_NullOutputIterator<TYPE>& bdeut_NullOutputIterator<TYPE>::operator++(int)
{
    return *this;
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
