// bdlc_flathashmap.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLC_FLATHASHMAP
#define INCLUDED_BDLC_FLATHASHMAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an open-addressed unordered map container.
//
//@CLASSES:
//   bdlc::FlatHashMap: open-addressed unordered map container
//
//@SEE_ALSO: bdlc_flathashtable, bdlc_flathashset
//
//@DESCRIPTION: This component defines a single class template,
// 'bdlc::FlatHashMap', that implements an open-addressed unordered map of
// items with unique keys.
//
// Unordered maps are useful in situations when there is no meaningful way to
// order key values, when the order of the keys is irrelevant to the problem
// domain, or (even if there is a meaningful ordering) the value of ordering
// the keys is outweighed by the higher performance provided by unordered maps
// (compared to ordered maps).  On platforms that support relevant SIMD
// instructions (e.g., SSE2), 'bdlc::FlatHashMap' generally exhibits better
// performance than 'bsl::unordered_map'.
//
// An instantiation of 'bdlc::FlatHashMap' is an allocator-aware,
// value-semantic type whose salient attributes are the collection of
// 'KEY-VALUE' pairs contained, without regard to order.  An instantiation may
// be provided with custom hash and key-equality functors, but those are not
// salient attributes.  In particular, when comparing element values for
// equality between two different 'bdlc::FlatHashMap' objects, the elements are
// compared using 'operator=='.
//
// The implemented data structure is inspired by Google's 'flat_hash_map'
// CppCon presentations (available on YouTube).  The implementation draws from
// Google's open source 'raw_hash_set.h' file at:
// https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal.
//
///Performance Caveats
///-------------------
// 'bdlc::FlatHashMap' is recommended for Intel platforms *only* (i.e., Linux
// and Windows, and pre-ARM Macs); on platforms using other processors (i.e.,
// Sun and AIX), 'bdlc::FlatHashMap' may have slower performance than
// 'bsl::unordered_map'.  However, note that 'bdlc::FlatHashMap' will use
// significantly less memory than 'bsl::unordered_map' on *all* platforms.
// Given the Intel-only performance caveat, it is recommended to benchmark
// before using 'bdlc::FlatHashMap' -- particularly on non-Intel production
// environments.
//
///Interface Differences with 'unordered_map'
///------------------------------------------
// A 'bdlc::FlatHashMap' meets most of the requirements of an unordered
// associative container with forward iterators in the C++11 Standard [23.2.5].
// It does not have the bucket interface, and locations of elements may change
// when the container is modified (and therefore iterators become invalid too).
// Allocator use follows BDE style, and the various allocator propagation
// attributes are not present (e.g., the allocator trait
// 'propagate_on_container_copy_assignment').  The maximum load factor of the
// container (the ratio of size to capacity) is maintained by the container
// itself and is not settable (the maximum load factor is implementation
// defined and fixed).
//
///Load Factor and Resizing
///------------------------
// An invariant of 'bdlc::FlatHashMap' is that
// '0 <= load_factor() <= max_load_factor() <= 1.0'.  Any operation that would
// result in 'load_factor() > max_load_factor()' for a 'bdlc::FlatHashMap'
// causes the capacity to increase.  This resizing allocates new memory, copies
// or moves all elements to the new memory, and reclaims the original memory.
// The transfer of elements involves rehashing each element to determine its
// new location.  As such, all iterators, pointers, and references to elements
// of the 'bdlc::FlatHashMap' are invalidated on a resize.
//
///Requirements on 'KEY', 'HASH', and 'EQUAL'
///------------------------------------------
// The template parameter type 'KEY' must be copy or move constructible.  The
// template parameter types 'HASH' and 'EQUAL' must be default and copy
// constructible function objects.
//
// 'HASH' must support a function-call operator compatible with the following
// statements for an object 'key' of type 'KEY':
//..
//  HASH        hash;
//  bsl::size_t result = hash(key);
//..
//
// 'EQUAL' must support a function-call operator compatible with the
//  following statements for objects 'key1' and 'key2' of type 'KEY':
//..
//  EQUAL equal;
//  bool  result = equal(key1, key2);
//..
// where the definition of the called function defines an equivalence
// relationship on keys that is both reflexive and transitive.
//
// 'HASH' and 'EQUAL' function objects are further constrained: if the
// comparator determines that two values are equal, the hasher must produce the
// same hash value for each.
//
///Iterator, Pointer, and Reference Invalidation
///---------------------------------------------
// Any change in capacity of a 'bdlc::FlatHashMap' invalidates all pointers,
// references, and iterators.  A 'bdlc::FlatHashMap' manipulator that erases an
// element invalidates all pointers, references, and iterators to the erased
// element.
//
///Exception Safety
///----------------
// A 'bdlc::FlatHashMap' is exception neutral, and all of the methods of
// 'bdlc::FlatHashMap' provide the basic exception safety guarantee (see
// {'bsldoc_glossary'|Basic Guarantee}).
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by 'bdlc::FlatHashMap' on C++11, and later,
// platforms only (where 'BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES' is defined),
// and are not supported on C++03 platforms.  Unfortunately, in C++03, there
// are user-defined types where a 'bslmf::MovableRef' will not safely degrade
// to an lvalue reference when a move constructor is not available (types
// providing a constructor template taking any type), so
// 'bslmf::MovableRefUtil::move' cannot be used directly on a user-supplied
// template parameter type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Gathering Document Statistics
/// - - - - - - - - - - - - - - - - - - - -
// Suppose one wished to gather statistics on the words appearing in a large
// set of documents on disk or in a database.  Gathering those statistics is
// intrusive (as one is competing for access to the documents with the regular
// users) and must be done as quickly as possible.  Moreover, the set of unique
// words appearing in those documents may be high.  The English language has in
// excess of a million words (albeit many appear infrequently), and, if the
// documents contain serial numbers, or Social Security numbers, or chemical
// formulas, etc., then the 'O[log(n)]' insertion time of ordered maps may well
// be inadequate.  An unordered map, having an 'O[1]' typical insertion cost,
// is a viable alternative.
//
// This example illustrates the use of 'bdlc::FlatHashMap' to gather one simple
// statistic (counts of unique words) on a portion of a single document.  To
// avoid irrelevant details of acquiring the data, the data is stored in static
// arrays:
//..
//  static char document[] =
//  " IN CONGRESS, July 4, 1776.\n"
//  "\n"
//  " The unanimous Declaration of the thirteen united States of America,\n"
//  "\n"
//  " When in the Course of human events, it becomes necessary for one\n"
//  " people to dissolve the political bands which have connected them with\n"
//  " another, and to assume among the powers of the earth, the separate\n"
//  " and equal station to which the Laws of Nature and of Nature's G-d\n"
//  " entitle them, a decent respect to the opinions of mankind requires\n"
//  " that they should declare the causes which impel them to the\n"
//  " separation.  We hold these truths to be self-evident, that all men\n"
//  " are created equal, that they are endowed by their Creator with\n"
//  " certain unalienable Rights, that among these are Life, Liberty and\n"
//  " the pursuit of Happiness.  That to secure these rights, Governments\n"
//  " are instituted among Men, deriving their just powers from the consent\n"
//  " of the governed, That whenever any Form of Government becomes\n";
//..
// First, we define an alias to make our code more comprehensible:
//..
//  typedef bdlc::FlatHashMap<bsl::string, int> WordTally;
//..
// Next, we create an (empty) flat hash map to hold our word tallies:
//..
//  WordTally wordTally;
//..
// Then, we define the set of characters that define word boundaries:
//..
//  const char *delimiters = " \n\t,:;.()[]?!/";
//..
// Next, we extract the words from our document.  Note that 'strtok' modifies
// the document array (which was not made 'const').
//
// For each iteration of the loop, a map entry matching the key value parsed by
// 'strtok' is obtained.  On the first occurrence of a word, the map has no
// such entry, so one is created with a default value of the mapped value (0,
// just what we want in this case) and inserted into the map where it is found
// on any subsequent occurrences of the word.  The 'operator[]' method returns
// a reference providing modifiable access to the mapped value.  Here, we apply
// the '++' operator to that reference to maintain a tally for the word:
//..
//  for (char *cur = strtok(document, delimiters);
//             cur;
//             cur = strtok(NULL,     delimiters)) {
//      ++wordTally[bsl::string(cur)];
//  }
//..
// Now that the data has been (quickly) gathered, we can indulge in analysis
// that is more time consuming.  For example, we can define a comparison
// function, copy the data to another container (e.g., 'bsl::vector'), sort the
// entries, and determine the 10 most commonly used words in the given
// documents:
//..
//  typedef bsl::pair<bsl::string, int> WordTallyEntry;
//      // Assignable equivalent to 'WordTally::value_type'.  Note that
//      // 'bsl::vector' requires assignable types.
//
//  struct WordTallyEntryCompare {
//      static bool lessThan(const WordTallyEntry& a,
//                           const WordTallyEntry& b) {
//          return a.second < b.second;
//      }
//      static bool greaterThan(const WordTallyEntry& a,
//                              const WordTallyEntry& b) {
//          return a.second > b.second;
//      }
//  };
//
//  bsl::vector<WordTallyEntry> array(wordTally.cbegin(), wordTally.cend());
//
//  assert(10 <= array.size());
//
//  bsl::partial_sort(array.begin(),
//                    array.begin() + 10,
//                    array.end(),
//                    WordTallyEntryCompare::greaterThan);
//..
// Notice that 'partial_sort' suffices here since we seek only the 10 most used
// words, not a complete distribution of word counts.
//
// Finally, we print the sorted portion of 'array':
//..
//  for (bsl::vector<WordTallyEntry>::const_iterator cur  = array.begin(),
//                                                   end  = cur + 10;
//                                                   end != cur; ++cur) {
//      printf("%-10s %4d\n", cur->first.c_str(), cur->second);
//  }
//..
// and standard output shows:
//..
//  the          13
//  of           10
//  to            7
//  that          4
//  are           4
//  and           4
//  which         3
//  these         3
//  them          3
//  among         3
//..

