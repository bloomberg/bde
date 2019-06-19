// bslstl_boyermoorehorspoolsearcher.h                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_BOYERMOOREHORSPOOLSEARCHER
#define INCLUDED_BSLSTL_BOYERMOOREHORSPOOLSEARCHER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'boyer_moore_horspool_searcher' class.
//
//@CLASSES:
//  bsl::boyer_moore_horspool_searcher: class template to search via BMH
//
//@SEE_ALSO: bslstl_defaultsearcher
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component defines a single class template,
// 'bsl::boyer_moore_horspool_searcher', that is compliant with section
// '[func.search.bmh]' of the C++ Standard (C++17 and later).  This class has
// several template parameters:
//
//: 'RNDACC_ITR_NEEDLE':
//:    The type used to specify (on construction) the range of values
//:    being sought (the "needle").
//
//: 'HASH':
//:    The functor type used to hash metadata for the unique values of
//:    the needle.  See {Requirements for 'HASH' and 'EQUAL'}.
//:
//: 'EQUAL':
//:    The functor type used to compare values when storing/accessing needle
//:    metadata.  See {Requirements for 'HASH' and 'EQUAL'}.
//:
//: 'ALLOCATOR':
//:    The allocator used to supply memory (for metadata).
//
// The class also provides a functor-style interface that accepts two iterators
// that define the range of values to be searched (the "haystack").  The
// iterators defining the haystack need not be of the same type as those that
// define the needle.  Moreover, the "search" method of the needle can be
// overloaded for an arbitrary number of different haystack iterators.  Once
// constructed, a single 'bsl::boyer_moore_horspool_searcher' object can be
// re-used to search multiple haystacks (for the same needle value).
//
///Algorithm
///---------
// The 'bsl::default_searcher' class provides an implementation of the
// well-known Boyer, Moore, Horspool Algorithm for string matching (see
// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm).
// It offers complexity of 'O(N)' for a haystack of length 'N' in the typical
// case.
//
///Iterator Requirements
///---------------------
// The two independent iterators types associated with this class -- one
// defining the needle, the other defining the haystack -- must meet the
// requirements of *RandomAccessIterator*.
//
// Additionally:
//: o These iterators can be constant.
//: o When dereferenced, both iterator types must refer to the same value type.
//
// The operations of either of the iterator types are allowed to throw
// exceptions.
//
///Requirements for 'HASH' and 'EQUAL'
///-----------------------------------
// The comparer class, 'EQUAL', must meet the requirements of
// *BinaryPredicate*:
//: o The class defines an 'operator()' method that, given an
//:   *RandomAccessIterator', 'iterator', can be invoked as
//:   'operator()(*iterator, *iterator)'.
//: o The return value must be contextually convertible to 'bool'.
//: o The supplied iterators can be constant.
//: o The class must be copyable.
//
// The comparer class is allowed to throw exceptions.
//
// The behavior is undefined unless two values that are deemed equal by the
// 'EQUAL' functor generate the same value by the 'HASH' functor.  That is:
//..
//  true == searcher.equal()(a, b);
//..
// implies:
//..
//  searcher.hash()(a) == searcher.hash()(b);
//..
//
///Optimization When 'value_type' is 'char'
///----------------------------------------
// This implementation handles needle metadata using a fixed size array when
// the 'value_type' is 'char'.  For needles of typical size, this choice
// results in a larger searcher object footprint than it would have if some
// dynamically sized container were used; however, the faster access during
// searches warrants the tradeoff.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The problem of searching a sequence of characters for a particular
// sub-sequence arises in many applications.  For example, one might need to
// know of some document contains a particular word of interest.  For example,
// suppose we would like to know the first occurrence of the word "United" in
// the Declaration of Independence (of the United States):
//
// First, we obtain the text of the document and word of interest as sequences
// of 'char' values.
//..
//  const char document[] =
//  " IN CONGRESS, July 4, 1776.\n"                // 28
//  "\n"                                           //  1
//  " The unanimous Declaration of the thirteen united States of America,\n"
// //----^----|----^----|----^----|----^----|----  // 44
// //                                              // --
// //                                              // 73rd character
// //
//  "\n"
//  // ...
//  " declare, That these United Colonies are, and of Right ought to be\n"
//  // ...
//  "Honor.";
//
//  const char *word = "United";
//..
// Then, we create a 'default_searcher' object (a functor) using the given
// 'word':
//..
//  bsl::boyer_moore_horspool_searcher<const char*> searchForUnited(
//                                                   word,
//                                                   word + bsl::strlen(word));
//..
// Notice that no equality comparison functor was specified so
// 'searchForUnited' will use 'bsl::equal_to<char>' by default.
//
// Now, we invoke our functor, specifying the range of the document to be
// searched:
//..
//  bsl::pair<const char *, const char *> result = searchForUnited(
//                                                            document,
//                                                            document
//                                                          + sizeof document);
//
//  bsl::size_t offset = result.first - document;
//
//  assert(120 == offset);
//  assert(static_cast<bsl::size_t>(result.second - result.first)
//             == bsl::strlen(word));
//..
// Finally, we notice that search correctly ignored the appearance of the word
// "united" (all lower case) in the second sentence.
//
///Example 2: Defining a Comparator and Hash
///- - - - - - - - - - - - - - - - - - - - -
// As seen in {Example 1} above, the default equality comparison functor is
// case sensitive.  If one needs case *in*-sensitive searches, a non-default
// equality comparison class *and* a non-default hash functor must be
// specified.
//
// First, define (at file scope if using a pre-C++11 compiler) an equality
// comparison class that provides the required functor interface:
//..
//  struct MyCaseInsensitiveCharComparer {
//      bool operator()(const char& a, const char& b) const {
//          return bsl::tolower(a) == bsl::tolower(b);
//      }
//  };
//..
// Then, define (again at file scope, if pre-C++11), a hash functor so that two
// values, irrespective of their case, hash to the same value.
//..
//  struct MyCaseInsensitiveCharHasher {
//      bool operator()(const char& value) const {
//          static bsl::hash<char> s_hash;
//          return s_hash(static_cast<char>(bsl::tolower(value)));
//      }
//  };
//..
// Now, specify a 'bsl::boyer_moore_horspool_searcher' type for, and create a
// searcher object to search for 'word':
//..
//  bsl::boyer_moore_horspool_searcher<const char *,
//                                     MyCaseInsensitiveCharHasher,
//                                     MyCaseInsensitiveCharComparer>
//                                                  searchForUnitedInsensitive(
//                                                   word,
//                                                   word + bsl::strlen(word));
//..
// Note that the new searcher object will use defaulted constructed
// 'MyCaseInsensitiveCharHasher' and 'MyCaseInsensitiveCharComparer' classes.
// If stateful functors are required such objects can be passed in the optional
// constructor arguments.
//
// Now, we invoke our new functor, specifying the same document searched in
// {Example 1}:
//..
//  bsl::pair<const char *, const char *> resultInsensitive =
//                                                  searchForUnitedInsensitive(
//                                                            document,
//                                                            document
//                                                          + sizeof document);
//
//  bsl::size_t offsetInsensitive = resultInsensitive.first - document;
//
//  assert( 72 == offsetInsensitive);
//  assert(static_cast<bsl::size_t>(resultInsensitive.second
//                                - resultInsensitive.first)
//             == bsl::strlen(word));
//..
// Finally, we find the next occurrence of 'word' by *reusing* the same
// searcher object, this time instructing it to begin its search just after the
// previous occurrence of 'word' was found:
//..
//  resultInsensitive = searchForUnitedInsensitive(resultInsensitive.second,
//                                                 document + sizeof document);
//
//  offsetInsensitive = resultInsensitive.first - document;
//
//  assert(120 == offsetInsensitive);
//  assert(static_cast<bsl::size_t>(resultInsensitive.second
//                                - resultInsensitive.first)
//             == bsl::strlen(word));
//..
//
///Example 3: Non-'char' Searches
/// - - - - - - - - - - - - - - -
// The 'default_searcher' class template is not constrained to searching for
// 'char' values.  Searches can be done on other types (see {Iterator
// Requirements}).  Moreover the container of the sequence being sought (the
// "needle") need not the same as the sequence being searched (the "haystack").
//
// Suppose one has data from an instrument that reports 'float' values and that
// inserts the sequence '{ FLT_MAX, FLT_MIN, FLT_MAX }' as a marker for the
// start and end of a test run.  We can assume the probability of the
// instrument reporting this sequence of readings is negligible and that data
// reported outside of the test runs is random noise.  Here is how we can
// search for the first test run data in the data sequence.
//
// First, we create a representation of the sequence that denotes the limit of
// a test run.
//..
//  const float       markerSequence[]     = { FLT_MAX , FLT_MIN , FLT_MAX };
//  const bsl::size_t markerSequenceLength = sizeof  markerSequence
//                                         / sizeof *markerSequence;
//..
// Next, we obtain the data to be searched.  (In this example, we will use
// simulated data.)
//..
//  bsl::vector<float> data;
//  doTestRun(&data);
//..
// Then, we define and create our searcher object:
//..
//  bsl::boyer_moore_horspool_searcher<const float *>
//                                       searchForMarker(markerSequence,
//                                                       markerSequence
//                                                     + markerSequenceLength);
//..
// Notice that no equality comparison functor was specified so
// 'searchForMarker' will use 'bsl::equal_to<float>' by default.
//
// Now, we invoke our searcher on the instrument data.
//..
//  typedef bsl::vector<float>::const_iterator DataConstItr;
//
//  const bsl::pair<DataConstItr, DataConstItr> notFound(data.cend(),
//                                                       data.cend());
//
//  bsl::pair<DataConstItr, DataConstItr> markerPosition = searchForMarker(
//                                                               data.cbegin(),
//                                                               data.cend());
//
//  assert(notFound != markerPosition);
//
//  DataConstItr startOfTestRun = markerPosition.second;
//..
// Finally, we locate the marker of the end of the first test run and pass the
// location of the first test run data to some other function for processing.
//..
//  markerPosition = searchForMarker(markerPosition.second, data.cend());
//
//  assert(notFound != markerPosition);
//
//  DataConstItr endOfTestRun = markerPosition.first;
//
//  processTestRun(startOfTestRun, endOfTestRun);
//..

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_iterator.h>
#include <bslstl_pair.h>
#include <bslstl_unorderedmap.h>

