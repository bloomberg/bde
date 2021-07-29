// bdlcc_stripedunorderedmap.h                                        -*-C++-*-
#ifndef INCLUDED_BDLCC_STRIPEDUNORDEREDMAP
#define INCLUDED_BDLCC_STRIPEDUNORDEREDMAP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a bucket-group locking (i.e., *striped*) unordered map.
//
//@CLASSES:
//  bdlcc::StripedUnorderedMap: Striped hash map
//
//@SEE_ALSO: bdlcc_stripedunorderedmultimap,
//           bdlcc_stripedunorderedcontainerimpl
//
//@DESCRIPTION: This component provides a single concurrent (fully thread-safe)
// associative container, 'bdlcc::StripedUnorderedMap', that partitions the
// underlying hash table into a (user defined) number of "bucket groups" and
// controls access to each bucket group by a separate read-write lock.  This
// design allows greater concurrency (and improved performance) than a
// 'bsl::unordered_map' object protected by a single lock.
//
// The terms "bucket", "load factor", and "rehash" have the same meaning as
// they do in the 'bslstl_unorderedmap' component (see
// {'bslstl_unorderedmap'|Unordered Map Configuration}).  A general
// introduction to these ideas can be found at:
// https://en.wikipedia.org/wiki/Hash_table
//
// 'bdlcc::StripedUnorderedMap' (and concurrent containers in general) does not
// provide iterators that allow users to manipulate or traverse the values of
// elements in a map.  Alternatively, this container provides the
// 'setComputedValue' method that allows users to change the value for a given
// key via a user provided functor and the 'visit' method that will apply a
// user provided functor the value of every key in the map.
//
// The 'bdlcc::StripedUnorderedMap' class is an *irregular* value-semantic
// type, even if 'KEY' and 'VALUE' are VSTs.  This class does not implement
// equality comparison, assignment operator, or copy constructor.
//
///Thread Safety
///-------------
// The 'bdlcc::StripedUnorderedMap' class template is fully thread-safe (see
// {'bsldoc_glossary'|Fully Thread-Safe}), assuming that the allocator is fully
// thread-safe.  Each method is executed by the calling thread.
//
///Runtime Complexity
///------------------
//..
//  +----------------------------------------------------+--------------------+
//  | Operation                                          | Complexity         |
//  +====================================================+====================+
//  | insert, setValue, setComputedValue, update         | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | erase, getValue                                    | Average: O[1]      |
//  |                                                    | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | visit(key, visitor)                                | Average: O[1]      |
//  | visitReadOnly(key, visitor)                        | Worst:   O[n]      |
//  +----------------------------------------------------+--------------------+
//  | insertBulk, k elements                             | Average: O[k]      |
//  |                                                    | Worst:   O[n*k]    |
//  +----------------------------------------------------+--------------------+
//  | eraseBulk, k elements                              | Average: O[k]      |
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
// This container provides several 'set*' methods and analogously named
// 'insert*' methods having semantics that are identical except for the meaning
// of the return value.  The rationale is best explained in the context of the
// 'bdlcc::StripedUnorderedMultiMap' class.  See
// {'bdlcc_stripedunorderedmultimap'|Set vs. Insert methods}.  The behavior as
// seen in *this* component is the degenerate case when the number of elements
// updated (or inserted) is limited to 0 or 1.
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
// This example shows some basic usage of 'bdlcc::StripedUnorderedMap'.
//
// First, we define a 'bdlcc::StripedUnorderedMap' object, 'myFriends', that
// maps 'int' to 'bsl::string':
//..
//  bdlcc::StripedUnorderedMap<int, bsl::string> myFriends;
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
// Then, we 'getValue' method to retrieve the previously inserted string
// associated with the value 1:
//..
//  bsl::string value;
//  bsl::size_t rc = myFriends.getValue(&value, 1);
//  assert(1      == rc);
//  assert("John" == value);
//..
// Now, we change the value associated with 1 from "John" to "Jack" and confirm
// that the size of the map has not changed:
//..
//  rc = myFriends.setValue(1, "Jack");
//  assert(1 == rc);
//  assert(6 == myFriends.size());
//
//  rc = myFriends.getValue(&value, 1);
//  assert(1      == rc);
//  assert("Jack" == value);
//..
// Finally, we erase the element '(3, "Jim")' from the map, confirm that the
// map size is decremented, and that element can no longer be found in the map:
//..
//  rc = myFriends.erase(3);
//  assert(1 == rc);
//  assert(5 == myFriends.size());
//
//  rc = myFriends.getValue(&value, 3);
//  assert(0 == rc);
//..
//
///Example 2: Track Stats
/// - - - - - - - - - - -
// This example uses the 'setComputedValue' and 'update' methods to keep track
// of user ID usage counts (stats).  A striped unordered map has the user ID as
// the key, and the count as the value.  There are 2 functors, one used to
// increase the count (and set it to '1' if the user ID has not been referenced
// yet), and the other is used to decrease the count.
//..
//  typedef bdlcc::StripedUnorderedMap<int, int> StatsMap;
//..
//
// First, define a functor, 'IncFunctor', that has parameters corresponding to
// the 'KEY' and 'VALUE' types of 'StatsMap' and, when invoked, adds 1 to
// whatever existing value is associated with the given 'KEY' value.  As a new
// value is initialized with default (0), adding 1 to it works correctly:
//..
//  struct IncFunctor {
//      bool operator()(int        *value,  // 'VALUE *'
//                      const int&)         // 'const KEY&'
//      {
//          *value += 1;
//          return true;
//      }
//  };
//..
//
// Next, define a functor, 'DecFunctor', that has parameters corresponding to
// the 'KEY' and 'VALUE' types of 'StatsMap' and, when invoked, subtracts 1
// from whatever existing value is associated with the given 'KEY' value:
//..
//      struct DecFunctor {
//          bool operator()(int        *value,  // 'VALUE *'
//                          const int&  )       // 'const KEY&'
//          {
//              *value -= 1;
//              return true;
//          }
//      };
//..
//
// Then, create 'myStats', a 'StatsMap' object with (as we did in {Example 1}
// default number of buckets, number of stripes, and allocator:
//..
//  StatsMap myStats;
//..
// Next, instantiate 'myIncFunctor' and 'myDecFunctor' from 'IncFunctor' and
// 'DecFunctor', respectively:
//..
//  IncFunctor myIncFunctor;
//  DecFunctor myDecFunctor;
//..
// Next, increase count for three user IDs:
//..
//  assert(0 == myStats.size());
//  int rc = myStats.setComputedValue(1001, myIncFunctor);
//  assert(0 == rc);
//  rc = myStats.setComputedValue(1002, myIncFunctor);
//  assert(0 == rc);
//  rc = myStats.setComputedValue(1003, myIncFunctor);
//  assert(0 == rc);
//  assert(3 == myStats.size());
//  int value = 0;
//  rc = myStats.getValue(&value, 1001);
//  assert(1 == rc);
//  assert(1 == value);
//..
// Now, increase count for existing user IDs.  Confirm that the values have
// been updated as expected.
//..
//  rc = myStats.setComputedValue(1001, myIncFunctor);
//  assert(1 == rc);
//  rc = myStats.setComputedValue(1002, myIncFunctor);
//  assert(1 == rc);
//  rc = myStats.setComputedValue(1001, myIncFunctor);
//  assert(1 == rc);
//  assert(3 == myStats.size());
//  rc = myStats.getValue(&value, 1001);
//  assert(1 == rc);
//  assert(3 == value);
//  rc = myStats.getValue(&value, 1002);
//  assert(1 == rc);
//  assert(2 == value);
//..
// Finally decrease count for existing user IDs.  Confirm that the values have
//  been updated as expected.
//..
//  int ret = myStats.update(1001, myDecFunctor);
//  assert(1 == ret);
//  ret = myStats.update(1003, myDecFunctor);
//  assert(1 == ret);
//  assert(3 == myStats.size());
//  rc = myStats.getValue(&value, 1001);
//  assert(1 == rc);
//  assert(2 == value);
//  rc = myStats.getValue(&value, 1003);
//  assert(1 == rc);
//  assert(0 == value);
//..
///Example 3: Visiting all the Container Elements
/// - - - - - - - - - - - - - - - - - - - - - - -
// This example uses the 'visit' method to apply a transformation (as defined
// by a functor) to the value of every key-value pair in the map.  This example
// will construct a map from names (type 'bsl::string') to some (arbitrary)
// measure of salary (type 'int'):
//..
//  typedef bdlcc::StripedUnorderedMap<bsl::string, int> SalaryMap;
//..
// First, define a functor, 'mySalaryAdjustmentVisitor', that increases values
// above 1000 by 3% and lower values by 5%.  The fractional part of increases
// are truncated to 'int' values:
//..
//  struct mySalaryAdjustmentVisitor {
//      bool operator()(int                *value,  // 'VALUE *'
//                      const bsl::string&)         // 'const KEY&'
//      {
//          if (*value <= 1000) {
//              *value = static_cast<int>(*value * 1.05);
//          } else {
//              *value = static_cast<int>(*value * 1.03);
//          }
//          return true;
//      }
//  };
//..
// Then, default create 'mySalaries', a 'SalaryMap' object:
//..
//  SalaryMap mySalaries;
//..
// Next, load 'mySalaries' with some representative elements:
//..
//  mySalaries.insert("Alex", 1000);
//  mySalaries.insert("John",  800);
//  mySalaries.insert("Rob",  1100);
//  assert(3 == mySalaries.size());
//..
// Now, apply 'mySalaryAdjustmentVisitor' to every element in the map:
//..
//  mySalaryAdjustmentVisitor func;
//  mySalaries.visit(func);
//  assert(3 == mySalaries.size());
//..
// Finally, confirm that the values have been adjusted as expected:
//..
//
//  int         value;
//  bsl::size_t rc;
//
//  rc = mySalaries.getValue(&value, "Alex");
//  assert(1    == rc);
//  assert(1050 == value);
//
//  rc = mySalaries.getValue(&value, "John");
//  assert(1    == rc);
//  assert( 840 == value);
//
//  rc = mySalaries.getValue(&value, "Rob");
//  assert(1    == rc);
//  assert(1133 == value);
//..