#include <bdlscm_version.h>

#include <bdlc_flathashtable.h>

#include <bslalg_hasstliterators.h>
#include <bslalg_swaputil.h>

#include <bslh_fibonaccibadhashwrapper.h>

#include <bslim_printer.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_destructorguard.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_addconst.h>
#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <bslstl_equalto.h>
#include <bslstl_hash.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#include <bsl_initializer_list.h>
#endif
#include <bsl_cstddef.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace bdlc {

// FORWARD DECLARATIONS
template <class KEY,
          class VALUE,
          class HASH  = bslh::FibonacciBadHashWrapper<bsl::hash<KEY> >,
          class EQUAL = bsl::equal_to<KEY> >
class FlatHashMap;

template <class KEY, class VALUE, class HASH, class EQUAL>
bool operator==(const FlatHashMap<KEY, VALUE, HASH, EQUAL> &lhs,
                const FlatHashMap<KEY, VALUE, HASH, EQUAL> &rhs);

template <class KEY, class VALUE, class HASH, class EQUAL>
bool operator!=(const FlatHashMap<KEY, VALUE, HASH, EQUAL> &lhs,
                const FlatHashMap<KEY, VALUE, HASH, EQUAL> &rhs);

template <class KEY, class VALUE, class HASH, class EQUAL>
void swap(FlatHashMap<KEY, VALUE, HASH, EQUAL>& a,
          FlatHashMap<KEY, VALUE, HASH, EQUAL>& b);

                       // ============================
                       // struct FlatHashMap_EntryUtil
                       // ============================

