// bslstl_defaultsearcher.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_DEFAULTSEARCHER
#define INCLUDED_BSLSTL_DEFAULTSEARCHER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'default_searcher' class.
//
//@CLASSES:
//  bsl::default_searcher:   class template to search via the "naive" algorithm
//  bslstl::DefaultSearcher: class template to search via the "naive" algorithm
//
//@SEE_ALSO: bslstl_boyermoorehorspoolsearcher
//
//@AUTHOR: Steven Breitstein (sbreitstein)
//
//@DESCRIPTION: This component defines a two class templates,
// 'bsl::default_searcher' and 'bslstl::DefaultSearcher'.  Both are compliant
// with section '[func.search.default]' of the C++ Standard (C++17 and later).
//
// 'bsl::default_searcher' is strictly limied to the Standard and is provided
// for clients for whom standard compliance is a priority.
// 'bslstl::DefaultSearcher' provides several additional interfaces (accessors)
// that are not mentioned in the Standard.
//
// Except where there is a relevant difference, both are described below as if
// they were one.
//
// This class template has two parameters:
//
//: 'FORWARD_ITR_NEEDLE':
//:   The iterator type that defines on construction the range of values to be
//:   sought (the "needle"), and
//:
//: 'EQUAL':
//:   an optional parameter that defines the class used to compare those
//:   values.
//
// The class also provides a functor-style interface that accepts two iterators
// that define the range of values to be searched (the "haystack").  Once
// constructed the searcher object can be re-used to search multiple haystacks
// (for the same needle).
//
// The iterators defining the haystack need not be of the same type as those
// that define the needle.  Moreover, the search method of the searcher can be
// overloaded for an arbitrary different haystack iterators (subject to
// {Iterator Requirements}). object can be used to search multiple haystacks
// (for the same needle).
//
///Algorithm
///---------
// The 'bslstl::DefaultSearcher' class uses the classic, "naive" algorithm.
// The needle is sought at the beginning of the haystack and, if not found
// there, the start position in the haystack is incremented.  That is repeated
// until the needle is found in the haystack or the end of the haystack is
// encountered.  The time complexity is 'O(M * N)', where 'M' is the length of
// the needle and 'N' is the length of the haystack.
//
// There are more sophisticated algorithms available; however, those typically
// require creating and retaining metadata derived from the needle and/or
// haystack.  If the needle is short and the haystack of moderate length, the
// naive algorithm may be faster than generating that metadata, especially if
// the search is one-time and the metadata cost cannot be amortized.
//
// Another advantage of the "default" searcher is that it accepts (relatively
// simple) *ForwardIterator*s whereas more sophisticated algorithms typically
// require (more fully-featured) *RandomIterator*s.
//
///Iterator Requirements
///---------------------
// The two independent iterator types associated with this class -- one
// defining the needle, the other defining the haystack -- must meet the
// requirements of *ForwardIterator*.
//
// Additionally:
//: o These iterators can be constant.
//: o When dereferenced, they must both refer to the same value type.
//
// Either of the iterator types are allowed to throw exceptions.
//
// Iterators defining needles are required to remain valid as long as the
// searcher object might be used.
//
///Comparer Functor Requirements
///-----------------------------
// The comparer class must meet the requirements of *BinaryPredicate*:
//: o The class defines an 'operator()' method, which, given an
//:   *ForwardIterator*, 'iterator', may be invoked as
//:   'operator()(*iterator, *iterator)'.
//: o The return value must be contextually convertible to 'bool'.
//: o The supplied iterators can be constant.
//: o The class must be copyable.
//
// The comparer class is allowed to throw exceptions.
//
///Optimizations for 'bslstl::DefaultSearcher'
///-------------------------------------------
// For certain template parameters this implementation improves performance by
// utilizing low-level operations.  The requirements to do so are:
//: o Both supplied iterators are pointers.
//: o The default equality comparison is allowed to default (to
//:   'bsl::equal_to<value_type>').
//: o The 'value_type' is bitwise-equality-comparable.
//
// Users supplying their own data types are advised to set the
// 'bslmf::IsBitwiseEqualityComparable' trait when applicable so that
// 'bslstl::DefaultSearcher' knows that the last optimization condition listed
// above is met.  See {'bslmf_isbitwiseequalitycomparable'}.
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
// First, we obtain the text of document and word of interest as sequences of
// 'char' values.
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
// Then, we create a 'bsl::default_searcher' object (a functor) using the given
// 'word':
//..
//  bsl::default_searcher<const char*> searchForUnited(
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
//  bsl::size_t offset = result.first - document;
//
//  assert(120 == offset);
//  assert(static_cast<bsl::size_t>(result.second - result.first)
//             == bsl::strlen(word));
//..
// Finally, we notice that search correctly ignored the appearance of the word
// "united" (all lower case) in the second sentence.
//
// {'bslstl_boyermoorehorspoolsearcher'|Example 1} shows how the same problem
// is addressed using 'bsl::boyer_moore_horspool_searcher'.
//
///Example 2: Defining a Comparator
/// - - - - - - - - - - - - - - - -
// As seen in {Example 1} above, the default equality comparison functor is
// case sensitive.  If one needs case *in*-sensitive searches, a non-default
// equality comparison class must be specified.
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
// Then, define a new 'bsl::default_searcher' type and create a searcher object
// to search for 'word':
//..
//  bsl::default_searcher<const char *,
//                        struct MyCaseInsensitiveCharComparer>
//                                                  searchForUnitedInsensitive(
//                                                  word,
//                                                  word + bsl::strlen(word));
//..
// Note that the new searcher object will used a default constructed
// 'MyCaseInsensitiveCharComparer' class.  If a equality comparison object
// requires state supplied on construction, such an object be explicitly
// created and supplied as the final constructor argument.
//
// Now, we invoke our new functor, specifying that the same document searched
// in {Example 1}:
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
// {'bslstl_boyermoorehorspoolsearcher'|Example 2} shows how the same problem
// is addressed using 'bsl::boyer_moore_horspool_searcher'.
//
///Example 3: Non-'char' Searches
/// - - - - - - - - - - - - - - -
// The "default" searcher class template is not constrained to searching for
// 'char' values.  Searches can be done on other types (see {Iterator
// Requirements}).  Moreover the container of the sequence being sought (the
// "needle") need not the same as the sequence being searched (the "haystack").
//
// Suppose one has data from an instrument that reports 'float' values and that
// inserts the sequence '{ FLT_MAX, FLT_MIN, FLT_MAX }' as a marker for the
// start and end of a test run.  We can assume the probably of the instrument
// reporting this sequence as readings is negligible and that data reported
// outside of the test runs is random noise.  Here is how we can search for the
// first test run data in the data sequence.
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
//  bsl::list<float> data;  // Container provides bidirectional iterators.
//  doTestRun(&data);
//..
// Then, we define and create our searcher object:
//..
//  bsl::default_searcher<const float *> searchForMarker(markerSequence,
//                                                       markerSequence
//                                                     + markerSequenceLength);
//..
// Notice that no equality comparison functor was specified so
// 'searchForMarker' will use 'bsl::equal_to<float>' by default.
//
// Now, we invoke our searcher on the instrument data.
//..
//  typedef bsl::list<float>::const_iterator DataConstItr;
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
//
// {'bslstl_boyermoorehorspoolsearcher'|Example 3} shows how the same problem
// is addressed using 'bsl::boyer_moore_horspool_searcher'.  Notice that other
// example uses 'data' from a container that provides random access iterators;
// whereas here, bidirectional iterators are used (and forward iterators would
// have sufficed).