#include <bdlscm_version.h>

#include <bdlcc_stripedunorderedcontainerimpl.h>

#include <bslmf_movableref.h>

#include <bsls_assert.h>

#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlcc {

                         // =========================
                         // class StripedUnorderedMap
                         // =========================

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class StripedUnorderedMap {
    // This class template defines a fully thread-safe container that provides
    // a mapping from keys (of template parameter type 'KEY') to their
    // associated mapped values (of template parameter type 'VALUE').
    //
    // The buckets of this hash map are guarded by 'numStripes' reader-writer
    // locks, a value specified on construction.  Partitioning the buckets
    // among several locks allows greater overall concurrency than a
    // 'bsl::unordered_map' object guarded by a single lock.
    //
    // The interface is inspired by, but not identical to that of
    // 'bsl::unordered_map'.  Notably absent are iterators, which are of
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
    StripedUnorderedMap(const StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>&);
                                                                    // = delete
    StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>&
                operator=(const StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>&);
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
        //  bool visitorFunction(const VALUE& value, const KEY& key);
        //      // Visit the specified 'value' attribute associated with the
        //      // specified 'key'.  Return 'true' if this function may be
        //      // called on additional elements, and 'false' otherwise (i.e.,
        //      // if no other elements should be visited).  Note that this
        //      // functor can *not* change the value associated with 'key'
        //      // and 'value'.
        //..

    // CREATORS
    explicit StripedUnorderedMap(
                   bsl::size_t       numInitialBuckets = k_DEFAULT_NUM_BUCKETS,
                   bsl::size_t       numStripes        = k_DEFAULT_NUM_STRIPES,
                   bslma::Allocator *basicAllocator = 0);
        // Create an empty 'StripedUnorderedMap' object, a fully thread-safe
        // hash map where access is partitioned into "stripes" (a group of
        // buckets protected a reader-writer mutex).  Optionally specify
        // 'numInitialBuckets' and 'numStripes' which define the minimum number
        // of buckets and the (fixed) number of stripes in this map.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The hash map has rehash enabled.  Note that the number of
        // stripes will not change after construction, but the number of
        // buckets may (unless rehashing is disabled via 'disableRehash').

    //! ~StripedUnorderedMap() = default;
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

    bsl::size_t erase(const KEY& key);
        // Erase from this hash map the element having the specified 'key'.
        // Return 1 on success and 0 if 'key' does not exist.  Note that the
        // returned value equals the number of elements removed.

    template <class RANDOM_ITER>
    bsl::size_t eraseBulk(RANDOM_ITER first, RANDOM_ITER last);
        // Erase from this hash map elements in this hash map having any of the
        // values in the keys contained between the specified 'first'
        // (inclusive) and 'last' (exclusive) random-access iterators.  The
        // iterators provide read access to a sequence of 'KEY' objects.  All
        // erasures are done by the calling thread and the order of erasure is
        // not specified.  Return the number of elements removed.  The behavior
        // is undefined unless 'first <= last'.  Note that the map may not have
        // an element for every value in 'keys'.

    bsl::size_t insert(const KEY& key, const VALUE& value);
        // Insert into this hash map an element having the specified 'key' and
        // 'value'.  If 'key' already exists in this hash map, the value
        // attribute of that element is set to 'value'.  Return 1 if an element
        // is inserted, and 0 if an existing element is updated.  Note that the
        // return value equals the number of elements inserted.

    bsl::size_t insert(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Insert into this hash map an element having the specified 'key' and
        // the specified move-insertable 'value'.  If 'key' already exists in
        // this hash map, the value attribute of that element is set to
        // 'value'.  Return 1 if an element is inserted, and 0 if an existing
        // element is updated.  The 'value' object is left in a valid but
        // unspecified state.  If 'value' is allocator-enabled and
        // 'allocator() != value.allocator()' this operation may cost as much
        // as a copy.  Note that the return value equals the number of elements
        // inserted.

    template <class RANDOM_ITER>
    bsl::size_t insertBulk(RANDOM_ITER first, RANDOM_ITER last);
        // Insert into this hash map elements having the key-value pairs
        // obtained between the specified 'first' (inclusive) and 'last'
        // (exclusive) random-access iterators.  The iterators provide read
        // access to a sequence of 'bsl::pair<KEY, VALUE>' objects.  If an
        // element having one of the keys already exists in this hash map, set
        // the value attribute to the corresponding value from 'data'.  All
        // insertions are done by the calling thread and the order of insertion
        // is not specified.  Return the number of elements inserted.  The
        // behavior is undefined unless 'first <= last'.

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

    int setComputedValue(const KEY&             key,
                         const VisitorFunction& visitor);
        // Invoke the specified 'visitor' on the value associated with the
        // specified 'key'.  The 'visitor' will be passed the address of the
        // value, and 'key'. If 'key' is not in the map, 'value' will be
        // default constructed.  That is, 'visitor' must be invocable with the
        // 'VisitorFunction' signature:
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
        // return value equals the number of elements found having 'key'.  Also
        // note that a return value of '0' implies that an element was
        // inserted.

    bsl::size_t setValue(const KEY& key, const VALUE& value);
        // Set the value attribute of the element in this hash map having the
        // specified 'key' to the specified 'value'.  If no such such element
        // exists, insert '(key, value)'.  Return 1 if 'key' was found, and 0
        // otherwise.  Note that the return value equals the number of elements
        // found having 'key'.

    bsl::size_t setValue(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Set the value attribute of the element in this hash map having the
        // specified 'key' to the specified move-insertable 'value'.  If no
        // such such element exists, insert '(key, value)'.  Return 1 if 'key'
        // was found, and 0 otherwise.  The 'value' object is left in a valid
        // but unspecified state.  If 'value' is allocator-enabled and
        // 'allocator() != value.allocator()' this operation may cost as much
        // as a copy.  Note that the return value equals the number of elements
        // found having 'key'.

    int update(const KEY& key, const VisitorFunction& visitor);
        // !DEPRECATED!: Use 'visit(key, visitor)' instead.
        //
        // Call the specified 'visitor' with the element (if one exists) in
        // this hash map having the specified 'key'.  That is:
        //..
        //  bool visitor(&value, key);
        //..
        // Return the number of elements updated or -1 if 'visitor' returned
        // 'false'.  'visitor' has exclusive access (i.e., write access) the
        // element for during its invocation.  The behavior is undefined if
        // hash map manipulators and 'getValue*' methods are invoked from
        // within 'visitor', as it may lead to a deadlock.

    int visit(const VisitorFunction& visitor);
        // Call the specified 'visitor' (in an unspecified order) on all
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
        // Call the specified 'visitor' with the element (if one exists) in
        // this hash map having the specified 'key'.  That is:
        //..
        //  bool visitor(&value, key);
        //..
        // Return the number of elements updated or -1 if 'visitor' returned
        // 'false'.  'visitor' has exclusive access (i.e., write access) the
        // element for during its invocation.  The behavior is undefined if
        // hash map manipulators and 'getValue*' methods are invoked from
        // within 'visitor', as it may lead to a deadlock.

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

    bsl::size_t getValue(VALUE *value, const KEY& key) const;
        // Load, into the specified '*value', the value attribute of the
        // element in this hash map having the specified 'key'.  Return 1 on
        // success and 0 if 'key' does not exist in this hash map.  Note that
        // the return value equals the number of values returned.

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
        // Call the specified 'visitor' (in an unspecified order) on all
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
        // the calling thread and the order of visitation is not specified.
        // The behavior is undefined if hash map manipulators are invoked from
        // within 'visitor', as it may lead to a deadlock.  Note that 'visitor'
        // can *not* change the value of the visited elements.

    int visitReadOnly(const KEY&                     key,
                      const ReadOnlyVisitorFunction& visitor) const;
        // Call the specified 'visitor' on element (if one exists) in this hash
        // map having the specified 'key'.  That is, for '(key, value)',
        // invoke:
        //..
        //  bool visitor(value, key);
        //..
        // Return the number of elements visited or '-1' if 'visitor' returned
        // 'false'.  'visitor' has read-only access to each element for
        // duration of each invocation.  The behavior is undefined if hash map
        // manipulators are invoked from within 'visitor', as it may lead to a
        // deadlock.

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

                         // -------------------------
                         // class StripedUnorderedMap
                         // -------------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::StripedUnorderedMap(
                                           bsl::size_t       numInitialBuckets,
                                           bsl::size_t       numStripes,
                                           bslma::Allocator *basicAllocator)
: d_imp(numInitialBuckets, numStripes, basicAllocator)
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::clear()
{
    d_imp.clear();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::disableRehash()
{
    d_imp.disableRehash();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::enableRehash()
{
    d_imp.enableRehash();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::erase(const KEY& key)
{
    return d_imp.eraseFirst(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::eraseBulk(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return d_imp.eraseBulkFirst(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::insert(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return d_imp.insertUnique(key, value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::insert(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    return d_imp.insertUnique(key, bslmf::MovableRefUtil::move(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::insertBulk(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return d_imp.insertBulkUnique(first, last);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::maxLoadFactor(
                                                        float newMaxLoadFactor)
{
    BSLS_ASSERT(0 < newMaxLoadFactor);

    d_imp.maxLoadFactor(newMaxLoadFactor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void
StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::rehash(bsl::size_t numBuckets)
{
    d_imp.rehash(numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::setComputedValue(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.setComputedValueFirst(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::setValue(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return d_imp.setValueFirst(key, value);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::setValue(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    return d_imp.setValueFirst(key, bslmf::MovableRefUtil::move(value));
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::update(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.update(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::visit(
                                                const VisitorFunction& visitor)
{
    return d_imp.visit(visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::visit(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return d_imp.visit(key, visitor);
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::bucketCount() const
{
    return d_imp.bucketCount();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::bucketIndex(
                                                          const KEY& key) const
{
    return d_imp.bucketIndex(key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::bucketSize(
                                                       bsl::size_t index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(bucketCount() > index);

    return d_imp.bucketSize(index);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::empty() const
{
    return d_imp.empty();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_imp.equalFunction();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::getValue(
                                                        VALUE       *value,
                                                        const  KEY&  key) const
{
    return d_imp.getValue(value, key);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_imp.hashFunction();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::isRehashEnabled() const
{
    return d_imp.isRehashEnabled();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::loadFactor() const
{
    return d_imp.loadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::maxLoadFactor() const
{
    return d_imp.maxLoadFactor();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::numStripes() const
{
    return d_imp.numStripes();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::visitReadOnly(
                                  const ReadOnlyVisitorFunction& visitor) const
{
    return d_imp.visitReadOnly(visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::visitReadOnly(
                                  const KEY&                     key,
                                  const ReadOnlyVisitorFunction& visitor) const
{
    return d_imp.visitReadOnly(key, visitor);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::size() const
{
    return d_imp.size();
}

                               // Aspects

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bslma::Allocator *StripedUnorderedMap<KEY, VALUE, HASH, EQUAL>::allocator()
                                                                          const
{
    return d_imp.allocator();
}

}  // close package namespace

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL>
struct UsesBslmaAllocator<bdlcc::StripedUnorderedMap<KEY, VALUE, HASH, EQUAL> >
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