template <class KEY, class VALUE, class ENTRY>
struct FlatHashMap_EntryUtil
    // This templated utility provides methods to construct an 'ENTRY' and a
    // method to extract the key from an 'ENTRY'.
{
    // CLASS METHODS
    template <class KEY_TYPE>
    static void construct(
                        ENTRY                                       *entry,
                        bslma::Allocator                            *allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE)  key);
        // Load into the specified 'entry' the 'ENTRY' value comprised of the
        // specified 'key' and a default constructed 'VALUE', using the
        // specified 'allocator' to supply memory.  'allocator' is ignored if
        // the (template parameter) type 'ENTRY' is not allocator aware.

    static const KEY& key(const ENTRY& entry);
        // Return the key of the specified 'entry'.
};

                            // =================
                            // class FlatHashMap
                            // =================

template <class KEY, class VALUE, class HASH, class EQUAL>
class FlatHashMap {
    // This class template implements a value-semantic container type holding
    // an unordered map of 'KEY-VALUE' pairs having unique keys that provides a
    // mapping from keys of (template parameter) type 'KEY' to their associated
    // mapped values of (template parameter) type 'VALUE'.  The (template
    // parameter) type 'HASH' is a functor providing the hash value for 'KEY'.
    // The (template parameter) type 'EQUAL' is a functor providing the
    // equality function for two 'KEY' values.  See {Requirements on 'KEY',
    // 'HASH', and 'EQUAL'} for more information.

  private:
    // PRIVATE TYPES
    typedef FlatHashTable<KEY,
                          bsl::pair<KEY, VALUE>,
                          FlatHashMap_EntryUtil<KEY,
                                                VALUE,
                                                bsl::pair<KEY, VALUE> >,
                          HASH,
                          EQUAL> ImplType;
        // This is the underlying implementation class.

    // FRIENDS
    friend bool operator==<>(const FlatHashMap&, const FlatHashMap&);
    friend bool operator!=<>(const FlatHashMap&, const FlatHashMap&);

    // The following verbose declaration is required by the xlC 12.1 compiler.
    template <class K, class V, class H, class E>
    friend void swap(FlatHashMap<K, V, H, E>&, FlatHashMap<K, V, H, E>&);

  public:
    // PUBLIC TYPES
    typedef bsl::pair<typename bsl::add_const<KEY>::type, VALUE> value_type;

    typedef KEY                                key_type;
    typedef VALUE                              mapped_type;
    typedef bsl::size_t                        size_type;
    typedef bsl::ptrdiff_t                     difference_type;
    typedef EQUAL                              key_compare;
    typedef HASH                               hasher;
    typedef value_type&                        reference;
    typedef const value_type&                  const_reference;
    typedef typename ImplType::iterator        iterator;
    typedef typename ImplType::const_iterator  const_iterator;

  private:
    // DATA
    ImplType d_impl;  // underlying flat hash table used by this flat hash map

  public:
    // CREATORS
    FlatHashMap();
    explicit FlatHashMap(bslma::Allocator *basicAllocator);
    explicit FlatHashMap(bsl::size_t capacity);
    FlatHashMap(bsl::size_t capacity, bslma::Allocator *basicAllocator);
    FlatHashMap(bsl::size_t       capacity,
                const HASH&       hash,
                bslma::Allocator *basicAllocator = 0);
    FlatHashMap(bsl::size_t       capacity,
                const HASH&       hash,
                const EQUAL&      equal,
                bslma::Allocator *basicAllocator = 0);
        // Create an empty 'FlatHashMap' object.  Optionally specify a
        // 'capacity' indicating the minimum initial size of the underlying
        // array of entries of this container.  If 'capacity' is not supplied
        // or is 0, no memory is allocated.  Optionally specify a 'hash'
        // functor used to generate the hash values associated with the keys of
        // elements in this container.  If 'hash' is not supplied, a
        // default-constructed object of the (template parameter) type 'HASH'
        // is used.  Optionally specify an equality functor 'equal' used to
        // determine whether the keys of two elements are equivalent.  If
        // 'equal' is not supplied, a default-constructed object of the
        // (template parameter) type 'EQUAL' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied or is 0, the currently installed default allocator is used.

