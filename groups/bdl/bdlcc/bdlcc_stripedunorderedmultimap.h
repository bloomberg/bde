// bdlcc_stripedunorderedmultimap.h                                   -*-C++-*-
#ifndef INCLUDED_BDLCC_STRIPEDUNORDEREDMULTIMAP
#define INCLUDED_BDLCC_STRIPEDUNORDEREDMULTIMAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bucket-group locking (*striped*) unordered multimap.
//
//@CLASSES:
//  bdlcc::StripedUnorderedMultiMap: Striped hash multimap
//
//@SEE_ALSO: bdlcc_stripedunorderedmap, bdlcc_stripedunorderedimpl
//
//@DESCRIPTION: This component provides a single concurrent (fully thread-safe)
// associative container, 'bdlcc::StripedUnorderedMultiMap', that partitions
// the underlying hash table into a (user defined) number of "bucket groups"
// and controls access to each bucket group by a separate read-write lock.
// This design allows greater concurrency (and improved performance) than a
// 'bsl::unordered_multimap' object protected by a single lock.
//
// 'bdlcc::StripedUnorderedMultiMap' differs from 'bdlcc::StripedUnorderedMap'
// in that the former allows multiple elements to have the same key value but
// the later requires that each element have a unique key value.  Methods of
// the two classes have similar names and semantics differing only where the
// different key policy pertains.
//
// The terms "bucket", "load factor", and "rehash" have the same meaning as
// they do in the 'bslstl_unorderedmultimap' component (see
// {'bslstl_unorderedmultimap'|Unordered Multimap Configuration}).  A general
// introduction to these ideas can be found at:
// https://en.wikipedia.org/wiki/Hash_table
//
// 'bdlcc::StripedUnorderedMultiMap' (and concurrent containers in general)
// does not provide iterators that allow users to manipulate or traverse the
// values of elements in a map.  Alternatively, this container provides the
// 'setComputedValue*' methods that allows users to change the value for a
// given key via a user provided functor and the 'visit' method that will apply
// a user provided functor the value of every key in the map.
//
// The 'bdlcc::StripedUnorderedMultiMap' class is an *irregular* value-semantic
// type, even if 'KEY' and 'VALUE' are VSTs.  This class does not implement
// equality comparison, assignment operator, or copy constructor.
//
///Thread Safety
///-------------
// The 'bdlcc::StripedUnorderedMultiMap' class template is fully thread-safe
// (see {'bsldoc_glossary'|Fully Thread-Safe}), assuming that the allocator is
// fully thread-safe.  Each method is executed by the calling thread.
//
///Runtime Complexity
///------------------
//..
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | insert, setValueFirst, setValueAll,                | Average: O[1]      |
//  | setComputedValueAll,  setComputedValueFirst, update| Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | eraseFirst, eraseAll, getValueFirst, getValueAll   | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | visit(key, visitor)                                | Average: O[1]      |
//  | visitReadOnly(key, visitor)                        | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | insertBulk, k elements                             | Average: O[k]      |
//  |                                                    | Worst:   O[n*k]    |
//  +----------------------------------------------------+--------------------+
//  | examine                                            | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | eraseBulkAll, k elements                           | Average: O[k]      |
//  |                                                    | Worst:   O[n*k]    |
//  +----------------------------------------------------+--------------------+
//  | rehash                                             | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | visit(visitor), visitReadOnly(visitor)             | O[n]               |
//  +----------------------------------------------------+--------------------+
//..
//
///Number of Stripes
///-----------------
// Performance improves monotonically when the number of stripes increases.
// However, the rate of improvement decreases, and reaches a plateau.  The
// plateau is reached roughly at four times the number of the threads
// *concurrently* using the hash map.
//
///Set vs. Insert Methods
///----------------------
// This container provides several 'set*' methods and similarly named 'insert*'
// methods that have nearly identical semantics.  Both update the value of an
// existing element and both add a new element if the element sought is not
// present.  Conceptually, the emphasis of the 'set*' methods is the former, so
// its return value is the number of elements updated, and the intent of
// 'insert*' methods is to add elements, so its return value is the number of
// new elements.
//
///Rehash
///------
//
///Concurrent Rehash
///- - - - - - - - -
// A rehash operation is a re-organization of the hash map to a different
// number of buckets.  This is a heavy operation that interferes with, but does
// *not* disallow, other operations on the container.  Rehash is warranted when
// the current load factor exceeds the current maximum allowed load factor.
// Expressed explicitly:
//..
//  bucketCount() <= maxLoadFactor() * size();
//..
// This above condition is tested implicitly by several methods and if found
// true (and if rehash is enabled and rehash is not underway), a rehash is
// started.  The methods that check the load factor are:
//
//: o All methods that insert elements (i.e., increase 'size()').
//: o The 'maxLoadFactor(newMaxLoadFactor)' method.
//: o The 'rehash' method.
//
///Rehash Control
/// - - - - - - -
// 'enableRehash' and 'disableRehash' methods are provided to control the
// rehash enable flag.  Note that disabling rehash does not impact a rehash in
// progress.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example shows some basic usage of 'bdlcc::StripedUnorderedMultiMap'.
//
// First, we define a 'bdlcc::StripedUnorderedMultiMap' object, 'myFriends',
// that maps 'int' to 'bsl::string':
//..
//  bdlcc::StripedUnorderedMultiMap<int, bsl::string> myFriends;
//..
// Notice that we are using the default value number of buckets, number of
// stripes, and allocator.
//
// Then, we insert three elements into the map and verify that the size is the
// expected value:
//..
//  assert(0 == myFriends.size());
//  myFriends.insert(0, "Alex");
//  myFriends.insert(1, "John");
//  myFriends.insert(2, "Rob");
//  assert(3 == myFriends.size());
//..
// Next, we demonstrate 'insertBulk' by creating a vector of three key-value
// pairs and add them to the map using a single method call:
//..
//  typedef bsl::pair<int, bsl::string> PairType;
//  bsl::vector<PairType> insertData;
//  insertData.push_back(PairType(3, "Jim"));
//  insertData.push_back(PairType(4, "Jeff"));
//  insertData.push_back(PairType(5, "Ian" ));
//  assert(3 == insertData.size())
//
//  assert(3 == myFriends.size());
//  myFriends.insertBulk(insertData.begin(), insertData.end());
//  assert(6 == myFriends.size());
//..
// Then, we use 'getValueFirst' method to retrieve the previously inserted
// string associated with the value 1:
//..
//  bsl::string value;
//  bsl::size_t rc = myFriends.getValueFirst(&value, 1);
//  assert(1      == rc);
//  assert("John" == value);
//..
// Now, we insert two additional elements, each having key values that already
// appear in the hash map:
//..
//  myFriends.insert(3, "Steve");
//  assert(7 == myFriends.size());
//
//  myFriends.insert(4, "Tim");
//  assert(8 == myFriends.size());
//..
// Finally, we use the 'getValueAll' method to retrieve both values associated
// with the key 3:
//..
//  bsl::vector<bsl::string> values;
//  rc = myFriends.getValueAll(&values, 3);
//  assert(2 == rc);
//
//  assert(2            == values.size());
//  assert(values.end() != bsl::find(values.begin(), values.end(), "Jim"));
//  assert(values.end() != bsl::find(values.begin(), values.end(), "Steve"));
//..
// Notice that the results have the expected number and values.  Also notice
// that we must search the results for the expected values because the order in
// which values are retrieved is not specified.

