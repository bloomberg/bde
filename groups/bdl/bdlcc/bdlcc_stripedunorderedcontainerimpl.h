// bdlcc_stripedunorderedcontainerimpl.h                              -*-C++-*-
#ifndef INCLUDED_BDLCC_STRIPEDUNORDEREDCONTAINERIMPL
#define INCLUDED_BDLCC_STRIPEDUNORDEREDCONTAINERIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common implementation of *striped* un-ordered map/multimap.
//
//@CLASSES:
//  bdlcc::StripedUnorderedContainerImpl: striped container for key-value types
//
//@SEE_ALSO: bdlcc:stripedunorderedmap, bdlcc:stripedunorderedmultimap
//
//@DESCRIPTON: This component provides 'bdlcc::StripedUnorderedContainerImpl',
// a common implementation for 'bdlcc:StripedUnorderedMap' and
// 'bdlcc::StripedUnorderedMultiMap', that are concurrent (fully thread-safe)
// associative containers that partition their underlying hash tables into a
// (user-defined) number of "bucket groups" and control access to each part of
// their hash tables by separate read-write locks.  For most methods, the "map"
// and "multimap" classes forward to the analogous method in this "impl" class
// with an additional argument that specifies if the calling class has unique
// keys or not.
//
///Thread Safety
///-------------
// The 'bdlcc::StripedUnorderedContrainerImpl' class template is fully
// thread-safe (see {'bsldoc_glossary'|Fully Thread-Safe}), assuming that the
// allocator is fully thread-safe.  Each method is executed by the calling
// thread.
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
//  | insertBulk, k elements                             | Average: O[k]      |
//  |                                                    | Worst:   O[n*k]    |
//  +----------------------------------------------------+--------------------+
//  | eraseBulk, k elements                              | Average: O[k]      |
//  |                                                    | Worst:   O[n*k]    |
//  +----------------------------------------------------+--------------------+
//  | rehash                                             | O[n]               |
//  +----------------------------------------------------+--------------------+
//  | visit                                              | O[n]               |
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
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bdlscm_version.h>

#include <bslalg_hashtableimputil.h>

#include <bslim_printer.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_destructorproctor.h>
#include <bslma_rawdeleterproctor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_readerwritermutex.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>   // BSLS_PLATFORM_CPU_X86_64

#include <bslstl_hash.h>
#include <bslstl_pair.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>     // 'NULL'
#include <bsl_functional.h>
#include <bsl_list.h>
#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace bdlcc {

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class StripedUnorderedContainerImpl;

                 // ========================================
                 // class StripedUnorderedContainerImpl_Node
                 // ========================================

template <class KEY, class VALUE>
class StripedUnorderedContainerImpl_Node {
    // This class template represents a node in the singly-linked list of
    // '(KEY, VALUE)' elements for each bucket of a hash map.

  private:
    // DATA
    mutable StripedUnorderedContainerImpl_Node *d_next_p;
        // Pointer to next element of the bucket

    bsls::ObjectBuffer<KEY>            d_key;
        // footprint of key

    bsls::ObjectBuffer<VALUE>          d_value;
        // Footprint of value

    bslma::Allocator                  *d_allocator_p;
        // memory allocator (held, not owned).

    // NOT IMPLEMENTED
    StripedUnorderedContainerImpl_Node(
                        const StripedUnorderedContainerImpl_Node<KEY, VALUE>&);
                                                                    // = delete
    StripedUnorderedContainerImpl_Node<KEY, VALUE>&
              operator=(const StripedUnorderedContainerImpl_Node<KEY, VALUE>&);
                                                                    // = delete

  public:
    // CREATORS
    StripedUnorderedContainerImpl_Node(
                       const KEY&                          key,
                       const VALUE&                        value,
                       StripedUnorderedContainerImpl_Node *nextPtr,
                       bslma::Allocator                   *basicAllocator = 0);
    StripedUnorderedContainerImpl_Node(
                       const KEY&                          key,
                       bslmf::MovableRef<VALUE>            value,
                       StripedUnorderedContainerImpl_Node *nextPtr,
                       bslma::Allocator                   *basicAllocator = 0);
        // Create a 'bdlcc::StripedUnorderedContainerImpl_Node' object having
        // the specified 'key' and 'value', and with the specified 'nextPtr'
        // pointer to the next node.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    StripedUnorderedContainerImpl_Node(
                       const KEY&                          key,
                       StripedUnorderedContainerImpl_Node *nextPtr,
                       bslma::Allocator                   *basicAllocator = 0);
        // Create a 'bdlcc::StripedUnorderedContainerImpl_Node' object having
        // the specified 'key' and a value initialized to 'VALUE()', and with
        // the specified 'nextPtr' pointer to the next node.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~StripedUnorderedContainerImpl_Node();
        // Destroy this object.

    // MANIPULATORS
    StripedUnorderedContainerImpl_Node **nextAddress();
        // Return the address of the pointer to the next node.

    void setNext(StripedUnorderedContainerImpl_Node *nextPtr);
        // Set this node's pointer-to-next-node to the specified 'nextPtr'.

    VALUE& value();
        // Return a reference providing modifiable access to the 'value'
        // attribute of this object.

    // ACCESSORS
    const KEY& key() const;
        // Return a 'const' reference to the 'key' attribute of this object.

    StripedUnorderedContainerImpl_Node *next() const;
        // Return the pointer to the next node.

    const VALUE& value() const;
        // Return a 'const' reference to the 'value' attribute of this object.

                               // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by 'StripedUnorderedContainerImpl_Node' to
        // allocate memory.
};

                // ==========================================
                // class StripedUnorderedContainerImpl_Bucket
                // ==========================================

template <class KEY, class VALUE>
class StripedUnorderedContainerImpl_Bucket {
    // This class represents a bucket of the hash map.  This class template
    // represents the head of in the singly-linked list of '(KEY, VALUE)'
    // elements in a hash map.

  private:
    // PRIVATE TYPES
    typedef BloombergLP::bslmf::MovableRefUtil MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    // DATA
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *d_head_p;
        // Pointer to the first element in the bucket

    StripedUnorderedContainerImpl_Node<KEY, VALUE> *d_tail_p;
        // Pointer to the last element in the bucket

    bsl::size_t                            d_size;
        // Number of nodes in this bucket

    bslma::Allocator                      *d_allocator_p;
        // memory allocator (held, not owned).

    // NOT IMPLEMENTED
    StripedUnorderedContainerImpl_Bucket(
          const StripedUnorderedContainerImpl_Bucket<KEY, VALUE>&); // = delete
    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& operator=(
          const StripedUnorderedContainerImpl_Bucket<KEY, VALUE>&); // = delete

  public:
    // TYPES
    enum BucketScope {
        // Enumeration to differentiate between processing all elements with
        // the same key, or just the first one (and typically the only one).

        e_BUCKETSCOPE_FIRST = 0,  // Act on first matching element found.
        e_BUCKETSCOPE_ALL         // Act on all matching elements.
    };

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(StripedUnorderedContainerImpl_Bucket,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit StripedUnorderedContainerImpl_Bucket(
                                         bslma::Allocator *basicAllocator = 0);
        // Create an empty 'bdlcc::StripedUnorderedContainerImpl_Bucket'
        // object.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    StripedUnorderedContainerImpl_Bucket(
           bslmf::MovableRef<StripedUnorderedContainerImpl_Bucket<KEY, VALUE> >
                                                                      original,
           bslma::Allocator                                          *);
        // Create a 'bdlcc::StripedUnorderedContainerImpl_Bucket' object having
        // the same value as the specified 'original' object.  The 'original'
        // object is left in a valid but unspecified state.

    ~StripedUnorderedContainerImpl_Bucket();
        // Destroy this object.

    // MANIPULATORS
    void addNode(StripedUnorderedContainerImpl_Node<KEY, VALUE> *nodePtr);
        // Add the specified 'nodePtr' node at the end of this bucket.

    void clear();
        // Empty 'StripedUnorderedContainerImpl_Bucket' and delete all nodes.

    StripedUnorderedContainerImpl_Node<KEY, VALUE> **headAddress();
        // Return the address of the head (node) of this bucket list.

    void incrementSize(int amount);
        // Increment the 'size' attribute of this bucket by the specified
        // 'amount'.

    void setHead(StripedUnorderedContainerImpl_Node<KEY, VALUE> *value);
        // Set the address of the head of this bucket list to the specified
        // 'value'.

    void setSize(bsl::size_t value);
        // Set the 'size' attribute of this bucket to the specified 'value'.

    void setTail(StripedUnorderedContainerImpl_Node<KEY, VALUE> *value);
        // Set the pointer of the tail of this bucket list to the specified
        // 'value'.

    template <class EQUAL>
    bsl::size_t setValue(const KEY&   key,
                         const EQUAL& equal,
                         const VALUE& value,
                         BucketScope  scope);
        // Set the value attribute of the element in this bucket having the
        // specified 'key' to the specified 'value', using the specified
        // 'equal' to compare keys.  If no such element exists, insert
        // '(key, value)'.  The behavior with respect to duplicate key values
        // in the bucket depends on the specified 'scope':
        //
        //: 'e_BUCKETSCOPE_ALL':
        //:   Set 'value' to every element in the bucket having 'key'.
        //:
        //: 'e_BUCKETSCOPE_FIRST':
        //:   Set 'value' to the first element found having 'key'.
        //
        // Return the number of elements found having 'key' that had their
        // value set.  Note that, when there are multiple elements having
        // 'key', the selection of "first" is unspecified and subject to
        // change.  Also note that specifying 'e_BUCKETSCOPE_FIRST' is more
        // performant when there is a single element in the bucket having
        // 'key'.