#include <bslscm_version.h>

#include <bslstl_equalto.h>
#include <bslstl_iterator.h>
#include <bslstl_pair.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>  // for 'BSLS_KEYWORD_CONSTEXPR'
#include <bsls_performancehint.h>

#include <cstring>  // for 'native_std::memcmp'
#include <utility>  // for 'native_std::make_pair'

namespace BloombergLP {
namespace bslstl {

                        // =====================
                        // class DefaultSearcher
                        // =====================

template <class FORWARD_ITR_NEEDLE,
          class EQUAL = bsl::equal_to<
               typename bsl::iterator_traits<FORWARD_ITR_NEEDLE>::value_type> >
class DefaultSearcher {
    // This class template defines functors that can search for the sequence of
    // 'value_type' values defined on construction (i.e., the "needle") in
    // sequences of 'value_type' values (i.e., "haystacks") passed to the
    // functor's 'operator()'.

    // PRIVATE TYPES
    typedef typename bsl::iterator_traits<FORWARD_ITR_NEEDLE>::value_type
                                                               value_type;
        // the type of the values that can be obtained by dereferencing a
        // 'FORWARD_ITR_NEEDLE' iterator

    typedef typename bsl::iterator_traits<FORWARD_ITR_NEEDLE>::
                                                             iterator_category
                                                              IteratorCategory;
    // DATA
    FORWARD_ITR_NEEDLE d_needleFirst;
    FORWARD_ITR_NEEDLE d_needleLast;
    EQUAL              d_equal;

