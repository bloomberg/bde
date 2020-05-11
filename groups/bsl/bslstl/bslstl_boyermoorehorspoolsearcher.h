// bslstl_boyermoorehorspoolsearcher.h                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_BOYERMOOREHORSPOOLSEARCHER
#define INCLUDED_BSLSTL_BOYERMOOREHORSPOOLSEARCHER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL-compliant 'boyer_moore_horspool_searcher' class.
//
//@CLASSES:
//  bsl::boyer_moore_horspool_searcher: class template to search via BMH
//  bslstl::BoyerMooreHorspoolSearcher: class template to search via BMH
//
//@SEE_ALSO: bslstl_defaultsearcher
//
//@DESCRIPTION: This component defines two class templates,
// 'bsl::boyer_moore_horspool_searcher' and
// 'bslstl::BoyerMooreHorspoolSearcher'.  Both are compliant with section
// '[func.search.bmh]' of the C++ Standard (C++17 and later).
//
// 'bsl::boyer_moore_horspool_searcher' is strictly limited to the Standard and
// is provided for clients for whom standard compliance is a priority.
// 'bslslt::BoyerMoreHorspoolSearcher' provides several accessors that are not
// mentioned in the Standard.  Moreover, 'bslslt::BoyerMoreHorspoolSearcher' is
// "plumbed" for BDE allocators and can be used with BDE standard containers
// whereas the compliant strict class always uses the currently installed
// default allocator.  See {Example 4} below.
//
// Except where there is a relevant difference, both are described below as if
// they were one.
//
// This class has several template parameters:
//
//: 'RNDACC_ITR_NEEDLE':
//:    The type used to specify (on construction) the range of values
//:    being sought (the "needle").
//:
//: 'HASH':
//:    The functor type used to hash metadata for the unique values of
//:    the needle.  See {Requirements for 'HASH' and 'EQUAL'}.
//:
//: 'EQUAL':
//:    The functor type used to compare values when storing/accessing needle
//:    metadata.  See {Requirements for 'HASH' and 'EQUAL'}.
//
// The class also provides a functor-style interface that accepts two iterators
// that define the range of values to be searched (the "haystack").  Once
// constructed, a single searcher object can be re-used to search multiple
// haystacks (for the same needle value).
//
// The iterators defining the haystack need not be of the same type as those
// that define the needle.  Moreover, the search method of the searcher can be
// overloaded for an arbitrary number of different haystack iterators (subject
// to {Iterator Requirements}).
//
///Algorithm
///---------
// The search algorithm used is an implementation of the well-known Boyer,
// Moore, Horspool (BMH) Algorithm for string matching (see
// https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm).
// In the typical case, this algorithm offers complexity of 'O(N)' for a
// haystack of length 'N'.
//
///Iterator Requirements
///---------------------
// The two independent iterators types associated with this class -- one
// defining the needle, the other defining the haystack -- must meet the
// requirements of *RandomAccessIterator*.
//
// Additionally:
//: o The iterators can be constant.
//: o When dereferenced, both iterator types must refer to the same value type.
//
// The operations of either of the iterator types are allowed to throw
// exceptions.
//
// Iterators defining needles are required to remain valid as long as the
// searcher object might be used.
//
///Requirements for 'HASH' and 'EQUAL'
///-----------------------------------
// The comparer class, 'EQUAL', must meet the requirements of
// *BinaryPredicate*:
//: o The class defines an 'operator()' method that, given a
//:   *RandomAccessIterator', 'iterator', can be invoked as
//:   'operator()(*iterator, *iterator)'.
//: o The return value must be contextually convertible to 'bool'.
//: o The supplied iterators can be constant.
//: o The class must be copyable.
//
// Comparer classes are allowed to throw exceptions.
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
///Optimizations for 'bslstl::BoyerMooreHorspoolSearcher'
///------------------------------------------------------
// This implementation handles needle metadata using a fixed size array when
// the 'value_type' is 'char' (either 'signed' or 'unsigned' flavors).  For
// needles of typical size, this choice results in somewhat more memory use
// than it would have if some dynamically sized container were used; however,
// the faster access during searches warrants the tradeoff.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The problem of searching a sequence of characters for a particular
// sub-sequence arises in many applications.  For example, one might need to
// know if some document contains a particular word of interest.  For example,
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
// Then, we create a 'default_searcher' object (a functor) using the given
// 'word':
//..
//  bsl::boyer_moore_horspool_searcher<const char *> searchForUnited(
//                                                          word,
//                                                          word
//                                                        + bsl::strlen(word));
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
// Finally, we notice that the search correctly ignored the appearance of the
// word "united" (all lower case) in the second sentence.
//
// {'bslstl_defaultsearcher'|Example 1} shows how the same problem is addressed using
// 'bsl::default_searcher'.
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
// values, irrespective of their case, hash to the same value:
//..
//  struct MyCaseInsensitiveCharHasher {
//      bool operator()(const char& value) const {
//          static bsl::hash<char> s_hash;
//          return s_hash(static_cast<char>(bsl::tolower(value)));
//      }
//  };
//..
// Now, specify 'bsl::boyer_moore_horspool_searcher' type for and create a
// searcher object to search for 'word':
//..
//  bsl::boyer_moore_horspool_searcher<const char *,
//                                     MyCaseInsensitiveCharHasher,
//                                     MyCaseInsensitiveCharComparer>
//                                                  searchForUnitedInsensitive(
//                                                          word,
//                                                          word
//                                                        + bsl::strlen(word));
//..
// Note that the new searcher object will use defaulted constructed
// 'MyCaseInsensitiveCharHasher' and 'MyCaseInsensitiveCharComparer' classes.
// If stateful functors are required such objects can be passed in the optional
// constructor arguments.
//
// Now, we invoke our searcher functor, specifying that the same document
// searched in {Example 1}:
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
// {'bslstl_defaultsearcher'|Example 2} shows how the same problem is addressed using
// 'bsl::default_searcher'.
//
///Example 3: Non-'char' Searches
/// - - - - - - - - - - - - - - -
// The BMH searcher class template is not constrained to searching for 'char'
// values.  Searches can be done on other types (see {Iterator Requirements}).
// Moreover the container of the sequence being sought (the "needle") need not
// the same as the sequence being searched (the "haystack").
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
//  bsl::vector<float> data;  // Container provides random access iterators.
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
// {'bslstl_defaultsearcher'|Example 3} shows how the same problem is addressed
// using 'bsl::default_searcher'.  Notice that other example uses 'data' from a
// container that provides bidirectional iterators (and forward iterators would
// have sufficed), whereas here random access iterators are required.
//
///Example 4: Caching Searcher Objects
///- - - - - - - - - - - - - - - - - -
// The construction of 'bsl::boyer_moore_horspool_searcher' objects is small
// (the needle must be scanned, meta-data calculated, and results saved) but
// can be non-neglibible when one needs a great number of them.  When there is
// a reasonable chance that one will have to repeat a given search, it can be
// worthwhile to cache the searcher objects for reuse.
//
///The Problem
/// -  -  -  -
// Suppose we have a long list of names, each consisting of a given name (first
// name) and a surname (last name), and that we wish to identify instances of
// reduplication of the given name in the surname.  That is, we want to
// identify the cases where the given name is a *case-insensitive* substring of
// the surname.  Examples include "Durand Durand", "James Jameson", "John St.
// John", and "Jean Valjean".  In this example we will not accept nicknames and
// other approximate name forms as matches (e.g., "Joe Joseph", "Jack
// Johnson").
//
// Since we want to perform our task as efficiently as possible, and since we
// expect many entries to have common given names (e.g., "John"), we decide to
// create a cache of searcher objects for those first names so they need not be
// reconstructed for each search of a surname.
//
///Design Choices
/// -  -  -  -  -
// To implement our cache we will use a 'bsl::unordered_map' container.
// Allocating types must meet certain requirements to work properly with
// allocator-enabled containers such as 'bsl::unordered_map'.
// 'bsl::boyer_moore_horspool_searcher' does not, so we will use
// 'bslstl::BoyerMooreHorspoolSearcher', which does.
//
// To clarify exposition, our cache will have the simple policy of retaining
// searcher objects indefinitely and ignore the real-world concern that our
// cache may grow so large that the search time exceeds construction time.
// Also, we will forgo techniques that might minimize the number of times data
// is copied.
//
///Steps
/// -  -
// First, we define our cache class:
//..
//                      // ====================================
//                      // class MyCaseInsensitiveSearcherCache
//                      // ====================================
//
//  class MyCaseInsensitiveSearcherCache {
//
//      // TYPES
//    public:
//      typedef bslstl::BoyerMooreHorspoolSearcher<
//                                               bsl::string::const_iterator,
//                                               MyCaseInsensitiveCharHasher,
//                                               MyCaseInsensitiveCharComparer>
//                                                                    Searcher;
//      // PRIVATE TYPES
//    private:
//      typedef bsl::unordered_map<bsl::string, Searcher> Map;
//
//      // DATA
//      Map d_map;
//
//      // PRIVATE MANIPULATORS
//      const Searcher& insertSearcher(const bsl::string& key);
//          // Insert into this cache a key-value pair where the key is the
//          // specified 'key' and the value is a 'Searcher' object created to
//          // seek the needle specified by the key part.  Note that this
//          // arrangement guarantees that the iterators used by this cached
//          // searcher object remain valid for the life of the searcher
//          // object.
//
//    public:
//      // CREATORS
//      explicit MyCaseInsensitiveSearcherCache(bslma::Allocator
//                                                        *basicAllocator = 0);
//          // Create an empty 'MyCaseInsensitiveSearcherCache' object.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default
//          // allocator is used.
//
//      // MANIPULATORS
//      const Searcher& getSearcher(const char *needle);
//          // Return a 'const'-reference to the cached server that can do a
//          // case-insensitive search for the specified 'needle'.  If such a
//          // searcher does not exist in the cache on entry, such a searcher
//          // is constructed, added to the cache, and returned (by
//          // 'const'-reference).
//
//      // ACCESSORS
//      bsl::size_t numSearchers() const;
//          // Return the number of searcher objects in this cache.
//  };
//..
// Notice (see the 'typedef' for 'Searcher') that we reuse the hash functor,
// 'MyCaseInsensitiveCharHasher', and equality comparison functor,
// 'MyCaseInsensitiveCharComparer', that were defined in {Example 2}.
//
// Note that 'MyCaseInsensitiveSearcherCache' itself is an allocating type.
// If we needed to make it compatible with BDE containers (e.g., to allow a
// cache of caches) a few additional features are needed.  As we have no such
// need, those features are deferred.
//
// Then, we implement the constructor:
//..
//                      // ------------------------------------
//                      // class MyCaseInsensitiveSearcherCache
//                      // ------------------------------------
//
//  // CREATORS
//  MyCaseInsensitiveSearcherCache::
//  MyCaseInsensitiveSearcherCache(bslma::Allocator *basicAllocator)
//  : d_map(basicAllocator)
//  {
//  }
//..
//
// Notice that 'basicAllocator' is simply forwarded to 'd_map'.
//
// Next, we implement the public methods:
//..
//  // MANIPULATORS
//  const
//  MyCaseInsensitiveSearcherCache::Searcher&
//  MyCaseInsensitiveSearcherCache::getSearcher(const char *needle)
//  {
//      bsl::string   key(needle);
//      Map::iterator findResult = d_map.find(key);
//
//      if (d_map.end() == findResult) {
//          return insertSearcher(key);                               // RETURN
//      } else {
//          return findResult->second;                                // RETURN
//      }
//  }
//
//  // ACCESSORS
//  bsl::size_t MyCaseInsensitiveSearcherCache::numSearchers() const
//  {
//      return d_map.size();
//  }
//..
// Then, to complete our class, we implement the cache class private method:
//..
//  // PRIVATE MANIPULATORS
//  const
//  MyCaseInsensitiveSearcherCache::Searcher&
//  MyCaseInsensitiveSearcherCache::insertSearcher(const bsl::string& key)
//  {
//      Searcher        dummy(key.begin(), key.begin()); // to be overwritten
//      Map::value_type value(key, dummy);
//
//      bsl::pair<Map::iterator, bool> insertResult = d_map.insert(value);
//      assert(true == insertResult.second);
//
//      Map::iterator iterator = insertResult.first;
//
//      iterator->second = Searcher(iterator->first.begin(),
//                                  iterator->first.end());
//      return iterator->second;
//  }
//..
// Notice creating our element is a two step process.  First, we insert the key
// with an arbitrary "dummy" searcher.  Once the key (a string) exists in the
// map (at an address that is stable for the life of the map) we create a
// searcher object that refers to that key string for its search sequence, and
// overwrite the "dummy" part of previously inserted element.
//
// Now, we show how the searcher object cache can be used.  In this example, a
// fixed array represents our source of name entries, in random order:
//..
//  struct {
//      const char *d_givenName_p;
//      const char *d_surname_p;
//  } DATA[] = {
//      // GIVEN     SURNAME
//      // --------  ------------
//      { "Donald" , "McDonald"    }
//    , { "John"   , "Johnson"     }
//    , { "John"   , "Saint Johns" }
//    , { "Jon"    , "Literjon"    }
//    , { "Jean"   , "Valjean"     }
//    , { "James"  , "Jameson"     }
//    , { "Will"   , "Freewill"    }
//    , { "John"   , "Johns"       }
//    , { "John"   , "John"        }
//    , { "John"   , "Jones"       }
//    , { "J'onn"  , "J'onzz"      }
//    , { "Donald" , "Donalds"     }
//    , { "Donald" , "Mac Donald"  }
//    , { "William", "Williams"    }
//    , { "Durand" , "Durand"      }
//    , { "John"   , "Johnstown"   }
//    , { "Major"  , "Major"       }
//    , { "Donald" , "MacDonald"   }
//    , { "Patrick", "O'Patrick"   }
//    , { "Chris",   "Christie"    }
//    , { "Don",     "London"      }
//      // ...
//    , { "Ivan"   , "Ivanovich"   }
//  };
//
//  bsl::size_t NUM_NAMES = sizeof DATA / sizeof *DATA;
//
//  typedef bsl::pair<const char *, const char *> Result;
//
//  MyAllocator                    myAllocator;
//  MyCaseInsensitiveSearcherCache searcherCache(&myAllocator);
//  bsl::string                    output;
//
//  for (bsl::size_t ti = 0; ti < NUM_NAMES; ++ti) {
//      const char * const givenName = DATA[ti].d_givenName_p;
//      const char * const surname   = DATA[ti].d_surname_p;
//
//      const MyCaseInsensitiveSearcherCache::Searcher& searcher =
//                                        searcherCache.getSearcher(givenName);
//
//      assert(&myAllocator == searcher.allocator());
//..
// Notice that each searcher object in the cache (correctly) uses the same
// allocator as we specified for the cache itself.
//
// The rest of the application:
//..
//      const Result result   = searcher(surname,
//                                       surname + bsl::strlen(surname));
//
//      const Result notFound = bsl::make_pair(surname + bsl::strlen(surname),
//                                       surname + bsl::strlen(surname));
//
//      char buffer[32];
//
//      if (notFound == result) {
//          sprintf(buffer, "ng: %-10s %-11s\n", givenName, surname);
//      } else {
//          sprintf(buffer, "OK: %-10s %-11s\n", givenName, surname);
//      }
//
//      output.append(buffer);
//  }
//..
// Finally, we examine the collected 'output' and confirm that our code is
// properly identifying the names of interest.
//..
//  assert(0 == bsl::strcmp(output.c_str(),
//                          "OK: Donald     McDonald   \n"
//                          "OK: John       Johnson    \n"
//                          "OK: John       Saint Johns\n"
//                          "OK: Jon        Literjon   \n"
//                          "OK: Jean       Valjean    \n"
//                          "OK: James      Jameson    \n"
//                          "OK: Will       Freewill   \n"
//                          "OK: John       Johns      \n"
//                          "OK: John       John       \n"
//                          "ng: John       Jones      \n"
//                          "ng: J'onn      J'onzz     \n"
//                          "OK: Donald     Donalds    \n"
//                          "OK: Donald     Mac Donald \n"
//                          "OK: William    Williams   \n"
//                          "OK: Durand     Durand     \n"
//                          "OK: John       Johnstown  \n"
//                          "OK: Major      Major      \n"
//                          "OK: Donald     MacDonald  \n"
//                          "OK: Patrick    O'Patrick  \n"
//                          "OK: Chris      Christie   \n"
//                          "OK: Don        London     \n"
//                          "OK: Ivan       Ivanovich  \n"));
//
//  assert(searcherCache.numSearchers() < NUM_NAMES);
//..