    template <class EQUAL>
    bsl::size_t setValue(const KEY&               key,
                         const EQUAL&             equal,
                         bslmf::MovableRef<VALUE> value);
        // Set the value attribute of the element in this bucket having the
        // specified 'key' to the specified 'value', using the specified
        // 'equal' to compare keys.  If no such element exists, insert
        // '(key, value)'.  If there are multiple elements in this hash map
        // having 'key' then set the value of the first such element found.
        // Return the number of elements found having 'key' that had their
        // value set.  Note that, when there are multiple elements having
        // 'key', the selection of "first" is unspecified and subject to
        // change.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this bucket contains no elements, and 'false'
        // otherwise.

    StripedUnorderedContainerImpl_Node<KEY, VALUE> *head() const;
        // Return the head (node) of this bucket list.

    bsl::size_t size() const;
        // Return the current number of elements in this bucket.

    StripedUnorderedContainerImpl_Node<KEY, VALUE> *tail() const;
        // Return address of the tail (node) of this bucket list.

                               // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by 'StripedUnorderedContainerImpl_Bucket'
        // to allocate memory.
};

template <class KEY,
          class VALUE,
          class HASH  = bsl::hash<KEY>,
          class EQUAL = bsl::equal_to<KEY> >
class StripedUnorderedContainerImpl_TestUtil;

class StripedUnorderedContainerImpl_LockElement;
class StripedUnorderedContainerImpl_LockElementReadGuard;
class StripedUnorderedContainerImpl_LockElementWriteGuard;

                     // ===================================
                     // class StripedUnorderedContainerImpl
                     // ===================================

template <class KEY, class VALUE, class HASH, class EQUAL>
class StripedUnorderedContainerImpl {
    // This class implements the logic for a striped hash multimap with logic
    // that supports a (unique) map as a special case.

  public:
    // TYPES
    enum {
        k_DEFAULT_NUM_BUCKETS  = 16, // Default # of buckets
        k_DEFAULT_NUM_STRIPES  =  4  // Default # of stripes
    };

    typedef StripedUnorderedContainerImpl_Node<KEY, VALUE> Node;
        // Node in a bucket.

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

  private:
    // PRIVATE CONSTANTS
    static const int k_REHASH_IN_PROGRESS = 1; // d_state bit 0
    static const int k_REHASH_ENABLED     = 2; // d_state bit 1

    // PRIVATE TYPES
    enum {
    #if BSLS_PLATFORM_CPU_X86 || BSLS_PLATFORM_CPU_X86_64
        k_PREFETCH_ENABLED = 1,
    #else
        k_PREFETCH_ENABLED = 0,
    #endif
        // Can be 0 or 1; if prefetch, we use 2 cachelines at a time
        k_EFFECTIVE_CACHELINE_SIZE = (1 + k_PREFETCH_ENABLED) *
                                            bslmt::Platform::e_CACHE_LINE_SIZE,
        // Cacheline size to use; may be 1 or 2 cachelines
        k_INT_PADDING = k_EFFECTIVE_CACHELINE_SIZE - sizeof(bsls::AtomicInt)
    };

    enum Multiplicity {
        // Enumeration to differentiate between inserting only unique keys and
        // inserting multiple values for the same key.

        e_INSERT_UNIQUE = 0, // Insert a new element having 'key' if no element
                             // in hash map has 'key'; otherwise, update the
                             // value attribute of the existing element.

        e_INSERT_ALWAYS      // Insert a new element having 'key' even if the
                             // map already has an element(s) having the 'key'
                             // attribute.
    };

    enum Scope {
        // Enumeration to differentiate between processing all elements with
        // the same key, or just the first one (and typically the only one).

        e_SCOPE_FIRST = 0,  // Act on first matching element found.
        e_SCOPE_ALL         // Act on all matching elements.
    };

    typedef StripedUnorderedContainerImpl_LockElement           LockElement;
    typedef StripedUnorderedContainerImpl_LockElementReadGuard  LERGuard;
    typedef StripedUnorderedContainerImpl_LockElementWriteGuard LEWGuard;

    // DATA
    bsl::size_t                       d_numStripes;
        // number of stripes

    bsl::size_t                       d_numBuckets;
        // number of buckets

    bsl::size_t                       d_hashMask;
        // d_numStripes - 1; this value is used to provide an efficient modulo
        // (using bit-wise '&') of d_numStripes (where d_numStripes must be a
        // power of 2).

    float                             d_maxLoadFactor;
        // maxLoadFactor (defaults to 1.0)

    HASH                              d_hasher;
        // hashing function for keys

    EQUAL                             d_comparator;
        // comparison function for keys

    mutable bsls::AtomicInt           d_state;
        //: o bit 0: 0-rehash not in progress; 1-rehash in progress.
        //: o bit 1: 0-rehash disabled;        1-rehash enabled.

    const char                        d_statePad[k_INT_PADDING];
        // padding, so that 'd_state' will have its own cache line

    bsls::AtomicInt                   d_numElements;
        // # of elements in the hash map

    const char                        d_numElementsPad[k_INT_PADDING];
        // padding, so that 'd_numElements' will have its own cache line

    bsl::vector<StripedUnorderedContainerImpl_Bucket<KEY,VALUE> >
                                      d_buckets;
        // hash table data, storing key-value pairs

    LockElement                      *d_locks_p;
        // Pointer to an array of locks for the stripes.  Note that mutex can't
        // be moved or copied, hence can't be in a vector.

    bslma::Allocator                 *d_allocator_p;
        // memory allocator (held, not owned)

    // FRIENDS
    friend class
               StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>;
    friend class StripedUnorderedContainerImpl_LockElement;

    // NOT IMPLEMENTED
    StripedUnorderedContainerImpl(
                const StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>&);
                                                                    // = delete
    StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>&
      operator=(const StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>&);
                                                                    // = delete

    // PRIVATE CLASS METHODS
    static bsl::size_t adjustBuckets(bsl::size_t numBuckets,
                                     bsl::size_t numStripes);
        // Return the number of buckets needed by this implementation for the
        // specified 'numBuckets' and 'numStripes'.  That value is the lowest
        // integer that is a power of 2 that is greater than or equal
        // 'numBuckets', 'numStripes', and 2.

    static bsl::size_t powerCeil(bsl::size_t num);
        // Return the nearest higher power of 2 for the specified 'num'.

    // PRIVATE MANIPULATORS
    void checkRehash();
        // Perform a rehash if the 'loadFactor() > maxLoadFactor()', and
        // 'true == canRehash()'.

    bsl::size_t erase(const KEY& key, Scope scope);
        // Remove from this hash map the element, if any, having the specified
        // 'key'.  If there a multiple elements having 'key' and the specified
        // 'scope' is 'e_SCOPE_ALL', erase them all; otherwise; erase just the
        // first element found.  Return the number of elements erased.  Note
        // that, when there are multiple elements having 'key', the selection
        // of "first" is unspecified and subject to change.

    template <class RANDOM_ITER>
    bsl::size_t eraseBulk(RANDOM_ITER first,
                          RANDOM_ITER last,
                          Scope       scope);
        // Erase from this hash map elements in this hash map having any of the
        // values in the keys contained between the specified 'first'
        // (inclusive) and 'last' (exclusive) random-access iterators.  The
        // iterators provide read access to a sequence of 'KEY' objects.  If
        // there are multiple elements for any key value and the specified
        // 'scope' is 'e_SCOPE_ALL' then erase them all; otherwise, erase just
        // the first such element found.  Return the number of elements erased.
        // The behavior is undefined unless 'first <= last'.  Note that, when
        // there are multiple elements having 'key', the selection of "first"
        // is unspecified and subject to change.

    bsl::size_t insert(const KEY&    key,
                       const VALUE&  value,
                       Multiplicity  multiplicity);
    bsl::size_t insert(const KEY&               key,
                       bslmf::MovableRef<VALUE> value,
                       Multiplicity             multiplicity);
        // Insert into this hash map an element having the specified 'key' and
        // 'value'.  The behavior with respect to duplicate key values in the
        // hash map depends on the specified 'multiplicity':
        //
        //: 'e_INSERT_ALWAYS':
        //:   The insertion occurs irrespective of other elements in the hash
        //:   map having the same 'key' value.
        //:
        //: 'e_INSERT_UNIQUE':
        //:   Insert a new element if no element in the hash map has the 'key'
        //:   value; otherwise, update the value attribute of the first element
        //:   found having 'key' to 'value'.
        //
        // Return the number of elements inserted.  Note that, when there are
        // multiple elements having 'key', the selection of "first" is
        // unspecified and subject to change.

    template <class RANDOM_ITER>
    bsl::size_t insertBulk(RANDOM_ITER  first,
                           RANDOM_ITER  last,
                           Multiplicity multiplicity);
        // Insert into this hash map elements having the key-value pairs
        // obtained between the specified 'first' (inclusive) and 'last'
        // (exclusive) random-access iterators.  The iterators provide read
        // access to a sequence of 'bsl::pair<KEY, VALUE>' objects.  The
        // behavior with respect to duplicate key values in the hash map
        // depends on the specified 'multiplicity':
        //
        //: 'e_INSERT_ALWAYS':
        //:   The insertion occurs irrespective of other elements in the hash
        //:   map having the same 'key' value.  Note that this is the only
        //:   way to adding elements with non-unique keys to the hash map.
        //:
        //: 'e_INSERT_UNIQUE':
        //:   Insert a new element if no element in the hash map has the 'key'
        //:   value; otherwise, update the value attribute of the first element
        //:   found having 'key' to 'value'.
        //
        // Return the number of elements inserted.  The behavior is undefined
        // unless 'first <= last'.  Note that, when there are multiple elements
        // having 'key', the selection of "first" is unspecified and subject to
        // change.