    template <class INPUT_ITERATOR>
    FlatHashMap(INPUT_ITERATOR    first,
                INPUT_ITERATOR    last,
                bslma::Allocator *basicAllocator = 0);
    template <class INPUT_ITERATOR>
    FlatHashMap(INPUT_ITERATOR    first,
                INPUT_ITERATOR    last,
                bsl::size_t       capacity,
                bslma::Allocator *basicAllocator = 0);
    template <class INPUT_ITERATOR>
    FlatHashMap(INPUT_ITERATOR    first,
                INPUT_ITERATOR    last,
                bsl::size_t       capacity,
                const HASH&       hash,
                bslma::Allocator *basicAllocator = 0);
    template <class INPUT_ITERATOR>
    FlatHashMap(INPUT_ITERATOR    first,
                INPUT_ITERATOR    last,
                bsl::size_t       capacity,
                const HASH&       hash,
                const EQUAL&      equal,
                bslma::Allocator *basicAllocator = 0);
        // Create a 'FlatHashMap' object initialized by insertion of the values
        // from the input iterator range specified by 'first' through 'last'
        // (including 'first', excluding 'last').  Optionally specify a
        // 'capacity' indicating the minimum initial size of the underlying
        // array of entries of this container.  If 'capacity' is not supplied
        // or is 0, no memory is allocated.  Optionally specify a 'hash'
        // functor used to generate hash values associated with the keys of the
        // elements in this container.  If 'hash' is not supplied, a
        // default-constructed object of the (template parameter) type 'HASH'
        // is used.  Optionally specify an equality functor 'equal' used to
        // determine whether the keys of two elements are equivalent.  If
        // 'equal' is not supplied, a default-constructed object of the
        // (template parameter) type 'EQUAL' is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // supplied or is 0, the currently installed default allocator is used.
        // The behavior is undefined unless 'first' and 'last' refer to a
        // sequence of valid values where 'first' is at a position at or before
        // 'last'.  Note that if a member of the input sequence has an
        // equivalent key to an earlier member, the later member will not be
        // inserted.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    FlatHashMap(bsl::initializer_list<value_type>  values,
                bslma::Allocator                  *basicAllocator = 0);
    FlatHashMap(bsl::initializer_list<value_type>  values,
                bsl::size_t                        capacity,
                bslma::Allocator                  *basicAllocator = 0);
    FlatHashMap(bsl::initializer_list<value_type>  values,
                bsl::size_t                        capacity,
                const HASH&                        hash,
                bslma::Allocator                  *basicAllocator = 0);
    FlatHashMap(bsl::initializer_list<value_type>  values,
                bsl::size_t                        capacity,
                const HASH&                        hash,
                const EQUAL&                       equal,
                bslma::Allocator                  *basicAllocator = 0);
        // Create a 'FlatHashMap' object initialized by insertion of the
        // specified 'values'.  Optionally specify a 'capacity' indicating the
        // minimum initial size of the underlying array of entries of this
        // container.  If 'capacity' is not supplied or is 0, no memory is
        // allocated.  Optionally specify a 'hash' functor used to generate
        // hash values associated with the keys of elements in this container.
        // If 'hash' is not supplied, a default-constructed object of the
        // (template parameter) type 'HASH' is used.  Optionally specify an
        // equality functor 'equal' used to determine whether the keys of two
        // elements are equivalent.  If 'equal' is not supplied, a
        // default-constructed object of the (template parameter) type 'EQUAL'
        // is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not supplied or is 0, the currently
        // installed default allocator is used.  Note that if a member of
        // 'values' has an equivalent key to an earlier member, the later
        // member will not be inserted.
#endif

    FlatHashMap(const FlatHashMap&  original,
                bslma::Allocator   *basicAllocator = 0);
        // Create a 'FlatHashMap' object having the same value, hasher, and
        // equality comparator as the specified 'original' object.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified or is 0, the currently installed
        // default allocator is used.

    FlatHashMap(bslmf::MovableRef<FlatHashMap> original);
        // Create a 'FlatHashMap' object having the same value, hasher,
        // equality comparator, and allocator as the specified 'original'
        // object.  The contents of 'original' are moved (in constant time) to
        // this object, 'original' is left in a (valid) unspecified state, and
        // no exceptions will be thrown.

    FlatHashMap(bslmf::MovableRef<FlatHashMap>  original,
                bslma::Allocator               *basicAllocator);
        // Create a 'FlatHashMap' object having the same value, hasher, and
        // equality comparator as the specified 'original' object, using the
        // specified 'basicAllocator' to supply memory.  If 'basicAllocator' is
        // 0, the currently installed default allocator is used.  The allocator
        // of 'original' remains unchanged.  If 'original' and the newly
        // created object have the same allocator then the contents of
        // 'original' are moved (in constant time) to this object, 'original'
        // is left in a (valid) unspecified state, and no exceptions will be
        // thrown; otherwise, 'original' is unchanged (and an exception may be
        // thrown).

    ~FlatHashMap();
        // Destroy this object and each of its elements.

    // MANIPULATORS
    FlatHashMap& operator=(const FlatHashMap& rhs);
        // Assign to this object the value, hasher, and equality functor of the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.

    FlatHashMap& operator=(bslmf::MovableRef<FlatHashMap> rhs);
        // Assign to this object the value, hasher, and equality comparator of
        // the specified 'rhs' object, and return a reference providing
        // modifiable access to this object.  If this object and 'rhs' use the
        // same allocator the contents of 'rhs' are moved (in constant time) to
        // this object.  'rhs' is left in a (valid) unspecified state.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    FlatHashMap& operator=(bsl::initializer_list<value_type> values);
        // Assign to this object the value resulting from first clearing this
        // map and then inserting each object in the specified 'values'
        // initializer list, ignoring those objects having a value whose key is
        // equivalent to that which appears earlier in the list; return a
        // reference providing modifiable access to this object.  This method
        // requires that the (template parameter) type 'KEY' be
        // 'copy-insertable' into this map (see {Requirements on 'KEY', 'HASH',
        // and 'EQUAL'}).