#include <bdlscm_version.h>

#include <bdlcc_stripedunorderedcontainerimpl.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>

#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlcc {

                    // ==============================
                    // class StripedUnorderedMultiMap
                    // ==============================

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class StripedUnorderedMultiMap {
    // This class template defines a fully thread-safe container that provides
    // a mapping from keys (of template parameter type 'KEY') to their
    // associated mapped values (of template parameter type 'VALUE').
    //
    // The buckets of this hash map are guarded by 'numStripes' reader-writer
    // locks, a value specified on construction.  Partitioning the buckets
    // among several locks allows greater overall concurrency than a
    // 'bsl::unordered_multimap' object guarded by a single lock.
    //
    // The interface is inspired by, but not identical to that of
    // 'bsl::unordered_multimap'.  Notably absent are iterators, which are of
    // limited practicality in the typical use case because they are readily
    // invalidated when the map population is open to modification by multiple
    // threads.

  private:
    // PRIVATE TYPES
    typedef StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL> Impl;

    // DATA
    Impl d_imp;
        // implementation of the striped hash map

    // NOT IMPLEMENTED
    StripedUnorderedMultiMap(
                     const StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>&);
                                                                    // = delete
    StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>&
           operator=(const StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>&);
                                                                    // = delete

  public:
    // PUBLIC CONSTANTS
    enum {
        k_DEFAULT_NUM_BUCKETS  = 16, // Default number of buckets
        k_DEFAULT_NUM_STRIPES  =  4  // Default number of stripes
    };

    // PUBLIC TYPES
    typedef bsl::pair<KEY, VALUE> KVType;
        // Value type of a bulk insert entry.

    typedef bsl::function<bool (VALUE *, const KEY&)> VisitorFunction;
        // An alias to a function meeting the following contract:
        //..
        //  bool visitorFunction(VALUE *value, const KEY& key);
        //      // Visit the specified 'value' attribute associated with the
        //      // specified 'key'.  Return 'true' if this function may be
        //      // called on additional elements, and 'false' otherwise (i.e.,
        //      // if no other elements should be visited).  Note that this
        //      // functor can change the value associated with 'key'.
        //..

    typedef bsl::function<bool (const VALUE&, const KEY&)>
                                                       ReadOnlyVisitorFunction;
        // An alias to a function meeting the following contract:
        //..
        //  bool readOnlyVisitorFunction(const VALUE& value, const KEY& key);
        //      // Visit the specified 'value' attribute associated with the
        //      // specified 'key'.  Return 'true' if this function may be
        //      // called on additional elements, and 'false' otherwise (i.e.,
        //      // if no other elements should be visited).  Note that this
        //      // functor can *not* change the values associated with 'key'
        //      // and 'value'.
        //..

    // CREATORS
    explicit StripedUnorderedMultiMap(
                   bsl::size_t       numInitialBuckets = k_DEFAULT_NUM_BUCKETS,
                   bsl::size_t       numStripes        = k_DEFAULT_NUM_STRIPES,
                   bslma::Allocator *basicAllocator = 0);
        // Create an empty 'StripedUnorderedMultiMap' object, a fully
        // thread-safe hash map where access is partitioned into "stripes" (a
        // group of buckets protected a reader-writer mutex).  Optionally
        // specify 'numInitialBuckets' and 'numStripes' which define the
        // minimum number of buckets and the (fixed) number of stripes in this
        // map.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.  The hash map has rehash enabled.  Note that the number of
        // stripes will not change after construction, but the number of
        // buckets may (unless rehashing is disabled via 'disableRehash').

    //! ~StripedUnorderedMultiMap() = default;
        // Destroy this hash map.

    // MANIPULATORS
    void clear();
        // Remove all elements from this hash map.  If rehash is in progress,
        // block until it completes.

    void disableRehash();
        // Prevent future rehash until 'enableRehash' is called.

    void enableRehash();
        // Allow rehash.  If conditions warrant, rehash will be started by the
        // *next* method call that observes the load factor is exceeded (see
        // {Concurrent Rehash}).  Note that calling
        // 'maxLoadFactor(maxLoadFactor())' (i.e., setting the maximum load
        // factor to its current value) will trigger a rehash if needed but
        // otherwise does not change the hash map.

    bsl::size_t eraseAll(const KEY& key);
        // Erase from this hash map the elements having the specified 'key'.
        // Return the number of elements erased.

    template <class RANDOM_ITER>
    bsl::size_t eraseBulkAll(RANDOM_ITER first, RANDOM_ITER last);
        // Erase from this hash map elements in this hash map having any of the
        // values in the keys contained between the specified 'first'
        // (inclusive) and 'last' (exclusive) random-access iterators.  The
        // iterators provide read access to a sequence of 'KEY' objects.  All
        // erasures are done by the calling thread and the order of erasure is
        // not specified.  Return the number of elements removed.  The behavior
        // is undefined unless 'first <= last'.  Note that the map may not have
        // an element for every value in 'keys'.

    bsl::size_t eraseFirst(const KEY& key);
        // Erase from this hash map the *first* element (of possibly many)
        // found to the specified 'key'.  Return the number of elements erased.
        // Note that method is more performant than 'eraseAll' when there is
        // one element having 'key'.

    void insert(const KEY& key, const VALUE& value);
        // Insert into this hash map an element having the specified 'key' and
        // 'value'.  Note that other elements having the same 'key' may exist
        // in this hash map.

    void insert(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Insert into this hash map an element having the specified 'key' and
        // the specified move-insertable 'value'.  The 'value' object is left
        // in a valid but unspecified state.  If 'value' is allocator-enabled
        // and 'allocator() != value.allocator()' this operation may cost as
        // much as a copy.  Note that other elements having the same 'key' may
        // exist in this hash map.

    template <class RANDOM_ITER>
    void insertBulk(RANDOM_ITER first, RANDOM_ITER last);
        // Insert into this hash map elements having the key-value pairs
        // obtained between the specified 'first' (inclusive) and 'last'
        // (exclusive) random-access iterators.  The iterators provide read
        // access to a sequence of 'bsl::pair<KEY, VALUE>' objects.  All
        // insertions are done by the calling thread and the order of insertion
        // is not specified.  The behavior is undefined unless 'first <= last'.

    void maxLoadFactor(float newMaxLoadFactor);
        // Set the maximum load factor of this unordered map to the specified
        // 'newMaxLoadFactor'.  If 'newMaxLoadFactor < loadFactor()', this
        // operation will cause an immediate rehash; otherwise, this operation
        // has a constant-time cost.  The rehash will increase the number of
        // buckets by a power of 2.  The behavior is undefined unless
        // '0 < newMaxLoadFactor'.

    void rehash(bsl::size_t numBuckets);
        // Recreate this hash map to one having at least the specified
        // 'numBuckets'.  This operation is a no-op if *any* of the following
        // are true: 1) rehash is disabled; 2) 'numBuckets' less or equals the
        // current number of buckets.  See {Rehash}.

    int setComputedValueAll(const KEY&             key,
                            const VisitorFunction& visitor);
        // Serially invoke the specified 'visitor' passing the specified 'key',
        // and the address of the value of each element in this hash map having
        // 'key'.  If 'key' is not in the map, 'value' will be default
        // constructed.  That is, for each '(key, value)' found, invoke:
        //..
        //  bool visitor(VALUE *value, const Key& key);
        //..
        // If no element in the map has 'key', insert '(key, VALUE())' and
        // invoke 'visitor' with 'value' pointing to the default constructed
        // value.  Return the number of elements visited or the negation of
        // that value if visitations stopped because 'visitor' returned
        // 'false'.  'visitor', when invoked, has exclusive access (i.e., write
        // access) to each element during each invocation.  The behavior is
        // undefined if hash map manipulators and 'getValue*' methods are
        // invoked from within 'visitor', as it may lead to a deadlock.  Note
        // that the 'setComputedValueFirst' method is more performant than the
        // when the hash map contains a single element for 'key'.  Also note
        // that a return value of '0' implies that an element was inserted.

    int setComputedValueFirst(const KEY&             key,
                              const VisitorFunction& visitor);
        // Invoke the specified 'visitor' passing the specified 'key', and the
        // address of the value attribute of the *first* element (of possibly
        // many elements) found in this hash map having 'key'.  If 'key' is not
        // in the map, 'value' will be default constructed.  That is, for
        // '(key, value)', invoke:
        //..
        //  bool visitor(VALUE *value, const Key& key);
        //..
        // If no element in the map has 'key', insert '(key, VALUE())' and
        // invoke 'visitor' with 'value' pointing to the default constructed
        // value.  Return 1 if 'key' was found and 'visitor' returned 'true', 0
        // if 'key' was not found, and -1 if 'key' was found and 'visitor'
        // returned 'false'.  'visitor', when invoked, has exclusive access
        // (i.e., write access) to the element.  The behavior is undefined if
        // hash map manipulators and 'getValue*' methods are invoked from
        // within 'visitor', as it may lead to a deadlock.  Note that the
        // return value equals the number of elements inserted.  Also note
        // that, when there are multiple elements having 'key', the selection
        // of "first" is implementation specific and subject to change.  Also
        // note that this method is more performant than the
        // 'setComputedValueAll' method when the hash map contains a single
        // element for 'key'.  Also note that a return value of '0' implies
        // that an element was inserted.

    bsl::size_t setValueAll(const KEY& key, const VALUE& value);
        // Set the value attribute of every element in this hash map having the
        // specified 'key' to the specified 'value'.  If no such such element
        // exists, insert '(key, value)'.  Return the number of elements found
        // with 'key'.  Note that if no elements were found, and a new value
        // was inserted, '0' is returned.

    bsl::size_t setValueFirst(const KEY& key, const VALUE& value);
        // Set the value attribute of the *first* element in this hash map (of
        // possibly many) found to have the specified 'key' to the specified
        // 'value'.  If no such such element exists, insert '(key, value)'.
        // Return the number of elements found with 'key'.  Note that if no
        // elements were found, and a new value was inserted, '0' is returned.
        // Also note that this method is more performant than 'setValueAll'
        // when there is one element having 'key' in the hash map.

    bsl::size_t setValueFirst(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Set the value attribute of the *first* element in this hash map (of
        // possibly many) found to have the specified 'key' to the specified
        // move-insertable 'value'.  If no such such element exists, insert
        // '(key, value)'.  Return the number of elements found with 'key'.
        // The 'value' object is left in a valid but unspecified state.  If
        // 'value' is allocator-enabled and 'allocator() != value.allocator()'
        // this operation may cost as much as a copy.  Note that if no elements
        // were found, and a new value was inserted, '0' is returned.  Also
        // note that this method is more performant than 'setValueAll' when
        // there is one element having 'key' in the hash map.

    int update(const KEY& key, const VisitorFunction& visitor);
        // !DEPRECATED!: Use 'visit(key, visitor)' instead.
        //
        // Serially call the specified 'visitor' on each element (if one
        // exists) in this hash map having the specified 'key' until every such
        // element has been updated or 'visitor' returns 'false'.  That is, for
        // '(key, value)', invoke:
        //..
        //  bool visitor(&value, key);
        //..
        // Return the number of elements visited or the negation of that value
        // if visitations stopped because 'visitor' returned 'false'.
        // 'visitor' has exclusive access (i.e., write access) to each element
        // for duration of each invocation.  The behavior is undefined if hash
        // map manipulators and 'getValue*' methods are invoked from within
        // 'visitor', as it may lead to a deadlock.

    int visit(const VisitorFunction& visitor);
        // Call the specified 'visitor' (in an unspecified order) on the
        // elements in this hash table until each such element has been visited
        // or 'visitor' returns 'false'.  That is, for '(key, value)', invoke:
        //..
        //  bool visitor(&value, key);
        //..
        // Return the number of elements visited or the negation of that value
        // if visitations stopped because 'visitor' returned 'false'.
        // 'visitor' has exclusive access (i.e., write access) to each element
        // for duration of each invocation.  Every element present in this hash
        // map at the time 'visit' is invoked will be visited unless it is
        // removed before 'visitor' is called for that element.  Each
        // visitation is done by the calling thread and the order of visitation
        // is not specified.  Elements inserted during the execution of 'visit'
        // may or may not be visited.  The behavior is undefined if hash map
        // manipulators and 'getValue*' methods are invoked from within
        // 'visitor', as it may lead to a deadlock.  Note that 'visitor' can
        // change the value of the visited elements.

    int visit(const KEY& key, const VisitorFunction& visitor);
        // Serially call the specified 'visitor' on each element (if one
        // exists) in this hash map having the specified 'key' until every such
        // element has been updated or 'visitor' returns 'false'.  That is, for
        // '(key, value)', invoke:
        //..
        //  bool visitor(&value, key);
        //..
        // Return the number of elements visited or the negation of that value
        // if visitations stopped because 'visitor' returned 'false'.
        // 'visitor' has exclusive access (i.e., write access) to each element
        // for duration of each invocation.  The behavior is undefined if hash
        // map manipulators and 'getValue*' methods are invoked from within
        // 'visitor', as it may lead to a deadlock.

    // ACCESSORS
    bsl::size_t bucketCount() const;
        // Return the number of buckets in the array of buckets maintained by
        // this hash map.  Note that unless rehash is disabled, the value
        // returned may be obsolete by the time it is received.

    bsl::size_t bucketIndex(const KEY& key) const;
        // Return the index of the bucket, in the array of buckets maintained
        // by this hash map, where elements having the specified 'key' are
        // inserted.  Note that unless rehash is disabled, the value returned
        // may be obsolete at the time it is returned.

    bsl::size_t bucketSize(bsl::size_t index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this hash
        // map.  The behavior is undefined unless
        // '0 <= index < bucketCount()'.  Note that unless rehash is disabled
        // the value returned may be obsolete by the time it is returned.

    bool empty() const;
        // Return 'true' if this hash map contains no elements, and 'false'
        // otherwise.

    EQUAL equalFunction() const;
        // Return (a copy of) the key-equality functor used by this hash map.
        // The returned function will return 'true' if two 'KEY' objects have
        // the same value, and 'false' otherwise.

    bsl::size_t getValueAll(bsl::vector<VALUE> *valuesPtr,
                            const KEY&          key) const;
        // Load, into the specified '*valuesPtr', the value attributes of every
        // element in this hash map having the specified 'key'.  Return the
        // number of elements found with 'key'.  Note that the order of the
        // values returned is not specified.

    bsl::size_t getValueFirst(VALUE *value, const KEY& key) const;
        // Load, into the specified '*value', the value attribute of the first
        // element found in this hash map having the specified 'key'.  Return 1
        // on success, and 0 if 'key' does not exist in this hash map.  Note
        // that the return value equals the number of values returned.

    HASH hashFunction() const;
        // Return (a copy of) the unary hash functor used by this hash map.
        // The return function will generate a hash value (of type
        // 'std::size_t') for a 'KEY' object.

    bool isRehashEnabled() const;
        // Return 'true' if rehash is enabled, or 'false' otherwise.

    float loadFactor() const;
        // Return the current quotient of the size of this hash map and the
        // number of buckets.  Note that the load factor is a measure of
        // container "fullness"; that is, a high load factor typically implies
        // many collisions (many elements landing in the same bucket) and that
        // decreases performance.  See {Rehash Control}.

    float maxLoadFactor() const;
        // Return the maximum load factor allowed for this hash map.  If an
        // insert operation would cause the load factor to exceed the
        // 'maxLoadFactor()' and rehashing is enabled, then that insert
        // increases the number of buckets and rehashes the elements of the
        // container into that larger set of buckets.  See {Rehash Control}.

    bsl::size_t numStripes() const;
        // Return the number of stripes in the hash.

    int visitReadOnly(const ReadOnlyVisitorFunction& visitor) const;
        // Call the specified 'visitor' (in an unspecified order) on the
        // elements in this hash table until each such element has been visited
        // or 'visitor' returns 'false'.  That is, for '(key, value)', invoke:
        //..
        //  bool visitor(value, key);
        //..
        // Return the number of elements visited or the negation of that value
        // if visitations stopped because 'visitor' returned 'false'.
        // 'visitor' has read-only access to each element for duration of each
        // invocation.  Every element present in this hash map at the time
        // 'visit' is invoked will be visited unless it is removed before
        // 'visitor' is called for that element.  Each visitation is done by
        // the calling thread and the order of visitation is not specified. The
        // behavior is undefined if hash map manipulators are invoked from
        // within 'visitor', as it may lead to a deadlock.  Note that 'visitor'
        // can *not* change the value of the visited elements.

    int visitReadOnly(const KEY&                     key,
                      const ReadOnlyVisitorFunction& visitor) const;
        // Serially call the specified 'visitor' on each element (if one
        // exists) in this hash map having the specified 'key' until every such
        // element has been visited or 'visitor' returns 'false'.  That is, for
        // '(key, value)', invoke:
        //..
        //  bool visitor(value, key);
        //..
        // Return the number of elements visited or the negation of that value
        // if visitations stopped because 'visitor' returned 'false'.
        // 'visitor' has read-only access to each element for duration of each
        // invocation.  The behavior is undefined if hash map manipulators are
        // invoked from within 'visitor', as it may lead to a deadlock.

    bsl::size_t size() const;
        // Return the current number of elements in this hash map.

                               // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this hash map to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator installed at that time is used.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                    // ------------------------------
                    // class StripedUnorderedMultiMap
                    // ------------------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::StripedUnorderedMultiMap(
                                           bsl::size_t       numInitialBuckets,
                                           bsl::size_t       numStripes,
                                           bslma::Allocator *basicAllocator)
: d_imp(numInitialBuckets, numStripes, basicAllocator)
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::clear()
{
    d_imp.clear();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::disableRehash()
{
    d_imp.disableRehash();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::enableRehash()
{
    d_imp.enableRehash();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::eraseAll(
                                                                const KEY& key)
{
    return d_imp.eraseAll(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::eraseBulkAll(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return d_imp.eraseBulkAll(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::eraseFirst(
                                                                const KEY& key)
{
    return d_imp.eraseFirst(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::insert(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    d_imp.insertAlways(key, value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::insert(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    d_imp.insertAlways(key, bslmf::MovableRefUtil::move(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::insertBulk(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    d_imp.insertBulkAlways(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::maxLoadFactor(
                                                        float newMaxLoadFactor)
{
    BSLS_ASSERT(0 < newMaxLoadFactor);

    d_imp.maxLoadFactor(newMaxLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::rehash(
                                                        bsl::size_t numBuckets)
{
    d_imp.rehash(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int
StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::setComputedValueAll(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.setComputedValueAll(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int
StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::setComputedValueFirst(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.setComputedValueFirst(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::setValueAll(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return d_imp.setValueAll(key, value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::setValueFirst(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return d_imp.setValueFirst(key, value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::setValueFirst(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    return d_imp.setValueFirst(key, bslmf::MovableRefUtil::move(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::update(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.update(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::visit(
                                                const VisitorFunction& visitor)
{
    return d_imp.visit(visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::visit(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.visit(key, visitor);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::bucketCount() const
{
    return d_imp.bucketCount();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::bucketIndex(
                                                          const KEY& key) const
{
    return d_imp.bucketIndex(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::bucketSize(
                                                       bsl::size_t index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(bucketCount() > index);

    return d_imp.bucketSize(index);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::empty() const
{
    return d_imp.empty();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_imp.equalFunction();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::getValueAll(
                                                 bsl::vector<VALUE> *valuesPtr,
                                                 const KEY&          key) const
{
    return d_imp.getValue(valuesPtr, key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::getValueFirst(
                                                         VALUE      *value,
                                                         const KEY&  key) const
{
    return d_imp.getValue(value, key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_imp.hashFunction();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::isRehashEnabled() const
{
    return d_imp.isRehashEnabled();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::loadFactor() const
{
    return d_imp.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float
StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::maxLoadFactor() const
{
    return d_imp.maxLoadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::numStripes()
                                                                          const
{
    return d_imp.numStripes();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::visitReadOnly(
                                  const ReadOnlyVisitorFunction& visitor) const
{
    return d_imp.visitReadOnly(visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::visitReadOnly(
                                  const KEY&                     key,
                                  const ReadOnlyVisitorFunction& visitor) const
{
    return d_imp.visitReadOnly(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::size() const
{
    return d_imp.size();
}

                               // Aspects

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bslma::Allocator *StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL>::
                                                              allocator() const
{
    return d_imp.allocator();
}

}  // close package namespace

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL>
struct
UsesBslmaAllocator<bdlcc::StripedUnorderedMultiMap<KEY, VALUE, HASH, EQUAL> >
    : bsl::true_type {
};

}  // close namespace bslma

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