  public:
    // CREATORS
    DefaultSearcher(FORWARD_ITR_NEEDLE needleFirst,
                    FORWARD_ITR_NEEDLE needleLast,
                    EQUAL              equal = EQUAL());
        // Create a 'DefaultSearcher' object that can search for the sequence
        // of 'value_type' values found in the specified range
        // '[needleFirst, needleLast)'.  Optionally supply a 'equal' functor
        // for use by 'operator()'.  The behavior is undefined unless
        // 'needleFirst' can be advanced to equal 'needleLast'.

    //! DefaultSearcher(const DefaultSearcher& original) = default;
        // Create a 'DefaultSearcher' object that has the same state as the
        // specified 'original' object.

    //! DefaultSearcher(DefaultSearcher&& original) = default;
        // Create a 'boyer_moore_horspool_searcher' object having the same
        // state as the specified 'original' on entry.  The 'original' object
        // is left in an unspecified (valid) state.

    //! ~DefaultSearcher() = default;
        // Destroy this 'DefaultSearcher' object.

    // MANIPULATORS
    //! DefaultSearcher& operator=(const DefaultSearcher& rhs) = default;
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this object.

    //! DefaultSearcher& operator=(DefaultSearcher&& rhs) = default;
        // Assign to this object the state of the specified 'rhs' had on entry
        // and return a non-'const' reference to this object.  The 'original'
        // object is left in an unspecified (valid) state.

    // ACCESSORS
    template<class FORWARD_ITR_HAYSTACK>
    bsl::pair<FORWARD_ITR_HAYSTACK, FORWARD_ITR_HAYSTACK> operator()(
                                    FORWARD_ITR_HAYSTACK haystackFirst,
                                    FORWARD_ITR_HAYSTACK haystackLast) const;
        // Search the specified range '[haystackFirst, haystackLast)' for the
        // first sequence of 'value_type' values specified on construction.
        // Return the range where those values are found, or the range
        // '[haystackLast, haystackLast)' if that sequence is not found.  The
        // search is performed using a "naive" algorithm that has time
        // complexity of:
        //..
        //    bsl::distance(needleFirst(), needleLast())
        //  * bsl::distance(haystackFirst, haystackLast);
        //..
        // Values of the "needle" sequence and the "haystack" sequence are
        // compared using the equality comparison functor specified on
        // construction.  The behavior is undefined unless 'haystackFirst' can
        // be advanced to equal 'haystackLast' and the iterators used to
        // construct this object, 'needleFirst()' and 'needleLast()', are still
        // valid.  Note that if the "needle" sequence is empty, the range
        // '[haystackFirst, haystackFirst)' is returned.  Also note that if the
        // "needle" sequence is longer than the "haystack" sequence -- thus,
        // impossible for the "needle" to be found in the "haystack" -- the
        // range '[haystackLast, haystackLast)' is returned.

                        // Non-Standard Accessors

    FORWARD_ITR_NEEDLE needleFirst() const;
        // Return an iterator referring to the first element of the sequence of
        // 'value_type' values that can be sought by this searcher object.

    FORWARD_ITR_NEEDLE needleLast() const;
        // Return an iterator referring to one past the last element of the
        // sequence of 'value_type' values that can be sought by this searcher
        // object.