#endif

    template <class KEY_TYPE>
    VALUE& operator[](BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE) key);
        // Return a reference providing modifiable access to the mapped value
        // associated with the specified 'key' in this map.  If this map does
        // not already contain an element having 'key', insert an element with
        // the 'key' and a default-constructed 'VALUE', and return a reference
        // to the newly mapped value.  If 'key' is movable, 'key' is left in a
        // (valid) unspecified state.

    void clear();
        // Remove all elements from this map.  Note that this map will be empty
        // after calling this method, but allocated memory may be retained for
        // future use.  See the 'capacity' method.

    bsl::pair<iterator, iterator> equal_range(const KEY& key);
        // Return a pair of iterators defining the sequence of modifiable
        // elements in this map having the specified 'key', where the first
        // iterator is positioned at the start of the sequence and the second
        // iterator is positioned one past the end of the sequence.  If this
        // map contains no elements having a key equivalent to 'key', then the
        // two returned iterators will have the same value.  Note that since a
        // map maintains unique keys, the range will contain at most one
        // element.

    bsl::size_t erase(const KEY& key);
        // Remove from this map the element whose key is equal to the specified
        // 'key', if it exists, and return 1; otherwise (there is no element
        // having 'key' in this map), return 0 with no other effect.  This
        // method invalidates all iterators and references to the removed
        // element.

    iterator erase(const_iterator position);
    iterator erase(iterator position);
        // Remove from this map the element at the specified 'position', and
        // return an iterator referring to the modifiable element immediately
        // following the removed element, or to the past-the-end position if
        // the removed element was the last element in the sequence of elements
        // maintained by this map.  This method invalidates all iterators and
        // references to the removed element.  The behavior is undefined unless
        // 'position' refers to an element in this map.

    iterator erase(const_iterator first, const_iterator last);
        // Remove from this map the elements starting at the specified 'first'
        // position up to, but not including, the specified 'last' position,
        // and return an iterator referencing the same element as 'last'.  This
        // method invalidates all iterators and references to the removed
        // elements.  The behavior is undefined unless 'first' and 'last' are
        // valid iterators on this map, and the 'first' position is at or
        // before the 'last' position in the iteration sequence provided by
        // this container.

    iterator find(const KEY& key);
        // Return an iterator referring to the modifiable element in this map
        // having the specified 'key', or 'end()' if no such entry exists in
        // this map.

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class VALUE_TYPE>
    bsl::pair<iterator, bool> insert(
                           BSLS_COMPILERFEATURES_FORWARD_REF(VALUE_TYPE) value)
#else
    template <class VALUE_TYPE>
    typename bsl::enable_if<bsl::is_convertible<VALUE_TYPE, value_type>::value,
                            bsl::pair<iterator, bool> >::type
                    insert(BSLS_COMPILERFEATURES_FORWARD_REF(VALUE_TYPE) value)
#endif
        // Insert the specified 'value' into this map if the key of 'value'
        // does not already exist in this map; otherwise, this method has no
        // effect.  Return a 'pair' whose 'first' member is an iterator
        // referring to the (possibly newly inserted) modifiable element in
        // this map whose key is equivalent to that of the element to be
        // inserted, and whose 'second' member is 'true' if a new element was
        // inserted, and 'false' if an element with an equivalent key was
        // already present.
    {
        // Note that some compilers require functions declared with 'enable_if'
        // to be defined inline.

        return d_impl.insert(BSLS_COMPILERFEATURES_FORWARD(VALUE_TYPE, value));
    }

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class VALUE_TYPE>
    iterator insert(const_iterator                                ,
                    BSLS_COMPILERFEATURES_FORWARD_REF(VALUE_TYPE) value)
#else
    template <class VALUE_TYPE>
    typename bsl::enable_if<bsl::is_convertible<VALUE_TYPE, value_type>::value,
                            iterator>::type
                    insert(const_iterator                                ,
                           BSLS_COMPILERFEATURES_FORWARD_REF(VALUE_TYPE) value)
#endif
        // Insert the specified 'value' into this map if the key of 'value'
        // does not already exist in this map; otherwise, this method has no
        // effect.  Return an iterator referring to the (possibly newly
        // inserted) modifiable element in this map whose key is equivalent to
        // that of the element to be inserted.  The supplied 'const_iterator'
        // is ignored.
    {
        // Note that some compilers require functions declared with 'enable_if'
        // to be defined inline.

        return d_impl.insert(BSLS_COMPILERFEATURES_FORWARD(VALUE_TYPE,
                                                           value)).first;
    }

    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Insert into this map the value of each element in the input iterator
        // range specified by 'first' through 'last' (including 'first',
        // excluding 'last').  The behavior is undefined unless 'first' and
        // 'last' refer to a sequence of valid values where 'first' is at a
        // position at or before 'last'.  Note that if the key of a member of
        // the input sequence is equivalent to the key of an earlier member,
        // the later member will not be inserted.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(bsl::initializer_list<value_type> values);
        // Insert into this map an element having the value of each object in
        // the specified 'values' initializer list if a value with an
        // equivalent key is not already contained in this map.  This method
        // requires that the (template parameter) type 'KEY' be copy-insertable
        // (see {Requirements on 'KEY', 'HASH', and 'EQUAL'}).