#include <bslmf_conditional.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>

#include <limits.h> // 'UCHAR_MAX'

namespace BloombergLP {
namespace bslstl {

                // ===========================================
                // class boyer_moore_horspool_searcher_CharImp
                // ===========================================

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
class boyer_moore_horspool_searcher_CharImp {
    // This class template implements the same interfaces as the
    // 'boyer_moore_horspool_searcher_GeneralImp'; however, the implementation
    // is specialized for a 'value_type' of 'char'.  Notably, needle metadata
    // is stored/accessed from a fixed size array, not a dynamically-sized
    // container.

  public:
    // TYPES
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type
                                                              value_type;
        // the type of the values that can be obtained by dereferencing a
        // 'RNDACC_ITR_NEEDLE'

    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                                              difference_type;
        // a signed type that can describe the distance between
        // 'RNDACC_ITR_NEEDLE' iterators

  private:
    // DATA
    native_std::size_t d_needleLength;
    difference_type    d_table[UCHAR_MAX + 1];  // skip on mismatch table
    ALLOCATOR          d_allocator;

  public:
    // CREATORS
    boyer_moore_horspool_searcher_CharImp(RNDACC_ITR_NEEDLE needleFirst,
                                          RNDACC_ITR_NEEDLE needleLast,
                                          HASH              hash,
                                          EQUAL             equal,
                                          const ALLOCATOR&  basicAllocator);
        // Create a 'boyer_moore_horspool_searcher_CharImp' object for the
        // sequence of 'char' values in the specified range
        // '[needleFirst, needlelast)'.  This implementation is invoked when
        // the specified 'hash' is 'bsl::hash<char>' and the specified 'equal'
        // is 'bsl::equal_to<char>'.  Neither functor is used because for the
        // special case of 'char' the needle metadata is maintained in a fixed
        // size array (256 elements).  As always, the specified
        // 'basicAllocator' is used to supply memory; however, as no memory
        // allocated by this object, 'basicAllocator' is not used.  The
        // behavior is undefined unless 'needleFirst' can be advanced to
        // 'needleLast'.