    int setComputedValue(const KEY&             key,
                         const VisitorFunction& visitor,
                         Scope                  scope);
        // Invoke the specified 'visitor' passing the specified 'key', and the
        // address of the attribute part of an element (of possibly many
        // elements) found in this hash map having 'key'.  That is, for
        // '(key, value)', invoke:
        //..
        //  bool visitor(&value, key);
        //..
        // If no element in the hash map has 'key', insert '(key, VALUE())' and
        // invoke to 'visitor' with 'value' pointing to the defsult constructed
        // value.  If there are multiple elements having 'key' and the
        // specified 'scope' is 'e_SCOPE_ALL' then apply 'visitor' to each of
        // them; otherwise, 'visitor' is applied to just the first such element
        // found.  Return the number of elements visited or the negation of
        // that value if visitations stopped because 'visitor' returned
        // 'false'.  'visitor' has exclusive access (i.e., write access) to the
        // element.  The behavior is undefined if hash map manipulators and
        // 'getValue*' methods are invoked from within 'visitor', as it may
        // lead to a deadlock.  Note that, when there are multiple elements
        // having 'key', the selection of "first" and the order applying
        // 'visitor' are unspecified and subject to change.  Also note that a
        // return value of '0' implies that an element was inserted.

    bsl::size_t setValue(const KEY&   key,
                         const VALUE& value,
                         Scope        scope);
        // Set the value attribute of the element in this hash map having the
        // specified 'key' to the specified 'value'.  If no such element
        // exists, insert '(key, value)'.  The behavior with respect to
        // duplicate key values in the bucket depends on the specified
        // 'scope':
        //
        //: 'e_SCOPE_ALL':
        //:   Set 'value' to every element in the bucket having 'key'.
        //:
        //: 'e_SCOPE_FIRST':
        //:   Set 'value' to the first element found having 'key'.
        //
        // Return the number of elements found having 'key'.  Note that if no
        // elements were found, and a new value was inserted, '0' is returned.
        // Also note that, when there are multiple elements having 'key', the
        // selection of "first" is unspecified and subject to change.  Also
        // note that specifying 'e_SCOPE_FIRST' is more performant when there
        // is a single element in the bucket having 'key'.

    // PRIVATE ACCESSORS
    bsl::size_t bucketIndex(const KEY& key, bsl::size_t numBuckets) const;
        // Return the index of the bucket, in the array of buckets maintained
        // by this hash map, where values having a key equivalent to the
        // specified 'key' would be inserted using the specified 'numBuckets'.
        // This operation does not lock the related stripe or check the rehash
        // state.  Note that 'numBuckets' does not have to be the current
        // number of buckets in this hash map.

    bsl::size_t bucketToStripe(bsl::size_t bucketIndex) const;
        // Return the stripe index associated with the specified 'bucketIndex'.

    LockElement *lockRead(bsl::size_t *bucketIdx, const KEY& key) const;
        // Lock for read the stripe related to the specified 'key', setting the
        // specified 'bucketIdx' to the bucket index associated with 'key'.
        // Return the address to the lock-element associated with the returned
        // 'bucketIdx'.

    LockElement *lockWrite(bsl::size_t *bucketIdx, const KEY& key) const;
        // Lock for write the stripe related to the specified 'key', setting
        // the specified 'bucketIdx' to the bucket index associated with 'key'.
        // Return the address to the lock-element associated with the returned
        // 'bucketIdx'.

  public:
    // CREATORS
    explicit StripedUnorderedContainerImpl(
                   bsl::size_t       numInitialBuckets = k_DEFAULT_NUM_BUCKETS,
                   bsl::size_t       numStripes        = k_DEFAULT_NUM_STRIPES,
                   bslma::Allocator *basicAllocator = 0);
        // Create an empty 'StripedUnorderedContainerImpl' object, a fully
        // thread-safe hash map where access is divided into "stripes" (a group
        // of buckets protected by a reader-write mutex).  Optionally specify
        // 'numInitialBuckets' and 'numStripes' which define the minimum number
        // of buckets and the (fixed) number of stripes in this map.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The hash map has rehash enabled.

    ~StripedUnorderedContainerImpl();
        // Destroy this hash map.  This method is *not* thread-safe.

    // MANIPULATORS
    void clear();
        // Remove all elements from this striped hash map.  If rehash is in
        // progress, block until it completes.

    void disableRehash();
        // Prevent rehash until the 'enableRehash' method is called.

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

    template <class RANDOM_ITER>
    bsl::size_t eraseBulkFirst(RANDOM_ITER first, RANDOM_ITER last);
        // Erase from this hash map elements in this hash map having any of the
        // values in the keys contained between the specified 'first'
        // (inclusive) and 'last' (exclusive) random-access iterators.  The
        // iterators provide read access to a sequence of 'KEY' objects.  If
        // there are multiple elements for any key value, erase just the first
        // such element found.  All erasures are done by the calling thread and
        // the order of erasure is not specified.  Return the number of
        // elements removed.  The behavior is undefined unless 'first <= last'.
        // Note that the map may not have an element for every value in 'keys'.

    bsl::size_t eraseFirst(const KEY& key);
        // Erase from this hash map the *first* element (of possibly many)
        // found to the specified 'key'.  Return the number of elements erased.
        // Note that method is more performant than 'eraseAll' when there is
        // one element having 'key'.

    void insertAlways(const KEY& key, const VALUE& value);
        // Insert into this hash map an element having the specified 'key' and
        // 'value'.  Note that other elements having the same 'key' may exist
        // in this hash map.

    void insertAlways(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Insert into this hash map an element having the specified 'key' and
        // the specified move-insertable 'value'.  The 'value' object is left
        // in a valid but unspecified state.  If 'value' is allocator-enabled
        // and 'allocator() != value.allocator()' this operation may cost as
        // much as a copy.  Note that other elements having the same 'key' may
        // exist in this hash map.

    template <class RANDOM_ITER>
    void insertBulkAlways(RANDOM_ITER first, RANDOM_ITER last);
        // Insert into this hash map elements having the key-value pairs
        // obtained between the specified 'first' (inclusive) and 'last'
        // (exclusive) random-access iterators.  The iterators provide read
        // access to a sequence of 'bsl::pair<KEY, VALUE>' objects.  All
        // insertions are done by the calling thread and the order of insertion
        // is not specified.  The behavior is undefined unless 'first <= last'.

    template <class RANDOM_ITER>
    bsl::size_t insertBulkUnique(RANDOM_ITER first, RANDOM_ITER last);
        // Insert into this hash map elements having the key-value pairs
        // obtained between the specified 'first' (inclusive) and 'last'
        // (exclusive) random-access iterators.  The iterators provide read
        // access to a sequence of 'bsl::pair<KEY, VALUE>' objects.  If an
        // element having one of the keys already exists in this hash map, set
        // the value attribute to the corresponding value from 'data'.  All
        // insertions are done by the calling thread and the order of insertion
        // is not specified.  Return the number of elements inserted.  The
        // behavior is undefined unless 'first <= last'.

    bsl::size_t insertUnique(const KEY& key, const VALUE& value);
        // Insert into this hash map an element having the specified 'key' and
        // 'value'.  If 'key' already exists in this hash map, the value
        // attribute of that element is set to 'value'.  Return 1 if an element
        // is inserted, and 0 if an existing element is updated.  Note that the
        // return value equals the number of elements inserted.

    bsl::size_t insertUnique(const KEY& key, bslmf::MovableRef<VALUE> value);
        // Insert into this hash map an element having the specified 'key' and
        // the specified move-insertable 'value'.  If 'key' already exists in
        // this hash map, the value attribute of that element is set to
        // 'value'.  Return 1 if an element is inserted, and 0 if an existing
        // element is updated.  The 'value' object is left in a valid but
        // unspecified state.  If 'value' is allocator-enabled and
        // 'allocator() != value.allocator()' this operation may cost as much
        // as a copy.  Note that the return value equals the number of elements
        // inserted.

    void maxLoadFactor(float newMaxLoadFactor);
        // Set the maximum load factor of this hash map to the specified
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
        // invoke 'visitor' with 'value' pointing to the defsult constructed
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
        // invoke 'visitor' with 'value' pointing to the defsult constructed
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
        // Set the value attribute of the element in this hash map having the
        // specified 'key' to the specified 'value'.  If no such element
        // exists, insert '(key, value)'.  If there are multiple elements in
        // this hash map having 'key' then set the value of the first such
        // element found.  Return the number of elements found having 'key'.
        // Note that if no elements were found, and a new value was inserted,
        // '0' is returned.  Also note that, when there are multiple elements
        // having 'key', the selection of "first" is unspecified and subject to
        // change.

    int update(const KEY& key, const VisitorFunction& visitor);
        // Serially call the specified 'visitor' on each element (if one
        // exists) in this hash map having the specified 'key' until every such
        // element has been updated or until 'visitor' returns 'false'.  That
        // is, for '(key, value)', invoke:
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
        // elements in this hash table having the specified 'key' either until
        // each such element has been visited or until 'visitor' returns
        // 'false.  That is, for '(key, value)', invoke:
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

    // ACCESSORS
    bsl::size_t bucketIndex(const KEY& key) const;
        // Return the index of the bucket, in the array of buckets maintained
        // by this hash map, where elements having the specified 'key' are
        // inserted.  Note that unless rehash is disabled, the value returned
        // may be obsolete at the time it is returned.

    bsl::size_t bucketCount() const;
        // Return the number of buckets in the array of buckets maintained by
        // this hash map.  Note that unless rehash is disabled, the value
        // returned may be obsolete by the time it is received.

    bsl::size_t bucketSize(bsl::size_t index) const;
        // Return the number of elements contained in the bucket at the
        // specified 'index' in the array of buckets maintained by this hash
        // map.  The behavior is undefined unless
        // '0 <= index < bucketCount()'.

    bool canRehash() const;
        // Return 'true' if rehash is enabled and rehash is not in progress,
        // and 'false' otherwise.

    bool empty() const;
        // Return 'true' if this hash map contains no elements, and 'false'
        // otherwise.

    EQUAL equalFunction() const;
        // Return (a copy of) the key-equality functor used by this hash map
        // that returns 'true' if two 'KEY' objects have the same value, and
        // 'false' otherwise.

    bsl::size_t getValue(VALUE *value, const KEY& key) const;
        // Load, into the specified '*value', the value attribute of the first
        // element (of possibly many elements) found in this hash map having
        // the specified 'key'.  Return 1 on success, and 0 if 'key' does not
        // exist in this hash.  Note that the return value equals the number of
        // values returned.  Also note that, when there are multiple elements
        // having 'key', the selection of "first" is implementation specific
        // and subject to change.

    bsl::size_t getValue(bsl::vector<VALUE> *valuesPtr, const KEY& key) const;
        // Load, into the specified '*valuesPtr', the value attributes of every
        // element in this hash map having the specified 'key'.  Return the
        // number of elements found with 'key'.  Note that the order of the
        // values returned is not specified.

    HASH hashFunction() const;
        // Return (a copy of) the unary hash functor used by this hash map to
        // generate a hash value (of type 'std::size_t') for a 'KEY' object.

    bool isRehashEnabled() const;
        // Return 'true' if rehash is enabled, or 'false' otherwise.

    float loadFactor() const;
        // Return the current quotient of the size of this hash map and the
        // number of buckets.  Note that the load factor is a measure of
        // container "fullness"; that is, a high load factor typically implies
        // many collisions (many elements landing in the same bucket) and that
        // decreases performance.

    float maxLoadFactor() const;
        // Return the maximum load factor allowed for this hash map.  If an
        // insert operation would cause the load factor to exceed the
        // 'maxLoadFactor()' and rehashing is enabled, then that insert
        // increases the number of buckets and rehashes the elements of the
        // container into that larger set of buckets.

    bsl::size_t numStripes() const;
        // Return the number of stripes in the hash.

    bsl::size_t size() const;
        // Return the current number of elements in this hash.

                               // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this hash map to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator installed at that time is used.
};