#endif

    void rehash(bsl::size_t minimumCapacity);
        // Change the capacity of this map to at least the specified
        // 'minimumCapacity', and redistribute all the contained elements into
        // a new sequence of entries according to their hash values.  If
        // '0 == minimumCapacity' and '0 == size()', the map is returned to the
        // default constructed state.  After this call, 'load_factor()' will be
        // less than or equal to 'max_load_factor()' and all iterators,
        // pointers, and references to elements of this map are invalidated.

    void reserve(bsl::size_t numEntries);
        // Change the capacity of this map to at least a capacity that can
        // accommodate the specified 'numEntries' (accounting for the load
        // factor invariant), and redistribute all the contained elements into
        // a new sequence of entries according to their hash values.  If
        // '0 == numEntries' and '0 == size()', the map is returned to the
        // default constructed state.  After this call, 'load_factor()' will be
        // less than or equal to 'max_load_factor()' and all iterators,
        // pointers, and references to elements of this map are invalidated.
        // Note that this method is effectively equivalent to:
        //..
        //     rehash(bsl::ceil(numEntries / max_load_factor()))
        //..

    void reset();
        // Remove all elements from this map and release all memory from this
        // map, returning the map to the default constructed state.

                          // Iterators

    iterator begin();
        // Return an iterator to the first element in the sequence of
        // modifiable elements maintained by this map, or the 'end' iterator if
        // this map is empty.

    iterator end();
        // Return an iterator to the past-the-end element in the sequence of
        // modifiable elements maintained by this map.

                             // Aspects

    void swap(FlatHashMap& other);
        // Exchange the value of this object as well as its hasher and equality
        // functors with those of the specified 'other' object.  The behavior
        // is undefined unless this object was created with the same allocator
        // as 'other'.

    // ACCESSORS
    bsl::size_t capacity() const;
        // Return the number of elements this map could hold if the load factor
        // were 1.

    bool contains(const KEY& key) const;
        // Return 'true' if this map contains an element having the specified
        // 'key', and 'false' otherwise.

    bsl::size_t count(const KEY& key) const;
        // Return the number of elements in this map having the specified
        // 'key'.  Note that since a flat hash map maintains unique keys, the
        // returned value will be either 0 or 1.

    bool empty() const;
        // Return 'true' if this map contains no elements, and 'false'
        // otherwise.

    bsl::pair<const_iterator, const_iterator> equal_range(
                                                         const KEY& key) const;
        // Return a pair of 'const_iterator's defining the sequence of elements
        // in this map having the specified 'key', where the first iterator is
        // positioned at the start of the sequence and the second iterator is
        // positioned one past the end of the sequence.  If this map contains
        // no elements having a key equivalent to 'key', then the two returned
        // iterators will have the same value.  Note that since a map maintains
        // unique keys, the range will contain at most one element.

    const_iterator find(const KEY& key) const;
        // Return a 'const_iterator' referring to the element in this map
        // having the specified 'key', or 'end()' if no such entry exists in
        // this map.

    HASH hash_function() const;
        // Return (a copy of) the unary hash functor used by this map to
        // generate a hash value (of type 'bsl::size_t') for a 'KEY' object.

    EQUAL key_eq() const;
        // Return (a copy of) the binary key-equality functor that returns
        // 'true' if the value of two 'KEY' objects are equivalent, and 'false'
        // otherwise.

    float load_factor() const;
        // Return the current ratio between the number of elements in this
        // container and its capacity.

    float max_load_factor() const;
        // Return the maximum load factor allowed for this map.  Note that if
        // an insert operation would cause the load factor to exceed
        // 'max_load_factor()', that same insert operation will increase the
        // capacity and rehash the entries of the container (see {Load Factor
        // and Resizing}).  Also note that the value returned by
        // 'max_load_factor' is implementation defined and cannot be changed by
        // the user.

    bsl::size_t size() const;
        // Return the number of elements in this map.

                          // Iterators

    const_iterator begin() const;
        // Return a 'const_iterator' to the first element in the sequence of
        // elements maintained by this map, or the 'end' iterator if this map
        // is empty.

    const_iterator cbegin() const;
        // Return a 'const_iterator' to the first element in the sequence of
        // elements maintained by this map, or the 'end' iterator if this map
        // is empty.

    const_iterator cend() const;
        // Return a 'const_iterator' to the past-the-end element in the
        // sequence of elements maintained by this map.

    const_iterator end() const;
        // Return a 'const_iterator' to the past-the-end element in the
        // sequence of elements maintained by this map.

                           // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this flat hash map to supply memory.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level', and return a
        // reference to the modifiable 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
bool operator==(const FlatHashMap<KEY, VALUE, HASH, EQUAL> &lhs,
                const FlatHashMap<KEY, VALUE, HASH, EQUAL> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'FlatHashMap' objects have the same
    // value if their sizes are the same and each element contained in one is
    // equal to an element of the other.  The hash and equality functors are
    // not involved in the comparison.

template <class KEY, class VALUE, class HASH, class EQUAL>
bool operator!=(const FlatHashMap<KEY, VALUE, HASH, EQUAL> &lhs,
                const FlatHashMap<KEY, VALUE, HASH, EQUAL> &rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'FlatHashMap' objects do not
    // have the same value if their sizes are different or one contains an
    // element equal to no element of the other.  The hash and equality
    // functors are not involved in the comparison.

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::ostream& operator<<(bsl::ostream&                               stream,
                         const FlatHashMap<KEY, VALUE, HASH, EQUAL>& map);
    // Write the value of the specified 'map' to the specified output 'stream'
    // in a single-line format, and return a reference providing modifiable
    // access to 'stream'.  If 'stream' is not valid on entry, this operation
    // has no effect.  Note that this human-readable format is not fully
    // specified and can change without notice.

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL>
void swap(FlatHashMap<KEY, VALUE, HASH, EQUAL>& a,
          FlatHashMap<KEY, VALUE, HASH, EQUAL>& b);
    // Exchange the value, the hasher, and the key-equality functor of the
    // specified 'a' and 'b' objects.  This function provides the no-throw
    // exception-safety guarantee if the two objects were created with the same
    // allocator and the basic guarantee otherwise.

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // ----------------------------
                       // struct FlatHashMap_EntryUtil
                       // ----------------------------

// CLASS METHODS
template <class KEY, class VALUE, class ENTRY>
template <class KEY_TYPE>
inline
void FlatHashMap_EntryUtil<KEY, VALUE, ENTRY>::construct(
                        ENTRY                                       *entry,
                        bslma::Allocator                            *allocator,
                        BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE)  key)
{
    BSLS_ASSERT_SAFE(entry);

    bsls::ObjectBuffer<VALUE> value;

    bslma::ConstructionUtil::construct(value.address(), allocator);
    bslma::DestructorGuard<VALUE> guard(value.address());

    bslma::ConstructionUtil::construct(
                                  entry,
                                  allocator,
                                  BSLS_COMPILERFEATURES_FORWARD(KEY_TYPE, key),
                                  bslmf::MovableRefUtil::move(value.object()));
}

template <class KEY, class VALUE, class ENTRY>
inline
const KEY& FlatHashMap_EntryUtil<KEY, VALUE, ENTRY>::key(const ENTRY& entry)
{
    return entry.first;
}

                            // -----------------
                            // class FlatHashMap
                            // -----------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap()
: d_impl(0, HASH(), EQUAL())
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              bslma::Allocator *basicAllocator)
: d_impl(0, HASH(), EQUAL(), basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(bsl::size_t capacity)
: d_impl(capacity, HASH(), EQUAL())
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, HASH(), EQUAL(), basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              bsl::size_t       capacity,
                                              const HASH&       hash,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, hash, EQUAL(), basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              bsl::size_t       capacity,
                                              const HASH&       hash,
                                              const EQUAL&      equal,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, hash, equal, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              bslma::Allocator *basicAllocator)