    // ACCESSORS
    difference_type badCharacterSkip(const value_type& value) const;
        // Return the number of positions to advance the search in the haystack
        // when the specified 'value' is found in the rightmost position of the
        // current (unsuccessful) match attempt.

    HASH hash() const;
        // Return the hashing functor supplied on construction.

    EQUAL equal() const;
        // Return the equality comparison functor supplied on construction.

    ALLOCATOR allocator() const;
        // Return the allocator supplied on construction.
};

                // ==============================================
                // class boyer_moore_horspool_searcher_GeneralImp
                // ==============================================

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
class boyer_moore_horspool_searcher_GeneralImp {
    // This class template implements the same interfaces as the
    // 'boyer_moore_horspool_searcher_CharImp' for arbitrary 'value_type'.

    // PUBLIC TYPES
  public:
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type
                                                              value_type;

    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                                              difference_type;
  private:
    // PRIVATE TYPES
    typedef bsl::unordered_map<value_type,      // key
                               difference_type, // value
                               HASH,
                               EQUAL,
                               ALLOCATOR> Map;
    // DATA
    difference_type d_needleLength;
    Map             d_map;

  public:
    // CREATORS
    boyer_moore_horspool_searcher_GeneralImp(
                                             RNDACC_ITR_NEEDLE needleFirst,
                                             RNDACC_ITR_NEEDLE needleLast,
                                             HASH              hash,
                                             EQUAL             equal,
                                             const ALLOCATOR&  basicAllocator);
        // Create a 'boyer_moore_horspool_searcher_CharImp' object for the
        // sequence of 'value_type' values in the specified range
        // '[needleFirst, needlelast)'.  The specified 'hash' and 'equal'
        // functors are used to store/access metadata associated with the
        // needle.  See {Requirements for 'HASH' and 'EQUAL'}.  The specified
        // 'basicAllocator' is used to supply memory.  The behavior is
        // undefined unless 'needleFirst' can be advanced to 'needleLast'.