#include <bslscm_version.h>

#include <bslstl_array.h>
#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_iterator.h>
#include <bslstl_pair.h>
#include <bslstl_unorderedmap.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmf_conditional.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_performancehint.h>

#include <cstring>  // 'memcpy'

#include <limits.h> // 'UCHAR_MAX'

namespace BloombergLP {
namespace bslstl {

                // ========================================
                // class BoyerMooreHorspoolSearcher_CharImp
                // ========================================

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
class BoyerMooreHorspoolSearcher_CharImp {
    // This class template implements the same interfaces as the
    // 'BoyerMooreHorspoolSearcher_GeneralImp'; however, the implementation is
    // specialized for a 'value_type' of 'char'.  Notably, needle metadata is
    // stored/accessed from a fixed size array, not a dynamically-sized
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
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    typedef unsigned char   ShortNeedleSkipType;
                                               // 'd_needleLength <= UCHAR_MAX'

    typedef difference_type  LongNeedleSkipType;
                                                // 'UCHAR_MAX < d_needleLength'

    typedef bsl::array<ShortNeedleSkipType, UCHAR_MAX + 1>
                                                         ShortNeedleSkipArray;
    typedef bsl::array< LongNeedleSkipType, UCHAR_MAX + 1>
                                                          LongNeedleSkipArray;