    EQUAL equal() const;
        // Return the functor used by this searcher object to compare
        // 'value_type' values.
};

                        // ===================================
                        // struct DefaultSearcher_CanOptimize
                        // ===================================

template <class FORWARD_ITR_NEEDLE,
          class EQUAL,
          class FORWARD_ITR_HAYSTACK>
struct DefaultSearcher_CanOptimize {
    // This component-private meta-function 'struct' provides a member
    // enumerator 'value' that is 'true' if all of the specified
    // 'FORWARD_ITR_NEEDLE,' 'EQUAL,' and 'FORWARD_ITR_HAYSTACK' meet the
    // criteria for an optimization of the default searcher, and has the value
    // 'false' otherwise.

    enum {

        value = (

        bsl::is_pointer<FORWARD_ITR_NEEDLE>::value
    &&  bsl::is_pointer<FORWARD_ITR_HAYSTACK>::value
    &&  bsl::is_same<EQUAL, // 'EQUAL' does 'value_type::operator=='
                     bsl::equal_to<
                          typename
                          bsl::iterator_traits<FORWARD_ITR_NEEDLE>::value_type>
                    >::value
    &&  bslmf::IsBitwiseEqualityComparable<FORWARD_ITR_NEEDLE>::value
    &&  bslmf::IsBitwiseEqualityComparable<FORWARD_ITR_HAYSTACK>::value
        )
    };
};

                        // ===============================
                        // struct DefaultSearcher_ImpUtil
                        // ===============================

struct DefaultSearcher_ImpUtil {
    // This component-private utility 'struct' provides two mutually exclusive
    // overloads of the 'doSearch' method -- i.e., just one of the two methods
    // is enabled at any time.  Enablement is decided by the
    // 'DefaultSearcher_CanOptimize' meta-function.

    template <class FORWARD_ITR_NEEDLE,
              class EQUAL,
              class FORWARD_ITR_HAYSTACK>
    static
    typename
    bsl::enable_if< DefaultSearcher_CanOptimize<FORWARD_ITR_NEEDLE,
                                                  EQUAL,
                                                  FORWARD_ITR_HAYSTACK>::value
                  , bsl::pair<FORWARD_ITR_HAYSTACK,
                              FORWARD_ITR_HAYSTACK>
    >::type doSearch(const FORWARD_ITR_HAYSTACK& haystackFirst,
                     const FORWARD_ITR_HAYSTACK& haystackLast,
                     const FORWARD_ITR_NEEDLE&   needleFirst,
                     const FORWARD_ITR_NEEDLE&   needleLast,
                     const EQUAL&                equal);

    template <class FORWARD_ITR_NEEDLE,
              class EQUAL,
              class FORWARD_ITR_HAYSTACK>
    static
    typename
    bsl::enable_if<!DefaultSearcher_CanOptimize<FORWARD_ITR_NEEDLE,
                                                  EQUAL,
                                                  FORWARD_ITR_HAYSTACK>::value
                  , bsl::pair<FORWARD_ITR_HAYSTACK,
                              FORWARD_ITR_HAYSTACK>
    >::type doSearch(const FORWARD_ITR_HAYSTACK& haystackFirst,
                     const FORWARD_ITR_HAYSTACK& haystackLast,
                     const FORWARD_ITR_NEEDLE&   needleFirst,
                     const FORWARD_ITR_NEEDLE&   needleLast,
                     const EQUAL&                equal);
        // Search the specified "haystack" sequence of 'value_type' values,
        // '[haystackFirst, hastackLast)', for the specified "needle" sequence
        // of 'value_type' values, '[needleFirst, needleLast)' where the
        // 'value_type' values are compared using the specified 'equal'
        // functor.  Return the range where the sought sequence of values are
        // found, or the range '[haystackLast, haystackLast)' if that sequence
        // is not found.  The search is performed using a "naive" algorithm
        // that has time complexity of:
        //..
        //    bsl::distance(needleFirst(), needleLast())
        //  * bsl::distance(haystackFirst, haystackLast);
        //..
        // Values of the "needle" sequence and the "haystack" sequences are
        // compared using the equality comparison functor specified on
        // construction except, possibly, if the 'DefaultSearcher_CanOptimize'
        // metafunction indicates that the template parameters are eligible for
        // optimization.  The optimized overload is enabled when needle and
        // haystack can be validly compared using 'native_std::memcmp', a
        // low-level function that is often highly optimized for its platform.
        // The behavior is undefined unless 'haystackFirst' can be advanced to
        // equal 'haystackLast'.  Note that if the "needle" sequence is empty,
        // the range '[haystackFirst, haystackFirst)' is returned.  Also note
        // that if the "needle" sequence is longer than the "haystack" sequence
        // -- thus, impossible for the "needle" to be found in the "haystack"
        // -- the range '[haystackLast, haystackLast)' is returned.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                        // ======================
                        // class default_searcher
                        // ======================

template<class ForwardIterator1,
         class BinaryPredicate = equal_to<
            typename bsl::iterator_traits<ForwardIterator1>::value_type> >
class default_searcher {
    // This class template defines functors that can search for the sequence of
    // 'value_type' values defined on construction in sequences of 'value_type'
    // values passed to the functor's 'operator()'.