    // ACCESSORS
    difference_type badCharacterSkip(const value_type& value) const;
        // Return the number of positions to advance the search in the haystack
        // when the specified 'value' is found in the rightmost position of the
        // current (unsuccessful) match attempt.

    HASH hash() const;
        // Return the hashing functor supplied on construction.

    EQUAL equal() const;
        // Return the equality comparison functor supplied on construction.

    ALLOCATOR allocator() const;
        // Return the allocator supplied on construction.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                        // ===================================
                        // class boyer_moore_horspool_searcher
                        // ===================================

template <class RNDACC_ITR_NEEDLE,
          class HASH = bsl::hash<
                typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>,
          class EQUAL = bsl::equal_to<
                typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>,
          class ALLOCATOR = bsl::allocator<bsl::pair<
        const typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type,
              typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                                    >
                                          >
         >
class boyer_moore_horspool_searcher {
    // This class template implements an STL-compliant searcher object that
    // uses the Boyer, Moore, Horsepool Algorithm.  Several non-standard
    // accessors are also provided.

  public:
    // TYPES
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type
                                                              value_type;
        // the type of the values that can be obtained by dereferencing a
        // 'RNDACC_ITR_NEEDLE'

  private:
    // PRIVATE TYPES
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                                              difference_type;
        // a signed type that can describe the distance between
        // 'RNDACC_ITR_NEEDLE' iterators

    typedef bsl::hash<
                  typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>
                                                                   DefaultHash;
        // the default type for the 'HASH' optional template parameter
    typedef bsl::equal_to<
                  typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>
                                                                  DefaultEqual;
        // the default type for the 'EQUAL' optional template parameter

    typedef bsl::allocator<bsl::pair<
        const typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type,
              typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                    >
                          >                                   DefaultAllocator;
        // the default type for the 'ALLOCATOR' optional template parameter

    enum {
        k_CAN_OPTIMIZE_FOR_CHAR = (
                  bsl::is_same<value_type, char>        ::value
               && bsl::is_same<HASH,       DefaultHash >::value
               && bsl::is_same<EQUAL,      DefaultEqual>::value)
    };

    typedef typename bsl::conditional<
             k_CAN_OPTIMIZE_FOR_CHAR,

             BloombergLP::bslstl::
             boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                                   HASH,
                                                   EQUAL,
                                                   ALLOCATOR>,
             BloombergLP::bslstl::
             boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                                      HASH,
                                                      EQUAL,
                                                      ALLOCATOR> >::type Imp;