    // PRIVATE MANIPULATORS
    void privateInstallNewTable(
                             const BoyerMooreHorspoolSearcher_CharImp& object);
        // Install in this object a table copied from the specified 'object'.
        // The behavior is undefined unless '0 == d_table_p' (on entry) and
        // 'd_needleLength' has been initialized.

    void privateInstallTableOverOld(
                             const BoyerMooreHorspoolSearcher_CharImp& object);
        // Copy the table of the specified 'object' over the table of this
        // object.  The behavior is undefined unless this object has a table
        // and 'privateUseShortNeedleOptimization()' returns the same value for
        // 'object' and for this object.

    void privateInstallMismatchedTable(
                             const BoyerMooreHorspoolSearcher_CharImp& object);
        // Replace in an exception-safe manner this object's table with a copy
        // of the table of the specified 'object'.  This object's table (on
        // entry), if any, is deleted.

    void privateDeleteTable();
        // Destroy and deallocate the 'd_table_p' member of this object, and
        // set 'd_table_p' to 0.

    // PRIVATE ACCESSORS
    void privateSetPostMoveState(BoyerMooreHorspoolSearcher_CharImp *object)
                                                                         const;
        // Set the specified 'object', the source object of a move operation,
        // to a (valid) state that is not specified to users.  The behavior is
        // undefined if 'this == object'.

    bool privateUseShortNeedleOptimization() const;
        // Return 'true' if this instantiation should used the "short needle
        // (space) optimization", and 'false' otherwise.  The behavior is
        // undefined unless 'd_needleLength' has been initialized.

    bool privateHasSameNeedleOptimization(
                              const BoyerMooreHorspoolSearcher_CharImp& object)
                                                                         const;
        // Return 'true' if the specified 'object' has the same value for
        // 'privateUseShortNeedleOptimization()' as this object, and 'false'
        // otherwise.

    // DATA
    native_std::size_t             d_needleLength;
    BloombergLP::bslma::Allocator *d_allocator_p;
    void                          *d_table_p;