    //DATA
    BloombergLP::bslstl::DefaultSearcher<ForwardIterator1,
                                         BinaryPredicate> d_imp;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    default_searcher(ForwardIterator1 pat_first,
                     ForwardIterator1 pat_last,
                     BinaryPredicate  pred = BinaryPredicate());
        // Create a 'default_searcher' object that can search for the sequence
        // of 'value_type' values found in the specified range
        // '[pat_first, pat_last)'.  Optionally supply a 'pred' functor for use
        // by 'operator()'.  See {Comparer Functor Requirements}.  The behavior
        // is undefined unless 'pat_first' can be advanced to equal 'pat_last'.

    //! default_searcher(const default_searcher& original) = default;
        // Create a 'default_searcher' object having same state as the
        // specified 'original' object.

    //! default_searcher(BloombergLP::bslmf::MovableRef<default_searcher>
    //!                                                    original) = default;
        // Create a 'default_searcher' object having same state as
        // the specified 'original' object. by moving (in constant time) the
        // state of 'original' to the new searcher.  The 'original' object is
        // left in an unspecified (valid) state.

    //! ~default_searcher() = default;
        // Destroy this 'default_searcher' object.

    // MANIPULATORS
    //! default_searcher& operator=(const default_searcher& rhs) = default;
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this searcher.

    //! default_searcher& operator=(
    //!                        BloombergLP::bslmf::MovableRef<default_searcher>
    //!                                                         rhs) = default;
        // Assign to this object the state of the specified 'rhs' object and
        // return a non-'const' reference to this searcher.

