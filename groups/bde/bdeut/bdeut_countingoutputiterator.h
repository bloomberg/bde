// bdeut_countingoutputiterator.h                  -*-C++-*-
#ifndef INCLUDED_BDEUT_COUNTINGOUTPUTITERATOR
#define INCLUDED_BDEUT_COUNTINGOUTPUTITERATOR

//@PURPOSE: Provide an output iterator that counts values output to it.
//
//@CLASSES:
//  bdeut_CountingOutputIterator:
//
//@SEE_ALSO: bdeut_nullinputiterator, bdeut_nulloutputiterator
//
//@AUTHOR: Vladimir Kliatchko (vkliatch)
//
//@DESCRIPTION:
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_ITERATOR
#include <iterator>
#define INCLUDED_ITERATOR
#endif

namespace BloombergLP {

                        // ==================================
                        // class bdeut_CountingOutputIterator
                        // ==================================

template<class OUTPUT>
class bdeut_CountingOutputIterator :
               public std::iterator<std::output_iterator_tag,
                                    std::iterator_traits<OUTPUT>::value_type> {

    int    *d_count_p;  // TBD doc
    OUTPUT  d_out;      // TBD doc
    int     d_max;      // TBD doc

  public:
    // TYPES
    typedef std::iterator_traits<OUTPUT>::value_type VALUE_TYPE;

    class AssignmentProxy {

        bdeut_CountingOutputIterator *d_iter_p;

      public:
        AssignmentProxy(bdeut_CountingOutputIterator *iter)
        : d_iter_p(iter)
        {
        }

        void operator=(const VALUE_TYPE& value)
        {
            d_iter_p->assignValue(value);
        }
    };

    // CREATORS
    bdeut_CountingOutputIterator(int *count, OUTPUT out, int max = INT_MAX);
        // Create a counting output iterator.

    bdeut_CountingOutputIterator(const bdeut_CountingOutputIterator& rhs);
        // Construct a copy of 'rhs'.

    ~bdeut_CountingOutputIterator();
        // Destroy this counting output iterator.

    // MANIPULATORS
    bdeut_CountingOutputIterator& operator=(
                                      const bdeut_CountingOutputIterator& rhs);

    AssignmentProxy operator*();

    bdeut_CountingOutputIterator& operator++();

    bdeut_CountingOutputIterator& operator++(int);

    AssignmentProxy& assignValue(const VALUE_TYPE& value);
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CREATORS

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>::bdeut_CountingOutputIterator(
                                                                 int    *count,
                                                                 OUTPUT  out,
                                                                 int     max)
: d_count_p(count)
, d_out(out)
, d_max(max)
{
    *d_count_p = 0;
}

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>::bdeut_CountingOutputIterator(
                                       const bdeut_CountingOutputIterator& rhs)
: d_count_p(rhs.d_count_p)
, d_out(rhs.d_out)
, d_max(rhs.d_max)
{
}

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>::~bdeut_CountingOutputIterator()
{
}

// MANIPULATORS

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>&
bdeut_CountingOutputIterator<OUTPUT>::operator=(
                                       const bdeut_CountingOutputIterator& rhs)
{
    d_count_p = rhs.d_count_p;
    d_out     = rhs.d_out;
    d_max     = rhs.d_max;

    return *this;
}

template<class OUTPUT>
inline
typename bdeut_CountingOutputIterator<OUTPUT>::AssignmentProxy
bdeut_CountingOutputIterator<OUTPUT>::operator*()
{
    return AssignmentProxy(this);
}

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>&
bdeut_CountingOutputIterator<OUTPUT>::operator++()
{
    if (*d_count_p < d_max) {
        ++d_out;
    }
    ++(*d_count_p);

    return *this;
}

template<class OUTPUT>
inline
bdeut_CountingOutputIterator<OUTPUT>&
bdeut_CountingOutputIterator<OUTPUT>::operator++(int)
{
    if (*d_count_p < d_max) {
        ++d_out;
    }
    ++(*d_count_p);

    return *this;
}

template<class OUTPUT>
inline
typename bdeut_CountingOutputIterator<OUTPUT>::AssignmentProxy&
bdeut_CountingOutputIterator<OUTPUT>::assignValue(const VALUE_TYPE& value)
{
    if (*d_count_p < d_max) {
        *d_out = value;
    }

    return AssignmentProxy(this);
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