  public:
    // CREATORS
    BoyerMooreHorspoolSearcher_CharImp(
                                RNDACC_ITR_NEEDLE              needleFirst,
                                RNDACC_ITR_NEEDLE              needleLast,
                                HASH                           hash,
                                EQUAL                          equal,
                                BloombergLP::bslma::Allocator *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_CharImp' object for the
        // sequence of 'char' values in the specified range
        // '[needleFirst, needlelast)'.  This implementation is invoked when
        // the specified 'hash' is 'bsl::hash<char>' and the specified 'equal'
        // is 'bsl::equal_to<char>'.  Neither functor is used because for the
        // special case of 'char' the needle metadata is maintained in a fixed
        // size array (256 elements).  As always, the specified
        // 'basicAllocator' is used to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The behavior
        // is undefined unless 'needleFirst' can be advanced to 'needleLast'.

    BoyerMooreHorspoolSearcher_CharImp(
                           const BoyerMooreHorspoolSearcher_CharImp& original);
        // Create a 'BoyerMooreHorspoolSearcher_CharImp' object having same
        // state as the specified 'original' object.  The allocator of
        // 'original' is propagated to the new object.

    BoyerMooreHorspoolSearcher_CharImp(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                                      original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'BoyerMooreHorspoolSearcher_CharImp' object having same
        // state as the specified 'original' object by moving (in constant
        // time) the state of the original object to the new object.  The
        // allocator of 'original' is propagated to the new object.  The
        // 'original' object is left in an unspecified (valid) state.

    BoyerMooreHorspoolSearcher_CharImp(
                    const BoyerMooreHorspoolSearcher_CharImp&  original,
                    BloombergLP::bslma::Allocator             *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_CharImp' object having the same
        // state as the specified 'original' object and that uses the specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    BoyerMooreHorspoolSearcher_CharImp(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                               original,
             BloombergLP::bslma::Allocator                    *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_CharImp' object having the same
        // state as the specified 'original' object and that uses the specified
        // 'basicAllocator' to supply memory.  The state of 'original' is moved
        // (in constant time) to the new searcher if
        // 'basicAllocator == original.allocator()', and is copied using
        // 'basicAllocator' otherwise.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The 'original' object is left
        // in an unspecified (valid) state.

    ~BoyerMooreHorspoolSearcher_CharImp();
        // Destroy this object;

    // MANIPULATORS
    BoyerMooreHorspoolSearcher_CharImp& operator=(
                                const BoyerMooreHorspoolSearcher_CharImp& rhs);
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this searcher object.

    BoyerMooreHorspoolSearcher_CharImp& operator=(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                                          rhs);
        // Assign to this object the state of the specified 'rhs' object and
        // return a non-'const' reference to this searcher.  The 'rhs' is left
        // in an unspecified (valid) state.

    // ACCESSORS
    difference_type badCharacterSkip(const value_type& value) const;
        // Return the number of positions to advance the search in the haystack
        // when the specified 'value' is found in the rightmost position of the
        // current (unsuccessful) match attempt.

    HASH hash() const;
        // Return the hashing functor supplied on construction.

    EQUAL equal() const;
        // Return the equality comparison functor supplied on construction.

    BloombergLP::bslma::Allocator *allocator() const;
        // Return the allocator supplied on construction.
};

                // ===========================================
                // class BoyerMooreHorspoolSearcher_GeneralImp
                // ===========================================

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
class BoyerMooreHorspoolSearcher_GeneralImp {
    // This class template implements the same interfaces as the
    // 'BoyerMooreHorspoolSearcher_CharImp' for arbitrary 'value_type'.

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
                               EQUAL> Map;  // skip-on-mismatch "table"

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    // DATA
    difference_type d_needleLength;
    Map             d_map;

  public:
    // CREATORS
    BoyerMooreHorspoolSearcher_GeneralImp(
                                RNDACC_ITR_NEEDLE              needleFirst,
                                RNDACC_ITR_NEEDLE              needleLast,
                                HASH                           hash,
                                EQUAL                          equal,
                                BloombergLP::bslma::Allocator *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_GeneralImp' object for the
        // sequence of 'value_type' values in the specified range
        // '[needleFirst, needlelast)'.  The specified 'hash' and 'equal'
        // functors are used to store/access metadata associated with the
        // needle.  See {Requirements for 'HASH' and 'EQUAL'}.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'needleFirst' can be
        // advanced to 'needleLast'.

    BoyerMooreHorspoolSearcher_GeneralImp(
                        const BoyerMooreHorspoolSearcher_GeneralImp& original);
        // Create a 'BoyerMooreHorspoolSearcher_GeneralImp' object having same
        // state as the specified 'original' object.  The allocator of
        // 'original' is propagated to the new object.

    BoyerMooreHorspoolSearcher_GeneralImp(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                                      original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'BoyerMooreHorspoolSearcher_GeneralImp' object having same
        // state as the specified 'original' object by moving (in constant
        // time) the state of the original object to the new object.  The
        // allocator of 'original' is propagated to the new object.  The
        // 'original' object is left in an unspecified (valid) state.

    BoyerMooreHorspoolSearcher_GeneralImp(
                 const BoyerMooreHorspoolSearcher_GeneralImp&  original,
                 BloombergLP::bslma::Allocator                *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_GeneralImp' object having the
        // same state as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    BoyerMooreHorspoolSearcher_GeneralImp(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                               original,
          BloombergLP::bslma::Allocator                       *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher_GeneralImp' object having same
        // state as the specified 'original' object and that uses
        // 'basicAllocator' to supply memory.  The state of 'original' is moved
        // (in constant time) to the new searcher if
        // 'basicAllocator == original.allocator()', and is copied using
        // 'basicAllocator' otherwise.  The 'original' object is left in an
        // unspecified (valid) state.

    // MANIPULATORS
    BoyerMooreHorspoolSearcher_GeneralImp& operator=(
                             const BoyerMooreHorspoolSearcher_GeneralImp& rhs);
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this searcher object.

    BoyerMooreHorspoolSearcher_GeneralImp& operator=(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                                          rhs);
        // Assign to this object the state of the specified 'rhs' object and
        // return a non-'const' reference to this searcher.  The 'rhs' is left
        // in an unspecified (valid) state.

    // ACCESSORS
    difference_type badCharacterSkip(const value_type& value) const;
        // Return the number of positions to advance the search in the haystack
        // when the specified 'value' is found in the rightmost position of the
        // current (unsuccessful) match attempt.

    HASH hash() const;
        // Return the hashing functor supplied on construction.

    EQUAL equal() const;
        // Return the equality comparison functor supplied on construction.

    BloombergLP::bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

                        // ================================
                        // class BoyerMooreHorspoolSearcher
                        // ================================

template <class RNDACC_ITR_NEEDLE,
          class HASH = bsl::hash<
                typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>,
          class EQUAL = bsl::equal_to<
                typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>
         >
class BoyerMooreHorspoolSearcher {
    // This class template implements an STL-compliant searcher object that
    // uses the Boyer, Moore, Horspool Algorithm.  Several non-standard
    // accessors are also provided.

  public:
    // TYPES
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type
                                                              value_type;
        // the type of the values that can be obtained by dereferencing a
        // 'RNDACC_ITR_NEEDLE'

    typedef bsl::hash<
                  typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>
                                                                   DefaultHash;
        // the default type for the 'HASH' optional template parameter

    typedef bsl::equal_to<
                  typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::value_type>
                                                                  DefaultEqual;
        // the default type for the 'EQUAL' optional template parameter

  private:
    // PRIVATE TYPES
    typedef typename bsl::iterator_traits<RNDACC_ITR_NEEDLE>::difference_type
                                                              difference_type;
        // a signed type that can describe the distance between
        // 'RNDACC_ITR_NEEDLE' iterators

    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    enum { k_CAN_OPTIMIZE_FOR_CHAR = (
                             1 == sizeof(value_type)
                       && bslmf::IsBitwiseEqualityComparable<value_type>::value
                       && bsl::is_trivially_copyable<difference_type>::value
                       && bsl::is_same<HASH,  DefaultHash >::value
                       && bsl::is_same<EQUAL, DefaultEqual>::value)
    };

    typedef typename bsl::conditional<
             k_CAN_OPTIMIZE_FOR_CHAR,

             BloombergLP::bslstl::
             BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                                HASH,
                                                EQUAL>,
             BloombergLP::bslstl::
             BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                                   HASH,
                                                   EQUAL> >::type Imp;

    // DATA
    RNDACC_ITR_NEEDLE d_needleFirst;   // start  of needle specified by CTOR
    RNDACC_ITR_NEEDLE d_needleLast;    // end    of needle specified by CTOR
    difference_type   d_needleLength;  // length of needle specified by CTOR

    Imp               d_imp;  // 'char'-optimized or general implementation

    // PRIVATE ACCESSORS
    void privateSetPostMoveState(BoyerMooreHorspoolSearcher *object) const;
        // Set the specified 'object', the source object of a move operation,
        // to a (valid) state that is not specified to users.  The behavior is
        // undefined if 'this == object'.

  public:
    // CREATORS
    BoyerMooreHorspoolSearcher(
                      RNDACC_ITR_NEEDLE               needleFirst,
                      RNDACC_ITR_NEEDLE               needleLast,
                      HASH                            hash           = HASH(),
                      EQUAL                           equal          = EQUAL(),
                      BloombergLP::bslma::Allocator  *basicAllocator = 0);
        // Create a 'BoyerMooreHorspoolSearcher' object that can search for the
        // sequence of 'value_type' values found in the specified range
        // '[needleFirst, needleLast)'.  Generate meta-data and save for use by
        // 'operator()'.  The complexity of this process is O(M) where M is
        // the length of the "needle".  Optionally specify a 'hash' functor
        // mapping mis-matched values to the size of the next step in the
        // search -- as large as, 'needleLast - needleFirst'.  Optionally
        // specify an 'equal' functor for use with 'hash' and for use by
        // 'operator()'.  See {Requirements for 'HASH' and 'EQUAL'}.
        // Optionally specify 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0 or not supplied, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // 'needleFirst' can be advanced to 'needleLast'.

    BoyerMooreHorspoolSearcher(const BoyerMooreHorspoolSearcher& original);
        // Create a 'BoyerMooreHorspoolSearcher' object having same state --
        // 'needleFirst()', 'needleLast()', 'hash()', and 'equal()' -- as the
        // specified 'original' object, and that uses the currently installed
        // default allocator to supply memory.

    BoyerMooreHorspoolSearcher(
           BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'BoyerMooreHorspoolSearcher' object having same state --
        // 'needleFirst()', 'needleLast()', 'hash()', and 'equal()' -- as the
        // specified 'original' object by moving (in constant time) the state
        // of 'original' to the new searcher.  The allocator of 'original' is
        // propagated for use in the newly created searcher.  The 'original'
        // object is left in an unspecified (valid) state.

    BoyerMooreHorspoolSearcher(
                            const BoyerMooreHorspoolSearcher&  original,
                            BloombergLP::bslma::Allocator     *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher' object having same state --
        // 'needleFirst()', 'needleLast()', 'hash()', and 'equal()' -- as the
        // specified 'original' object.  Optionally specify a  'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    BoyerMooreHorspoolSearcher(
                     BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher>
                                                               original,
                     BloombergLP::bslma::Allocator            *basicAllocator);
        // Create a 'BoyerMooreHorspoolSearcher' object having same state --
        // 'needleFirst()', 'needleLast()', 'hash()', and 'equal()' -- as the
        // specified 'original' object.  The specified 'basicAllocator' is used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The state of 'original' is moved (in
        // constant time) to the new searcher if
        // 'basicAllocator == original.allocator()', and is move-inserted (in
        // linear time) using 'basicAllocator' otherwise.  The 'original'
        // object is left in an unspecified (valid) state.

    //! ~BoyerMooreHorspoolSearcher() = default;
        // Destroy this 'BoyerMooreHorspoolSearcher' object.

    // MANIPULATORS
    BoyerMooreHorspoolSearcher& operator=(const BoyerMooreHorspoolSearcher&
                                                                          rhs);
        // Assign to this object the state -- 'needleFirst()', 'needleLast()',
        // 'hash()', and 'equal()' -- of the specified 'rhs' object, and return
        // a non-'const' reference to this searcher.

    BoyerMooreHorspoolSearcher& operator=(
               BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher> rhs);
        // Assign to this object the state of the specified 'rhs' object --
        // 'needleFirst()', 'needleLast()', 'hash()', and 'equal()' -- and
        // return a non-'const' reference to this searcher.  The 'rhs' is left
        // in an unspecified (valid) state.

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
        // be advanced to 'haystackLast' and the iterators used to construct
        // this object, 'needleFirst()' and 'needleLast()', are still valid.
        // Note that if the "needle" sequence is empty, the range
        // '[haystackFirst, haystackFirst)' is returned.  Also note that if the
        // "needle" sequence is longer than the "haystack" sequence -- thus,
        // impossible for the "needle" to be found in the "haystack" -- the
        // range '[haystackLast, haystackLast)' is returned.

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

    BloombergLP::bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
namespace bsl {


                        // ===================================
                        // class boyer_moore_horspool_searcher
                        // ===================================

template <class RandomAccessIterator1,
          class Hash = hash<
                typename iterator_traits<RandomAccessIterator1>::value_type>,
          class BinaryPredicate = equal_to<
                typename iterator_traits<RandomAccessIterator1>::value_type> >
class boyer_moore_horspool_searcher {
    // This class template implements an STL-compliant searcher object that
    // uses the Boyer, Moore, Horspool Algorithm.

  private:
    // DATA
    BloombergLP::bslstl::BoyerMooreHorspoolSearcher<RandomAccessIterator1,
                                                    Hash,
                                                    BinaryPredicate> d_imp;

  public:
    // CREATORS
    boyer_moore_horspool_searcher(RandomAccessIterator1 pat_first,
                                  RandomAccessIterator1 pat_last,
                                  Hash                  hf   = Hash(),
                                  BinaryPredicate       pred =
                                                            BinaryPredicate());
        // Create a 'boyer_moore_horspool_searcher' object that can search for
        // the sequence of 'value_type' values found in the specified range
        // '[pat_first, pat_last)'.  Generate meta-data and save for use by
        // 'operator()'.  The complexity of this process is O(M) where M is
        // 'pat_last - pat_first'.  Optionally specify 'hf', a hash functor,
        // that maps mis-matched values to the size of the next step in the
        // search -- as large as, 'pat_Last - pat_First'.  Optionally specify
        // 'pred', an equality comparison functor for use with 'hash' and for
        // use by 'operator()'.  See {Requirements for 'HASH' and 'EQUAL'}.
        // The behavior is undefined unless 'pat_First' can be advanced to
        // 'pat_Last'.

    //! boyer_moore_horspool_searcher(
    //!                          const boyer_moore_horspool_searcher& original)
    //!                                                              = default;
        // Create a 'boyer_moore_horspool_searcher' object having same state as
        // the specified 'original' object.

    //! boyer_moore_horspool_searcher(
    //!           BloombergLP::bslmf::MovableRef<boyer_moore_horspool_searcher>
    //!                                                    original) = default;
        // Create a 'boyer_moore_horspool_searcher' object having same state as
        // the specified 'original' object by moving (in constant time) the
        // state of 'original' to the new searcher.  The 'original' object is
        // left in an unspecified (valid) state.

    //! ~boyer_moore_horspool_searcher() = default;
        // Destroy this 'boyer_moore_horspool_searcher' object.

    // MANIPULATORS
    //! boyer_moore_horspool_searcher& operator=(
    //!                    const boyer_moore_horspool_searcher& rhs) = default;
        // Assign to this object the state of the specified 'rhs' object, and
        // return a non-'const' reference to this searcher.

    //! boyer_moore_horspool_searcher& operator=(
    //!           BloombergLP::bslmf::MovableRef<boyer_moore_horspool_searcher>
    //!                                                         rhs) = default;
        // Assign to this object the state of the specified 'rhs' object and
        // return a non-'const' reference to this searcher.  The 'rhs' is left
        // in an unspecified (valid) state.

    // ACCESSORS
    template <class RandomAccessIterator2>
    pair<RandomAccessIterator2,
         RandomAccessIterator2> operator()(RandomAccessIterator2 first,
                                           RandomAccessIterator2 last) const;
        // Search the specified range '[first, last)' for the first sequence of
        // the 'value_type' values specified on construction.  Return the range
        // where those values are found, or the range '[last, last)' if that
        // sequence is not found.  The search is performed using an
        // implementation of the Boyer Moore Horspool algorithm and has a
        // complexity of O(N) for random text.  The behavior is undefined
        // unless 'first' can be advanced to 'last' and the iterators used to
        // construct this object are still valid.  Note that if the sought
        // sequence is empty, the range '[first, first)' is returned.  Also
        // note that if the sought sequence is longer than the searched
        // sequence -- thus, the sought sequence cannot be found -- the range
        // '[last, last)' is returned.
};

}  // close namespace bsl
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS

// ----------------------------------------------------------------------------
//                          INLINE DEFINITIONS
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace bslstl {

                // ----------------------------------------
                // class BoyerMooreHorspoolSearcher_CharImp
                // ----------------------------------------

// PRIVATE MANIPULATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateInstallNewTable(
                                      const BoyerMooreHorspoolSearcher_CharImp&
                                                                        object)
{
    BSLS_ASSERT(0 == d_table_p);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
        ShortNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                          ShortNeedleSkipArray;
        native_std::memcpy(
                 arrayPtr->data(),
                 static_cast<ShortNeedleSkipArray *>(object.d_table_p)->data(),
                 UCHAR_MAX + 1);

        d_table_p = arrayPtr;
    } else {
        LongNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                           LongNeedleSkipArray;

        native_std::copy(
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cbegin(),
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cend(),
                arrayPtr->begin());

        d_table_p = arrayPtr;
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateInstallTableOverOld(
                                      const BoyerMooreHorspoolSearcher_CharImp&
                                                                        object)
{
    BSLS_ASSERT(d_table_p);
    BSLS_ASSERT(privateHasSameNeedleOptimization(object));

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
        ShortNeedleSkipArray *arrayPtr = static_cast<ShortNeedleSkipArray *>(
                                                                    d_table_p);
        native_std::memcpy(
                 arrayPtr->data(),
                 static_cast<ShortNeedleSkipArray *>(object.d_table_p)->data(),
                 UCHAR_MAX + 1);

    } else {
        LongNeedleSkipArray *arrayPtr = static_cast< LongNeedleSkipArray *>(
                                                                    d_table_p);
        native_std::copy(
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cbegin(),
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cend(),
                arrayPtr->begin());
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateInstallMismatchedTable(
                                      const BoyerMooreHorspoolSearcher_CharImp&
                                                                        object)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                 object.privateUseShortNeedleOptimization())) {
        ShortNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                          ShortNeedleSkipArray;
        native_std::memcpy(
                 arrayPtr->data(),
                 static_cast<ShortNeedleSkipArray *>(object.d_table_p)->data(),
                 UCHAR_MAX + 1);

        privateDeleteTable();

        d_table_p = arrayPtr;
    } else {
        LongNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                           LongNeedleSkipArray;
        native_std::copy(
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cbegin(),
                static_cast<LongNeedleSkipArray *>(object.d_table_p)->cend(),
                arrayPtr->begin());
        privateDeleteTable();

        d_table_p = arrayPtr;
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateDeleteTable()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
        d_allocator_p->deleteObjectRaw(static_cast<ShortNeedleSkipArray *>(
                                                                   d_table_p));
    } else {
        d_allocator_p->deleteObjectRaw(static_cast< LongNeedleSkipArray *>(
                                                                   d_table_p));
    }
    d_table_p = 0;
}

// PRIVATE ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateSetPostMoveState(
                                    BoyerMooreHorspoolSearcher_CharImp *object)
                                                                          const
{
    BSLS_ASSERT(0    != object);
    BSLS_ASSERT(this != object);

    object->d_needleLength = 0;
    object->d_table_p      = 0;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
bool
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateUseShortNeedleOptimization()
                                                                          const
{
    return d_needleLength <= UCHAR_MAX;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
bool
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::privateHasSameNeedleOptimization(
                                      const BoyerMooreHorspoolSearcher_CharImp&
                                                                        object)
                                                                          const
{
    return this->privateUseShortNeedleOptimization()
       == object.privateUseShortNeedleOptimization();
}

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
BoyerMooreHorspoolSearcher_CharImp(
                                 RNDACC_ITR_NEEDLE              needleFirst,
                                 RNDACC_ITR_NEEDLE              needleLast,
                                 HASH                           ,
                                 EQUAL                          ,
                                 BloombergLP::bslma::Allocator *basicAllocator)
: d_needleLength(needleLast - needleFirst)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_table_p(0)
{
    BSLS_ASSERT(needleFirst <= needleLast);

    if (0 == d_needleLength) {
        return;                                                       // RETURN
    }

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
        ShortNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                          ShortNeedleSkipArray;
        native_std::memset(arrayPtr->data(),
                           static_cast<ShortNeedleSkipType>(d_needleLength),
                           UCHAR_MAX + 1);
        d_table_p = arrayPtr;
    } else {
        LongNeedleSkipArray *arrayPtr = new (*d_allocator_p)
                                                           LongNeedleSkipArray;

        native_std::fill(arrayPtr->begin(), arrayPtr->end(), d_needleLength);

        d_table_p = arrayPtr;
    }

    for (RNDACC_ITR_NEEDLE current  = needleFirst,
                           last     = needleLast - 1;
                           last    != current; ++current) {

        const unsigned char index     = static_cast<unsigned char>(*current);
        native_std::size_t  skipValue = d_needleLength
                                      - 1
                                      - (current - needleFirst);

        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
            BSLS_ASSERT(skipValue <= UCHAR_MAX);

            (*static_cast<ShortNeedleSkipArray *>(d_table_p))[index]
                                 = static_cast<ShortNeedleSkipType>(skipValue);
        } else {
            (*static_cast< LongNeedleSkipArray *>(d_table_p))[index]
                                 = static_cast< LongNeedleSkipType>(skipValue);
        }
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
BoyerMooreHorspoolSearcher_CharImp(
                            const BoyerMooreHorspoolSearcher_CharImp& original)
: d_needleLength(original.d_needleLength)
, d_allocator_p( original.d_allocator_p)
, d_table_p(0)
{
    if (0 < d_needleLength) {
        privateInstallNewTable(original);
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
BoyerMooreHorspoolSearcher_CharImp(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                                      original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_needleLength(MoveUtil::access(original).d_needleLength)
, d_allocator_p( MoveUtil::access(original).d_allocator_p)
, d_table_p(     MoveUtil::access(original).d_table_p)
{
    privateSetPostMoveState(&MoveUtil::access(original));
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
BoyerMooreHorspoolSearcher_CharImp(
                     const BoyerMooreHorspoolSearcher_CharImp&  original,
                     BloombergLP::bslma::Allocator             *basicAllocator)
: d_needleLength(original.d_needleLength)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_table_p(0)
{
    if (0 < d_needleLength) {
        privateInstallNewTable(original);
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
BoyerMooreHorspoolSearcher_CharImp(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                                original,
             BloombergLP::bslma::Allocator                     *basicAllocator)
: d_needleLength(MoveUtil::access(original).d_needleLength)
, d_allocator_p( bslma::Default::allocator(basicAllocator))
, d_table_p(0)
{
    if (d_allocator_p == MoveUtil::access(original).d_allocator_p) {

        d_table_p = MoveUtil::access(original).d_table_p;

        privateSetPostMoveState(&MoveUtil::access(original));
    } else {
        if (0 < d_needleLength) {
            privateInstallNewTable(MoveUtil::access(original));
        }
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::
~BoyerMooreHorspoolSearcher_CharImp()
{
    privateDeleteTable();
}

// MANIPULATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>&
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::operator=(
                                      const BoyerMooreHorspoolSearcher_CharImp&
                                                                           rhs)
{
    if (0 < rhs.d_needleLength) {
        if (d_table_p && privateHasSameNeedleOptimization(rhs)) {
            privateInstallTableOverOld(rhs);
        } else {
            privateInstallMismatchedTable(rhs);
        }
    } else {
        privateDeleteTable();
    }

    d_needleLength = rhs.d_needleLength;

    return *this;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>&
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::operator=(
             BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_CharImp>
                                                                           rhs)
{
    if (d_allocator_p == MoveUtil::access(rhs).d_allocator_p) {
        d_allocator_p->deallocate(d_table_p);
        d_table_p                       = MoveUtil::access(rhs).d_table_p;
        MoveUtil::access(rhs).d_table_p = 0;
    } else {
        if (0 < MoveUtil::access(rhs).d_needleLength) {
            if (d_table_p && privateHasSameNeedleOptimization(
                                                      MoveUtil::access(rhs))) {
                privateInstallTableOverOld(rhs);
            } else {
                privateInstallMismatchedTable(MoveUtil::access(rhs));
            }
        } else {
            privateDeleteTable();
        }
    }

    d_needleLength = MoveUtil::access(rhs).d_needleLength;

    return *this;
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
typename
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::difference_type
BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                   HASH,
                                   EQUAL>::badCharacterSkip(
                                                       const value_type& value)
                                                                          const
{
    unsigned char index = static_cast<unsigned char>(value);

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                        privateUseShortNeedleOptimization())) {
        return (*static_cast<ShortNeedleSkipArray *>(d_table_p))[index];
                                                                      // RETURN
    } else {
        return (*static_cast< LongNeedleSkipArray *>(d_table_p))[index];
                                                                      // RETURN
    }
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
HASH BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                        HASH,
                                        EQUAL>::hash() const
{
    return HASH();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
EQUAL BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                         HASH,
                                         EQUAL>::equal() const
{
    return EQUAL();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BloombergLP::bslma::Allocator *BoyerMooreHorspoolSearcher_CharImp<
                                                     RNDACC_ITR_NEEDLE,
                                                     HASH,
                                                     EQUAL>::allocator() const
{
    return d_allocator_p;
}

                // -------------------------------------------
                // class BoyerMooreHorspoolSearcher_GeneralImp
                // -------------------------------------------

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::
BoyerMooreHorspoolSearcher_GeneralImp(
                                 RNDACC_ITR_NEEDLE              needleFirst,
                                 RNDACC_ITR_NEEDLE              needleLast,
                                 HASH                           hash,
                                 EQUAL                          equal,
                                 BloombergLP::bslma::Allocator *basicAllocator)
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

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::
BoyerMooreHorspoolSearcher_GeneralImp(
                         const BoyerMooreHorspoolSearcher_GeneralImp& original)
: d_needleLength(original.d_needleLength)
, d_map(original, original.allocator())
{
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                         HASH,
                                         EQUAL>::
BoyerMooreHorspoolSearcher_GeneralImp(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                                      original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_needleLength(MoveUtil::move(MoveUtil::access(original).d_needleLength))
, d_map(         MoveUtil::move(MoveUtil::access(original).d_map))
{
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::
BoyerMooreHorspoolSearcher_GeneralImp(
                  const BoyerMooreHorspoolSearcher_GeneralImp&  original,
                  BloombergLP::bslma::Allocator                *basicAllocator)
: d_needleLength(original.d_needleLength)
, d_map(original.d_map, basicAllocator)
{
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::
BoyerMooreHorspoolSearcher_GeneralImp(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                                original,
          BloombergLP::bslma::Allocator                        *basicAllocator)
: d_needleLength(MoveUtil::move(MoveUtil::access(original).d_needleLength))
, d_map(         MoveUtil::move(MoveUtil::access(original).d_map),
                 basicAllocator)
{
}

// MANIPULATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>&
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::operator=(
                              const BoyerMooreHorspoolSearcher_GeneralImp& rhs)
{
    d_needleLength = rhs.d_needleLength;
    d_map          = rhs.d_map;

    return *this;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>&
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::operator=(
          BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher_GeneralImp>
                                                                           rhs)
{
    d_needleLength = MoveUtil::move(MoveUtil::access(rhs).d_needleLength);
    d_map          = MoveUtil::move(MoveUtil::access(rhs).d_map);

    return *this;
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
typename
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::difference_type
BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                      HASH,
                                      EQUAL>::badCharacterSkip(
                                                 const value_type& value) const
{
    typename Map::const_iterator result = d_map.find(value);

    return d_map.cend() == result ? d_needleLength : result->second;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
HASH BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                           HASH,
                                           EQUAL>::hash() const
{
    return d_map.hash_function();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
EQUAL BoyerMooreHorspoolSearcher_GeneralImp<RNDACC_ITR_NEEDLE,
                                            HASH,
                                            EQUAL>::equal() const
{
    return d_map.key_eq();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BloombergLP::bslma::Allocator *BoyerMooreHorspoolSearcher_GeneralImp<
                                                      RNDACC_ITR_NEEDLE,
                                                      HASH,
                                                      EQUAL>::allocator() const
{
    return d_map.get_allocator().mechanism();
}

                        // --------------------------------
                        // class BoyerMooreHorspoolSearcher
                        // --------------------------------

// PRIVATE ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
void
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::privateSetPostMoveState(
                           BoyerMooreHorspoolSearcher *object) const
{
    BSLS_ASSERT(0    != object);
    BSLS_ASSERT(this != object);

    object->d_needleFirst  = d_needleFirst;
    object->d_needleLast   = d_needleFirst;
    object->d_needleLength = 0;
}

// CREATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::
BoyerMooreHorspoolSearcher(RNDACC_ITR_NEEDLE              needleFirst,
                           RNDACC_ITR_NEEDLE              needleLast,
                           HASH                           hash,
                           EQUAL                          equal,
                           BloombergLP::bslma::Allocator *basicAllocator)
: d_needleFirst( needleFirst)
, d_needleLast(  needleLast)
, d_needleLength(bsl::distance(needleFirst, needleLast))
, d_imp(needleFirst, needleLast, hash, equal, basicAllocator)
{
    BSLS_ASSERT(needleFirst <= needleLast);
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::
BoyerMooreHorspoolSearcher(const BoyerMooreHorspoolSearcher& original)
: d_needleFirst( original.d_needleFirst)
, d_needleLast(  original.d_needleLast)
, d_needleLength(original.d_needleLength)
, d_imp(         original.d_imp,
                 BloombergLP::bslma::Default::defaultAllocator())
{
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::
BoyerMooreHorspoolSearcher(
           BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_needleFirst( MoveUtil::move(MoveUtil::access(original).d_needleFirst))
, d_needleLast(  MoveUtil::move(MoveUtil::access(original).d_needleLast))
, d_needleLength(MoveUtil::move(MoveUtil::access(original).d_needleLength))
, d_imp(         MoveUtil::move(MoveUtil::access(original).d_imp))
{
    privateSetPostMoveState(&MoveUtil::access(original));
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::
BoyerMooreHorspoolSearcher(const BoyerMooreHorspoolSearcher&  original,
                           BloombergLP::bslma::Allocator     *basicAllocator)
: d_needleFirst( original.d_needleFirst)
, d_needleLast(  original.d_needleLast)
, d_needleLength(original.d_needleLength)
, d_imp(         original.d_imp, basicAllocator)
{
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::
BoyerMooreHorspoolSearcher(
    BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher>  original,
    BloombergLP::bslma::Allocator                              *basicAllocator)
: d_needleFirst( MoveUtil::move(MoveUtil::access(original).d_needleFirst))
, d_needleLast(  MoveUtil::move(MoveUtil::access(original).d_needleLast))
, d_needleLength(MoveUtil::move(MoveUtil::access(original).d_needleLength))
, d_imp(         MoveUtil::move(MoveUtil::access(original).d_imp),
                 basicAllocator)
{
    privateSetPostMoveState(&MoveUtil::access(original));
}

// MANIPULATORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                              HASH,
                              EQUAL>&
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::operator=(
                                         const BoyerMooreHorspoolSearcher& rhs)
{
    d_needleFirst  = rhs.d_needleFirst;
    d_needleLast   = rhs.d_needleLast;
    d_needleLength = rhs.d_needleLength;
    d_imp          = rhs.d_imp;

    return *this;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>&
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::operator=(
                BloombergLP::bslmf::MovableRef<BoyerMooreHorspoolSearcher> rhs)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(this != &MoveUtil::access(rhs))) {

        d_needleFirst  = MoveUtil::move(MoveUtil::access(rhs).d_needleFirst);
        d_needleLast   = MoveUtil::move(MoveUtil::access(rhs).d_needleLast);
        d_needleLength = MoveUtil::move(MoveUtil::access(rhs).d_needleLength);
        d_imp          = MoveUtil::move(MoveUtil::access(rhs).d_imp);

        privateSetPostMoveState(&MoveUtil::access(rhs));
    }

    return *this;
}

// ACCESSORS
template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
template <class RNDACC_ITR_HAYSTACK>
bsl::pair<RNDACC_ITR_HAYSTACK, RNDACC_ITR_HAYSTACK>
BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                           HASH,
                           EQUAL>::operator()(
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

        // Check in reverse order for match.

        const EQUAL comparator(equal());

        for (native_std::size_t idx = d_needleLength - 1;
             comparator(haystackFirst[possibleMatch + idx],
                        d_needleFirst[idx]);
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
          class EQUAL>
inline
RNDACC_ITR_NEEDLE BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                                             HASH,
                                             EQUAL>::needleFirst() const
{
    return d_needleFirst;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
RNDACC_ITR_NEEDLE BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                                             HASH,
                                             EQUAL>::needleLast() const
{
    return d_needleLast;
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
HASH BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                                HASH,
                                EQUAL>::hash() const
{
    return d_imp.hash();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
EQUAL BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                                 HASH,
                                 EQUAL>::equal() const
{
    return d_imp.equal();
}

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
inline
BloombergLP::bslma::Allocator *BoyerMooreHorspoolSearcher<
                                                      RNDACC_ITR_NEEDLE,
                                                      HASH,
                                                      EQUAL>::allocator() const
{
    return d_imp.allocator();
}

}  // close package namespace
}  // close enterprise namespace

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS
namespace bsl {

                        // -----------------------------------
                        // class boyer_moore_horspool_searcher
                        // -----------------------------------

// CREATORS
template <class RandomAccessIterator1,
          class Hash,
          class BinaryPredicate>
inline
boyer_moore_horspool_searcher<RandomAccessIterator1,
                              Hash,
                              BinaryPredicate>::boyer_moore_horspool_searcher(
                                               RandomAccessIterator1 pat_first,
                                               RandomAccessIterator1 pat_last,
                                               Hash                  hf,
                                               BinaryPredicate       pred)
: d_imp(pat_first, pat_last, hf, pred)
{
    BSLS_ASSERT(pat_first <= pat_last);
}

// ACCESSORS
template <class RandomAccessIterator1,
          class Hash,
          class BinaryPredicate>
template <class RandomAccessIterator2>
inline
pair<RandomAccessIterator2,
     RandomAccessIterator2> boyer_moore_horspool_searcher<
                                                  RandomAccessIterator1,
                                                  Hash,
                                                  BinaryPredicate>::operator()(
                                                   RandomAccessIterator2 first,
                                                   RandomAccessIterator2 last)
                                                                          const
{
    BSLS_ASSERT(first <= last);

    return d_imp(first, last);
}

}  // close namespace bsl
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_FUNCTORS

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace BloombergLP {
namespace bslma {

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
struct UsesBslmaAllocator<
    BloombergLP::bslstl::BoyerMooreHorspoolSearcher<RNDACC_ITR_NEEDLE,
                                                    HASH,
                                                    EQUAL>
    > : bsl::true_type
{};

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
struct UsesBslmaAllocator<
    BloombergLP::bslstl::BoyerMooreHorspoolSearcher_GeneralImp<
                                                             RNDACC_ITR_NEEDLE,
                                                             HASH,
                                                             EQUAL>
    > : bsl::true_type
{};

template <class RNDACC_ITR_NEEDLE,
          class HASH,
          class EQUAL>
struct UsesBslmaAllocator<
    BloombergLP::bslstl::BoyerMooreHorspoolSearcher_CharImp<RNDACC_ITR_NEEDLE,
                                                            HASH,
                                                            EQUAL>
    > : bsl::true_type
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