                // ============================================
                // class StripedUnorderedContainerImpl_TestUtil
                // ============================================

template <class KEY, class VALUE, class HASH, class EQUAL>
class StripedUnorderedContainerImpl_TestUtil {
    // This class implements a test utility that gives the test driver access
    // to the lock / unlock method of the Read/Write mutex.  Its purpose is to
    // allow testing that the locking actually happens as planned.

    // PRIVATE TYPES
    typedef StripedUnorderedContainerImpl_LockElement LockElement;

    // DATA
    StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>& d_hash;

  public:
    // CREATORS
    explicit StripedUnorderedContainerImpl_TestUtil(
                 StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>& hash);
        // Create a 'StripedUnorderedContainerImpl_TestUtil' object to test
        // locking in the specified 'hash'.

    //! ~StripedUnorderedContainerImpl_TestUtil() = default;
        // Destroy this object.

        // MANIPULATORS
    void lockRead(const KEY& key);
        // Call the 'lockRead' method of 'bdlcc::StripedUnorderedContainerImpl'
        // 'd_locks_p' lock of the specified 'key'.

    void lockWrite(const KEY& key);
        // Call the 'lockWrite' method of
        // 'bdlcc::StripedUnorderedContainerImpl' 'd_locks_p' lock of the
        // specified 'key'.

    void unlockWrite(const KEY& key);
        // Call the 'unlockWrite' method of
        // 'bdlcc::StripedUnorderedContainerImpl' 'd_locks_p' lock of the
        // specified 'key'.

    void unlockRead(const KEY& key);
        // Call the 'unlockRead' method of
        // 'bdlcc::StripedUnorderedContainerImpl' 'd_locks_p' lock of the
        // specified 'key'.
};

                // ===============================================
                // class StripedUnorderedContainerImpl_LockElement
                // ===============================================

class StripedUnorderedContainerImpl_LockElement {
    // A mutex + support info; padded to cacheline size, one per stripe

  private:
    // PRIVATE TYPES
    typedef bslmt::ReaderWriterMutex   LockType;

    // PRIVATE CONSTANTS
    enum {
    #if BSLS_PLATFORM_CPU_X86 || BSLS_PLATFORM_CPU_X86_64
        k_PREFETCH_ENABLED = 1,
    #else
        k_PREFETCH_ENABLED = 0,
    #endif
        // Can be 0 or 1; if prefetch, we use 2 cachelines at a time
        k_EFFECTIVE_CACHELINE_SIZE = (1 + k_PREFETCH_ENABLED) *
                                            bslmt::Platform::e_CACHE_LINE_SIZE,
        // Cacheline size to use; may be 1 or 2 cachelines
        k_LOCK_PADDING = k_EFFECTIVE_CACHELINE_SIZE >= sizeof(LockType) ?
                         k_EFFECTIVE_CACHELINE_SIZE -  sizeof(LockType) :
                     2 * k_EFFECTIVE_CACHELINE_SIZE -  sizeof(LockType)
    };

    // DATA
    LockType        d_lock;
    const char      d_pad[k_LOCK_PADDING];

  public:
    // CREATORS
    StripedUnorderedContainerImpl_LockElement();
        // Create an empty 'StripedUnorderedContainerImpl_LockElement' object.

    // MANIPULATORS
    void lockR();
        // Read lock the lock element.

    void lockW();
        // Write lock the lock element.

    void unlockR();
        // Read unlock the lock element.

    void unlockW();
        // Write unlock the lock element.
};


         // ========================================================
         // class StripedUnorderedContainerImpl_LockElementReadGuard
         // ========================================================

class StripedUnorderedContainerImpl_LockElementReadGuard {
    // A guard pattern on StripedUnorderedContainerImpl_LockElement, to release
    // on exception, for a lock element locked as read.

  private:
    // DATA
    StripedUnorderedContainerImpl_LockElement *d_lockElement_p;
        // Guarded LockElement pointer

  public:
    // CREATORS
    explicit StripedUnorderedContainerImpl_LockElementReadGuard(
                    StripedUnorderedContainerImpl_LockElement *lockElementPtr);
        // Create a guard object
        // 'StripedUnorderedContainerImpl_LockElementReadGuard' for the
        // specified 'lockElementPtr'
        // 'bdlcc::StripedUnorderedContainerImpl_LockElement' object.

    ~StripedUnorderedContainerImpl_LockElementReadGuard();
        // Release the guarded object

    // MANIPULATORS
    void release();
        // Release the guarded object
};

          // =========================================================
          // class StripedUnorderedContainerImpl_LockElementWriteGuard
          // =========================================================

class StripedUnorderedContainerImpl_LockElementWriteGuard {
    // A guard pattern on StripedUnorderedContainerImpl_LockElement, to release
    // on exception, for a lock element locked as write.

  private:
    // DATA
    StripedUnorderedContainerImpl_LockElement *d_lockElement_p;
        // Guarded LockElement pointer

  public:
    // CREATORS
    explicit StripedUnorderedContainerImpl_LockElementWriteGuard(
                    StripedUnorderedContainerImpl_LockElement *lockElementPtr);
        // Create a guard object
        // 'StripedUnorderedContainerImpl_LockElementWriteGuard' for the
        // specified 'lockElementPtr'
        // 'bdlcc::StripedUnorderedContainerImpl_LockElement' object.

    ~StripedUnorderedContainerImpl_LockElementWriteGuard();
        // Release the guarded object

    // MANIPULATORS
    void release();
        // Release the guarded object
};

struct StripedUnorderedContainerImpl_SortItem {
    // A vector element needed for efficient sorting for the 'insertBulk' and
    // 'eraseBulk' methods.
  public:
    // PUBLIC DATA
    int         d_stripeIdx;
    int         d_dataIdx;
    bsl::size_t d_hashVal;
};

// FREE OPERATOR
bool operator<(const StripedUnorderedContainerImpl_SortItem& lhs,
               const StripedUnorderedContainerImpl_SortItem& rhs);
    // Return 'true' if the specified 'lhs' is smaller than the specified 'rhs'
    // in the order of stripe, and data.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                // ----------------------------------------
                // class StripedUnorderedContainerImpl_Node
                // ----------------------------------------

// CREATORS
template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE>::
                                            StripedUnorderedContainerImpl_Node(
                            const KEY&                          key,
                            const VALUE&                        value,
                            StripedUnorderedContainerImpl_Node *nextPtr,
                            bslma::Allocator                   *basicAllocator)
: d_next_p(nextPtr)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bslma::ConstructionUtil::construct(d_key.address(),
                                       d_allocator_p,
                                       key);
    bslma::DestructorProctor<KEY> proctor(&d_key.object());

    bslma::ConstructionUtil::construct(d_value.address(),
                                       d_allocator_p,
                                       value);
    proctor.release();
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE>::
                                            StripedUnorderedContainerImpl_Node(
                            const KEY&                          key,
                            bslmf::MovableRef<VALUE>            value,
                            StripedUnorderedContainerImpl_Node *nextPtr,
                            bslma::Allocator                   *basicAllocator)
: d_next_p(nextPtr)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bslma::ConstructionUtil::construct(d_key.address(),
                                       d_allocator_p,
                                       key);
    bslma::DestructorProctor<KEY> proctor(&d_key.object());

    VALUE& dummy = value;
    bslalg::ScalarPrimitives::moveConstruct(d_value.address(),
                                            dummy,
                                            d_allocator_p);
    proctor.release();
}

template <class KEY, class VALUE>
StripedUnorderedContainerImpl_Node<KEY, VALUE>::
    StripedUnorderedContainerImpl_Node(
        const KEY&                          key,
        StripedUnorderedContainerImpl_Node *nextPtr,
        bslma::Allocator                   *basicAllocator)