    // ACCESSORS
    template<class ForwardIterator2>
    BSLS_KEYWORD_CONSTEXPR
    pair<ForwardIterator2, ForwardIterator2> operator()(
                                                  ForwardIterator2 first,
                                                  ForwardIterator2 last) const;
        // Search the specified range '[first, last)' for the first sequence of
        // 'value_type' values specified on construction.  Return the range
        // where those values are found, or the range '[last, last)' if that
        // sequence is not found.  The search is performed using a "naive"
        // algorithm that has time complexity of:
        //..
        //    bsl::distance(pat_first, pat_last) * bsl::distance(first, last);
        //..
        // Values of the two sequences are compared using the equality
        // comparison functor specified on construction.  The behavior is
        // undefined unless 'first' can be advanced to equal 'last' and the
        // iterators used to construct this object are still valid.  Note that
        // if the sought sequence is empty, the range '[first, first)' is
        // returned.  Also note that if the sequence sought is longer than the
        // sequence searched -- thus the sought sequence cannot be found -- the
        // range '[last, last)' is returned.
};

}  // close namespace 'bsl'

// ----------------------------------------------------------------------------
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslstl {

                        // ---------------------
                        // class DefaultSearcher
                        // ---------------------

//CREATORS
template  <class FORWARD_ITR_NEEDLE,
           class EQUAL>
inline
DefaultSearcher<FORWARD_ITR_NEEDLE,
                EQUAL>::
DefaultSearcher(FORWARD_ITR_NEEDLE needleFirst,
                FORWARD_ITR_NEEDLE needleLast,
                EQUAL              equal)
: d_needleFirst(needleFirst)
, d_needleLast( needleLast)
, d_equal(equal)
{
    BSLS_ASSERT(0 <= bsl::distance(needleFirst, needleLast));
}

// ACCESSORS
template <class FORWARD_ITR_NEEDLE,
          class EQUAL>
template <class FORWARD_ITR_HAYSTACK>
inline
bsl::pair<FORWARD_ITR_HAYSTACK, FORWARD_ITR_HAYSTACK>
DefaultSearcher<FORWARD_ITR_NEEDLE, EQUAL>::operator()(
                                       FORWARD_ITR_HAYSTACK haystackFirst,
                                       FORWARD_ITR_HAYSTACK haystackLast) const
{
    BSLMF_ASSERT((bsl::is_same<
               typename bsl::iterator_traits<FORWARD_ITR_NEEDLE  >::value_type,
               typename bsl::iterator_traits<FORWARD_ITR_HAYSTACK>::value_type
                             >::value));

    return BloombergLP::bslstl::
           DefaultSearcher_ImpUtil::doSearch<FORWARD_ITR_NEEDLE,
                                              EQUAL,
                                              FORWARD_ITR_HAYSTACK>(
                                                                 haystackFirst,
                                                                 haystackLast,
                                                                 d_needleFirst,
                                                                 d_needleLast,
                                                                 d_equal);

}

template <class FORWARD_ITR_NEEDLE, class EQUAL>
inline
FORWARD_ITR_NEEDLE DefaultSearcher<FORWARD_ITR_NEEDLE, EQUAL>::needleFirst()
                                                                          const
{
    return d_needleFirst;
}

template <class FORWARD_ITR_NEEDLE, class EQUAL>
inline
FORWARD_ITR_NEEDLE DefaultSearcher<FORWARD_ITR_NEEDLE, EQUAL>::needleLast()
                                                                          const
{
    return d_needleLast;
}

template <class FORWARD_ITR_NEEDLE, class EQUAL>
inline
EQUAL DefaultSearcher<FORWARD_ITR_NEEDLE, EQUAL>::equal() const
{
    return d_equal;
}

                        // -----------------------------
                        // class DefaultSearcher_ImpUtil
                        // -----------------------------

// CLASS METHODS
template <class FORWARD_ITR_NEEDLE, class EQUAL, class FORWARD_ITR_HAYSTACK>
inline
typename
bsl::enable_if<
    ! DefaultSearcher_CanOptimize<FORWARD_ITR_NEEDLE,
                                   EQUAL,
                                   FORWARD_ITR_HAYSTACK>::value
    , bsl::pair<FORWARD_ITR_HAYSTACK, FORWARD_ITR_HAYSTACK>
>::type DefaultSearcher_ImpUtil::doSearch(
                                     const FORWARD_ITR_HAYSTACK& haystackFirst,
                                     const FORWARD_ITR_HAYSTACK& haystackLast,
                                     const FORWARD_ITR_NEEDLE&   needleFirst,
                                     const FORWARD_ITR_NEEDLE&   needleLast,
                                     const EQUAL&                equal)
{
    BSLS_ASSERT(0 <= bsl::distance(haystackFirst, haystackLast));

    for (FORWARD_ITR_HAYSTACK itrHaystackOuter  = haystackFirst;
                              itrHaystackOuter != haystackLast;
                            ++itrHaystackOuter) {

        FORWARD_ITR_HAYSTACK itrHaystackInner = itrHaystackOuter;

        for (FORWARD_ITR_NEEDLE itrNeedle = needleFirst;
                              /* tests below */ ;
                              ++itrNeedle, ++itrHaystackInner) {

            if (needleLast == itrNeedle) {         // Needle match in haystack!
                return native_std::make_pair(itrHaystackOuter,
                                             itrHaystackInner);       // RETURN
            }

            if (haystackLast == itrHaystackInner) {     // Hit end of haystack.
                return native_std::make_pair(haystackLast,
                                             haystackLast);           // RETURN
            }

            if (equal(*itrHaystackInner, *itrNeedle)) {
                continue;  // Needle match still possible.
            } else {
                break;     // Move starting point in haystack and try again.
            }
        }
    }

    // Ran out of haystack without match.

    return native_std::make_pair(haystackLast, haystackLast);
}

template <class FORWARD_ITR_NEEDLE, class EQUAL, class FORWARD_ITR_HAYSTACK>
inline
typename
bsl::enable_if<
      DefaultSearcher_CanOptimize<FORWARD_ITR_NEEDLE,
                                   EQUAL,
                                   FORWARD_ITR_HAYSTACK>::value
    , bsl::pair<FORWARD_ITR_HAYSTACK, FORWARD_ITR_HAYSTACK>
>::type DefaultSearcher_ImpUtil::doSearch(
                                     const FORWARD_ITR_HAYSTACK& haystackFirst,
                                     const FORWARD_ITR_HAYSTACK& haystackLast,
                                     const FORWARD_ITR_NEEDLE&   needleFirst,
                                     const FORWARD_ITR_NEEDLE&   needleLast,
                                     const EQUAL&                )
{
    ///Implementation Note
    ///-------------------
    // This specialization is used only when the 'EQUAL' template parameter
    // was specified as 'bsl::equal_to<value_type>', the default value.  We
    // ignore that argument and perform its "moral equivalent" for the various
    // ranges using 'native_std::memcmp'.

    typedef typename bsl::iterator_traits<FORWARD_ITR_HAYSTACK>::
                                                               difference_type
                                                            HaystackDifference;

    typedef typename bsl::iterator_traits<FORWARD_ITR_NEEDLE>::difference_type
                                                              NeedleDifference;

    const   NeedleDifference   needleLength =   needleLast -   needleFirst;
    const HaystackDifference haystackLength = haystackLast - haystackFirst;

    BSLS_ASSERT(0 <=   needleLength);
    BSLS_ASSERT(0 <= haystackLength);

    if (haystackLength < needleLength) {                       // Cannot match.
        return native_std::make_pair(haystackLast, haystackLast);     // RETURN
    }

    if (0 == needleLength || 0 == haystackLength) {
        return native_std::make_pair(haystackFirst, haystackFirst);   // RETURN
    }

    for (FORWARD_ITR_HAYSTACK itr = haystackFirst;
                              itr < haystackLast - needleLength + 1; ++itr) {

            FORWARD_ITR_HAYSTACK itrInner          = itr;
            FORWARD_ITR_NEEDLE   needleItrInner    = needleFirst;
            NeedleDifference     needleLengthInner = needleLength;

            if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(*itr == *needleFirst)) {
                if (1 == needleLength) {
                    return native_std::make_pair(itr, itr + needleLength);
                                                                      // RETURN
                }
                           ++itrInner;
                     ++needleItrInner;
                  --needleLengthInner;
            } else {
                continue; // Avoided 'memcmp' call
            }

            if (0 == native_std::memcmp(         itrInner,
                                           needleItrInner,
                                        needleLengthInner)) {
                return native_std::make_pair(itr, itr + needleLength);
                                                                      // RETURN
            }
    }

    // Ran out of haystack without match.
    return native_std::make_pair(haystackLast, haystackLast);
}

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
                        // ----------------------
                        // class default_searcher
                        // ----------------------

// CREATORS
template <class ForwardIterator1,
          class BinaryPredicate>
inline
BSLS_KEYWORD_CONSTEXPR
default_searcher<ForwardIterator1,
                 BinaryPredicate>::default_searcher(ForwardIterator1 pat_first,
                                                    ForwardIterator1 pat_last,
                                                    BinaryPredicate  pred)
: d_imp(pat_first, pat_last, pred)
{
}

        // ACCESSORS
template <class ForwardIterator1,
          class BinaryPredicate>
template <class ForwardIterator2>
inline
BSLS_KEYWORD_CONSTEXPR
pair<ForwardIterator2,
     ForwardIterator2> default_searcher<ForwardIterator1,
                                        BinaryPredicate>::operator()(
                                                   ForwardIterator2 first,
                                                   ForwardIterator2 last) const
{
    return d_imp(first, last);
}

}  // close namespace 'bsl'


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