    // DATA
    RNDACC_ITR_NEEDLE d_needleFirst;   // start  of needle specified by CTOR
    RNDACC_ITR_NEEDLE d_needleLast;    // end    of needle specified by CTOR
    difference_type   d_needleLength;  // length of needle specified by CTOR

    Imp               d_imp;  // 'char'-optimized or general implementation

  public:
    // CREATORS
    boyer_moore_horspool_searcher(
                               RNDACC_ITR_NEEDLE needleFirst,
                               RNDACC_ITR_NEEDLE needleLast,
                               HASH              hash           = HASH(),
                               EQUAL             equal          = EQUAL(),
                               const ALLOCATOR&  basicAllocator = ALLOCATOR());
        // Create a 'boyer_moore_horspool_searcher' object that can search for
        // the sequence of 'value_type' values found in the specified range
        // '[needleFirst, needleLast)'.  Generate meta-data and save for use by
        // 'operator()'.  The complexity of of this process is O(M) where M is
        // the length of the "needle".  Optionally specify a 'hash' functor
        // mapping mis-matched values to the size of the next step in the
        // search -- as large as, 'needleLast - needleFirst'.  Optionally
        // specify an 'equal' functor for use with 'hash' and for use by
        // 'operator()'.  See {Requirements for 'HASH' and 'EQUAL'}.
        // Optionally specify 'basicAllocator' to supply memory.  The behavior
        // is undefined unless 'needleFirst' can be advanced to equal
        // 'needleLast'.

    boyer_moore_horspool_searcher(
                         const boyer_moore_horspool_searcher&  original,
                         const ALLOCATOR&                      basicAllocator);
        // Create a 'boyer_moore_horspool_searcher' object having same state as
        // the specified 'original' object and that uses 'basicAllocator' to
        // supply memory.

    //! ~boyer_moore_horspool_searcher() = default;
        // Destroy this 'boyer_moore_horspool_searcher' object.

    //! boyer_moore_horspool_searcher(boyer_moore_horspool_searcher&&
    //!                                                    original) = default;
        // Create a 'boyer_moore_horspool_searcher' object having the same
        // state as the specified 'original' on entry.  The 'original' object
        // is left in an unspecified (valid) state.

    // MANIPULATORS
    //! boyer_moore_horspool_searcher& operator=(
    //!                    const boyer_moore_horspool_searcher& rhs) = default;
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    // ACCESSORS
    template<class RNDACC_ITR_HAYSTACK>
    bsl::pair<RNDACC_ITR_HAYSTACK, RNDACC_ITR_HAYSTACK> operator()(
                                       RNDACC_ITR_HAYSTACK haystackFirst,
                                       RNDACC_ITR_HAYSTACK haystackLast) const;
        // Search the specified range '[haystackFirst, haystackLast)' for the
        // first sequence of 'value_type' values specified on construction.
        // Return the range where those values are found, or the range
        // '[haystackLast, haystackLast)' if that sequence is not found.  The
        // search is performed using an implementation of the Boyer Moore
        // Horspool algorithm and has a complexity of O(N) for random text.
        // Values of the "needle" sequence and the "haystack" sequence are
        // compared using the equality comparison functor specified on
        // construction.  The behavior is undefined unless 'haystackFirst' can
        // be advanced to equal 'haystackLast'.  Note that if the "needle"
        // sequence is empty, the range '[haystackFirst, haystackFirst)' is
        // returned.  Also note that if the "needle" sequence is longer than
        // the "haystack" sequence -- thus, impossible for the "needle" to be
        // found in the "haystack" -- the range '[haystackLast, haystackLast)'
        // is returned.

                        // Non-Standard Accessors

    RNDACC_ITR_NEEDLE needleFirst() const;
        // Return an iterator referring to the first element of the sequence of
        // 'value_type' values that can be sought by this searcher object.

    RNDACC_ITR_NEEDLE needleLast() const;
        // Return an iterator referring to one past the last element of the
        // sequence of 'value_type' values that can be sought by this searcher
        // object.

    HASH hash() const;
        // Return the hashing functor supplied on construction.

    EQUAL equal() const;
        // Return the equality comparison functor supplied on construction.

    ALLOCATOR allocator() const;
        // Return the allocator mechanism supplied on construction.
};

}  // close namespace bsl

// ----------------------------------------------------------------------------
// TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslstl {

                // -------------------------------------------
                // class boyer_moore_horspool_searcher_CharImp
                // -------------------------------------------

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::
boyer_moore_horspool_searcher_CharImp(RNDACC_ITR_NEEDLE needleFirst,
                                      RNDACC_ITR_NEEDLE needleLast,
                                      HASH              ,
                                      EQUAL             ,
                                      const ALLOCATOR&  basicAllocator)
: d_needleLength(needleLast - needleFirst)
, d_allocator(basicAllocator)
{
    BSLS_ASSERT(needleFirst <= needleLast);

    for (int i = 0; i < UCHAR_MAX + 1; ++i) {
        d_table[i] = d_needleLength;
    }

#if 1
    if (0 < d_needleLength) {
        for (RNDACC_ITR_NEEDLE current  = needleFirst,
                               last     = needleLast - 1;
                               last    != current; ++current) {
            d_table[static_cast<unsigned char>(*current)]
                                                     = d_needleLength
                                                     - 1
                                                     - (current - needleFirst);
        }
    }
#else
    if (0 < d_needleLength) {
        for (RNDACC_ITR_NEEDLE current  = needleFirst,
                               last     = needleLast;
                               last    != current; ++current) {
            const unsigned char   characterAtNeedleIndex = *current;
            const difference_type needleIndex            = current
                                                         - needleFirst;
            const difference_type numCharactersToSkip    = d_needleLength
                                                         - 1
                                                         - needleIndex;

            d_table[characterAtNeedleIndex] = numCharactersToSkip;
        }
    }
#endif
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
typename
boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::difference_type
boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL,
                                      ALLOCATOR>::badCharacterSkip(
                                                       const value_type& value)
                                                                          const
{
    return d_table[static_cast<unsigned char>(value)];
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
HASH boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                           HASH,
                                           EQUAL,
                                           ALLOCATOR>::hash() const
{
    return HASH();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
EQUAL boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                            HASH,
                                            EQUAL,
                                            ALLOCATOR>::equal() const
{
    return EQUAL();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
ALLOCATOR boyer_moore_horspool_searcher_CharImp<RNDACC_ITR_NEEDLE,
                                                HASH,
                                                EQUAL,
                                                ALLOCATOR>::allocator() const
{
    return d_allocator;
}

                // ----------------------------------------------
                // class boyer_moore_horspool_searcher_GeneralImp
                // ----------------------------------------------

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                         HASH,
                                         EQUAL,
                                         ALLOCATOR>::
boyer_moore_horspool_searcher_GeneralImp(RNDACC_ITR_NEEDLE needleFirst,
                                         RNDACC_ITR_NEEDLE needleLast,
                                         HASH              hash,
                                         EQUAL             equal,
                                         const ALLOCATOR&  basicAllocator)
: d_needleLength(needleLast - needleFirst)
, d_map(0, hash, equal, basicAllocator)
{
    BSLS_ASSERT(needleFirst <= needleLast);

    if (0 < d_needleLength) {
        for (RNDACC_ITR_NEEDLE current  = needleFirst,
                               last     = needleLast - 1;
                               last    != current; ++current) {
            d_map.insert(native_std::make_pair(*current,
                                               d_needleLength
                                             - 1
                                             - (current - needleFirst)));
        }
    }
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
typename
boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                         HASH,
                                         EQUAL,
                                         ALLOCATOR>::difference_type
boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                         HASH,
                                         EQUAL,
                                         ALLOCATOR>::badCharacterSkip(
                                                       const value_type& value)
                                                                          const
{
    typename Map::const_iterator result = d_map.find(value);

    return d_map.cend() == result ? d_needleLength : result->second;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
HASH boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                              HASH,
                                              EQUAL,
                                              ALLOCATOR>::hash() const
{
    return d_map.hash_function();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
EQUAL boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                               HASH,
                                               EQUAL,
                                               ALLOCATOR>::equal() const
{
    return d_map.key_eq();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
ALLOCATOR boyer_moore_horspool_searcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                                   HASH,
                                                   EQUAL,
                                                   ALLOCATOR>::allocator()
                                                                          const
{
    return d_map.get_allocator();
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                        // -----------------------------------
                        // class boyer_moore_horspool_searcher
                        // -----------------------------------

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                              HASH,
                              EQUAL,
                              ALLOCATOR>::
boyer_moore_horspool_searcher(RNDACC_ITR_NEEDLE needleFirst,
                              RNDACC_ITR_NEEDLE needleLast,
                              HASH              hash,
                              EQUAL             equal,
                              const ALLOCATOR&  basicAllocator)
: d_needleFirst( needleFirst)
, d_needleLast(  needleLast)
, d_needleLength(bsl::distance(needleFirst, needleLast))
, d_imp(needleFirst, needleLast, hash, equal, basicAllocator)
{
    BSLS_ASSERT(needleFirst <= needleLast);
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                              HASH,
                              EQUAL,
                              ALLOCATOR>::
boyer_moore_horspool_searcher(
                          const boyer_moore_horspool_searcher&  original,
                          const ALLOCATOR&                      basicAllocator)
: d_needleFirst( original.needleFirst())
, d_needleLast(  original.needleLast())
, d_needleLength(bsl::distance(original.needleFirst(), original.needleLast()))
, d_imp(original.needleFirst(),
         original.needleLast(),
         original.hash(),
         original.equal(),
         basicAllocator)
{
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
template <class RNDACC_ITR_HAYSTACK>
bsl::pair<RNDACC_ITR_HAYSTACK, RNDACC_ITR_HAYSTACK>
boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                              HASH,
                              EQUAL,
                              ALLOCATOR>::operator()(
                                        RNDACC_ITR_HAYSTACK haystackFirst,
                                        RNDACC_ITR_HAYSTACK haystackLast) const
{
    BSLS_ASSERT(0 <= haystackLast - haystackFirst);

    if (0 == d_needleLength) {
        return native_std::make_pair(haystackFirst, haystackFirst);   // RETURN
    }

    native_std::size_t haystackLength = haystackLast - haystackFirst;

    for (native_std::size_t possibleMatch  = 0;
         d_needleLength + possibleMatch <= haystackLength;
         possibleMatch += d_imp.badCharacterSkip(haystackFirst[possibleMatch
                                                             + d_needleLength
                                                             - 1])) {
        // check for match in reverse order
        for (native_std::size_t idx = d_needleLength - 1;
             equal()(haystackFirst[possibleMatch + idx], d_needleFirst[idx]);
             --idx) {

            if (0 == idx) { // No difference found
                return native_std::make_pair(haystackFirst + possibleMatch,
                                             haystackFirst + possibleMatch
                                                           + d_needleLength);
                                                                      // RETURN
            }
         }
    }

    return native_std::make_pair(haystackLast, haystackLast);
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
RNDACC_ITR_NEEDLE boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                                HASH,
                                                EQUAL,
                                                ALLOCATOR>::needleFirst() const
{
    return d_needleFirst;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
RNDACC_ITR_NEEDLE boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                                HASH,
                                                EQUAL,
                                                ALLOCATOR>::needleLast() const
{
    return d_needleLast;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
HASH boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL,
                                   ALLOCATOR>::hash() const
{
    return d_imp.hash();
}

template <class RNDACC_ITR_NEEDLE, class HASH, class EQUAL, class ALLOCATOR>
inline
EQUAL boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                    HASH,
                                    EQUAL,
                                    ALLOCATOR>::equal() const
{
    return d_imp.equal();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
inline
ALLOCATOR boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                        HASH,
                                        EQUAL,
                                        ALLOCATOR>::allocator() const
{
    return d_imp.allocator();
}

}  // close namespace bsl

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
struct UsesBslmaAllocator<
    bsl::boyer_moore_horspool_searcher<RNDACC_ITR_NEEDLE,
                                       HASH,
                                       EQUAL,
                                       ALLOCATOR>
    > : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
struct UsesBslmaAllocator<
    BloombergLP::bslstl::boyer_moore_horspool_searcher_GeneralImp<
                                                             RNDACC_ITR_NEEDLE,
                                                             HASH,
                                                             EQUAL,
                                                             ALLOCATOR>
    > : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL,
          class ALLOCATOR>
struct UsesBslmaAllocator<
    BloombergLP::bslstl::boyer_moore_horspool_searcher_CharImp<
                                                             RNDACC_ITR_NEEDLE,
                                                             HASH,
                                                             EQUAL,
                                                             ALLOCATOR>
    > : bsl::is_convertible<Allocator*, ALLOCATOR>
{};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