: d_impl(0, HASH(), EQUAL(), basicAllocator)
{
    insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              bsl::size_t       capacity,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, HASH(), EQUAL(), basicAllocator)
{
    insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              bsl::size_t       capacity,
                                              const HASH&       hash,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, hash, EQUAL(), basicAllocator)
{
    insert(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                              INPUT_ITERATOR    first,
                                              INPUT_ITERATOR    last,
                                              bsl::size_t       capacity,
                                              const HASH&       hash,
                                              const EQUAL&      equal,
                                              bslma::Allocator *basicAllocator)
: d_impl(capacity, hash, equal, basicAllocator)
{
    insert(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                             bsl::initializer_list<value_type>  values,
                             bslma::Allocator                  *basicAllocator)
: FlatHashMap(values.begin(),
              values.end(),
              0,
              HASH(),
              EQUAL(),
              basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                             bsl::initializer_list<value_type>  values,
                             bsl::size_t                        capacity,
                             bslma::Allocator                  *basicAllocator)
: FlatHashMap(values.begin(),
              values.end(),
              capacity,
              HASH(),
              EQUAL(),
              basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                             bsl::initializer_list<value_type>  values,
                             bsl::size_t                        capacity,
                             const HASH&                        hash,
                             bslma::Allocator                  *basicAllocator)
: FlatHashMap(values.begin(),
              values.end(),
              capacity,
              hash,
              EQUAL(),
              basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                             bsl::initializer_list<value_type>  values,
                             bsl::size_t                        capacity,
                             const HASH&                        hash,
                             const EQUAL&                       equal,
                             bslma::Allocator                  *basicAllocator)
: FlatHashMap(values.begin(),
              values.end(),
              capacity,
              hash,
              equal,
              basicAllocator)
{
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                            const FlatHashMap&  original,
                                            bslma::Allocator   *basicAllocator)
: d_impl(original.d_impl, basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                       bslmf::MovableRef<FlatHashMap> original)
: d_impl(bslmf::MovableRefUtil::move(
                               bslmf::MovableRefUtil::access(original).d_impl))
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::FlatHashMap(
                                bslmf::MovableRef<FlatHashMap>  original,
                                bslma::Allocator               *basicAllocator)
: d_impl(bslmf::MovableRefUtil::move(
                               bslmf::MovableRefUtil::access(original).d_impl),
         basicAllocator)
{
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>::~FlatHashMap()
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>&
FlatHashMap<KEY, VALUE, HASH, EQUAL>::operator=(const FlatHashMap& rhs)
{
    d_impl = rhs.d_impl;

    return *this;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>&
FlatHashMap<KEY, VALUE, HASH, EQUAL>::operator=(
                                            bslmf::MovableRef<FlatHashMap> rhs)
{
    FlatHashMap& lvalue = rhs;

    d_impl = bslmf::MovableRefUtil::move(lvalue.d_impl);

    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
FlatHashMap<KEY, VALUE, HASH, EQUAL>&
FlatHashMap<KEY, VALUE, HASH, EQUAL>::operator=(
                                      bsl::initializer_list<value_type> values)
{
    FlatHashMap tmp(values.begin(),
                    values.end(),
                    0,
                    d_impl.hash_function(),
                    d_impl.key_eq(),
                    d_impl.allocator());

    this->swap(tmp);

    return *this;
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class KEY_TYPE>
inline
VALUE& FlatHashMap<KEY, VALUE, HASH, EQUAL>::operator[](
                               BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE) key)
{
    return d_impl[BSLS_COMPILERFEATURES_FORWARD(KEY_TYPE, key)].second;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void
FlatHashMap<KEY, VALUE, HASH, EQUAL>::clear()
{
    d_impl.clear();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::pair<typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator,
          typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator>
FlatHashMap<KEY, VALUE, HASH, EQUAL>::equal_range(const KEY& key)
{
    return d_impl.equal_range(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::size_t FlatHashMap<KEY, VALUE, HASH, EQUAL>::erase(const KEY& key)
{
    return d_impl.erase(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
FlatHashMap<KEY, VALUE, HASH, EQUAL>::erase(const_iterator position)
{
    BSLS_ASSERT_SAFE(position != end());

    return d_impl.erase(position);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
FlatHashMap<KEY, VALUE, HASH, EQUAL>::erase(iterator position)
{
    // Note that this overload is necessary to avoid ambiguity when the key is
    // an iterator.

    BSLS_ASSERT_SAFE(position != end());

    return d_impl.erase(position);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
FlatHashMap<KEY, VALUE, HASH, EQUAL>::erase(const_iterator first,
                                            const_iterator last)
{
    return d_impl.erase(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
FlatHashMap<KEY, VALUE, HASH, EQUAL>::find(const KEY& key)
{
    return d_impl.find(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::insert(INPUT_ITERATOR first,
                                                  INPUT_ITERATOR last)
{
    d_impl.insert(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
template <class KEY, class VALUE, class HASH, class EQUAL>
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::insert(
                                      bsl::initializer_list<value_type> values)
{
    insert(values.begin(), values.end());
}
#endif

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::rehash(bsl::size_t minimumCapacity)
{
    d_impl.rehash(minimumCapacity);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::reserve(bsl::size_t numEntries)
{
    d_impl.reserve(numEntries);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::reset()
{
    d_impl.reset();
}

                          // Iterators

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
                                  FlatHashMap<KEY, VALUE, HASH, EQUAL>::begin()
{
    return d_impl.begin();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::iterator
                                    FlatHashMap<KEY, VALUE, HASH, EQUAL>::end()
{
    return d_impl.end();
}

                             // Aspects

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void FlatHashMap<KEY, VALUE, HASH, EQUAL>::swap(FlatHashMap& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    d_impl.swap(other.d_impl);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t FlatHashMap<KEY, VALUE, HASH, EQUAL>::capacity() const
{
    return d_impl.capacity();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool FlatHashMap<KEY, VALUE, HASH, EQUAL>::contains(const KEY& key) const
{
    return d_impl.contains(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t FlatHashMap<KEY, VALUE, HASH, EQUAL>::count(const KEY& key) const
{
    return d_impl.count(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool FlatHashMap<KEY, VALUE, HASH, EQUAL>::empty() const
{
    return d_impl.empty();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::pair<typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator,
          typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator>
FlatHashMap<KEY, VALUE, HASH, EQUAL>::equal_range(const KEY& key) const
{
    return d_impl.equal_range(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator
FlatHashMap<KEY, VALUE, HASH, EQUAL>::find(const KEY& key) const
{
    return d_impl.find(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH FlatHashMap<KEY, VALUE, HASH, EQUAL>::hash_function() const
{
    return d_impl.hash_function();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL FlatHashMap<KEY, VALUE, HASH, EQUAL>::key_eq() const
{
    return d_impl.key_eq();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float FlatHashMap<KEY, VALUE, HASH, EQUAL>::load_factor() const
{
    return d_impl.load_factor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float FlatHashMap<KEY, VALUE, HASH, EQUAL>::max_load_factor() const
{
    return d_impl.max_load_factor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t FlatHashMap<KEY, VALUE, HASH, EQUAL>::size() const
{
    return d_impl.size();
}

                          // Iterators

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator
                      FlatHashMap<KEY, VALUE, HASH, EQUAL>::begin() const
{
    return d_impl.begin();
}
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator
                     FlatHashMap<KEY, VALUE, HASH, EQUAL>::cbegin() const
{
    return d_impl.cbegin();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator
                       FlatHashMap<KEY, VALUE, HASH, EQUAL>::cend() const
{
    return d_impl.cend();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
typename FlatHashMap<KEY, VALUE, HASH, EQUAL>::const_iterator
                        FlatHashMap<KEY, VALUE, HASH, EQUAL>::end() const
{
    return d_impl.end();
}

                             // Aspects

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bslma::Allocator *FlatHashMap<KEY, VALUE, HASH, EQUAL>::allocator() const
{
    return d_impl.allocator();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::ostream& FlatHashMap<KEY, VALUE, HASH, EQUAL>::print(
                                            bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start();

    const_iterator iter = begin();
    while (iter != end()) {
        printer.printValue(*iter);
        ++iter;
    }

    printer.end();

    return stream;
}

}  // close package namespace

// FREE OPERATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool bdlc::operator==(const FlatHashMap<KEY, VALUE, HASH, EQUAL>& lhs,
                      const FlatHashMap<KEY, VALUE, HASH, EQUAL>& rhs)
{
    return lhs.d_impl == rhs.d_impl;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool bdlc::operator!=(const FlatHashMap<KEY, VALUE, HASH, EQUAL>& lhs,
                      const FlatHashMap<KEY, VALUE, HASH, EQUAL>& rhs)
{
    return lhs.d_impl != rhs.d_impl;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::ostream& bdlc::operator<<(
                            bsl::ostream&                               stream,
                            const FlatHashMap<KEY, VALUE, HASH, EQUAL>& map)
{
    return map.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void bdlc::swap(FlatHashMap<KEY, VALUE, HASH, EQUAL>& a,
                FlatHashMap<KEY, VALUE, HASH, EQUAL>& b)
{
    bslalg::SwapUtil::swap(&a.d_impl, &b.d_impl);
}

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslalg {

template <class KEY, class VALUE, class HASH, class EQUAL>
struct HasStlIterators<bdlc::FlatHashMap<KEY, VALUE, HASH, EQUAL> >
: bsl::true_type {
};

}  // close namespace bslalg

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL>
struct UsesBslmaAllocator<bdlc::FlatHashMap<KEY, VALUE, HASH, EQUAL> >
: bsl::true_type {
};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