: d_next_p(nextPtr)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    bslma::ConstructionUtil::construct(d_key.address(),
                                       d_allocator_p,
                                       key);
    bslma::DestructorProctor<KEY> proctor(&d_key.object());

    bslma::ConstructionUtil::construct(d_value.address(), d_allocator_p);
    proctor.release();
}


template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE>::
                                          ~StripedUnorderedContainerImpl_Node()
{
    // Destroy the object buffers content
    bslma::DestructionUtil::destroy(d_key.address());
    bslma::DestructionUtil::destroy(d_value.address());
}


// MANIPULATORS
template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE> **
                  StripedUnorderedContainerImpl_Node<KEY, VALUE>::nextAddress()
{
    return &d_next_p;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Node<KEY, VALUE>::setNext(
                       StripedUnorderedContainerImpl_Node<KEY, VALUE> *nextPtr)
{
    d_next_p = nextPtr;
}

template <class KEY, class VALUE>
inline
VALUE& StripedUnorderedContainerImpl_Node<KEY, VALUE>::value()
{
    return d_value.object();
}

// ACCESSORS
template <class KEY, class VALUE>
inline
const KEY& StripedUnorderedContainerImpl_Node<KEY, VALUE>::key() const
{
    return d_key.object();
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE> *
                   StripedUnorderedContainerImpl_Node<KEY, VALUE>::next() const
{
    return d_next_p;
}

template <class KEY, class VALUE>
inline
const VALUE& StripedUnorderedContainerImpl_Node<KEY, VALUE>::value() const
{
    return d_value.object();
}

                               // Aspects

template <class KEY, class VALUE>
inline
bslma::Allocator *
              StripedUnorderedContainerImpl_Node<KEY, VALUE>::allocator() const
{
    return d_allocator_p;
}

               // ------------------------------------------
               // class StripedUnorderedContainerImpl_Bucket
               // ------------------------------------------

// CREATORS
template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::
                                          StripedUnorderedContainerImpl_Bucket(
                                              bslma::Allocator *basicAllocator)
: d_head_p(NULL)
, d_tail_p(NULL)
, d_size(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::
                                          StripedUnorderedContainerImpl_Bucket(
           bslmf::MovableRef<StripedUnorderedContainerImpl_Bucket<KEY, VALUE> >
                                                                      original,
           bslma::Allocator                                          *)
: d_head_p(MoveUtil::move(MoveUtil::access(original).d_head_p))
, d_tail_p(MoveUtil::move(MoveUtil::access(original).d_tail_p))
, d_size(  MoveUtil::access(original).d_size)
, d_allocator_p(MoveUtil::access(original).d_allocator_p)
{
    MoveUtil::access(original).d_head_p = NULL;
    MoveUtil::access(original).d_tail_p = NULL;
    MoveUtil::access(original).d_size   = 0;
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::
                                        ~StripedUnorderedContainerImpl_Bucket()
{
    clear();
}

// MANIPULATORS
template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::addNode(
                       StripedUnorderedContainerImpl_Node<KEY, VALUE> *nodePtr)
{
    BSLS_ASSERT(nodePtr->next() == NULL);

    if (d_head_p == NULL) {
        d_head_p = nodePtr;
    }
    else {
        d_tail_p->setNext(nodePtr);
    }
    d_tail_p = nodePtr;
    ++d_size;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::clear()
{
    // Delete all content in a loop
    for (StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode = d_head_p;
                                                            curNode != NULL;) {
        StripedUnorderedContainerImpl_Node<KEY, VALUE> *nextPtr =
                                                               curNode->next();
        d_allocator_p->deleteObject(curNode);
        curNode = nextPtr;
    }
    d_head_p = NULL;
    d_tail_p = NULL;
    d_size = 0;
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE>
              **StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::headAddress()
{
    return &d_head_p;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::incrementSize(
                                                                    int amount)
{
    d_size += amount;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::setHead(
                         StripedUnorderedContainerImpl_Node<KEY, VALUE> *value)
{
    d_head_p = value;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::setSize(
                                                             bsl::size_t value)
{
    d_size = value;
}

template <class KEY, class VALUE>
inline
void StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::setTail(
                         StripedUnorderedContainerImpl_Node<KEY, VALUE> *value)
{
    d_tail_p = value;
}

template <class KEY, class VALUE>
template <class EQUAL>
bsl::size_t StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::setValue(
                                                            const KEY&   key,
                                                            const EQUAL& equal,
                                                            const VALUE& value,
                                                            BucketScope  scope)
{
    if (d_head_p == NULL) {
        d_head_p = new (*d_allocator_p)
                                StripedUnorderedContainerImpl_Node<KEY, VALUE>(
                                                                key,
                                                                value,
                                                                NULL,
                                                                d_allocator_p);
        d_tail_p = d_head_p;
        d_size = 1;
        return 0;                                                     // RETURN
    }

    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode = d_head_p;
    int                                             count   = 0;
    for (; curNode != NULL; curNode = curNode->next()) {
        if (equal(curNode->key(), key)) {
            curNode->value() = value;
            if (e_BUCKETSCOPE_FIRST == scope) {
                return 1;                                             // RETURN
            }
            ++count;
        }
    }
    if (count > 0) {
        return count;                                                 // RETURN
    }
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *newNode =
           new (*d_allocator_p) StripedUnorderedContainerImpl_Node<KEY, VALUE>(
                                                                key,
                                                                value,
                                                                NULL,
                                                                d_allocator_p);
    d_tail_p->setNext(newNode);
    d_tail_p = d_tail_p->next();
    ++d_size;
    return 0;
}

template <class KEY, class VALUE>
template <class EQUAL>
bsl::size_t StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::setValue(
                                                const KEY&               key,
                                                const EQUAL&             equal,
                                                bslmf::MovableRef<VALUE> value)
{
    if (d_head_p == NULL) {
        d_head_p = new (*d_allocator_p)
                                StripedUnorderedContainerImpl_Node<KEY, VALUE>(
                                            key,
                                            bslmf::MovableRefUtil::move(value),
                                            NULL,
                                            d_allocator_p);
        d_tail_p = d_head_p;
        d_size = 1;
        return 0;                                                     // RETURN
    }
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode = d_head_p;
    for (; curNode != NULL; curNode = curNode->next()) {
        if (equal(curNode->key(), key)) {
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
            curNode->value() = bslmf::MovableRefUtil::move(value);
#else
            curNode->value() = value;
#endif
            return 1;                                                 // RETURN
        }
    }
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *newNode =
           new (*d_allocator_p) StripedUnorderedContainerImpl_Node<KEY, VALUE>(
                                            key,
                                            bslmf::MovableRefUtil::move(value),
                                            NULL,
                                            d_allocator_p);
    d_tail_p->setNext(newNode);
    d_tail_p = d_tail_p->next();
    ++d_size;
    return 0;
}

// ACCESSORS
template <class KEY, class VALUE>
inline
bool StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::empty() const
{
    return d_size == 0;
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE>
                *StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::head() const
{
    return d_head_p;
}

template <class KEY, class VALUE>
inline
bsl::size_t StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::size() const
{
    return d_size;
}

template <class KEY, class VALUE>
inline
StripedUnorderedContainerImpl_Node<KEY, VALUE> *
                 StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::tail() const
{
    return d_tail_p;
}

                               // Aspects

template <class KEY, class VALUE>
inline
bslma::Allocator *StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::allocator()
                                                                          const
{
    return d_allocator_p;
}

             // -----------------------------------------------
             // class StripedUnorderedContainerImpl_LockElement
             // -----------------------------------------------

// CREATORS
inline
StripedUnorderedContainerImpl_LockElement::
                                    StripedUnorderedContainerImpl_LockElement()
: d_pad()
{
    (void)d_pad;
}

// MANIPULATORS
inline
void StripedUnorderedContainerImpl_LockElement::lockR()
{
    d_lock.lockRead();
}

inline
void StripedUnorderedContainerImpl_LockElement::lockW()
{
    d_lock.lockWrite();
}

inline
void StripedUnorderedContainerImpl_LockElement::unlockR()
{
    d_lock.unlockRead();
}

inline
void StripedUnorderedContainerImpl_LockElement::unlockW()
{
    d_lock.unlockWrite();
}

         // --------------------------------------------------------
         // class StripedUnorderedContainerImpl_LockElementReadGuard
         // --------------------------------------------------------

// CREATORS
inline
StripedUnorderedContainerImpl_LockElementReadGuard::
                            StripedUnorderedContainerImpl_LockElementReadGuard(
                     StripedUnorderedContainerImpl_LockElement *lockElementPtr)
: d_lockElement_p(lockElementPtr)
{
}

// MANIPULATORS
inline
StripedUnorderedContainerImpl_LockElementReadGuard::
                          ~StripedUnorderedContainerImpl_LockElementReadGuard()
{
    release();
}

inline
void StripedUnorderedContainerImpl_LockElementReadGuard::release()
{
    if (d_lockElement_p) {
        d_lockElement_p->unlockR();
        d_lockElement_p = NULL;
    }
}

          // ---------------------------------------------------------
          // class StripedUnorderedContainerImpl_LockElementWriteGuard
          // ---------------------------------------------------------

// CREATORS
inline
StripedUnorderedContainerImpl_LockElementWriteGuard::
                           StripedUnorderedContainerImpl_LockElementWriteGuard(
                     StripedUnorderedContainerImpl_LockElement *lockElementPtr)
: d_lockElement_p(lockElementPtr)
{
}

// MANIPULATORS
inline
StripedUnorderedContainerImpl_LockElementWriteGuard::
                         ~StripedUnorderedContainerImpl_LockElementWriteGuard()
{
    release();
}

inline
void StripedUnorderedContainerImpl_LockElementWriteGuard::release()
{
    if (d_lockElement_p) {
        d_lockElement_p->unlockW();
        d_lockElement_p = NULL;
    }
}

                      // -----------------------------------
                      // class StripedUnorderedContainerImpl
                      // -----------------------------------

// PRIVATE CLASS METHODS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
         StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::adjustBuckets(
                                                        bsl::size_t numBuckets,
                                                        bsl::size_t numStripes)
{
    // 'numBuckets' must not less than 2, and must be a power of 2.  To avoid
    // unused stripes, we also require numBuckets >= numStripes.
    if (numBuckets < 2) {
        numBuckets = 2;
    }
    if (numBuckets < numStripes) {
        numBuckets = numStripes;
    }
    numBuckets = powerCeil(numBuckets);
    return numBuckets;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::powerCeil(
                                                               bsl::size_t num)
{
    if (num <= 1) {
        return 1;                                                     // RETURN
    }
    int power = 2;
    --num;
    while (num >>= 1) {
        power <<= 1;
    }
    return power;
}

// PRIVATE MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::checkRehash()
{
    float loadF = loadFactor();
    if (d_maxLoadFactor < loadF && canRehash()) {
        int ratio = static_cast<int>(loadF / d_maxLoadFactor);
        int growthFactor = 2;
        while (growthFactor < ratio) {
            growthFactor <<= 1;
        }
        bsl::size_t newNumBuckets = d_numBuckets * growthFactor;
        rehash(newNumBuckets);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::erase(
                                                              const KEY& key,
                                                              Scope      scope)
{
    bool        eraseAll = scope == e_SCOPE_ALL;
    bsl::size_t bucketIdx;
    LEWGuard    guard(lockWrite(&bucketIdx, key));

    StripedUnorderedContainerImpl_Bucket<KEY, VALUE> &bucket =
                                                          d_buckets[bucketIdx];

    typedef StripedUnorderedContainerImpl_Node<KEY, VALUE> Node;

    bsl::size_t count = 0;

    Node **prevNodeAddress = bucket.headAddress();
    Node  *prevNode        = NULL;
    while (*prevNodeAddress) {
        Node *node = *prevNodeAddress;
        if (d_comparator(node->key(), key)) {
            *prevNodeAddress = node->next();
            if (bucket.tail() == node) {
                bucket.setTail(prevNode);
            }
            d_allocator_p->deleteObject(node);
            bucket.incrementSize(-1);
            d_numElements.addRelaxed(-1);
            ++count;
            if (!eraseAll) {
                return count;                                         // RETURN
            }
        }
        else {
            prevNode        = node;
            prevNodeAddress = node->nextAddress();
        }
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::eraseBulk(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last,
                                                             Scope       scope)
{
    BSLS_ASSERT(first <= last);

    bool        eraseAll = scope == e_SCOPE_ALL;
    bsl::size_t count    = 0;
    // For each key, store in a vector its stripe, location, and hash value.
    int dataSize = static_cast<int>(last - first);

    bsl::vector<StripedUnorderedContainerImpl_SortItem>
                        sortIdxs(dataSize, bslma::Default::defaultAllocator());
    for (int i = 0; i < dataSize; ++i) {
        sortIdxs[i].d_hashVal   = d_hasher(first[i]);
        bsl::size_t bucketIdx   =
            bslalg::HashTableImpUtil::computeBucketIndex(sortIdxs[i].d_hashVal,
                                                         d_numBuckets);
        sortIdxs[i].d_stripeIdx = static_cast<int>(bucketToStripe(bucketIdx));
        sortIdxs[i].d_dataIdx   = i;
    }
    // Sort it by stripe, and location
    bsl::sort(sortIdxs.begin(), sortIdxs.end());

    // Lock each stripe, and process all data points in it.  Do not recalculate
    // hash code (hence keeping the hash value).
    int curStripeIdx;
    for (int j = 0; j < dataSize;) {
        curStripeIdx = sortIdxs[j].d_stripeIdx;
        LockElement& lockElement = d_locks_p[curStripeIdx];
        lockElement.lockW();
        LEWGuard guard(&lockElement);
        for (; j < dataSize && sortIdxs[j].d_stripeIdx == curStripeIdx; ++j) {
            int          dataIdx   = sortIdxs[j].d_dataIdx;
            bsl::size_t  bucketIdx =
                                  bslalg::HashTableImpUtil::computeBucketIndex(
                                                         sortIdxs[j].d_hashVal,
                                                         d_numBuckets);

            StripedUnorderedContainerImpl_Bucket<KEY, VALUE> &bucket =
                                                          d_buckets[bucketIdx];

            const KEY& key  = first[dataIdx];

            Node **prevNodeAddress = bucket.headAddress();
            Node  *prevNode        = NULL;
            while (*prevNodeAddress) {
                Node *node = *prevNodeAddress;
                if (d_comparator(node->key(), key)) {
                    *prevNodeAddress = node->next();
                    if (bucket.tail() == node) {
                        bucket.setTail(prevNode);
                    }
                    d_allocator_p->deleteObject(node);
                    bucket.incrementSize(-1);
                    d_numElements.addRelaxed(-1);
                    ++count;
                    if (!eraseAll) {
                        break;
                    }
                }
                else {
                    prevNode        = node;
                    prevNodeAddress = node->nextAddress();
                }
            }
        }
    }

    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insert(
                                                     const KEY&   key,
                                                     const VALUE& value,
                                                     Multiplicity multiplicity)
{
    bool insertAlways = multiplicity == e_INSERT_ALWAYS;

    bsl::size_t bucketIdx;
    LEWGuard    guard(lockWrite(&bucketIdx, key));

    bsl::size_t ret = 0;
    if (insertAlways) {
        // Insert, ignoring an existing value if any.  Use only in multimap.
        Node *node = new (*d_allocator_p)
                          StripedUnorderedContainerImpl_Node<KEY, VALUE>(key,
                                                                value,
                                                                NULL,
                                                                d_allocator_p);
        d_buckets[bucketIdx].addNode(node);
    }
    else {
        // Update only the first value if key exists.  Use only in hash map.
        ret = d_buckets[bucketIdx].setValue(
        key,
        d_comparator,
        value,
        StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::e_BUCKETSCOPE_FIRST);
    }
    if (ret == 1) {
        return 0;                                                     // RETURN
    }
    guard.release();
    d_numElements.addRelaxed(1);
    checkRehash();
    return 1;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insert(
                                         const KEY&               key,
                                         bslmf::MovableRef<VALUE> value,
                                         Multiplicity             multiplicity)
{
    bool insertAlways = multiplicity == e_INSERT_ALWAYS;

    bsl::size_t bucketIdx;
    LEWGuard    guard(lockWrite(&bucketIdx, key));

    bsl::size_t ret = 0;
    if (insertAlways) {
        // Insert, ignoring an existing value if any.  Use only in multimap.
        Node *node = new (*d_allocator_p)
            Node(key, bslmf::MovableRefUtil::move(value), NULL, d_allocator_p);
        d_buckets[bucketIdx].addNode(node);
    }
    else {
        // Update only the first value if key exists.  Use only in hash map.
        ret = d_buckets[bucketIdx].setValue(
                                           key,
                                           d_comparator,
                                           bslmf::MovableRefUtil::move(value));
    }
    if (ret == 1) {
        return 0;                                                     // RETURN
    }
    guard.release();
    d_numElements.addRelaxed(1);
    checkRehash();
    return 1;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertBulk(
                                                     RANDOM_ITER  first,
                                                     RANDOM_ITER  last,
                                                     Multiplicity multiplicity)
{
    BSLS_ASSERT(first <= last);

    bool        insertAlways = multiplicity == e_INSERT_ALWAYS;
    bsl::size_t count        = 0;
    int         dataSize     = static_cast<int>(last - first);

    bsl::vector<StripedUnorderedContainerImpl_SortItem>
                        sortIdxs(dataSize, bslma::Default::defaultAllocator());

    // For each key, store in a vector its stripe, location, and hash value.
    for (int i = 0; i < dataSize; ++i) {
        sortIdxs[i].d_hashVal   = d_hasher(first[i].first);
        bsl::size_t bucketIdx =
           bslalg::HashTableImpUtil::computeBucketIndex(sortIdxs[i].d_hashVal,
                                                        d_numBuckets);
        sortIdxs[i].d_stripeIdx = static_cast<int>(bucketToStripe(bucketIdx));
        sortIdxs[i].d_dataIdx   = i;
    }
    // Sort it by stripe, and location
    bsl::sort(sortIdxs.begin(), sortIdxs.end());

    // Lock each stripe, and process all data points in it.  Do not recalculate
    // hash code (hence keeping the hash value).
    int curStripeIdx;
    for (int j = 0; j < dataSize;) {
        curStripeIdx = sortIdxs[j].d_stripeIdx;
        LockElement& lockElement = d_locks_p[curStripeIdx];
        lockElement.lockW();
        LEWGuard guard(&lockElement);
        for (; j < dataSize && sortIdxs[j].d_stripeIdx == curStripeIdx; ++j) {
            int          dataIdx   = sortIdxs[j].d_dataIdx;
            bsl::size_t  bucketIdx =
                bslalg::HashTableImpUtil::computeBucketIndex(
                                                         sortIdxs[j].d_hashVal,
                                                         d_numBuckets);
            const KEY&   key   = first[dataIdx].first;
            const VALUE& value = first[dataIdx].second;

            if (insertAlways) {
                // Insert, ignoring an existing value if any.  Use only in
                // multimap.
                StripedUnorderedContainerImpl_Node<KEY, VALUE> *node =
                                                           new (*d_allocator_p)
                                StripedUnorderedContainerImpl_Node<KEY, VALUE>(
                                                                key,
                                                                value,
                                                                NULL,
                                                                d_allocator_p);
                d_buckets[bucketIdx].addNode(node);
                ++count;
                d_numElements.addRelaxed(1);
            } else {
                bsl::size_t ret = d_buckets[bucketIdx].setValue(
                    key,
                    d_comparator,
                    value,
                    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>::
                                                          e_BUCKETSCOPE_FIRST);
                if (ret == 0) {
                    ++count;
                    d_numElements.addRelaxed(1);
                }
            }
        }
    }
    checkRehash();
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setComputedValue(
                                                const KEY&             key,
                                                const VisitorFunction& visitor,
                                                Scope                  scope)
{
    typedef StripedUnorderedContainerImpl_Bucket<KEY, VALUE> BucketClass;
    typename BucketClass::BucketScope setAll = scope == e_SCOPE_ALL
                                            ? BucketClass::e_BUCKETSCOPE_ALL
                                            : BucketClass::e_BUCKETSCOPE_FIRST;

    bsl::size_t                bucketIdx;
    LEWGuard                   guard(lockWrite(&bucketIdx, key));

    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket =
                                                          d_buckets[bucketIdx];
    // Loop on the elements in the list
    int                                             count = 0;
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode = bucket.head();
    for (; curNode != NULL; curNode = curNode->next()) {
        if (d_comparator(curNode->key(), key)) {
            bool ret = visitor(&curNode->value(), key);
            if (false == setAll) {
                return ret ? 1 : -1;                                  // RETURN
            }
            ++count;
            if (false == ret) {
                return -count;                                        // RETURN
            }
        }
    }
    if (count > 0) {
        return count;                                                 // RETURN
    }

    // Not found - process as false, and return 0.
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *addNode =
                                                           new (*d_allocator_p)
                 StripedUnorderedContainerImpl_Node<KEY, VALUE>(key,
                                                                NULL,
                                                                d_allocator_p);
    {
        bslma::RawDeleterProctor<
            StripedUnorderedContainerImpl_Node<KEY, VALUE>,
            bslma::Allocator>
            proctor(addNode, d_allocator_p);

        visitor(&addNode->value(), key);
        proctor.release();
    }

    if (bucket.head() == NULL) {
        bucket.setHead(addNode);
        bucket.setTail(addNode);
    }
    else {
        bucket.tail()->setNext(addNode);
        bucket.setTail(addNode);
    }
    d_numElements.addRelaxed(1);
    bucket.incrementSize(1);
    guard.release();
    checkRehash();
    return 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setValue(
                                                            const KEY&   key,
                                                            const VALUE& value,
                                                            Scope        scope)
{
    typedef StripedUnorderedContainerImpl_Bucket<KEY, VALUE> BucketClass;
    typename BucketClass::BucketScope setAll = scope == e_SCOPE_ALL
                                            ? BucketClass::e_BUCKETSCOPE_ALL
                                            : BucketClass::e_BUCKETSCOPE_FIRST;

    bsl::size_t                bucketIdx;
    LEWGuard                   guard(lockWrite(&bucketIdx, key));

    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket =
                                                          d_buckets[bucketIdx];

    bsl::size_t count = bucket.setValue(key, d_comparator, value, setAll);
    if (count == 0) {
        guard.release();
        d_numElements.addRelaxed(1);
        checkRehash();
    }
    return count;
}

// PRIVATE ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::bucketIndex(
                                                  const KEY&  key,
                                                  bsl::size_t numBuckets) const
{
    bsl::size_t hashVal   = d_hasher(key);
    bsl::size_t bucketIdx =
             bslalg::HashTableImpUtil::computeBucketIndex(hashVal, numBuckets);
    return bucketIdx;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::bucketToStripe(
                                                 bsl::size_t bucketIndex) const
{
    return bucketIndex & d_hashMask;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedContainerImpl_LockElement *
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::lockRead(
                                                       bsl::size_t  *bucketIdx,
                                                       const KEY&    key) const
{
    // From key, get hash value, and current number of buckets.
    bsl::size_t  hashVal    = d_hasher(key);
    bsl::size_t  numBuckets = d_numBuckets;
    bsl::size_t  bucketIndex  =
           bslalg::HashTableImpUtil::computeBucketIndex(hashVal, d_numBuckets);
    bsl::size_t  stripeIdx = bucketToStripe(bucketIndex);
    LockElement& lockElement = d_locks_p[stripeIdx];
    lockElement.lockR();
    // When we get the lock, did the number of buckets change?
    if (numBuckets != d_numBuckets) {
        bucketIndex =
           bslalg::HashTableImpUtil::computeBucketIndex(hashVal, d_numBuckets);
    }
    *bucketIdx = bucketIndex;
    return &lockElement;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedContainerImpl_LockElement *
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::lockWrite(
                                                       bsl::size_t  *bucketIdx,
                                                       const KEY&    key) const
{
    // From key, get hash value, and current number of buckets.
    bsl::size_t  hashVal      = d_hasher(key);
    bsl::size_t  numBuckets   = d_numBuckets;
    bsl::size_t  bucketIndex  =
           bslalg::HashTableImpUtil::computeBucketIndex(hashVal, d_numBuckets);
    bsl::size_t  stripeIdx   = bucketToStripe(bucketIndex);
    LockElement& lockElement = d_locks_p[stripeIdx];
    lockElement.lockW();

    // When we get the lock, did the number of buckets change?
    if (numBuckets != d_numBuckets) {
        bucketIndex =
           bslalg::HashTableImpUtil::computeBucketIndex(hashVal, d_numBuckets);
    }
    *bucketIdx = bucketIndex;
    return &lockElement;
}

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::
                                                 StripedUnorderedContainerImpl(
                                           bsl::size_t       numInitialBuckets,
                                           bsl::size_t       numStripes,
                                           bslma::Allocator *basicAllocator)
: d_numStripes(powerCeil(numStripes))
, d_numBuckets(adjustBuckets(numInitialBuckets, d_numStripes))
, d_hashMask(d_numStripes - 1)
, d_maxLoadFactor(1.0)
, d_hasher()
, d_comparator()
, d_statePad()
, d_numElementsPad()
, d_buckets(d_numBuckets, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_state       = k_REHASH_ENABLED; // Rehash enabled, not in progress
    d_numElements = 0; // Hash empty

    // Allocate array of 'LockElement' objects, and construct them.
    d_locks_p = reinterpret_cast<LockElement*>(
                  d_allocator_p->allocate(d_numStripes * sizeof(LockElement)));
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        bslma::ConstructionUtil::construct(&d_locks_p[i], d_allocator_p);
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::
                                               ~StripedUnorderedContainerImpl()
{
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        bslma::DestructionUtil::destroy(&d_locks_p[i]);
    }
    d_allocator_p->deallocate(d_locks_p);
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::clear()
{
    // Locking all stripes will inherently block until a rehash will complete
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        d_locks_p[i].lockW();
    }
    for (bsl::size_t j = 0; j < d_numBuckets; ++j) {
        d_buckets[j].clear();
    }
    d_numElements = 0;
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        d_locks_p[i].unlockW();
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::disableRehash()
{
    for (;;) {
        int oldState = d_state.load();
        int newState = oldState & ~k_REHASH_ENABLED;
        if (oldState == d_state.testAndSwap(oldState, newState)) {
            return;                                                   // RETURN
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::enableRehash()
{
    for (;;) {
        int oldState = d_state.load();
        int newState = oldState | k_REHASH_ENABLED;
        if (oldState == d_state.testAndSwap(oldState, newState)) {
            break;
        }
    }
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::eraseAll(
                                                                const KEY& key)
{
    return erase(key, e_SCOPE_ALL);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::eraseBulkAll(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return eraseBulk(first, last, e_SCOPE_ALL);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::eraseBulkFirst(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return eraseBulk(first, last, e_SCOPE_FIRST);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::eraseFirst(
                                                                const KEY& key)
{
    return erase(key, e_SCOPE_FIRST);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertAlways(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    insert(key, value, e_INSERT_ALWAYS);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertAlways(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    insert(key, bslmf::MovableRefUtil::move(value), e_INSERT_ALWAYS);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertBulkAlways(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    insertBulk(first, last, e_INSERT_ALWAYS);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
template <class RANDOM_ITER>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertBulkUnique(
                                                             RANDOM_ITER first,
                                                             RANDOM_ITER last)
{
    BSLS_ASSERT(first <= last);

    return insertBulk(first, last, e_INSERT_UNIQUE);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertUnique(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return insert(key, value, e_INSERT_UNIQUE);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::insertUnique(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    return insert(key, bslmf::MovableRefUtil::move(value), e_INSERT_UNIQUE);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::maxLoadFactor(
                                                        float newMaxLoadFactor)
{
    BSLS_ASSERT(0 < newMaxLoadFactor);

    d_maxLoadFactor = newMaxLoadFactor;
    checkRehash();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
void StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::rehash(
                                                        bsl::size_t numBuckets)
{
    // 'numBuckets' must not less than 2, and must be a power of 2.  To avoid
    // unused stripes, we also require numBuckets >= numStripes.
    if (numBuckets < 2) {
        numBuckets = 2;
    }
    if (numBuckets < d_numStripes) {
        numBuckets = d_numStripes;
    }
    numBuckets = powerCeil(numBuckets);

    if (numBuckets == d_numBuckets) { // Skip if no change in # of buckets
        return;                                                       // RETURN
    }
    if (!canRehash()) { // Skip if can't rehash
        return;                                                       // RETURN
    }

    // Set state to rehash
    int oldState = d_state.testAndSwap(
                                      k_REHASH_ENABLED,
                                      k_REHASH_ENABLED | k_REHASH_IN_PROGRESS);
    if (oldState != k_REHASH_ENABLED) { // State changed under our feet
        return;                                                       // RETURN
    }

    // Allocate a new data vector
    bsl::vector<StripedUnorderedContainerImpl_Bucket<KEY,VALUE> > newBuckets(
                                                                numBuckets,
                                                                d_allocator_p);

    // Main loop on stripes: lock a stripe and process all buckets in it
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        d_locks_p[i].lockW();
        // Loop on the buckets of the current stripe.  This is simple, as the
        // stripe is the last bits in a bucket index.  We start with the
        // current stripe as the first bucket, and add 'd_numStripes' for the
        // next bucket, until 'd_numBuckets'.
        for (bsl::size_t j = i; j < d_numBuckets; j += d_numStripes) {
            StripedUnorderedContainerImpl_Bucket<KEY, VALUE> &bucket =
                                                                  d_buckets[j];
            // Process the nodes in the bucket.  Note that we do not need to
            // delete the old node and allocate a new one, but can simply move
            // it.
            for (StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode =
                                             bucket.head(); curNode != NULL;) {
                StripedUnorderedContainerImpl_Node<KEY, VALUE> *nextPtr =
                                                               curNode->next();

                bsl::size_t newBucketIdx = bucketIndex(curNode->key(),
                                                       numBuckets);
                curNode->setNext(NULL);
                newBuckets[newBucketIdx].addNode(curNode);
                curNode = nextPtr;
            }
            bucket.setHead(NULL);
            bucket.setTail(NULL);
            bucket.setSize(0);
        }
    }
    // Swap 'newBuckets' and 'd_buckets'.  This requires the same allocator.
    d_buckets.swap(newBuckets);

    // Update number of buckets.
    d_numBuckets = numBuckets;

    // Unlock all stripes.  This could not be done on the spot, as we store the
    /// rehashed data in a new set of buckets.
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        d_locks_p[i].unlockW();
    }

    // Rehash no longer in progress
    d_state = d_state & ~k_REHASH_IN_PROGRESS;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setComputedValueAll(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return setComputedValue(key, visitor, e_SCOPE_ALL);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setComputedValueFirst(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    return setComputedValue(key, visitor, e_SCOPE_FIRST);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setValueAll(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return setValue(key, value, e_SCOPE_ALL);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setValueFirst(
                                                            const KEY&   key,
                                                            const VALUE& value)
{
    return setValue(key, value, e_SCOPE_FIRST);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::setValueFirst(
                                                const KEY&               key,
                                                bslmf::MovableRef<VALUE> value)
{
    bsl::size_t bucketIdx;
    LEWGuard    guard(lockWrite(&bucketIdx, key));

    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket =
                                                          d_buckets[bucketIdx];

    bsl::size_t count = bucket.setValue(key,
                                        d_comparator,
                                        bslmf::MovableRefUtil::move(value));
    if (count == 0) {
        guard.release();
        d_numElements.addRelaxed(1);
        checkRehash();
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
int StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::update(
                                                const KEY&             key,
                                                const VisitorFunction& visitor)
{
    bsl::size_t bucketIdx;
    LEWGuard    guard(lockWrite(&bucketIdx, key));

    StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket =
                                                          d_buckets[bucketIdx];

    // Loop on the elements in the list
    int                                             count = 0;
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode = bucket.head();
    for (; curNode != NULL; curNode = curNode->next()) {
        if (d_comparator(curNode->key(), key)) {
            ++count;
            bool ret = visitor(&curNode->value(), key);
            if (ret == false) {
                return -count;                                        // RETURN
            }
        }
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
int StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::visit(
                                                const VisitorFunction& visitor)
{
    // Main loop on stripes: lock a stripe and process all buckets in it
    int count = 0;
    for (bsl::size_t i = 0; i < d_numStripes; ++i) {
        d_locks_p[i].lockW();
        // Loop on the buckets of the current stripe.  This is simple, as the
        // stripe is the last bits in a bucket index.  We start with the
        // current stripe as the first bucket, and add 'd_numStripes' for the
        // next bucket, until 'd_numBuckets'.
        for (bsl::size_t j = i; j < d_numBuckets; j += d_numStripes) {
            StripedUnorderedContainerImpl_Bucket<KEY, VALUE> &bucket =
                                                                  d_buckets[j];
            // Loop on the nodes in the bucket.
            for (StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode =
                                                bucket.head(); curNode != NULL;
                                                   curNode = curNode->next()) {
                ++count;
                bool ret = visitor(&(curNode->value()), curNode->key());
                if (!ret) {
                    d_locks_p[i].unlockW();
                    return -count;                                    // RETURN
                }
            }
        }
        d_locks_p[i].unlockW();
    }
    return count;
}

// ACCESSORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::bucketCount() const
{
    return d_numBuckets;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::bucketIndex(
                                                          const KEY& key) const
{
    return bucketIndex(key, d_numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::bucketSize(
                                                       bsl::size_t index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(index < bucketCount());

    return d_buckets[index].size();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::canRehash() const
{
    return d_state == k_REHASH_ENABLED;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::empty() const
{
    for (bsl::size_t i = 0; i < d_numBuckets; ++i) {
        if (!d_buckets[i].empty()) {
            return false;                                             // RETURN
        }
    }
    return true;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
EQUAL
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::equalFunction() const
{
    return d_comparator;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::getValue(
                                                         VALUE      *value,
                                                         const KEY&  key) const
{
    BSLS_ASSERT(NULL != value);

    bsl::size_t bucketIdx;
    LERGuard    guard(lockRead(&bucketIdx, key));

    const StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket = d_buckets[
                                                                    bucketIdx];
    // Loop on the elements in the list
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode  = bucket.head();
    for (; curNode != NULL; curNode = curNode->next()) {
        if (d_comparator(curNode->key(), key)) {
            *value = curNode->value();
            return 1;                                                 // RETURN
        }
    }
    return 0;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::getValue(
                                                 bsl::vector<VALUE> *valuesPtr,
                                                 const KEY&          key) const
{
    BSLS_ASSERT(NULL != valuesPtr);

    valuesPtr->clear();

    bsl::size_t bucketIdx;
    LERGuard    guard(lockRead(&bucketIdx, key));

    bsl::size_t                                             count  = 0;
    const StripedUnorderedContainerImpl_Bucket<KEY, VALUE>& bucket = d_buckets[
                                                                    bucketIdx];
    // Loop on the elements in the list
    StripedUnorderedContainerImpl_Node<KEY, VALUE> *curNode  = bucket.head();
    for (; curNode != NULL; curNode = curNode->next()) {
        if (d_comparator(curNode->key(), key)) {
            valuesPtr->push_back(curNode->value());
            ++count;
        }
    }
    return count;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
HASH
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::hashFunction() const
{
    return d_hasher;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bool
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::isRehashEnabled() const
{
    return d_state & k_REHASH_ENABLED;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::loadFactor() const
{
    return static_cast<float>(d_numElements.loadRelaxed()) /
           static_cast<float>(d_numBuckets);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
float
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::maxLoadFactor() const
{
    return d_maxLoadFactor;
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::numStripes() const
{
    return static_cast<bsl::size_t>(d_numStripes);
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bsl::size_t
StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::size() const
{
    return d_numElements.loadRelaxed();
}

                               // Aspects

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
bslma::Allocator *
      StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>::allocator() const
{
    return d_allocator_p;
}

             // --------------------------------------------
             // class StripedUnorderedContainerImpl_TestUtil
             // --------------------------------------------

// CREATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>::
                                        StripedUnorderedContainerImpl_TestUtil(
                  StripedUnorderedContainerImpl<KEY, VALUE, HASH, EQUAL>& hash)
: d_hash(hash)
{
}

// MANIPULATORS
template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>::lockRead(
                                                                const KEY& key)
{
    bsl::size_t  bucketIdx = d_hash.bucketIndex(key);
    bsl::size_t  stripeIdx = d_hash.bucketToStripe(bucketIdx);
    LockElement& lockElement = d_hash.d_locks_p[stripeIdx];
    lockElement.lockR();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>::
                                                      lockWrite(const KEY& key)
{
    bsl::size_t  bucketIdx   = d_hash.bucketIndex(key);
    bsl::size_t  stripeIdx   = d_hash.bucketToStripe(bucketIdx);
    LockElement& lockElement = d_hash.d_locks_p[stripeIdx];
    lockElement.lockW();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>::
                                                     unlockRead(const KEY& key)
{
    bsl::size_t  bucketIdx   = d_hash.bucketIndex(key);
    bsl::size_t  stripeIdx   = d_hash.bucketToStripe(bucketIdx);
    LockElement& lockElement = d_hash.d_locks_p[stripeIdx];
    lockElement.unlockR();
}

template <class KEY, class VALUE, class HASH, class EQUAL>
inline
void StripedUnorderedContainerImpl_TestUtil<KEY, VALUE, HASH, EQUAL>::
                                                    unlockWrite(const KEY& key)
{
    bsl::size_t  bucketIdx   = d_hash.bucketIndex(key);
    bsl::size_t  stripeIdx   = d_hash.bucketToStripe(bucketIdx);
    LockElement& lockElement = d_hash.d_locks_p[stripeIdx];
    lockElement.unlockW();
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlcc::operator<(const StripedUnorderedContainerImpl_SortItem& lhs,
                      const StripedUnorderedContainerImpl_SortItem& rhs)
{
    if (lhs.d_stripeIdx < rhs.d_stripeIdx) {
        return true;                                                  // RETURN
    }
    if (lhs.d_stripeIdx > rhs.d_stripeIdx) {
        return false;                                                 // RETURN
    }
    if (lhs.d_dataIdx < rhs.d_dataIdx) {
        return true;                                                  // RETURN
    }
    return false;
}

namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL>
struct UsesBslmaAllocator<bdlcc::StripedUnorderedContainerImpl<KEY,
                                                               VALUE,
                                                               HASH,
                                                               EQUAL> >
                                                             : bsl::true_type {
};

}  // close namespace bslma

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
