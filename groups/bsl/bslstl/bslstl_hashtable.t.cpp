// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtableiterator.h>  // usage example
#include <bslstl_iterator.h>           // 'distance', in usage example

#include <bslalg_bidirectionallink.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_exceptionguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bsltf_convertiblevaluewrapper.h>
#include <bsltf_degeneratefunctor.h>
#include <bsltf_evilbooleantype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <limits>
#include <stdexcept>  // to verify correct exceptions are thrown

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for 'strcmp'

// To resolve gcc warnings, while printing 'size_t' arguments portably on
// Windows, we use a macro and string literal concatenation to produce the
// correct 'printf' format flag.
#ifdef ZU
#undef ZU
#endif

#if defined BSLS_PLATFORM_CMP_MSVC
#  define ZU "%Iu"
#else
#  define ZU "%zu"
#endif

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    BSLS_ASSERT_OPT(a.allocator() == b.allocator());

    using namespace bsl;
    swap(a, b);
}

// The following 'using' directives must come *after* the definition of
// 'invokeAdlSwap' (above).

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
//
// (informal plan to be written up later)
// HashTable is most similar to the 'unordered_multimap' container, although
// the 'key' is implicitly also part of the 'value_type', rather than being
// storing as distinct fields in a pair.
//
// There are a wide variety of potential types to independently instantiate
// each of the template parameters of HashTable with.  To ease testing, we
// will have a primary test driver having a single type parameter that can be
// easily driven by the template testing facility, delegated to by multiple
// test configuration classes, which act as a C++03 template alias to an
// appropriate configuration of the test driver.  The main 'switch' will
// therefore run multiple instantiations of each test driver function to
// establish the necessary properties.
//
// First we will validate that HashTable is a valid value-semantic type.  This
// is difficult in the case that the stored elements are not themselves value-
// semantic, so this early testing will be limited to only those types that
// provide a full range of required behavior; testing of non-value semantic
// elements, or awkward hash and compare functors, will be deferred past the
// initial 10 cases.
// To establish value semantics, we will test the following class members, and
// a couple of specific test-support functions that simplify the test space:
//     default constructor
//     copy constructor
//     destructor
//     copy assignment operator
//     operator ==/!=
//
//   Accessors and manipulators
//     'HashTable::allocator'
//     'HashTable::elementListRoot'
//     insertValue        - a test function using 'insert' restricted
//                          to ValueType
//     verifyListContents - key accessor to validate the list root points to a
//                          list having the right set of values, and arranged
//                          so that elements with equivalent keys, determined
//                          by a supplied comparator, are stored contiguously.
//
// Therefore, 'hasher' and 'comparator' are not salient attributes, even though
// value ultimately depends on 'comparator' to define key-equivalent groups.
// Likewise, no 'insert*' operation forms the primary manipulator, nor is
// 'maxLoadFactor' a concern in establishing value - insert operations must
// satisfy constraints implied by all these additional moving parts, and will
// all be established in test cases following the value-semantic test sequence.
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//  TBD....
//
//
//           ( A '*' IN THE TABLE BELOW INDICATES THE TEST CASE HAS )
//           (   BEEN IDENTIFIED BUT HAS NOT YET BEEN COMPLETED.    )
//
//           ( NOTE THAT ALL TESTING AT THE MOMENT ASSUMES A 'set'- )
//           ( LIKE KEY_CONFIG AND THERE IS NO 'map'-LIKE EQUIVALENT)
//           ( NOR MACHINERY FOR CREATING SUITABLE TEST SEQUENCES.  )
//
//           ( WE ARE STILL LOOKING FOR A MINIMAL-BUT-COMPLETE SET  )
//           ( OF FUNCTORS AND NON-BDE ALLOCATORS TO COMPRISE TEST  )
//           (          KITS TO INVOKE FOR EACH TEST CASE.          )
//
// TYPES
//*[19] typedef ALLOCATOR                              AllocatorType;
//*[19] typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[19] typedef typename KEY_CONFIG::KeyType           KeyType;
//*[19] typedef typename KEY_CONFIG::ValueType         ValueType;
//*[19] typedef bslalg::BidirectionalNode<ValueType>   NodeType;
//*[19] typedef typename AllocatorTraits::size_type    SizeType;
//
// CREATORS
//*[11] HashTable(const ALLOCATOR&  allocator = ALLOCATOR());
//*[ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
//*[ 7] HashTable(const HashTable& original);
//*[ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
//*[ 2] ~HashTable();
//
// MANIPULATORS
//*[ 9] operator=(const HashTable& rhs);
//*[13] insert(const SOURCE_TYPE& obj);
//*[13] insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
//*[15] insertIfMissing(bool *isInsertedFlag, const SOURCE_TYPE& obj);
//*[15] insertIfMissing(bool *isInsertedFlag, const ValueType& obj);
//*[16] insertIfMissing(const KeyType& key);
//*[  ] remove(bslalg::BidirectionalLink *node);
//*[ 2] removeAll();
//*[12] rehashForNumBuckets(SizeType newNumBuckets);
//*[12] reserveForNumElements(SizeType numElements);
//*[14] setMaxLoadFactor(float loadFactor);
//*[ 8] swap(HashTable& other);
//
//      ACCESSORS
// [ 4] allocator() const;
//*[ 4] comparator() const;
//*[ 4] hasher() const;
//*[ 4] size() const;
//*[18] maxSize() const;
//*[ 4] numBuckets() const;
//*[18] maxNumBuckets() const;
//*[13] loadFactor() const;
//*[ 4] maxLoadFactor() const;
//*[ 4] elementListRoot() const;
//*[17] find(const KeyType& key) const;
//*[17] findRange(BLink **first, BLink **last, const KeyType& k) const;
//*[ 6] findEndOfRange(bslalg::BidirectionalLink *first) const;
//*[ 4] bucketAtIndex(SizeType index) const;
//*[ 4] bucketIndexForKey(const KeyType& key) const;
//*[ 4] countElementsInBucket(SizeType index) const;
//
//*[ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
//*[ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
//// specialized algorithms:
//*[ 8] void swap(HashTable& a, HashTable& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//*[  ] USAGE EXAMPLE
//
// Class HashTable_ImpDetails
//*[  ] size_t nextPrime(size_t n);
//*[  ] bslalg::HashTableBucket *defaultBucketAddress();
//
// Class HashTable_Util<ALLOCATOR>
//*[  ] initAnchor(bslalg::HashTableAnchor *, SizeType, const ALLOC&);
//*[  ] destroyBucketArray(bslalg::HashTableBucket *, SizeType, const ALLOC&)
//
// Class HashTable_ListProctor
//*[  ] TBD...
//
// Class HashTable_ArrayProctor
//*[  ] TBD...
//
// TEST TEST APPARATUS AND GENERATOR FUNCTIONS
//*[ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
//*[ 3] HashTable& gg(HashTable *object, const char *spec);
//*[ 2] insertElement(HashTable<K, H, E, A> *, const K::ValueType&)
//*[ 3] verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
//
//*[  ] CONCERN: The type employs the expected size optimizations.
//*[  ] CONCERN: The type has the necessary type traits.

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define BSL_TF_EQ      BSLTF_TEMPLATETESTFACILITY_COMPARE_EQUAL
#define BSL_TF_NOT_EQ  BSLTF_TEMPLATETESTFACILITY_COMPARE_NOT_EQUAL

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

bool verbose;
bool veryVerbose;
bool veryVeryVerbose;
bool veryVeryVeryVerbose;

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace UsageExamples {

///Usage
///-----
// This section illustrates intended use of this component.  The
// 'bslstl::HashTable' class template provides a common foundation for
// implementing the four standard unordered containers:
//: o 'bsl::unordered_map'
//: o 'bsl::unordered_multiset'
//: o 'bsl::unordered_multimap'
//: o 'bsl::unordered_set'
// This and the subsequent examples in this component use the
// 'bslstl::HashTable' class to implement several model container classes, each
// providing a small but representative sub-set of the functionality of one of
// the standard unordered containers.
//
///Example 1: Implementing a Hashed Set Container
///----------------------------------------------
// Suppose we wish to implement, 'MyHashedSet', a greatly abbreviated version
// of 'bsl::unordered_set'.  The 'bslstl::HashTable' class template can be used
// as the basis of that implementation.
//
// First, we define 'UseEntireValueAsKey', a class template we can use to
// configure 'bslstl::HashTable' to use its entire elements as keys for its
// hasher, a policy suitable for a set container.  (Later, in {Example2}, we
// will define 'UseFirstValueOfPairAsKey' for use in a map container.  Note
// that, in practice, developers can use the existing classes in
// {'bslstl_unorderedmapkeyconfiguration'} and
// {'bslstl_unorderedsetkeyconfiguration'}.)
//..
                            // ==========================
                            // struct UseEntireValueAsKey
                            // ==========================

    template <class VALUE_TYPE>
    struct UseEntireValueAsKey {
        // This 'struct' provides a namespace for types and methods that define
        // the policy by which the key value of a hashed container (i.e., the
        // value passed to the hasher) is extracted from the objects stored in
        // the hashed container (the 'value' type).

        typedef VALUE_TYPE ValueType;
            // Alias for 'VALUE_TYPE', the type stored in the hashed container.

        typedef ValueType KeyType;
            // Alias for the type passed to the hasher by the hashed container.
            // In this policy, that type is 'ValueType'.

        static const KeyType& extractKey(const ValueType& value);
            // Return the key value for the specified 'value'.  In this policy,
            // that is 'value' itself.
    };

                            // --------------------------
                            // struct UseEntireValueAsKey
                            // --------------------------

    template <class VALUE_TYPE>
    inline
    const typename UseEntireValueAsKey<VALUE_TYPE>::KeyType&
                   UseEntireValueAsKey<VALUE_TYPE>::extractKey(
                                                        const ValueType& value)
    {
        return value;
    }
//..
// Next, we define 'MyPair', a class template that can hold a pair of values of
// arbitrary types.  This will be used to in 'MyHashedSet' to return the status
// of the 'insert' method, which must provide an iterator to the inserted value
// and a boolean value indicating if the value is newly inserted if it
// previously exiting in the set.  The 'MyPair' class template will also appear
// in {Example 2} and {Example 3}.  Note that in practice, users can use the
// standard 'bsl::pair' in this role; the 'MyPair class template is used in
// these examples to avoid creating a dependency of 'bslstl_hashtable' on
// 'bslstl_pair'.
//..
                        // =============
                        // struct MyPair
                        // =============

    template <class FIRST_TYPE, class SECOND_TYPE>
    struct MyPair {
        // PUBLIC TYPES
        typedef  FIRST_TYPE  first_type;
        typedef SECOND_TYPE second_type;

        // DATA
        first_type  first;
        second_type second;

        // CREATORS
        MyPair();
            // Create a 'MyPair' object with a default constructed 'first'
            // member and a default constructed 'second' member.

        MyPair(first_type firstValue, second_type secondValue);
            // Create a 'MyPair' object with a 'first' member equal to the
            // specified 'firstValue' and the 'second' member equal to the
            // specified 'secondValue'.
    };

    // FREE OPERATORS
    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    bool operator==(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
                    const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' MyPair objects have
        // the same value, and 'false' otherwise.  'lhs' has the same value as
        // 'rhs' if 'lhs.first == rhs.first' and 'lhs.second == rhs.second'.

    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    bool operator!=(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
                    const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' MyPair objects do not
        // have the same value, and 'false' otherwise.  'lhs' does not have the
        // same value as 'rhs' if 'lhs.first != rhs.first' or
        // 'lhs.second != rhs.second'.

                        // -------------
                        // struct MyPair
                        // -------------

    // CREATORS
    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    MyPair<FIRST_TYPE,SECOND_TYPE>::MyPair()
    : first()
    , second()
    {
    }

    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    MyPair<FIRST_TYPE,SECOND_TYPE>::MyPair( first_type firstValue,
                                           second_type secondValue)
    : first(firstValue)
    , second(secondValue)
    {
    }

    // FREE OPERATORS
    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    bool operator==(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
                    const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class FIRST_TYPE, class SECOND_TYPE>
    inline
    bool operator!=(const MyPair<FIRST_TYPE, SECOND_TYPE>& lhs,
                    const MyPair<FIRST_TYPE, SECOND_TYPE>& rhs)
    {
        return lhs.first != rhs.first || lhs.second != rhs.second;
    }
//..
// Then, we define our 'MyHashedSet' class template with an instance of
// 'bststl::HashTable' (configured using 'UseEntireValueAsKey') as its sole
// data member.  We provide 'insert' method, to allow us to populate these
// sets, and the 'find' method to allow us to examine those elements.  We also
// provide 'size' and 'bucket_count' accessor methods to let us check the inner
// workings of our class.
//
// Note that the standard classes define aliases for the templated parameters
// and other types.  In the interest of brevity, this model class (and the
// classes in the subsequent examples) do not define such aliases except where
// strictly needed for the example.
//..
                            // =================
                            // class MyHashedSet
                            // =================

    template <class KEY,
              class HASH      = bsl::hash<KEY>,
              class EQUAL     = bsl::equal_to<KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedSet
    {
      private:
        // PRIVATE TYPES
        typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
        typedef typename AllocatorTraits::difference_type difference_type;
        typedef BloombergLP::bslstl::HashTableIterator<const KEY,
                                                       difference_type>
                                                          iterator;

        // DATA
        BloombergLP::bslstl::HashTable<UseEntireValueAsKey<KEY>,
                                       HASH,
                                       EQUAL,
                                       ALLOCATOR> d_impl;
      public:
        // TYPES
        typedef typename AllocatorTraits::size_type size_type;
        typedef iterator                            const_iterator;

        // CREATORS
        explicit MyHashedSet(size_type        initialNumBuckets = 0,
                             const HASH&      hash              = HASH(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());
            // Create an empty 'MyHashedSet' object having a maximum load
            // factor of 1.  Optionally specify at least 'initialNumBuckets' in
            // this container's initial array of buckets.  If
            // 'initialNumBuckets' is not supplied, an implementation defined
            // value is used.  Optionally specify a 'hash' used to generate the
            // hash values associated to the keys extracted from the values
            // contained in this object.  If 'hash' is not supplied, a
            // default-constructed object of type 'HASH()' is used.  Optionally
            // specify a key-equality functor 'keyEqual' used to verify that
            // two key values are the same.  If 'keyEqual' is not supplied, a
            // default-constructed object of type 'EQUAL' is used.  Optionally
            // specify an 'allocator' used to supply memory.  If 'allocator' is
            // not supplied, a default-constructed object of the (template
            // parameter) type 'ALLOCATOR' is used.  If the 'ALLOCATOR' is
            // 'bsl::allocator' (the default), then 'allocator' shall be
            // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
            // 'bsl::allocator' and 'allocator' is not supplied, the currently
            // installed default allocator will be used to supply memory.

        //! ~MyHashedSet() = default;
            // Destroy this object.

        // MANIPULATORS
        MyPair<const_iterator, bool> insert(const KEY& value);
            // Insert the specified 'value' into this set if the specified
            // 'value' does not already exist in this set; otherwise, this
            // method has no effect.  Return a pair whose 'first' member is an
            // iterator providing non-modifiable access to the (possibly newly
            // inserted) 'KEY' object having 'value' (according to 'EQUAL') and
            // whose 'second' member is 'true' if a new value was inserted, and
            // 'false' if the value was already present.

        // ACCESSORS
        size_type bucket_count() const;
            // Return the number of buckets in this set.

        const_iterator cend() const;
            // Return an iterator providing non-modifiable access to the
            // past-the-end element (in the sequence of 'KEY' objects)
            // maintained by this set.

        const_iterator find(const KEY& key) const;
            // Return an iterator providing non-modifiable access to the 'KEY'
            // object in this set having the specified 'key', if such an entry
            // exists, and the iterator returned by the 'cend' method
            // otherwise.

        size_type size() const;
            // Return the number of elements in this set.
    };
//..
// Next, we implement the methods of 'MyHashedSet'.  In many cases, the
// implementations consist mainly in forwarding arguments to and returning
// values from the underlying 'bslstl::HashTable'.
//..
                            // =================
                            // class MyHashedSet
                            // =================

    // CREATORS
    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::MyHashedSet(
                                            size_type        initialNumBuckets,
                                            const HASH&      hash,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
//..
// Note that the 'insertIfMissing' method of 'bslstl::HashTable' provides the
// semantics needed for adding values (unique values only) to sets.
//..
    // MANIPULATORS
    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    MyPair<typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::iterator,
           bool>    MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::insert(
                                                              const KEY& value)
    {
        typedef MyPair<iterator, bool> ResultType;

        bool                       isInsertedFlag = false;
        bslalg::BidirectionalLink *result         = d_impl.insertIfMissing(
                                                               &isInsertedFlag,
                                                               value);
        return ResultType(iterator(result), isInsertedFlag);
    }

    // ACCESSORS
    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size_type
             MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::bucket_count() const
    {
        return d_impl.numBuckets();
    }

    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
             MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::cend() const
    {
        return const_iterator();
    }

    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::const_iterator
             MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::find(const KEY& key)
                                                                          const
    {
        return const_iterator(d_impl.find(key));
    }

    template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
    inline
    typename MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size_type
             MyHashedSet<KEY, HASH, EQUAL, ALLOCATOR>::size() const
    {
        return d_impl.size();
    }

    void main1()
    {
if (verbose) {
    printf("Usage Example1\n");
}
//..
// Finally, we create 'mhs', an instance of 'MyHashedSet', exercise it, and
// confirm that it behaves as expected.
//..
    MyHashedSet<int> mhs;
    ASSERT( 0    == mhs.size());
    ASSERT( 1    == mhs.bucket_count());
//..
// Notice that the newly created set is empty and has a single bucket.
//
// Inserting a value (10) succeeds the first time but correctly fails on the
// second attempt.
//..
    MyPair<MyHashedSet<int>::const_iterator, bool> status;

    status = mhs.insert(10);
    ASSERT( 1    ==  mhs.size());
    ASSERT(10    == *status.first)
    ASSERT(true  ==  status.second);

    status = mhs.insert(10);
    ASSERT( 1    ==  mhs.size());
    ASSERT(10    == *status.first)
    ASSERT(false ==  status.second);
//..
// We can insert a different value (20) and thereby increase the set size to 2.
//..
    status = mhs.insert(20);
    ASSERT( 2    ==  mhs.size());
    ASSERT(20    == *status.first)
    ASSERT(true  ==  status.second);
//..
// Each of the inserted values (10, 20) can be found in the set.
//..
    MyHashedSet<int>::const_iterator itr, end = mhs.cend();

    itr = mhs.find(10);
    ASSERT(end !=  itr);
    ASSERT(10  == *itr);

    itr = mhs.find(20);
    ASSERT(end !=  itr);
    ASSERT(20  == *itr);
//..
// However, a value known to absent from the set (0), is correctly reported as
// not there.
//..
    itr = mhs.find(0);
    ASSERT(end ==  itr);
//..
    }

///Example 2: Implementing a Hashed Map Container
///----------------------------------------------
// Suppose we wish to implement, 'MyHashedMap', a greatly abbreviated version
// of 'bsl::unordered_map'.  As with 'MyHashedSet' (see {Example 1}), the
// 'bslstl::HashTable' class template can be used as the basis of our
// implementation.
//
// First, we define 'UseFirstValueOfPairAsKey', a class template we can use to
// configure 'bslstl::HashTable' to use the 'first' member of each element,
// each a 'MyPair', as the key-value for hashing.  Note that, in practice,
// developers can use class defined in {'bslstl_unorderedmapkeyconfiguration'}.
//..
                            // ===============================
                            // struct UseFirstValueOfPairAsKey
                            // ===============================

    template <class VALUE_TYPE>
    struct UseFirstValueOfPairAsKey {
        // This 'struct' provides a namespace for types and methods that define
        // the policy by which the key value of a hashed container (i.e., the
        // value passed to the hasher) is extracted from the objects stored in
        // the hashed container (the 'value' type).

        typedef VALUE_TYPE ValueType;
            // Alias for 'VALUE_TYPE', the type stored in the hashed container.
            // For this policy 'ValueType' must define a public member named
            // 'first' of type 'first_type'.

        typedef typename ValueType::first_type KeyType;
            // Alias for the type passed to the hasher by the hashed container.
            // In this policy, that type is the type of the 'first' element of
            // 'ValueType'.

        static const KeyType& extractKey(const ValueType& value);
            // Return the key value for the specified 'value'.  In this policy,
            // that is the value of the 'first' member of 'value'.
    };

                            // -------------------------------
                            // struct UseFirstValueOfPairAsKey
                            // -------------------------------

    template <class VALUE_TYPE>
    inline
    const typename UseFirstValueOfPairAsKey<VALUE_TYPE>::KeyType&
                   UseFirstValueOfPairAsKey<VALUE_TYPE>::extractKey(
                                                        const ValueType& value)
    {
        return value.first;
    }
//..
// Next, we define our 'MyHashedMap' class template with an instance of
// 'bststl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
// sole data member.  In this example, we choose to implement 'operator[]'
// (corresponding to the signature method of 'bsl::unordered_map') to allow us
// to populate our maps and to examine their elements.
//..
                            // =================
                            // class MyHashedMap
                            // =================

    template <class KEY,
              class VALUE,
              class HASH      = bsl::hash<KEY>,
              class EQUAL     = bsl::equal_to<KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedMap
    {
      private:
        // PRIVATE TYPES
        typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;

        typedef BloombergLP::bslstl::HashTable<
                        UseFirstValueOfPairAsKey<MyPair<const KEY, VALUE> >,
                        HASH,
                        EQUAL,
                        ALLOCATOR>                     HashTable;

        // DATA
        HashTable d_impl;

      public:
        // TYPES
        typedef typename AllocatorTraits::size_type size_type;

        // CREATORS
        explicit MyHashedMap(size_type        initialNumBuckets = 0,
                             const HASH&      hash              = HASH(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());
        // Create an empty 'MyHashedMap' object having a maximum load factor
        // of 1.  Optionally specify at least 'initialNumBuckets' in this
        // container's initial array of buckets.  If 'initialNumBuckets' is not
        // supplied, one empty bucket shall be used and no memory allocated.
        // Optionally specify 'hash' to generate the hash values associated
        // with the key-value pairs contained in this unordered map.  If 'hash'
        // is not supplied, a default-constructed object of (template
        // parameter) 'HASH' is used.  Optionally specify a key-equality
        // functor 'keyEqual' used to determine whether two keys have the same
        // value.  If 'keyEqual' is not supplied, a default-constructed object
        // of (template parameter) 'EQUAL' is used.  Optionally specify an
        // 'allocator' used to supply memory.  If 'allocator' is not supplied,
        // a default-constructed object of the (template parameter) type
        // 'ALLOCATOR' is used.  If 'ALLOCATOR' is 'bsl::allocator' (the
        // default), then 'allocator' shall be convertible to
        // 'bslma::Allocator *'.  If 'ALLOCATOR' is 'bsl::allocator' and
        // 'allocator' is not supplied, the currently installed default
        // allocator will be used to supply memory.  Note that more than
        // 'initialNumBuckets' buckets may be created in order to preserve the
        // bucket allocation strategy of the hash-table (but never fewer).

        //! ~MyHashedMap() = default;
            // Destroy this object.

        // MANIPULATORS
        VALUE& operator[](const KEY& key);
            // Return a reference providing modifiable access to the
            // mapped-value associated with the specified 'key' in this
            // unordered map; if this unordered map does not already contain a
            // 'value_type' object with 'key', first insert a new 'value_type'
            // object having 'key' and a default-constructed 'VALUE' object.
            // This method requires that the (template parameter) type 'KEY' is
            // "copy-constructible" and the (template parameter) 'VALUE' is
            // "default-constructible".
    };
//..
// Then, we implement the methods 'MyHashedMap'.  The construct need merely
// forward its arguments to the constructor of 'd_impl',
//..
                            // =================
                            // class MyHashedMap
                            // =================

    // CREATORS
    template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
    inline
    MyHashedMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::MyHashedMap(
                                            size_type        initialNumBuckets,
                                            const HASH&      hash,
                                            const EQUAL&     keyEqual,
                                            const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
//..
// As with 'MyHashedSet', the 'insertIfMissing' method of 'bslstl::HashTable'
// provides the semantics we need: an element is inserted only if no such
// element (no element with the same key) in the container, and a reference to
// that element ('node') is returned.  Here, we use 'node' to obtain and return
// a modifiable reference to the 'second' member of the (possibly newly added)
// element.  Note that the 'static_cast' from 'HashTableLink *' to
// 'HashTableNode *' is valid because the nodes derive from the link type (see
// 'bslalg_bidirectionallink' and 'bslalg_hashtableimputil').
//..
    // MANIPULATORS
    template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
    inline
    VALUE& MyHashedMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::operator[](
                                                                const KEY& key)
    {
        typedef typename HashTable::NodeType           HashTableNode;
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *node = d_impl.insertIfMissing(key);
        return static_cast<HashTableNode *>(node)->value().second;
    }

    void main2()
    {
if (verbose) {
    printf("Usage Example2\n");
}
//..
// Finally, we create 'mhm', an instance of 'MyHashedMap', exercise it, and
// confirm that it behaves as expected.  We can add an element (with key value
// of 0).
//..
    MyHashedMap<int, double> mhm;

    mhm[0] = 1.234;
    ASSERT(1.234 == mhm[0]);
//..
// We can change the value of the element with key value 0.
//..
    mhm[0] = 4.321;
    ASSERT(4.321 == mhm[0]);
//..
// We can add a new element (key value 1), without changing the previously
// existing element (key value 0).
//..
    mhm[1] = 5.768;
    ASSERT(5.768 == mhm[1]);
    ASSERT(4.321 == mhm[0]);
//..
// Accessing a non-existing element (key value 2) creates that element and
// populates it with the default value of the mapped value (0.0).
//..
    ASSERT(0.000 == mhm[2]);
//..
    }
//
///Example 3: Implementing a Hashed Multi-Map Container
///----------------------------------------------------
// Suppose we wish to implement, 'MyHashedMultiMap', a greatly abbreviated
// version of 'bsl::unordered_multimap'.  As with 'MyHashedSet' and
// 'MyHashedMap' (see {Example 1}, and {Example 2}, respectively), the
// 'bslstl::HashTable' class template can be used as the basis of our
// implementation.
//
// First, we need a class template to configure 'bslstl::HashTable' to extract
// key values in manner appropriate for maps.  The previously defined
// 'UseFirstValueOfPairAsKey' class template (see {Example 2}) suits perfectly.
//
// Next, we define our 'MyHashedMultiMap' class template with an instance of
// 'bststl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
// sole data member.  In this example, we choose to implement an 'insert'
// method to populate our container, and an 'equal_range' method (a signature
// method of the multi containers) to provide access to those elements.
//..
                            // ======================
                            // class MyHashedMultiMap
                            // ======================

    template <class KEY,
              class VALUE,
              class HASH      = bsl::hash<KEY>,
              class EQUAL     = bsl::equal_to<KEY>,
              class ALLOCATOR = bsl::allocator<KEY> >
    class MyHashedMultiMap
    {
      private:
        // PRIVATE TYPES
        typedef MyPair<const KEY, VALUE>                  value_type;
        typedef bsl::allocator_traits<ALLOCATOR>          AllocatorTraits;
        typedef typename AllocatorTraits::difference_type difference_type;

        typedef BloombergLP::bslstl::HashTable<
                           UseFirstValueOfPairAsKey<MyPair<const KEY, VALUE> >,
                           HASH,
                           EQUAL,
                           ALLOCATOR>                     HashTable;

        // DATA
        HashTable d_impl;

      public:
        // TYPES
        typedef typename AllocatorTraits::size_type  size_type;

        typedef BloombergLP::bslstl::HashTableIterator<value_type,
                                                       difference_type>
                                                                      iterator;
        typedef BloombergLP::bslstl::HashTableIterator<const value_type,
                                                       difference_type>
                                                                const_iterator;

        // CREATORS
        explicit MyHashedMultiMap(
                             size_type        initialNumBuckets = 0,
                             const HASH&      hash              = HASH(),
                             const EQUAL&     keyEqual          = EQUAL(),
                             const ALLOCATOR& allocator         = ALLOCATOR());
        // Create an empty 'MyHashedMultiMap' object having a maximum load
        // factor of 1.  Optionally specify at least 'initialNumBuckets' in
        // this container's initial array of buckets.  If 'initialNumBuckets'
        // is not supplied, an implementation defined value is used.
        // Optionally specify a 'hash', a hash-functor used to generate the
        // hash values associated to the key-value pairs contained in this
        // object.  If 'hash' is not supplied, a default-constructed object of
        // (template parameter) 'HASH' type is used.  Optionally specify a
        // key-equality functor 'keyEqual' used to verify that two key values
        // are the same.  If 'keyEqual' is not supplied, a default-constructed
        // object of (template parameter) 'EQUAL' type is used.  Optionally
        // specify an 'allocator' used to supply memory.  If 'allocator' is not
        // supplied, a default-constructed object of the (template parameter)
        // 'ALLOCATOR' type is used.  If 'ALLOCATOR' is 'bsl::allocator' (the
        // default), then 'allocator' shall be convertible to
        // 'bslma::Allocator *'.  If the 'ALLOCATOR' is 'bsl::allocator' and
        // 'allocator' is not supplied, the currently installed default
        // allocator will be used to supply memory.

        //! ~MyHashedMultiMap() = default;
            // Destroy this object.

        // MANIPULATORS
        template <class SOURCE_TYPE>
        iterator insert(const SOURCE_TYPE& value);
            // Insert the specified 'value' into this multi-map, and return an
            // iterator to the newly inserted element.  This method requires
            // that the (class template parameter) types 'KEY' and 'VALUE'
            // types both be "copy-constructible", and that the (function
            // template parameter) 'SOURCE_TYPE' be convertible to the
            // (class template parameter) 'VALUE' type.

        // ACCESSORS
        MyPair<const_iterator, const_iterator> equal_range(const KEY& key)
                                                                         const;
            // Return a pair of iterators providing non-modifiable access to
            // the sequence of 'value_type' objects in this container matching
            // the specified 'key', where the the first iterator is positioned
            // at the start of the sequence and the second iterator is
            // positioned one past the end of the sequence.  If this container
            // contains no 'value_type' objects matching 'key' then the two
            // returned iterators will have the same value.
    };
//..
// Then, we implement the methods 'MyHashedMultiMap'.  The construct need
// merely forward its arguments to the constructor of 'd_impl',
//..
                            // ======================
                            // class MyHashedMultiMap
                            // ======================

    // CREATORS
    template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
    inline
    MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::MyHashedMultiMap(
                                           size_type        initialNumBuckets,
                                           const HASH&      hash,
                                           const EQUAL&     keyEqual,
                                           const ALLOCATOR& allocator)
    : d_impl(hash, keyEqual, initialNumBuckets, 1.0f, allocator)
    {
    }
//..
// Note that here we forgo use of the 'insertIfMissing' method and use the
// 'insert' method of 'bslstl::HashTable'.  This method supports the semantics
// of the multi containers: there can be more than one element with the same
// key value.
//..
    // MANIPULATORS
    template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
    template <class SOURCE_TYPE>
    inline
    typename MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::iterator
             MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::insert(
                                                      const SOURCE_TYPE& value)
    {
        return iterator(d_impl.insert(value));
    }
//..
// The 'equal_range' method need only convert the values returned by the
// 'findRange' method to the types expected by the caller.
//..
    // ACCESSORS
    template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
    MyPair<typename MyHashedMultiMap<KEY,
                                     VALUE,
                                     HASH,
                                     EQUAL,
                                     ALLOCATOR>::const_iterator,
           typename MyHashedMultiMap<KEY,
                                     VALUE,
                                     HASH,
                                     EQUAL, ALLOCATOR>::const_iterator>
    MyHashedMultiMap<KEY, VALUE, HASH, EQUAL, ALLOCATOR>::equal_range(
                                                          const KEY& key) const
    {
        typedef MyPair<const_iterator, const_iterator> ResultType;
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_impl.findRange(&first, &last, key);
        return ResultType(const_iterator(first), const_iterator(last));
    }

    void main3()
    {
if (verbose) {
    printf("Usage Example3\n");
}
//..
// Finally, we create 'mhmm', an instance of 'MyHashedMultiMap', exercise it,
// and confirm that it behaves as expected.
//
// We define several aliases to make our code more concise.
//..
    typedef MyHashedMultiMap<int, double>::iterator       Iterator;
    typedef MyHashedMultiMap<int, double>::const_iterator ConstIterator;
    typedef MyPair<ConstIterator, ConstIterator>          ConstRange;
//..
// Searching for an element (key value 10) in a newly created, empty container
// correctly shows the absence of any such element.
//..
    MyHashedMultiMap<int, double> mhmm;

    ConstRange range;
    range = mhmm.equal_range(10);
    ASSERT(range.first == range.second);
//..
// We can insert a value (the pair 10, 100.00) into the container...
//..
    MyPair<const int, double> value(10, 100.00);

    Iterator itr;

    itr = mhmm.insert(value);
    ASSERT(value == *itr);
//..
// ... and we can do so again.
//..
    itr = mhmm.insert(value);
    ASSERT(value == *itr);
//..
// We can now find elements with the key value of 10.
//..
    range = mhmm.equal_range(10);
    ASSERT(range.first != range.second);
//..
// As expected, there are two such elements, and both are identical in key
// value (10) and mapped value (100.00).
//..
    int count = 0;
    for (ConstIterator cur  = range.first,
                       end  = range.second;
                       end != cur; ++cur, ++count) {
        ASSERT(value == *cur);
    }
    ASSERT(2 == count);
//..
    }
//
///Example 4: Implementing a Custom Container
///------------------------------------------
// Although the 'bslstl::HashTable' class was created to be a common
// implementation for the standard unordered classes, this class can also be
// used in its own right to address other user problems.
//
// Suppose that we wish to retain a record of sales orders, that each record is
// characterized by several integer attributes, and that we must be able to
// find records based on *any* of those attributes.  We can use
// 'bslstl::HashTable' to implement a custom container supporting multiple
// key-values.
//
// First, we define 'MySalesRecord', our record class:
//..
    enum { MAX_DESCRIPTION_SIZE = 16 };

    typedef struct MySalesRecord {
        int  orderNumber;                        // unique
        int  customerId;                         // no constraint
        int  vendorId;                           // no constraint
        char description[MAX_DESCRIPTION_SIZE];  // ascii string
    } MySalesRecord;
//..
// Notice that only each 'orderNumber' is unique.  We expect multiple sales to
// any given customer ('customerId') and multiple sales by any given vendor
// ('vendorId').
//
// We will use a 'bslstl::HashTable' object (a hashtable) to save record values
// based on the unique 'orderNumber', and two auxiliary hashtables to provide
// map 'customerId' and 'vendorId' values to the addresses of the records in
// the first 'bslstl::HashTable' object.  Note that this implementation relies
// on the fact that nodes in our hashtables remain stable until they are
// removed and that in this application we do *not* allow the removal (or
// modification) of records once they are inserted.
//
// To configure these hashtables, we will need several policy objects to
// extract relevant portions the 'MySalesRecord' objects for hashing.
//
// Next, define 'UseOrderNumberAsKey', a policy class for the hashtable holding
// the sales record objects.  Note that the 'ValueType' is 'MySalesRecord' and
// that the 'extractKey' method selects the 'orderNumber' attribute:
//..
                            // ==========================
                            // struct UseOrderNumberAsKey
                            // ==========================

    struct UseOrderNumberAsKey {
        // This 'struct' provides a namespace for types and methods that define
        // the policy by which the key value of a hashed container (i.e., the
        // value passed to the hasher) is extracted from the objects stored in
        // the hashed container (the 'value' type).

        typedef MySalesRecord ValueType;
            // Alias for 'MySalesRecord', the type stored in the first
            // hashtable.

        typedef int KeyType;
            // Alias for the type passed to the hasher by the hashed container.
            // In this policy, the value passed to the hasher is the
            // 'orderNumber' attribute, an 'int' type.

        static const KeyType& extractKey(const ValueType& value);
            // Return the key value for the specified 'value'.  In this policy,
            // that is the 'orderNumber' attribute of 'value'.
    };

                            // --------------------------
                            // struct UseOrderNumberAsKey
                            // --------------------------

    inline
    const UseOrderNumberAsKey::KeyType&
          UseOrderNumberAsKey::extractKey(const ValueType& value)
    {
        return value.orderNumber;
    }
//..
// Then, we define 'UseCustomerIdAsKey', the policy class for the hashtable
// that will multiply map 'customerId' to the addresses of records in the first
// hashtable.  Note that in this policy class the 'ValueType' is
// 'const MySalesRecord *'.
//..
                            // =========================
                            // struct UseCustomerIdAsKey
                            // =========================

    struct UseCustomerIdAsKey {
        // This 'struct' provides a namespace for types and methods that define
        // the policy by which the key value of a hashed container (i.e., the
        // value passed to the hasher) is extracted from the objects stored in
        // the hashed container (the 'value' type).

        typedef const MySalesRecord *ValueType;
            // Alias for 'const MySalesRecord *', the type stored in second
            // hashtable, a pointer to the record stored in the first
            // hashtable.

        typedef int KeyType;
            // Alias for the type passed to the hasher by the hashed container.
            // In this policy, the value passed to the hasher is the
            // 'orderNumber' attribute, an 'int' type.

        static const KeyType& extractKey(const ValueType& value);
            // Return the key value for the specified 'value'.  In this policy,
            // that is the 'customerId' attribute of 'value'.
    };

                            // -------------------------
                            // struct UseCustomerIdAsKey
                            // -------------------------

    inline
    const UseCustomerIdAsKey::KeyType&
          UseCustomerIdAsKey::extractKey(const ValueType& value)
    {
        return value->customerId;
    }
//..
// Notice that, since the values in the second hashtable are addresses, the
// key-value is extracted by reference.  This second hashtable allows what
// map-like semantics, *without* having to store key-values; those reside in
// the records in the first hashtable.
//
// The 'UseVendorIdAsKey' class, the policy class for the hashtable providing
// an index by 'vendorId', is almost a near clone of 'UseCustomerIdAsKey'.  It
// is shown for completeness:
//..
                            // =======================
                            // struct UseVendorIdAsKey
                            // ========================

    struct UseVendorIdAsKey {
        // This 'struct' provides a namespace for types and methods that define
        // the policy by which the key value of a hashed container (i.e., the
        // value passed to the hasher) is extracted from the objects stored in
        // the hashed container (the 'value' type).

        typedef const MySalesRecord *ValueType;
            // Alias for 'const MySalesRecord *', the type stored in second
            // hashtable, a pointer to the record stored in the first
            // hashtable.

        typedef int KeyType;
            // Alias for the type passed to the hasher by the hashed container.
            // In this policy, the value passed to the hasher is the
            // 'vendorId' attribute, an 'int' type.

        static const KeyType& extractKey(const ValueType& value);
            // Return the key value for the specified 'value'.  In this policy,
            // that is the 'vendorId' attribute of 'value'.
    };

                            // -----------------------
                            // struct UseVendorIdAsKey
                            // -----------------------

    inline
    const UseVendorIdAsKey::KeyType&
          UseVendorIdAsKey::extractKey(const ValueType& value)
    {
        return value->vendorId;
    }
//..
// Next, we define 'MySalesRecordContainer', our customized container:
//..
                            // ----------------------------
                            // class MySalesRecordContainer
                            // ----------------------------

    class MySalesRecordContainer
    {
      private:
        // PRIVATE TYPES
        typedef BloombergLP::bslstl::HashTable<
                      UseOrderNumberAsKey,
                      bsl::hash<    UseOrderNumberAsKey::KeyType>,
                      bsl::equal_to<UseOrderNumberAsKey::KeyType> >
                                                          RecordsByOrderNumber;
        typedef bsl::allocator_traits<
              bsl::allocator<UseOrderNumberAsKey::ValueType> > AllocatorTraits;
        typedef AllocatorTraits::difference_type               difference_type;
//..
// The 'ItrByOrderNumber' type is used to provide access to the elements of the
// first hash table, the one that stores the records.
//..

        typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord,
                                                       difference_type>
                                                              ItrByOrderNumber;
//..
// The 'ItrPtrById' type is used to provide access to the elements of the other
// hashtables, the ones that store pointers into the first hashtable.
//..
        typedef BloombergLP::bslstl::HashTableIterator<const MySalesRecord *,
                                                       difference_type>
                                                                    ItrPtrById;
//..
// If we were to provide iterators of type 'ItrPtrById' to our users,
// dereferencing the iterator would provide a 'MySalesRecord' pointer, which
// would then have to be dereferences.  Instead, we use 'ItrPtrById' to define
// 'ItrById' in which accessors have been overriden to provide that extra
// dereference implicitly.
//..
        class ItrById : public ItrPtrById
        {
          public:
            // CREATORS
            explicit ItrById(bslalg::BidirectionalLink *node)
            : ItrPtrById(node)
            {
            }

            // ACCESSORS
            const MySalesRecord& operator*() const
            {
                return *ItrPtrById::operator*();
            }

            const MySalesRecord *operator->() const
            {
                return &(*ItrPtrById::operator*());
            }
        };

        typedef BloombergLP::bslstl::HashTable<
                      UseCustomerIdAsKey,
                      bsl::hash<    UseCustomerIdAsKey::KeyType>,
                      bsl::equal_to<UseCustomerIdAsKey::KeyType> >
                                                       RecordsPtrsByCustomerId;
        typedef BloombergLP::bslstl::HashTable<
                      UseVendorIdAsKey,
                      bsl::hash<    UseVendorIdAsKey::KeyType>,
                      bsl::equal_to<UseVendorIdAsKey::KeyType> >
                                                         RecordsPtrsByVendorId;
        // DATA
        RecordsByOrderNumber    d_recordsByOrderNumber;
        RecordsPtrsByCustomerId d_recordptrsByCustomerId;
        RecordsPtrsByVendorId   d_recordptrsByVendorId;

      public:
        // PUBLIC TYPES
        typedef ItrByOrderNumber  ConstItrByOrderNumber;
        typedef ItrById           ConstItrById;

        // CREATORS
        explicit MySalesRecordContainer(bslma::Allocator *basicAllocator = 0);
            // Create an empty 'MySalesRecordContainer' object.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        //! ~MySalesRecordContainer() = default;
            // Destroy this object.

        // MANIPULATORS
        MyPair<ConstItrByOrderNumber, bool> insert(const MySalesRecord& value);
            // Insert the specified 'value' into this set if the specified
            // 'value' does not already exist in this set; otherwise, this
            // method has no effect.  Return a pair whose 'first' member is an
            // iterator providing non-modifiable access to the (possibly newly
            // inserted) 'MySalesRecord' object having 'value' and whose
            // 'second' member is 'true' if a new value was inserted, and
            // 'false' if the value was already present.

        // ACCESSORS
        ConstItrByOrderNumber cend() const;
            // Return an iterator providing non-modifiable access to the
            // past-the-end element (in the sequence of 'MySalesRecord'
            // objects) maintained by this set.

        ConstItrByOrderNumber findByOrderNumber(int value) const;
            // Return an iterator providing non-modifiable access to the
            // 'MySalesRecord' object in this set having the specified 'value',
            // if such an entry exists, and the iterator returned by the 'cend'
            // method otherwise.
//..
// Notice that this interface provides map-like semantics for finding records.
// We need only specify the 'orderNumber' attribute of the record of interest;
// however, the return value is set-like: we get access to the record, not the
// more complicated key-value/record pair that a map would have provided.
//
// Internally, the hash table need only store the records themselves.  A map
// would have had to manage key-value/record pairs, where the key-value would
// be a copy of part of the record.
//..
        MyPair<ConstItrById, ConstItrById> findByCustomerId(int value) const;
            // Return a pair of iterators providing non-modifiable access to
            // the sequence of 'MySalesRecord' objects in this container having
            // a 'customerId' attribute equal to the specified 'value' where
            // the first iterator is positioned at the start of the sequence
            // and the second iterator is positioned one past the end of the
            // sequence.  If this container has no such objects, then the two
            // iterators will be equal.

        MyPair<ConstItrById, ConstItrById> findByVendorId(int value) const;
            // Return a pair of iterators providing non-modifiable access to
            // the sequence of 'MySalesRecord' objects in this container having
            // a 'vendorId' attribute equal to the specified 'value' where the
            // first iterator is positioned at the start of the sequence and
            // the second iterator is positioned one past the end of the
            // sequence.  If this container has no such objects, then the two
            // iterators will be equal.
    };
//..
// Then, we implement the methods of 'MySalesRecordContainer', our customized
// container:
//..
                            // ----------------------------
                            // class MySalesRecordContainer
                            // ----------------------------

    // CREATORS
    inline
    MySalesRecordContainer::MySalesRecordContainer(
                                              bslma::Allocator *basicAllocator)
    : d_recordsByOrderNumber(basicAllocator)
    , d_recordptrsByCustomerId(basicAllocator)
    , d_recordptrsByVendorId(basicAllocator)
    {
    }

    // MANIPULATORS
    inline
    MyPair<MySalesRecordContainer::ConstItrByOrderNumber, bool>
    MySalesRecordContainer::insert(const MySalesRecord& value)
    {
        // Insert into internal container that will own the record.

        bool                                    isInsertedFlag = false;
        BloombergLP::bslalg::BidirectionalLink *result         =
                d_recordsByOrderNumber.insertIfMissing(&isInsertedFlag, value);

        // Index by other record attributes

        RecordsByOrderNumber::NodeType *nodePtr =
                         static_cast<RecordsByOrderNumber::NodeType *>(result);

        d_recordptrsByCustomerId.insert(&nodePtr->value());
          d_recordptrsByVendorId.insert(&nodePtr->value());

        // Return of insertion.

        return MyPair<ConstItrByOrderNumber, bool>(
                                                 ConstItrByOrderNumber(result),
                                                 isInsertedFlag);
    }

    // ACCESSORS
    inline
    MySalesRecordContainer::ConstItrByOrderNumber
    MySalesRecordContainer::cend() const
    {
        return ConstItrByOrderNumber();
    }

    inline
    MySalesRecordContainer::ConstItrByOrderNumber
    MySalesRecordContainer::findByOrderNumber(int value) const
    {
        return ConstItrByOrderNumber(d_recordsByOrderNumber.find(value));
    }

    inline
    MyPair<MySalesRecordContainer::ConstItrById,
           MySalesRecordContainer::ConstItrById>
    MySalesRecordContainer::findByCustomerId(int value) const
    {
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_recordptrsByCustomerId.findRange(&first, &last, value);

        return MyPair<ConstItrById, ConstItrById>(ConstItrById(first),
                                                  ConstItrById(last));
    }

    inline
    MyPair<MySalesRecordContainer::ConstItrById,
           MySalesRecordContainer::ConstItrById>
    MySalesRecordContainer::findByVendorId(int value) const
    {
        typedef BloombergLP::bslalg::BidirectionalLink HashTableLink;

        HashTableLink *first;
        HashTableLink *last;
        d_recordptrsByVendorId.findRange(&first, &last, value);

        return MyPair<ConstItrById, ConstItrById>(ConstItrById(first),
                                                  ConstItrById(last));
    }

    void main4()
    {
if (verbose) {
    printf("Usage Example4\n");
}
//..
// Now, create an empty container and load it with some sample data.
//..
        MySalesRecordContainer msrc;

        const MySalesRecord DATA[] = {
            { 1000, 100, 10, "hello" },
            { 1001, 100, 20, "world" },
            { 1002, 200, 10, "how" },
            { 1003, 200, 20, "are" },
            { 1004, 100, 10, "you" },
            { 1005, 100, 20, "today" }
        };
        const int numDATA = sizeof DATA / sizeof *DATA;

if (verbose) {
        printf("Insert sales records into container.\n");
}

        for (int i = 0; i < numDATA; ++i) {
            const int orderNumber   = DATA[i].orderNumber;
            const int customerId    = DATA[i].customerId;
            const int vendorId      = DATA[i].vendorId;
            const char *description = DATA[i].description;

if (verbose) {
            printf("%d: %d %d %s\n",
                   orderNumber,
                   customerId,
                   vendorId,
                   description);
}
            MyPair<MySalesRecordContainer::ConstItrByOrderNumber,
                   bool> status = msrc.insert(DATA[i]);
            ASSERT(msrc.cend() != status.first);
            ASSERT(true        == status.second);
        }
//..
// We find on standard output:
//..
//  Insert sales records into container.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
//..
// We can search our container by order number and find the expected records.
//..
if (verbose) {
        printf("Find sales records by order number.\n");
}
        for (int i = 0; i < numDATA; ++i) {
            const int orderNumber   = DATA[i].orderNumber;
            const int customerId    = DATA[i].customerId;
            const int vendorId      = DATA[i].vendorId;
            const char *description = DATA[i].description;

if (verbose) {
            printf("%d: %d %d %s\n",
                   orderNumber,
                   customerId,
                   vendorId,
                   description);
}
            MySalesRecordContainer::ConstItrByOrderNumber itr =
                                           msrc.findByOrderNumber(orderNumber);
            ASSERT(msrc.cend() != itr);
            ASSERT(orderNumber == itr->orderNumber);
            ASSERT(customerId  == itr->customerId);
            ASSERT(vendorId    == itr->vendorId);
            ASSERT(0 == strcmp(description, itr->description));
        }
//..
// We find on standard output:
//..
//  Find sales records by order number.
//  1000: 100 10 hello
//  1001: 100 20 world
//  1002: 200 10 how
//  1003: 200 20 are
//  1004: 100 10 you
//  1005: 100 20 today
//..
// We can search our container by customer identifier and find the expected
// records.
//..
if (verbose) {
        printf("Find sales records by customer identifier.\n");
}

        for (int customerId = 100; customerId <= 200; customerId += 100) {
            MyPair<MySalesRecordContainer::ConstItrById,
                   MySalesRecordContainer::ConstItrById> result =
                                             msrc.findByCustomerId(customerId);
            int count = bsl::distance(result.first, result.second);
if (verbose) {
            printf("customerId %d, count %d\n", customerId, count);
}

            for (MySalesRecordContainer::ConstItrById itr  = result.first,
                                                      end  = result.second;
                                                      end != itr; ++itr) {
if (verbose) {
                printf("\t\t%d %d %d %s\n",
                       itr->orderNumber,
                       itr->customerId,
                       itr->vendorId,
                       itr->description);
}
            }
        }
//..
// We find on standard output:
//..
//  Find sales records by customer identifier.
//  customerId 100, count 4
//              1005 100 20 today
//              1004 100 10 you
//              1001 100 20 world
//              1000 100 10 hello
//  customerId 200, count 2
//              1003 200 20 are
//              1002 200 10 how
//..
// Lastly, we can search our container by vendor identifier and find the
// expected records.
//..
if (verbose) {
        printf("Find sales records by vendor identifier.\n");
}

        for (int vendorId = 10; vendorId <= 20; vendorId += 10) {
            MyPair<MySalesRecordContainer::ConstItrById,
                   MySalesRecordContainer::ConstItrById> result =
                                                 msrc.findByVendorId(vendorId);
            int count = bsl::distance(result.first, result.second);
if (verbose) {
            printf("vendorId %d, count %d\n", vendorId, count);
}

            for (MySalesRecordContainer::ConstItrById itr  = result.first,
                                                      end  = result.second;
                                                      end != itr; ++itr) {
if (verbose) {
                printf("\t\t%d %d %d %s\n",
                       (*itr).orderNumber,
                       (*itr).customerId,
                       (*itr).vendorId,
                       (*itr).description);
}
            }
        }
//..
// We find on standard output:
//..
//  Find sales records by vendor identifier.
//  vendorId 10, count 3
//              1004 100 10 you
//              1002 200 10 how
//              1000 100 10 hello
//  vendorId 20, count 3
//              1005 100 20 today
//              1003 200 20 are
//              1001 100 20 world
//..
    }
}  // close namespace UsageExamples

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
bslma::TestAllocator *g_bsltfAllocator_p = 0;

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line idx  spec                 results
    //---- ---  --------             -------
    { L_,    0, "",                  "" },
    { L_,    1, "A",                 "A" },
    { L_,    2, "AA",                "AA" },
    { L_,   20, "B",                 "B" },
    { L_,    5, "AB",                "AB" },
    { L_,    5, "BA",                "AB" },
    { L_,   19, "AC",                "AC" },
    { L_,   23, "CD",                "CD" },
    { L_,    7, "ABC",               "ABC" },
    { L_,    7, "ACB",               "ABC" },
    { L_,    7, "BAC",               "ABC" },
    { L_,    7, "BCA",               "ABC" },
    { L_,    7, "CAB",               "ABC" },
    { L_,    7, "CBA",               "ABC" },
    { L_,   17, "BAD",               "ABD" },
    { L_,    4, "ABCA",              "AABC" },
    { L_,    6, "ABCB",              "ABBC" },
    { L_,    8, "ABCC",              "ABCC" },
    { L_,    3, "ABCABC",            "AABBCC" },
    { L_,    3, "AABBCC",            "AABBCC" },
    { L_,    9, "ABCD",              "ABCD" },
    { L_,    9, "ACBD",              "ABCD" },
    { L_,   18, "BEAD",              "ABDE" },
    { L_,   10, "ABCDE",             "ABCDE" },
    { L_,   22, "FEDCB",             "BCDEF" },
    { L_,   11, "FEDCBA",            "ABCDEF" },
    { L_,   14, "ABCDEFGHI",         "ABCDEFGHI" },
    { L_,   16, "ABCDEFGHIJKLMNOPQ", "ABCDEFGHIJKLMNOPQ" },
    { L_,   16, "DHBIMACOPELGFKNJQ", "ABCDEFGHIJKLMNOPQ" }
};

static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

static
const float DEFAULT_MAX_LOAD_FACTOR[] = {
     0.125f,
//     0.618f,
     1.0f,
//     1.618f,
     8.0f,
};
static const int DEFAULT_MAX_LOAD_FACTOR_SIZE =
              sizeof DEFAULT_MAX_LOAD_FACTOR / sizeof *DEFAULT_MAX_LOAD_FACTOR;

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;

//=============================================================================
//                           HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------

namespace bsl
{
                    // ============================================
                    // class template bsl::equal_to specializations
                    // ============================================

template <>
struct equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    // This class template specialization provides a functor to determine when
    // two '::BloombergLP::bsltf::NonEqualComparableTestType' objects have the
    // same value.  Usually, the primary template calls 'operator==' to
    // determine the result, but that cannot work for the test type that is
    // specifically designed to not have an 'operator=='.  In such cases, we
    // must provide an explicit specialization of this template if we expect
    // the test type to work in our test cases along side other types.

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          first_argument_type;
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType
                                                          second_argument_type;
    typedef bool                                          result_type;

    // ACCESSORS
    bool operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                    const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                         const;
        // Return 'true' if the specified 'a' and 'b' objects have the same
        // value, and 'false' otherwise.  Two 'NonEqualComparableTestType'
        // objects have the same value if ... (TBD)
};

                    // ========================================
                    // class template bsl::hash specializations
                    // ========================================

template <>
struct hash< ::BloombergLP::bsltf::NonEqualComparableTestType> {
    // This class template specialization provides a functor to determine a
    // hash value for '::BloombergLP::bsltf::NonEqualComparableTestType'
    // objects.

    // TYPES
    typedef ::BloombergLP::bsltf::NonEqualComparableTestType argument_type;
    typedef size_t                                           result_type;

    // ACCESSORS
    size_t operator()(const ::BloombergLP::bsltf::NonEqualComparableTestType&
                                                                  value) const;
};

}  // close namespace bsl

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes
{

                       // ===========================
                       // class AwkwardMaplikeElement
                       // ===========================

class AwkwardMaplikeElement {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while
    // the class itself *is* equality-comparable (as required of a value
    // semantic type) so that a HashTable of these objects should have a well-
    // defined 'operator=='.  Note that this class is a specific example for a
    // specific problem, rather than a template providing the general test type
    // for keys distinct from values, as the template test facility requires an
    // explicit specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  public:
    // CREATORS
    AwkwardMaplikeElement();

    explicit
    AwkwardMaplikeElement(int value);

    explicit
    AwkwardMaplikeElement(const bsltf::NonEqualComparableTestType& value);

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    const bsltf::NonEqualComparableTestType& key() const;
        // Return a non-modifiable reference to the 'key' of this object.
};

bool operator==(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AwkwardMaplikeElement' objects have
    // the same value if ... (TBD)

bool operator!=(const AwkwardMaplikeElement& lhs,
                const AwkwardMaplikeElement& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AwkwardMaplikeElement' objects
    // do not have the same value if ... (TBD)

void debugprint(const AwkwardMaplikeElement& value);

}  // close namespace TestTypes

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace BloombergLP {
namespace bslstl {
// HashTable-specific print function.
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
        const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t);
}  // close namespace BloombergLP::bslstl

namespace bsltf {

template <>
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                               const TestTypes::AwkwardMaplikeElement& object);

}  // close namespace BloombergLP::bsltf
}  // close namespace BloombergLP

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace
{

                       // ===============
                       // class BoolArray
                       // ===============

class BoolArray {
    // This class holds a set of boolean flags, the number of which is
    // specified when such an object is constructed.  The number of flags is
    // fixed at the time of construction, and the object is neither copy-
    // constructible nor copy-assignable.  The flags can be set and tested
    // using 'operator[]'.  This class is a lightweight alternative to
    // 'vector<bool>' as there is no need to introduce such a component
    // dependency for a simple test facility (in the same package).
    // TBD The constructor should support a 'bslma::Allocator *' argument to
    // supply memory for the dynamically allocated array.

  private:
    // DATA
    bool   *d_data;
    size_t  d_size;

  private:
    BoolArray(const BoolArray&); // = delete;
        // not implemented

    BoolArray& operator=(const BoolArray&);  // = delete
        // not implemented

  public:
    // CREATORS
    explicit BoolArray(size_t n);
        // Create a 'BoolArray' object holding 'n' boolean flags.

    ~BoolArray();
        // Destroy this object, reclaiming any allocated memory.

    // MANIPULATORS
    bool& operator[](size_t index);
        // Return a modifiable reference to the boolean flag at the specified
        // 'index'.  The behavior is undefined unless 'index < size()'.

    // ACCESSORS
    bool operator[](size_t index) const;
        // Return the value of the boolean flag at the specified 'index'.  The
        // behavior is undefined unless 'index < size()'.

    size_t size() const;
        // Return the number of boolean flags held by this object.
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===============================
                       // class GroupedEqualityComparator
                       // ===============================

template <class TYPE, int GROUP_SIZE>
class GroupedEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE', but in a
    // way that multiple distinct value will compare equal with each other.
    // The function-call operator compares the integer returned by the class
    // method 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.

  public:
    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const;
        // Return 'true' if the integer representation of the specified 'lhs'
        // divided by 'GROUP_SIZE' (rounded down) is equal to than integer
        // representation of the specified 'rhs' divided by 'GROUP_SIZE'.
};

                       // ===================
                       // class GroupedHasher
                       // ===================

template <class TYPE, class HASHER, int GROUP_SIZE>
class GroupedHasher : private HASHER {
    // This test class provides a mechanism that defines a function-call
    // operator that returns the same hash for multiple values.  The
    // function-call operator invoke the (template parameter) type 'HASHER' on
    // the integer returned by the class method
    // 'TemplateTestFacility::getIdentifier' divided by the (template
    // parameter) GROUP_SIZE.  'HASHER' shall be a class that can be invoked as
    // if it has the following method:
    //..
    //  int operator()(int value);
    //..

  public:
    // ACCESSORS
    size_t operator() (const TYPE& value);
        // Return the hash value of the integer representation of the specified
        // 'value' divided by 'GROUP_SIZE' (rounded down) is equal to than
        // integer representation of the specified 'rhs' divided by
        // 'GROUP_SIZE'.
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ============================
                       // class TestEqualityComparator
                       // ============================

template <class TYPE>
class TestEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CREATORS
    //! TestEqualityComparator(const TestEqualityComparator& original) =
    //                                                                 default;
        // Create a copy of the specified 'original'.

    explicit TestEqualityComparator(int id = 0);
        // Create a 'TestEqualityComparator'.  Optionally, specify 'id' that
        // can be used to identify the object.

    // MANIPULATORS
    void setId(int value);

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const;
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.

    int id() const;
        // Return the 'id' of this object.

    size_t count() const;
        // Return the number of times 'operator()' is called.
};

template <class TYPE>
bool operator==(const TestEqualityComparator<TYPE>& lhs,
                const TestEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specificed 'lhs' and 'rhs' have the same value,
    // and 'false' otherwise.  Two 'TestEqualityComparator' functors have the
    // same value if they have the same 'id'.

template <class TYPE>
bool operator!=(const TestEqualityComparator<TYPE>& lhs,
                const TestEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specificed 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'TestEqualityComparator' functors do
    // not have the same value if they do not have the same 'id'.

                       // =====================
                       // class TestHashFunctor
                       // =====================

template <class TYPE>
class TestHashFunctor {
    // This value-semantic class adapts a class meeting the C++11 'Hash'
    // requirements (C++11 [hash.requirements], 17.6.3.4) with an additional
    // 'mixer' attribute, that constitutes the value of this class, and is
    // used to mutate the value returned from the adapted hasher when calling
    // 'operator()'.  Note that this class privately inherits from the empty
    // base class 'bsl::hash' in order to exploit a compiler optimization.

    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    int         d_id;           // identifier for the functor
    mutable int d_count;        // number of times 'operator()' is called

  public:
    // CREATORS
    //! TestHashFunctor(const TestHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit TestHashFunctor(int id = 0);
        // Create a 'TestHashFunctor'.  Optionally, specify 'id' that can be
        // used to identify the object.

    // MANIPULATORS
    void setId(int value);
        // Set the 'id' of this object to the specified value.  Note that the
        // 'id' contributes to the value produced by the 'operator()' method,
        // so the 'id' of a 'TestHashFunctor' should not be changed for
        // functors that are installed in 'HashTable' objects.

    // ACCESSORS
    native_std::size_t operator() (const TYPE& obj) const;
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.

    int id() const;
        // Return the 'id' of this object.

    size_t count() const;
        // Return the number of times 'operator()' is called.
};

template <class TYPE>
bool operator==(const TestHashFunctor<TYPE>& lhs,
                const TestHashFunctor<TYPE>& rhs);
    // Return 'true' if the specificed 'lhs' and 'rhs' have the same value,
    // and 'false' otherwise.  Two 'TestHashFunctor' functors have the same
    // value if they have the same 'id'.

template <class TYPE>
bool operator!=(const TestHashFunctor<TYPE>& lhs,
                const TestHashFunctor<TYPE>& rhs);
    // Return 'true' if the specificed 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'TestHashFunctor' functors do not
    // have the same value if they do not have the same 'id'.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ========================
                       // class TestFacilityHasher
                       // ========================

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestFacilityHasher : private HASHER { // exploit empty base
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.  The function-call operator is implemented by calling the wrapper
    // functor, 'HASHER', with integers converted from objects of 'KEY' by the
    // class method 'TemplateTestFacility::getIdentifier'.    Note that this
    // class privately inherits from the specified 'HASHER' class in order to
    // exploit any compiler optimizations for empty base classes.

  public:
    TestFacilityHasher(const HASHER& hash = HASHER());              // IMPLICIT

    // ACCESSORS
    native_std::size_t operator() (const KEY& k) const;
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ================================
                       // class TestConvertibleValueHasher
                       // ================================

template <class KEY, class HASHER = ::bsl::hash<int> >
class TestConvertibleValueHasher : private TestFacilityHasher<KEY, HASHER> {
    // TBD This test class provides...

    typedef TestFacilityHasher<KEY, HASHER> Base;

  public:
    TestConvertibleValueHasher(const HASHER& hash = HASHER());      // IMPLICIT

    // ACCESSORS
    native_std::size_t operator()(const bsltf::ConvertibleValueWrapper<KEY>& k) const;
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
};

                       // ====================================
                       // class TestConvertibleValueComparator
                       // ====================================

template <class KEY>
class TestConvertibleValueComparator {
    // This test class provides...

  public:
    // ACCESSORS
      bsltf::EvilBooleanType operator() (
                           const bsltf::ConvertibleValueWrapper<KEY>& a,
                           const bsltf::ConvertibleValueWrapper<KEY>& b) const;
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =======================
                       // class GenericComparator
                       // =======================

class GenericComparator {
  public:
    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bsltf::EvilBooleanType operator() (const ARG1_TYPE& arg1,
                                       const ARG2_TYPE& arg2) const;
        // Return 'true' if 'arg1' has the same value as 'arg2', for some
        // unspecified definition that defaults to 'operator==', but may use
        // some other functionality.
};

                       // ===================
                       // class GenericHasher
                       // ===================

class GenericHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.  The function-call operator is implemented by calling the wrapper
    // functor, 'HASHER', with integers converted from objects of 'KEY' by the
    // class method 'TemplateTestFacility::getIdentifier'.

  public:
    // ACCESSORS
    template <class KEY>
    native_std::size_t operator() (const KEY& k) const;
        // Return a hash code for the specified 'k' using the wrapped 'HASHER'.
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===========================
                       // class DefaultOnlyComparator
                       // ===========================

template <class TYPE>
class DefaultOnlyComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  Hoever
    // it is a perverse type intended for testing templates against their
    // minimum requirements, and as such is neither copyable nor swappable, and
    // is only default constructible.  The function call operator will return
    // a perverse type that is convertible-to-bool.

  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator() (const TYPE& lhs, const TYPE& rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' have the same value.
    {
        return BSL_TF_EQ(lhs, rhs);
    }
};

                       // =======================
                       // class DefaultOnlyHasher
                       // =======================

template <class TYPE>
class DefaultOnlyHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that returns the same hash for 'TYPE' values using the standard
    // 'bsl::hash' functor.  However, this class is a perverse type intended
    // for testing templates against their minimum requirements, and as such is
    // neither copyable nor swappable, and is only default constructible.

  private:
    DefaultOnlyHasher(const DefaultOnlyHasher&); // = delete
        // not implemented

    void operator=(const DefaultOnlyHasher&); // = delete;
        // not implemented

    void operator&();  // = delete;
        // not implemented

    template<class T>
    void operator,(const T&); // = delete;
        // not implemented

    template<class T>
    void operator,(T&); // = delete;
        // not implemented

  public:
    DefaultOnlyHasher() {}

    // ACCESSORS
    size_t operator() (const TYPE& value)
        // Return the hash of the specified 'value'.
    {
        return bsl::hash<TYPE>().operator()(value);
    }
};


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =============================
                       // class FunctionPointerPolicies
                       // =============================

template <class KEY>
struct FunctionPointerPolicies {
    typedef size_t HashFunction(const KEY&);
    typedef bool   ComparisonFunction(const KEY&, const KEY&);

    static size_t hash(const KEY& k);

    static bool compare(const KEY& lhs, const KEY& rhs);
};

                       // ========================
                       // class MakeDefaultFunctor
                       // ========================

template <class FUNCTOR>
struct MakeDefaultFunctor {
    static FUNCTOR make();
};

template <class FUNCTOR, bool ENABLE_SWAP>
struct MakeDefaultFunctor<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> > {
    static bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> make();
};

template <class KEY>
struct MakeDefaultFunctor<size_t (*)(const KEY&)> {
    typedef size_t FunctionType(const KEY&);

    static FunctionType *make();
};

template <class KEY>
struct MakeDefaultFunctor<bool (*)(const KEY&, const KEY&)> {
    typedef bool FunctionType(const KEY&, const KEY&);

    static FunctionType *make();
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===================
                       // class MakeAllocator
                       // ===================

template <class ALLOCATOR>
struct MakeAllocator {
    // TBD This utility class template ...

    // PUBLIC TYPES
    typedef ALLOCATOR AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *);
};

template <class TYPE>
struct MakeAllocator<bsl::allocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsl::allocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
};

template <class TYPE>
struct MakeAllocator<bsltf::StdTestAllocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdTestAllocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
};

template <class TYPE, bool A, bool B, bool C, bool D>
struct MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdStatefulAllocator<TYPE, A, B, C, D> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
};

                       // =================
                       // class ObjectMaker
                       // =================

// The 'ObjectMaker' template and its associated specializations customize the
// act of creating a object-under-test, in-place, using an allocator configured
// according to some plan.  Generally, the plans are spelled out 'a' -> 'z',
// with each letter corresponding to a specific way of passing an allocator
// to the test object's constructor.  In practice, we currently define only the
// configurations 'a' -> 'd', and they are called sequentially.  As we become
// more thorough in testing, additional configurations will present themself,
// and specific tests will want different subsets of the available range of
// configurations.  It is likely we will have functions that return a string
// literal describing the recommended range of configurations to test, for a
// given (named) plan, for a given specialization - as not all allocators will
// support all configurations.  Rather than mindlessly testing each option for
// each set of types for each test, it would make sense to offer a simplified
// set of configurations for the more restricted allocators.
template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
struct ObjectMaker {
    // TBD This utility class template ...

    typedef          ALLOCATOR             AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<ALLOCATOR>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>   Obj;

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER           hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
struct ObjectMaker<KEY_CONFIG,
                   HASHER,
                   COMPARATOR,
                   bsl::allocator<typename KEY_CONFIG::ValueType> > {
    // TBD This utility class template specialization...

    typedef bsl::allocator<typename KEY_CONFIG::ValueType> AllocatorType;
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER           hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..
};

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
struct ObjectMaker<
                KEY_CONFIG,
                HASHER,
                COMPARATOR,
                bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                             A, B, C, D> > {
    // TBD This utility class template specialization...

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    typedef bsltf::StdStatefulAllocator<ValueType, A, B, C, D>   AllocatorType;
    typedef typename bsl::allocator_traits<AllocatorType>::size_type  SizeType;

    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, AllocatorType>
                                                                           Obj;

    static const char * specForBootstrapTests() { return "ab"; }
    static const char * specForCopyTests()      { return "ab"; }
    static const char * specForDefaultTests()   { return "ab"; }

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER           hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
    // construct a 'HashTable' object at the specified 'obj' address using the
    // allocator determined by the specified 'config', and passing the
    // specified 'hash', 'compare', 'initialBuckets' and 'initialMaxLoadFactor'
    // to the constructor.  Return an allocator object that will compare equal
    // to the allocator that is expected to be used to construct the
    // 'HashTable' object.  The specified 'objectAllocator' may, or may not, be
    // used to construct the 'HashTable' object according to the specified
    // 'config':
    //..
    //  config   allocator
    //  'a'      use the specified 'objectAllocator'
    //  'b'      use the default supplied by the constructor
    //  'c'      explicitly pass a null pointer of type 'bslma::Allocator *'
    //  'd'      explicitly pass the default allocator
    //..
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//                         test support functions

template <class ALLOCATOR>
bslma::TestAllocator *extractTestAllocator(ALLOCATOR&);
    // Return a null pointer value, as there is no way to extract a test
    // allocator from an unknown allocator type.  This function should be
    // overloaded for allocator types for which it is possible to extract a
    // test allocator.

template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsl::allocator<TYPE>& alloc);
    // Return the address of the allocator 'mechanism' wrapped by the specified
    // 'alloc' if it is a test allocator (which can be found by 'dynamic_cast')
    // and a null pointer value otherwise.

template <class TYPE>
bslma::TestAllocator *extractTestAllocator(bsltf::StdTestAllocator<TYPE>&);
    // Return the address of the installed StdTestAllocator if it is a test
    // allocator, and a null pointer value otherwise.  Note that all
    // 'bsltf::StdTestAllocator's share the same allocator, which is set by
    // the 'bsltf::StdTestAllocatorConfiguration' utility.  We can determine if
    // this is wrapping a test allocator using 'dynamic_cast'.

template <class TYPE, bool A, bool B, bool C, bool D>
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc);
    // Return the address of the test allocator wrapped by the specified
    // 'alloc'.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//       test support functions dealing with hash and comparator functors

void setHasherState(bsl::hash<int> *hasher, int id);
void setHasherState(TestHashFunctor<int> *hasher, int id);

bool isEqualHasher(const bsl::hash<int>&, const bsl::hash<int>&);
    // Provide an overloaded function to compare hash functors.  Return 'true'
    // because 'bsl::hash' is stateless.

bool isEqualHasher(const TestHashFunctor<int>& lhs,
                   const TestHashFunctor<int>& rhs);
    // Provide an overloaded function to compare hash functors.  Return
    // 'lhs == rhs'.

template <class KEY>
void setComparatorState(bsl::equal_to<KEY> *comparator, int id);

template <class KEY>
void setComparatorState(TestEqualityComparator<KEY> *comparator, int id);


template <class KEY>
bool isEqualComparator(const bsl::equal_to<KEY>&, const bsl::equal_to<KEY>&);
    // Provide an overloaded function to compare comparators.  Return 'true'
    // because 'bsl::equal_to' is stateless.

template <class KEY>
bool isEqualComparator(const TestEqualityComparator<KEY>& lhs,
                       const TestEqualityComparator<KEY>& rhs);
    // Provide an overloaded function to compare comparators.  Return
    // 'lhs == rhs'.

}  // close unnamed namespace

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace bsl
{
                    // --------------------------------------------
                    // class template bsl::equal_to specializations
                    // --------------------------------------------

inline
bool equal_to< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
                    (const ::BloombergLP::bsltf::NonEqualComparableTestType& a,
                     const ::BloombergLP::bsltf::NonEqualComparableTestType& b)
                                                                          const
{
    return BSL_TF_EQ(a, b);
}

                    // ----------------------------------------
                    // class template bsl::hash specializations
                    // ----------------------------------------

// not inlining initially due to static local data
size_t hash< ::BloombergLP::bsltf::NonEqualComparableTestType>::operator()
          (const ::BloombergLP::bsltf::NonEqualComparableTestType& value) const
{
    static const bsl::hash<int> EVALUATE_HASH = bsl::hash<int>();
    return EVALUATE_HASH(value.data());
}

}  // close namespace bsl

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes
{
                       // ---------------------------
                       // class AwkwardMaplikeElement
                       // ---------------------------

// CREATORS

inline
AwkwardMaplikeElement::AwkwardMaplikeElement()
: d_data()
{
}

inline
AwkwardMaplikeElement::AwkwardMaplikeElement(int value)
: d_data(value)
{
}

inline
AwkwardMaplikeElement::AwkwardMaplikeElement(
                                const bsltf::NonEqualComparableTestType& value)
: d_data(value)
{
}

// MANIPULATORS
inline
void AwkwardMaplikeElement::setData(int value)
{
    d_data.setData(value);
}

// ACCESSORS
inline
int AwkwardMaplikeElement::data() const
{
    return d_data.data();
}

inline
const bsltf::NonEqualComparableTestType& AwkwardMaplikeElement::key() const
{
    return d_data;
}

}  // close namespace TestTypes

inline
bool TestTypes::operator==(const AwkwardMaplikeElement& lhs,
                           const AwkwardMaplikeElement& rhs)
{
    return BSL_TF_EQ(lhs.data(), rhs.data());
}

inline
bool TestTypes::operator!=(const AwkwardMaplikeElement& lhs,
                           const AwkwardMaplikeElement& rhs)
{
    return !(lhs == rhs);
}

inline
void TestTypes::debugprint(const AwkwardMaplikeElement& value)
{
    bsls::debugprint(value.data());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace BloombergLP
{

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void bslstl::debugprint(
         const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t)
{
    if (0 == t.size()) {
        printf("<empty>");
    }
    else {
        for (Link *it = t.elementListRoot(); it; it = it->nextLink()) {
            const typename KEY_CONFIG::KeyType& key =
                                           ImpUtil::extractKey<KEY_CONFIG>(it);
            bsls::BslTestUtil::callDebugprint(static_cast<char>(
                             bsltf::TemplateTestFacility::getIdentifier(key)));
        }
    }
    fflush(stdout);
}

namespace bsltf {

template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                               const TestTypes::AwkwardMaplikeElement& object)
{
    return object.data();
}

}  // close namespace BloombergLP::bsltf
}  // close namespace BloombergLP

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace
{
                       // -------------------------------
                       // class GroupedEqualityComparator
                       // -------------------------------

// ACCESSORS
template <class TYPE, int GROUP_SIZE>
bool GroupedEqualityComparator<TYPE, GROUP_SIZE>::
operator() (const TYPE& lhs, const TYPE& rhs) const
{
    int leftValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
                  / GROUP_SIZE;
    int rightValue = bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs)
                   / GROUP_SIZE;

    return leftValue == rightValue;
}

                       // -------------------
                       // class GroupedHasher
                       // -------------------

// ACCESSORS
template <class TYPE, class HASHER, int GROUP_SIZE>
inline
size_t GroupedHasher<TYPE, HASHER, GROUP_SIZE>::operator() (const TYPE& value)
{
    int groupNum = bsltf::TemplateTestFacility::getIdentifier<TYPE>(value)
                 / GROUP_SIZE;

    return HASHER::operator()(groupNum);
}


                       // ----------------------------
                       // class TestEqualityComparator
                       // ----------------------------

// CREATORS
template <class TYPE>
inline
TestEqualityComparator<TYPE>::TestEqualityComparator(int id)
: d_id(id)
, d_count(0)
{
}

// MANIPULATORS
template <class TYPE>
inline
void TestEqualityComparator<TYPE>::setId(int value)
{
    d_id = value;
}

// ACCESSORS
template <class TYPE>
inline
bool TestEqualityComparator<TYPE>::operator() (const TYPE& lhs,
                                               const TYPE& rhs) const
{
    ++d_count;

    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
        == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
}

template <class TYPE>
inline
int TestEqualityComparator<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t TestEqualityComparator<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
bool operator==(const TestEqualityComparator<TYPE>& lhs,
                const TestEqualityComparator<TYPE>& rhs)
{
    return lhs.id() == rhs.id();
}

template <class TYPE>
inline
bool operator!=(const TestEqualityComparator<TYPE>& lhs,
                const TestEqualityComparator<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // ---------------------
                       // class TestHashFunctor
                       // ---------------------

// CREATORS
template <class TYPE>
inline
TestHashFunctor<TYPE>::TestHashFunctor(int id)
: d_id(id)
, d_count(0)
{
}

// MANIPULATORS
template <class TYPE>
inline
void TestHashFunctor<TYPE>::setId(int value)
{
    d_id = value;
}

// ACCESSORS
template <class TYPE>
inline
native_std::size_t TestHashFunctor<TYPE>::operator() (const TYPE& obj) const
{
    ++d_count;

    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj) + d_id;
}

template <class TYPE>
inline
int TestHashFunctor<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t TestHashFunctor<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
bool operator==(const TestHashFunctor<TYPE>& lhs,
                const TestHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

template <class TYPE>
inline
bool operator!=(const TestHashFunctor<TYPE>& lhs,
                const TestHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // ------------------------
                       // class TestFacilityHasher
                       // ------------------------

// CREATORS
template <class KEY, class HASHER>
inline
TestFacilityHasher<KEY, HASHER>::TestFacilityHasher(const HASHER& hash)
: HASHER(hash)
{
}

    // ACCESSORS
template <class KEY, class HASHER>
inline
native_std::size_t
TestFacilityHasher<KEY, HASHER>::operator() (const KEY& k) const
{
    return HASHER::operator()(
                           bsltf::TemplateTestFacility::getIdentifier<KEY>(k));
}

                       // --------------------------------
                       // class TestConvertibleValueHasher
                       // --------------------------------

template <class KEY, class HASHER>
inline
TestConvertibleValueHasher<KEY, HASHER>::TestConvertibleValueHasher(
                                                            const HASHER& hash)
: HASHER(hash)
{
}

// ACCESSORS
template <class KEY, class HASHER>
inline
native_std::size_t
TestConvertibleValueHasher<KEY, HASHER>::operator()
                           (const bsltf::ConvertibleValueWrapper<KEY>& k) const
{
    return Base::operator()(k);
}

                       // ------------------------------------
                       // class TestConvertibleValueComparator
                       // ------------------------------------

// ACCESSORS
template <class KEY>
inline
bsltf::EvilBooleanType TestConvertibleValueComparator<KEY>::operator() (
                            const bsltf::ConvertibleValueWrapper<KEY>& a,
                            const bsltf::ConvertibleValueWrapper<KEY>& b) const
{
    return BSL_TF_EQ(a, b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------
                       // class GenericComparator
                       // -----------------------

// ACCESSORS
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bsltf::EvilBooleanType GenericComparator::operator() (
                                                   const ARG1_TYPE& arg1,
                                                   const ARG2_TYPE& arg2) const
{
    return BSL_TF_EQ(arg1, arg2);
}

                       // -------------------
                       // class GenericHasher
                       // -------------------

// ACCESSORS
template <class KEY>
native_std::size_t GenericHasher::operator() (const KEY& k) const
{
    // do not inline initially due to static local data

    static const TestFacilityHasher<KEY> HASHER;
    return HASHER(k);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------------
                       // class FunctionPointerPolicies
                       // -----------------------------

// do not inline initially due to static local data
template <class KEY>
size_t FunctionPointerPolicies<KEY>::hash(const KEY& k)
{
    static const TestFacilityHasher<KEY> s_hasher = TestFacilityHasher<KEY>();
    return s_hasher(k);
}

template <class KEY>
inline
bool FunctionPointerPolicies<KEY>::compare(const KEY& lhs, const KEY& rhs)
{
    return BSL_TF_EQ(lhs, rhs);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ------------------------
                       // class MakeDefaultFunctor
                       // ------------------------

template <class FUNCTOR>
inline
FUNCTOR MakeDefaultFunctor<FUNCTOR>::make()
{
    return FUNCTOR();
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP>
MakeDefaultFunctor<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> >::make()
{
    return bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::
                          cloneBaseObject(MakeDefaultFunctor<FUNCTOR>::make());
}

template <class KEY>
inline
typename MakeDefaultFunctor<size_t (*)(const KEY&)>::FunctionType *
MakeDefaultFunctor<size_t (*)(const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::hash;
}

template <class KEY>
inline
typename MakeDefaultFunctor<bool (*)(const KEY&, const KEY&)>::FunctionType *
MakeDefaultFunctor<bool (*)(const KEY&, const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::compare;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -------------------
                       // class MakeAllocator
                       // -------------------

template <class ALLOCATOR>
typename MakeAllocator<ALLOCATOR>::AllocatorType
MakeAllocator<ALLOCATOR>::make(bslma::Allocator *)
{
    return AllocatorType();
}

template <class TYPE>
typename MakeAllocator<bsl::allocator<TYPE> >::AllocatorType
MakeAllocator<bsl::allocator<TYPE> >::make(bslma::Allocator *basicAllocator)
{
    return AllocatorType(basicAllocator);
}

template <class TYPE>
typename MakeAllocator<bsltf::StdTestAllocator<TYPE> >::AllocatorType
MakeAllocator<bsltf::StdTestAllocator<TYPE> >::
make(bslma::Allocator *basicAllocator)
{
    // This method is a little bit of overkill (heavy on the assertions) as
    // a left-over from when we were trying hard to nail down a tricky bug
    // that manifest only on the IBM AIX compiler.  It should probably be
    // cleaned up for final release.

    typedef bsltf::StdTestAllocatorConfiguration BsltfAllocConfig;

    bslma::Allocator *installedAlloc = BsltfAllocConfig::delegateAllocator();

    bslma::TestAllocator *currentAllocator
                        = dynamic_cast<bslma::TestAllocator *>(installedAlloc);

    bslma::TestAllocator *newAllocator
                        = dynamic_cast<bslma::TestAllocator *>(basicAllocator);

    ASSERTV(g_bsltfAllocator_p, installedAlloc, currentAllocator, newAllocator,
            g_bsltfAllocator_p && installedAlloc &&
            currentAllocator   && newAllocator);

    ASSERTV(currentAllocator,   newAllocator,
            currentAllocator->name(), newAllocator->name(),
            currentAllocator == newAllocator);

    return AllocatorType();
}

template <class TYPE, bool A, bool B, bool C, bool D>
typename
MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> >::AllocatorType
MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> >::
make(bslma::Allocator *basicAllocator)
{
    bslma::TestAllocator *alloc = dynamic_cast<bslma::TestAllocator *>(
                                                               basicAllocator);
    ASSERT(alloc);
    return AllocatorType(alloc);
}

                       // -----------------
                       // class ObjectMaker
                       // -----------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
ALLOCATOR
ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    switch (config) {
      case 'a': {
          ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(objectAllocator);
          *objPtr = new (*fa) Obj(objAlloc);
          return objAlloc;                                            // RETURN
      } break;
      case 'b': {
          *objPtr = new (*fa) Obj();
          return ALLOCATOR();                                         // RETURN
      } break;
      case 'c': {
          ALLOCATOR result = ALLOCATOR();
          *objPtr = new (*fa) Obj(result);
          return result;                                              // RETURN
      } break;
      case 'd': {
          ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
          *objPtr = new (*fa) Obj(objAlloc);
          return objAlloc;                                            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLS_PLATFORM_CMP_MSVC)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLS_PLATFORM_CMP_IBM)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
ALLOCATOR
ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator,
           const HASHER           hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    switch (config) {
      case 'a': {
          ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(objectAllocator);
          *objPtr = new (*fa) Obj(hash,
                                 compare,
                                 initialBuckets,
                                 initialMaxLoadFactor,
                                 objAlloc);
          return objAlloc;                                            // RETURN
      } break;
      case 'b': {
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor);
          return ALLOCATOR();                                         // RETURN
      } break;
      case 'c': {
          ALLOCATOR result = ALLOCATOR();
          *objPtr = new (*fa) Obj(hash,
                                 compare,
                                 initialBuckets,
                                 initialMaxLoadFactor,
                                 result);
          return result;                                              // RETURN
      } break;
      case 'd': {
          ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor,
                                  objAlloc);
          return objAlloc;                                            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLS_PLATFORM_CMP_MSVC)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLS_PLATFORM_CMP_IBM)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    switch (config) {
      case 'a': {
          *objPtr = new (*fa) Obj(objectAllocator);
          return objectAllocator;                                     // RETURN
      } break;
      case 'b': {
          *objPtr = new (*fa) Obj();
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
      case 'c': {
          *objPtr = new (*fa) Obj((bslma::Allocator *)0);
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
      case 'd': {
          *objPtr = new (*fa) Obj(bslma::Default::defaultAllocator());
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLS_PLATFORM_CMP_MSVC)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLS_PLATFORM_CMP_IBM)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsl::allocator<typename KEY_CONFIG::ValueType> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator,
           const HASHER           hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    switch (config) {
      case 'a': {
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor,
                                  objectAllocator);
          return objectAllocator;                                     // RETURN
      } break;
      case 'b': {
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor);
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
      case 'c': {
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor,
                                  (bslma::Allocator *)0);
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
      case 'd': {
          *objPtr = new (*fa) Obj(hash,
                                  compare,
                                  initialBuckets,
                                  initialMaxLoadFactor,
                                  bslma::Default::defaultAllocator());
          return AllocatorType(bslma::Default::allocator());          // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLS_PLATFORM_CMP_MSVC)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLS_PLATFORM_CMP_IBM)
    throw 0; // This will never be reached, but satisfied compiler warnings.
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                   A, B, C, D> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                                A, B, C, D> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator)
{
    // bsltf::StdStatefulAllocator objects are not DefaultConstructible.
    // We know that the default allocator installed for this test driver will
    // be a test allocator, so we can safely expect a 'dynamic_cast' to not
    // return a null pointer.  Likewise, the 'objectAllocator' should not be a
    // null pointer either, so we can simply construct the desired allocator
    // object using the test allocator specified by the 'config' parameter, and
    // use that to explicitly construct the desired 'HashTable' object into
    // '*objPtr'.  Note that there is no distinction between config 'a' or 'b'
    // for this allocator, it would be useful if we could find some way to skip
    // config 'a' when running the various test cases.

    bslma::TestAllocator *alloc = 'a' == config
                                ? objectAllocator
                                : dynamic_cast<bslma::TestAllocator *>(
                                           bslma::Default::defaultAllocator());
    ASSERT(alloc);

    AllocatorType result = MakeAllocator<AllocatorType>::make(alloc);
    *objPtr = new (*fa) Obj(result);
    return result;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR,
          bool A, bool B, bool C, bool D>
typename
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                   A, B, C, D> >::AllocatorType
ObjectMaker<KEY_CONFIG,
            HASHER,
            COMPARATOR,
            bsltf::StdStatefulAllocator<typename KEY_CONFIG::ValueType,
                                                                A, B, C, D> >::
makeObject(Obj                  **objPtr,
           char                   config,
           bslma::Allocator      *fa,  // "footprint" allocator
           bslma::TestAllocator  *objectAllocator,
           const HASHER           hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    // bsltf::StdStatefulAllocator objects are not DefaultConstructible.
    // We know that the default allocator installed for this test driver will
    // be a test allocator, so we can safely expect a 'dynamic_cast' to not
    // return a null pointer.  Likewise, the 'objectAllocator' should not be a
    // null pointer either, so we can simply construct the desired allocator
    // object using the test allocator specified by the 'config' parameter, and
    // use that to explicitly construct the desired 'HashTable' object into
    // '*objPtr'.  Note that there is no distinction between config 'a' or 'b'
    // for this allocator, it would be useful if we could find some way to skip
    // config 'a' when running the various test cases.

    bslma::TestAllocator *alloc = 'a' == config
                                ? objectAllocator
                                : dynamic_cast<bslma::TestAllocator *>(
                                           bslma::Default::defaultAllocator());
    ASSERT(alloc);

    AllocatorType result = MakeAllocator<AllocatorType>::make(alloc);
    *objPtr = new (*fa) Obj(hash,
                            compare,
                            initialBuckets,
                            initialMaxLoadFactor,
                            result);
    return result;
}


                       // ---------------
                       // class BoolArray
                       // ---------------

// CREATORS
inline
BoolArray::BoolArray(size_t n)
: d_data(new bool[n])
, d_size(n)
{
    for (size_t i = 0; i != n; ++i) {
        d_data[i] = false;
    }
}

inline
BoolArray::~BoolArray()
{
    delete[] d_data;
}

inline
bool& BoolArray::operator[](size_t index)
{
    BSLS_ASSERT_SAFE(index < d_size);

    return d_data[index];
}

inline
bool BoolArray::operator[](size_t index) const
{
    BSLS_ASSERT_SAFE(index < d_size);

    return d_data[index];
}

inline
size_t BoolArray::size() const
{
    return d_size;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//                         test support functions

template <class ALLOCATOR>
inline
bslma::TestAllocator *
extractTestAllocator(ALLOCATOR&)
{
    // In general, there is no way to extract a test allocator from an unknown
    // allocator type.

    return 0;
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsl::allocator<TYPE>& alloc)
{
    // If a BDE 'bsl::allocator' is wrapping a test allocator, it can be found
    // by 'dynamic_cast'ing the underlying 'mechanism'.

    return dynamic_cast<bslma::TestAllocator *>(alloc.mechanism());
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsltf::StdTestAllocator<TYPE>&)
{
    // All 'bsltf::StdTestAllocator's share the same allocator, which is set by
    // the 'bsltf::StdTestAllocatorConfiguration' utility.  We can determine if
    // this is wrapping a test allocator using 'dynamic_cast'.

    return dynamic_cast<bslma::TestAllocator *>(
                    bsltf::StdTestAllocatorConfiguration::delegateAllocator());
}

template <class TYPE, bool A, bool B, bool C, bool D>
inline
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc)
{
    return alloc.testAllocator();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline
void setHasherState(bsl::hash<int> *hasher, int id)
{
    (void) hasher;
    (void) id;
}

inline
bool isEqualHasher(const bsl::hash<int>&, const bsl::hash<int>&)
{
    return true;
}

inline
void setHasherState(TestHashFunctor<int> *hasher, int id)
{
    hasher->setId(id);
}

inline
bool isEqualHasher(const TestHashFunctor<int>& lhs,
                   const TestHashFunctor<int>& rhs)
{
    return lhs == rhs;
}

template <class KEY>
inline
void setComparatorState(bsl::equal_to<KEY> *comparator, int id)
{
    (void) comparator;
    (void) id;
}

template <class KEY>
inline
void setComparatorState(TestEqualityComparator<KEY> *comparator, int id)
{
    comparator->setId(id);
}


template <class KEY>
inline
bool isEqualComparator(const bsl::equal_to<KEY>&, const bsl::equal_to<KEY>&)
{
    return true;
}

template <class KEY>
inline
bool isEqualComparator(const TestEqualityComparator<KEY>& lhs,
                       const TestEqualityComparator<KEY>& rhs)
{
    return lhs == rhs;
}

}  // close unnamed namespace

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace
{

bool expectPoolToAllocate(int n)
    // Return 'true' if the memory pool used by the container under test is
    // expected to allocate memory on the inserting the specified 'n'th
    // element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

size_t predictNumBuckets(size_t length, float maxLoadFactor)
    // Return the minimum number of buckets necessary to support the specified
    // 'length' array of elements in a 'HashTable' having the specified
    // 'maxLoadFactor' without rehashing.  Note that typically the result will
    // be passed to a 'HashTable' constructor or reserve call, which may in
    // turn choose to create even more buckets to preserve its growth strategy.
    // This function does not attempt to predict that growth strategy, but
    // merely predict the minimum number of buckets that strategy must
    // accommodate.
{
    return static_cast<size_t>(ceil(static_cast<double>(length) /
                                                        maxLoadFactor));
}

template<class KEY_CONFIG, class COMPARATOR, class VALUES>
int verifyListContents(Link              *containerList,
                       const COMPARATOR&  compareKeys,
                       const VALUES&      expectedValues,
                       size_t             expectedSize)
    // NOTE: THIS TEST IS EXPENSIVE, WITH QUADRATIC COMPLEXITY ON LIST LENGTH
    //                       DO NOT CALL IN A TIGHT LOOP
    // Verify the specified 'containerList' has the specified 'expectedSize'
    // number of elements, and contains the same values as the array in the
    // specified 'expectedValues', and that the elements in the list are
    // arranged so that elements whose keys compare equal using the specified
    // 'compareKeys' predicate are all arranged contiguously within the list.
    // Return 0 if 'container' has the expected values, and a non-zero value
    // otherwise.
{
    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;

    // Check to avoid creating an array of length zero.
    if (0 == containerList) {
        ASSERTV(0 == expectedSize);
        return 0;                                                     // RETURN
    }

    BoolArray foundValues(expectedSize);
    size_t i = 0;
    for (Link *cursor = containerList;
         cursor;
         cursor = cursor->nextLink(), ++i)
    {
        const ValueType& element = ImpUtil::extractValue<KEY_CONFIG>(cursor);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);
        size_t j = 0;
        do {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                ASSERTV(j, expectedValues[j], element, !foundValues[j]);
                foundValues[j] = true;
                break;
            }
        }
        while (++j != expectedSize);
    }
    ASSERTV(expectedSize, i, expectedSize == i);
    if (expectedSize != i) {
        return -2;                                                    // RETURN
    }

    int missing = 0;
    for (size_t j = 0; j != expectedSize; ++j) {
        if (!foundValues[j]) {
            // Check to avoid spurious overflow warnings - we will never have
            // so many elements we need to worry about this.
            if (++missing == INT_MAX) {
                return missing;                                       // RETURN
            }
        }
    }

    if (missing > 0) {
        return missing;                                               // RETURN
    }

    // All elements are present, check the contiguity requirement
    // Note that this test is quadratic in the length of the list, although we
    // will optimize for the case of duplicates actually occurring.
    for (Link *cursor = containerList; cursor; cursor = cursor->nextLink()) {
        const KeyType& key = ImpUtil::extractKey<KEY_CONFIG>(cursor);

        Link *next = cursor->nextLink();
        // Walk to end of key-equivalent sequence
        while (next &&
               compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            cursor = next;
            next   = next->nextLink();
        }

        // Check there are no more equivalent keys in the list.
        // Note that this test also serves to check there are no duplicates in
        // the preceding part of the list, as this check would have failed
        // earlier if that were the case.
        while (next &&
               !compareKeys(key, ImpUtil::extractKey<KEY_CONFIG>(next))) {
            next = next->nextLink();
        }

        if (0 != next) {
            return -3; // code for discontiguous list                 // RETURN
        }
    }

    return 0;  // 0 indicates a successful test!
}

template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
                  bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
                  const typename KEY_CONFIG::ValueType&              value)
    // Insert an element into the specified 'hashTable' and return the address
    // of the new node, unless the insertion would cause the hash table to
    // exceed its 'maxLoadFactor' and rehash, in which case return a null
    // pointer value.
{
    BSLS_ASSERT(hashTable);

    // static_casts are necessary to avoid warnings with gcc.  We have
    // determined that within the scope of this test driver, these casts are
    // not discarding important information, even on 64-bit platforms.
    if (static_cast<double>(hashTable->size() + 1) >
                             static_cast<double>(hashTable->maxLoadFactor()) *
                             static_cast<double>(hashTable->numBuckets()) ) {
        return 0;                                                     // RETURN
    }
    return hashTable->insert(value);
}

template <class KEY_CONFIG, class HASHER>
bool isValidHashTable(bslalg::BidirectionalLink      *listRoot,
                      const bslalg::HashTableBucket&  arrayRoot,
                      native_std::size_t              arrayLength)
{
    // We perform the const-cast inside this function as we know:
    // i/  The function we call does not make any writes to the bucket array.
    // ii/ It is much simpler to cast in this one place than in each of our
    //     call sites.
    bslalg::HashTableAnchor anchor(
                             const_cast<bslalg::HashTableBucket *>(&arrayRoot),
                             arrayLength,
                             listRoot);
    return bslalg::HashTableImpUtil::isWellFormed<KEY_CONFIG, HASHER>(anchor);
}

}  // close unnamed namespace

// ============================================================================
//                         TEST DRIVER HARNESS
// ----------------------------------------------------------------------------

// - - - - - - Configuration policies to instantiate HashTable with - - - - - -

template <class ELEMENT>
struct BasicKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value;
    }
};


template <class ELEMENT>
struct BsltfConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef int     KeyType;
    typedef ELEMENT ValueType;

    static const int& extractKey(const ValueType& value)
    {
        // Note that this function MUST return a reference, but we have no
        // actual storage to return a reference to.  As we know the specific
        // usage patterns of this test driver, we can ensure that no two hash
        // computations happen while the first result is still held as a
        // reference.  Unfortunately, we have no such guarantee on simultaneous
        // evaluations in the HashTable facility itself, so we cycle through a
        // cache of 16 results, as no reference should be so long lived that we
        // see 16 live references.
        static int results_cache[16] = {};
        static int index = -1;

        if (16 == ++index) {
            index = 0;
        }

        results_cache[index] =
                             bsltf::TemplateTestFacility::getIdentifier(value);
        return results_cache[index];
    }
};

struct TrickyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that
    // can support a 'HashTable'.  It might be consistent with use as a 'set'
    // or a 'multiset' container.

    typedef bsltf::NonEqualComparableTestType KeyType;
    typedef TestTypes::AwkwardMaplikeElement  ValueType;

    static const KeyType& extractKey(const ValueType& value)
    {
        return value.key();
    }
};

// - - - - - Main test driver harness, that implements the test cases - - - - -

template <class KEY_CONFIG,
          class HASHER,
          class COMPARATOR,
          class ALLOCATOR>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'HashTable'
    // container.  The parameterized 'KEY_CONFIG', 'HASHER', 'COMPARATOR' and
    // 'ALLOCATOR' specifies the configuration, hasher type, comparator type
    // and allocator type respectively.  Each "testCase*" method tests a
    // specific aspect of
    // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.  Each test case
    // should be invoked with various parameterized type to fully test the
    // container.

  private:
    // TYPES
    typedef bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR> Obj;
        // Type under testing.

    typedef typename Obj::SizeType   SizeType;
    typedef typename Obj::KeyType    KeyType;
    typedef typename Obj::ValueType  ValueType;
        // Shorthands

    typedef bsltf::TestValuesArray<ValueType> TestValues;

    typedef ObjectMaker<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>    ObjMaker;

  private:
    // TEST APPARATUS
    //-------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) char
    // values to be appended to the 'map<KEY, VALUE, COMP, ALLOC>' object.
    //
    // LANGUAGE SPECIFICATION:
    // -----------------------
    //
    // <SPEC>       ::= <EMPTY>   | <LIST>
    //
    // <EMPTY>      ::=
    //
    // <LIST>       ::= <ITEM>    | <ITEM><LIST>
    //
    // <ITEM>       ::= <ELEMENT> | <CLEAR>
    //
    // <ELEMENT>    ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // ""           Has no effect; leaves the object empty.
    // "A"          Insert the value corresponding to A.
    // "AA"         Insert two values both corresponding to A.
    // "ABC"        Insert three values corresponding to A, B and C.
    //-------------------------------------------------------------------------

    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify a zero 'verbose' to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified object with its value adjusted
        // according to the specified 'spec'.

  public:
    // TEST CASES

    static void testCase14();

    static void testCase13();

    static void testCase12();

    static void testCase11();

    //        Test case 10 is not supported

    static void testCase9();

    static void testCase8();

    static void testCase7();

    static void testCase6();

    //        Test case 5 is not supported

    static void testCase4();

    static void testCase3();

    static void testCase2();

    //        Test case 1 is handled separately

};

// - - - - - Wrapper to forward test-class adapters to the main harness - - - -

template <class CONFIGURED_DRIVER>
struct TestDriver_ForwardTestCasesByConfiguation {

    // TEST CASES

    static void testCase26() { CONFIGURED_DRIVER::testCase26(); }

    static void testCase25() { CONFIGURED_DRIVER::testCase25(); }

    static void testCase24() { CONFIGURED_DRIVER::testCase24(); }

    static void testCase23() { CONFIGURED_DRIVER::testCase23(); }

    static void testCase22() { CONFIGURED_DRIVER::testCase22(); }

    static void testCase21() { CONFIGURED_DRIVER::testCase21(); }

    static void testCase20() { CONFIGURED_DRIVER::testCase20(); }

    static void testCase19() { CONFIGURED_DRIVER::testCase19(); }

    static void testCase18() { CONFIGURED_DRIVER::testCase18(); }

    static void testCase17() { CONFIGURED_DRIVER::testCase17(); }

    static void testCase16() { CONFIGURED_DRIVER::testCase16(); }

    static void testCase15() { CONFIGURED_DRIVER::testCase15(); }

    static void testCase14() { CONFIGURED_DRIVER::testCase14(); }

    static void testCase13() { CONFIGURED_DRIVER::testCase13(); }

    static void testCase12() { CONFIGURED_DRIVER::testCase12(); }

    static void testCase11() { CONFIGURED_DRIVER::testCase11(); }

    // there is no testCase10();

    static void testCase9() { CONFIGURED_DRIVER::testCase9(); }

    static void testCase8() { CONFIGURED_DRIVER::testCase8(); }

    static void testCase7() { CONFIGURED_DRIVER::testCase7(); }

    static void testCase6() { CONFIGURED_DRIVER::testCase6(); }

    // there is no testCase5();

    static void testCase4() { CONFIGURED_DRIVER::testCase4(); }

    static void testCase3() { CONFIGURED_DRIVER::testCase3(); }

    static void testCase2() { CONFIGURED_DRIVER::testCase2(); }

    // there is no testCase1();
};

// - - - - - - - - - - Pre-packaged test harness adapters - - - - - - - - - - -
// The template test facility, bsltf, requires class templates taking a single
// argument, that is the element type to vary in the test.  We desire a variety
// of configurations pushing the various policy parameters of the 'HashTable'
// class template, such as the type of functors, the key extraction policy,
// etc. so we write some simple adapters that will generate the appropriate
// instantiation of the test harness, from a template parameterized on only the
// element type (to be tested).  Note that in C++11 we would use the alias-
// template facility to define these templates, rather than using public
// inheritance through a dispatcher-shim.

template <class ELEMENT>
struct TestDriver_BasicConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_StatefulConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestHashFunctor<ELEMENT>
                     , TestEqualityComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT> >
                     , bsltf::DegenerateFunctor<TestEqualityComparator<ELEMENT> >
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguationWithNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT>, false>
                     , bsltf::DegenerateFunctor<TestEqualityComparator<ELEMENT>, false>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_BsltfConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BsltfConfig<ELEMENT>
                     , ::bsl::hash<int>
                     , ::bsl::equal_to<int>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Basic configuration to test a key-type different to the value-type.
    // This is a simple attempt to deliver something approximating a map with
    // 'int' as key, where the 'int' is computed for each element.  This is
    // the simplest way to generate a configuration compatible with the 'bsltf'
    // template testing framework used above, without rewriting each test case
    // to additional support for separate test tables of pairs to initialize a
    // more traditional-style map, with different math for computed values and
    // separate logic for duplicate elements and groups.
};

template <class ELEMENT>
struct TestDriver_FunctionPointers
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(*)(const ELEMENT&)
                     , bool(*)(const ELEMENT&,const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_ConvertibleValueConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestConvertibleValueHasher<ELEMENT>
                     , TestConvertibleValueComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_GenericFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_StdAllocatorConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdTestAllocator<ELEMENT>
                     >
       > {
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation1
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT>
                     >
       > {
    // Propagate all allocator operations.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation2
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT, false>
                     >
       > {
    // Propagate all allocator operations but copy construction.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation3
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT, true, false>
                     >
       > {
    // Propagate all allocator operations but swap.
};

struct TestDriver_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , TestFacilityHasher<TrickyConfig::KeyType>
                     , ::bsl::equal_to<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};

// - - - - - Special configurations for testing default constructor - - - - - -

template <class ELEMENT>
struct TestDriver_DefaultOnlyFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , DefaultOnlyHasher<ELEMENT>
                     , DefaultOnlyComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
};

struct TestDriver_AwkwardMaplikeForDefault
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , DefaultOnlyHasher<TrickyConfig::KeyType>
                     , DefaultOnlyComparator<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};

// - - - - - - - - - - - Special adapters for test case 6 - - - - - - - - - - -
// As initially written, test case 6 requires special handling with distinct
// functor classes to demonstrate and test the necessary key-equivalent groups.
// We hope to fold these cases in as regular test cases that pass through the
// remaining test cases as well, using the familiar pattern above.

template <class ELEMENT>
struct TestCase_GroupedUniqueKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values to the same hash code, but maintaining a unique key value for
    // each of those cases.  This should lead to a high rate of collisions.
};

template <class ELEMENT>
struct TestCase_GroupedSharedKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<ELEMENT, 5>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values to the same hash code, and similarly arranging for those keys to
    // compare equal to each other.  This should lead to behavior similar to a
    // multiset.
};

template <class ELEMENT>
struct TestCase6_DegenerateConfiguration
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver<
                   BasicKeyConfig<ELEMENT>,
                   bsltf::DegenerateFunctor<GroupedHasher<ELEMENT, bsl::hash<int>, 5> >,
                   bsltf::DegenerateFunctor<GroupedEqualityComparator<ELEMENT, 5> >,
                   ::bsl::allocator<ELEMENT> >
       > {
};

template <class ELEMENT>
struct TestCase6_DegenerateConfigurationNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver<
            BasicKeyConfig<ELEMENT>,
            bsltf::DegenerateFunctor<GroupedHasher<ELEMENT, bsl::hash<int>, 5>, false >,
            bsltf::DegenerateFunctor<GroupedEqualityComparator<ELEMENT, 5>, false >,
            ::bsl::allocator<ELEMENT> >
       > {
};

struct TestDriverForCase6_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , GroupedHasher<TrickyConfig::KeyType, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<TrickyConfig::KeyType, 5>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
int TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ggg(
                                                           Obj        *object,
                                                           const char *spec,
                                                           int         verbose)
{
#if !defined(AJM_HAS_FIXED_TESTARRAY_TO_NOT_USE_DEFAULT_ALLOCATOR)
    bslma::DefaultAllocatorGuard guard(
                                      &bslma::NewDeleteAllocator::singleton());
#endif
    bslma::TestAllocator tda("generated values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    enum { SUCCESS = -1 };

    for (int i = 0; spec[i]; ++i) {
        if ('A' <= spec[i] && spec[i] <= 'Z') {
            if (!insertElement(object, VALUES[spec[i] - 'A'])) {
                if (verbose) {
                    printf("Error, spec string ('%s') longer than the"
                           "'HashTable' can support without a rehash.\n",
                           spec);
                }

                // Discontinue processing this spec.

                return i;                                             // RETURN
            }
        }
        else {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }

            // Discontinue processing this spec.

            return i;                                                 // RETURN
        }
   }
   return SUCCESS;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::gg(Obj        *object,
                                                          const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING DEFAULT CONSTRUCTOR:
    //   The default constructor creates an empty container having one bucket.
    //   However, any attempt to insert is going to allocate a new bucket
    //   array, and any attempt to use the 'insertElement' function should fail
    //   as it protects against such use.  The state of a default constructed
    //   HashTable is that of a value-constructed HashTable requesting zero
    //   initial buckets, a maximum load factor of '1.0', and default values
    //   for the functors.  This state is extensively tested as a starting
    //   point for most other test cases.  However, the default constructor
    //   also allows for a different type of hasher/comparator functor that
    //   is DefaultConstructible, but is not CopyConstructible.  Such functors
    //   produce a HashTable that, in turn, is not CopyConstructible - but may
    //   be Swappable if the functor, in turn, are swappable.  It is important
    //   to test all the other methods of this class work with such a type,
    //   although we are not really adding anything new other than constructing
    //   into this state - therefore, the default constructor should be the
    //   last method tested, so that it can check every other method beging
    //   called for a type that is only default constructible, relying on their
    //   otherwise validated behavior from known states.
    //
    // Concerns:
    //: 1 An object created with the default constructor (with or without a
    //:   supplied allocator) has the contractually specified default value.
    //:
    //: 2 If an allocator is NOT supplied to the default constructor, the
    //:   default allocator in effect at the time of construction becomes the
    //:   object allocator for the resulting object.
    //:
    //: 3 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 4 Supplying a null allocator address has the same effect as not
    //:   supplying an allocator.
    //:
    //: 5 Supplying an allocator to the default constructor has no effect on
    //:   subsequent object values.
    //:
    //: 6 Any memory allocation is from the object allocator.
    //:
    //: 7 There is no temporary allocation from any allocator.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //:
    //: 9 QoI: The default constructor allocates no memory.
    //:
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, default-construct three distinct
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     7 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //
    // Testing:
    //   HashTable(const A& allocator);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nTesting with various allocator configurations.\n");

#if 0
    // Create an object that all default constructed 'HashTable' objects should
    // have the same value as.  Note that value does not depend on allocator or
    // the ordering functors, so we take the simplest form that we have already
    // tested back in test case 2.
#else
    // Change of plan, as we cannot assume that the two functor objects are
    // copy-constructible.l
#endif

    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const ALLOCATOR dfltAlloc = MakeAllocator<ALLOCATOR>::make(&da);
//    const Obj DEFAULT(HASHER(), COMPARATOR(), 0, 1.0, dfltAlloc);

    const char *ALLOC_SPEC = ObjMaker::specForDefaultTests();

    for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
        const char CONFIG = *cfg;  // how we specify the allocator

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        // ----------------------------------------------------------------

        if (veryVerbose) {
            printf("\n\tTesting default constructor.\n");
        }

        Obj       *objPtr;
        ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                   CONFIG,
                                                   &fa,
                                                   &sa);
        Obj& mX = *objPtr;  const Obj& X = mX;

        // Verify any attribute allocators are installed properly.

        ASSERTV(CONFIG, expAlloc == X.allocator());

        const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
        const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

        // It is important that these allocators are found, or else the
        // following tests will break severely, dereferencing null
        // pointer.

        BSLS_ASSERT_OPT(oa);
        BSLS_ASSERT_OPT(noa);

        // Confirm the expected state.

        ASSERTV(CONFIG, 0 == X.size());
        ASSERTV(CONFIG, 1 == X.numBuckets());
        ASSERTV(CONFIG, 0 == X.elementListRoot());
        ASSERTV(CONFIG, 1.0f == X.maxLoadFactor());
        ASSERTV(CONFIG, 0.0f == X.loadFactor());
        ASSERTV(CONFIG, 0 == X.countElementsInBucket(0));

        const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
        ASSERTV(CONFIG, 0 == bucket.first());
        ASSERTV(CONFIG, 0 == bucket.last());

//        ASSERTV(CONFIG, isEqualComparator(COMPARATOR(), X.comparator()));
//        ASSERTV(CONFIG, isEqualHasher(HASHER(), X.hasher()));

        // Add any additional fine-grained tests that might be interesting.


        // Verify no allocation from the object/non-object allocators.

        ASSERTV(CONFIG,  oa->numBlocksTotal(), 0 ==  oa->numBlocksTotal());
        ASSERTV(CONFIG, noa->numBlocksTotal(), 0 == noa->numBlocksTotal());

        // ----------------------------------------------------------------

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase13()
{
    // ------------------------------------------------------------------------
    // TESTING setMaxLoadFactor METHOD
    //
    // Concerns:
    //: 1 ...
    //:
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //*  setMaxLoadFactor
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting 'setMaxLoadFactor'.\n");
    }

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("Testing exteme values.\n");
    {

        Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);  const Obj& X = mX;

        mX.setMaxLoadFactor(std::numeric_limits<float>::max());
        ASSERT(std::numeric_limits<float>::max() == X.maxLoadFactor());

        mX.setMaxLoadFactor(std::numeric_limits<float>::infinity());
        ASSERT(std::numeric_limits<float>::infinity() == X.maxLoadFactor());
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose) printf("Testing exceptional trigger values.\n");
    {
        Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);
        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::min());
            ASSERT(!"setMaxLoadFactor(min) should throw a 'logic_error'");
        }
        catch (const std::logic_error &) {
            // This is the expected code path
        }
        catch (...) {
            ASSERT(!"rehash(max size_t) threw the wrong exception type");
        }

        try {
            mX.setMaxLoadFactor(std::numeric_limits<float>::denorm_min());
            ASSERT(!"setMaxLoadFactor(denorm) should throw a 'logic_error'");
        }
        catch (const std::logic_error &) {
            // This is the expected code path
        }
        catch (...) {
            ASSERT(!"rehash(max size_t) threw the wrong exception type");
        }
    }
#endif

    if (verbose) printf("Negative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(1.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(0.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(-1.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(
                                     std::numeric_limits<float>::quiet_NaN()));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(
                                     -std::numeric_limits<float>::infinity()));
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(
                                      std::numeric_limits<float>::infinity()));
    }

}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING 'insert' METHODS
    //
    // Concerns:
    //: 1 ...
    //:
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //   insert(const SOURCE_TYPE& obj);
    //   insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting is not yet implemented.\n");
    }


    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    const TestValues VALUES;  // contains 52 distinct increasing values


    // Probably want to pick these up as values from some injected policy, so
    // that we can test with stateful variants
    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

#if 1 // defined(THE_TEST_IS_YET_TO_COME)
    const size_t MAX_LENGTH = 9;

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        if (verbose) {
            printf("\nTesting with various allocator configurations.\n");
        }

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting bootstrap constructor.\n");
            }

            Obj                  *objPtr;

            //const size_t NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

//            ALLOCATOR objAlloc  = MakeAllocator<ALLOCATOR>::make(&sa);

            ALLOCATOR expAllocator = ObjMaker::makeObject( &objPtr
                                                         , CONFIG
                                                         , &fa
                                                         , &sa
//                                                         , objAlloc
                                                         , HASH
                                                         , COMPARE
                                                         , 0
                                                         , MAX_LF);

            Obj&                   mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAllocator == X.allocator());


            bslma::TestAllocator  *oa = extractTestAllocator(expAllocator);
            bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null
            // pointer.
            BSLS_ASSERT_OPT(oa);
            BSLS_ASSERT_OPT(noa);

            // QoI: Verify no allocation from the object/non-object allocators
            // if no buckets are requested (as per the default constructor).
            if (0 == LENGTH) {
                ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                        0 ==  oa->numBlocksTotal());
            }
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Record blocks used by the initial bucket array
            const bsls::Types::Int64 INITIAL_OA_BLOCKS  = oa->numBlocksTotal();


            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.
            // None of these accessors are deemed tested until their own test
            // case, but many witnesses give us some confidence in the state.
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                    INITIAL_OA_BLOCKS ==  oa->numBlocksTotal());
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting 'insert(element)'.\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa->numBlocksTotal(),   oa->numBlocksInUse(),
                        oa->numBlocksTotal() == oa->numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    // This will fail on the initial insert as we must also
                    // create the bucket array, so there is an extra pass.
                    // Not sure why that means the block counts get out of
                    // synch though, is this catching a real bug?
#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    if (VALUE_TYPE_USES_ALLOCATOR  ||
                                                expectPoolToAllocate(LENGTH)) {
                        ASSERTV(CONFIG, tam.isTotalUp());
                        ASSERTV(CONFIG, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, tam.isTotalSame());
                        ASSERTV(CONFIG, tam.isInUseSame());
                    }
#endif

                    // Verify no temporary memory is allocated from the object
                    // allocator.
                    // BROKEN TEST CONDITION
                    // We need to think carefully about how we allow for the
                    // allocation of the bucket-array

#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif

                    ASSERTV(CONFIG, LENGTH - 1,
                            BSL_TF_EQ(
                                    KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                    ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(CONFIG, LENGTH - 1,
                            BSL_TF_EQ(
                                   VALUES[LENGTH - 1],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPARE,
                                                           VALUES,
                                                           LENGTH));
                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0;j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    SizeType i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa->numBlocksTotal();

                mX.removeAll();

                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa->numBlocksTotal();

                ASSERTV(MAX_LF, LENGTH, CONFIG, BB == AA);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                "\n\tRepeat testing 'insert(element)', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (verbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH - 1 == X.size());
                if (veryVerbose) {
                    printf("\t\t\tBEFORE: ");
                    P(X);
                }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch);

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // The number of buckets should not have changed, so no
                    // reason to allocate a fresh bucket array
//                    ASSERTV(MAX_LF, LENGTH, CONFIG,
//                            bucketCount,   X.numBuckets(),
//                            bucketCount == X.numBuckets());

                    // These tests assume that the object allocator is used
                    // only if stored elements also allocate memory.  This
                    // does not allow for rehashes as the container grows.
                    // Hence we run the same test sequence a second time after
                    // clearing the container, so we can validate knowing that
                    // no rehashes should be necessary, and will in fact show
                    // up as a memory use error.  'LENGTH' was the high-water
                    // mark of the initial run on the container before removing
                    // all elements.
                    if ((LENGTH < X.size() && expectPoolToAllocate(LENGTH))
                        || VALUE_TYPE_USES_ALLOCATOR) {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalUp());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseUp());
                    }
                    else {
                        ASSERTV(CONFIG, LENGTH, tam.isTotalSame());
                        ASSERTV(CONFIG, LENGTH, tam.isInUseSame());
                    }

                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            BSL_TF_EQ(
                                    KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                    ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[LENGTH - 1],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                    guard.release();
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0; j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
            {
                Link *ITER[MAX_LENGTH + 1];

                // The first loop adds a duplicate in front of each already
                // inserted element
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = mX.insert(VALUES[tj]);
                    ASSERT(0 != ITER[tj]);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(VALUES[tj]),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        2 * LENGTH == X.size());

                // The second loop adds another duplicate in front of each
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = mX.insert(VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

#if defined BDE_BUILD_TARGET_EXC
            {
#if 0
                // This test shows up a non-conformance in 'bsl::allocator'
                // which has undefined behavior when asked for this many
                // buckets, rather than simply throwing a 'std::bad_alloc'.

                try {
                    Obj mX(HASH,
                           COMPARE,
                           native_std::numeric_limits<int>::max(),
                           1e-30);
                    ASSERT(false);
                }
                catch(const native_std::bad_allocr&) {
                    // This is the expected code path
                }
                catch(...) {
                    ASSERT(!!"The wrong exception type was thrown.");
                }
#endif

                try {
                    Obj mBad(HASH,
                             COMPARE,
                             native_std::numeric_limits<SizeType>::max(),
                             1e-30);
                    ASSERT(false);
                }
                catch(const native_std::length_error&) {
                    // This is the expected code path
                }
                catch(...) {
                    ASSERT(!!"The wrong exception type was thrown.");
                }

                Obj mR(HASH,
                       COMPARE,
                       3,
                       1e-30);
                try {
                    mR.insert(VALUES[0]);

                    P(mR.numBuckets())
                    P(mR.size());

                    ASSERT(false);
                }
                catch(const native_std::length_error& e) {
                    // This is the expected code path
                }
                catch(...) {
                    ASSERT(!!"The wrong exception type was thrown.");
                }

            }
#endif

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(MAX_LF, LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
    }
#endif
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING REHASH METHODS
    //
    // Concerns:
    //: 1 'rehashForNumBuckets' allocates at least the specified number of
    //:   buckets.
    //:
    //: 2 'reserveForNumElements' allocates sufficient buckets so that, after
    //:   the rehash, 'numBuckets() / maxLoadFactor()' >= the specified number
    //:   of elements.
    //:
    //: 3 Neither rehash function affects the value of the object.
    //:
    //: 4 Neither rehash function affects the order of the inserted elements
    //:   with the same value.
    //:
    //: 5 'rehashForNumBuckets' is a no-op if the requested number of buckets
    //:   is less than the current 'numBuckets' in the object.
    //:
    //: 6 'reserveForNumElements' is a no-op if the requested number of elements
    //:   can already be accommodated without exceeding the 'maxLoadFactor' of
    //:   the object.
    //:
    //: 7 Any memory allocation is from the object allocator.
    //:
    //: 8 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 9 'rehashForNumBuckets' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //:10 'rehashForNumBuckets' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //:
    //:11 'reserveForNumElements' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //:12 'reserveForNumElements' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //:
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   rehashForNumBuckets(SizeType newNumBuckets);
    //   reserveForNumElements(SizeType numElements);
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const size_t REHASH_SIZE[] = {
        0,
        1,
        2,
        3,
        5,
        8,
        12,
        13,
        2099
    };
    const int NUM_REHASH_SIZE = sizeof REHASH_SIZE / sizeof *REHASH_SIZE;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        for (int tj = 0; tj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++tj) {
            const float  MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[tj];
            const size_t NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_REHASH_SIZE; ++tk) {
                bslma::TestAllocator  oa("object",  veryVeryVeryVerbose);
                ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&oa);

                Obj mX(Z, objAlloc); const Obj& X = mX;

                if (veryVerbose) { T_ P_(LINE) P_(Z) P(X) }

                const size_t OLD_NUM_BUCKETS = X.numBuckets();
                const size_t NEW_NUM_BUCKETS = std::max(
                                      REHASH_SIZE[tk],
                                      predictNumBuckets(X.size() + 1, MAX_LF));

                const size_t EXP_NUM_BUCKETS =
                      bslstl::HashTable_ImpDetails::nextPrime(NEW_NUM_BUCKETS);

                bslma::TestAllocatorMonitor oam(&oa);

                mX.rehashForNumBuckets(REHASH_SIZE[tk]);

                ASSERTV(LINE, tk, MAX_LF, X == Z);
                ASSERTV(LINE, tk, MAX_LF, REHASH_SIZE[tk], X.numBuckets(),
                        REHASH_SIZE[tk] <= X.numBuckets());

                // If request is smaller than current number of buckets, there
                // should be no effect, no change in buckets, and no chance to
                // allocate memory.
                if (REHASH_SIZE[tk] <= OLD_NUM_BUCKETS) {
                    ASSERTV(LINE, tk, MAX_LF,
                            OLD_NUM_BUCKETS == X.numBuckets());
                    ASSERTV(LINE, tk, MAX_LF, oam.isTotalSame());
                    ASSERTV(LINE, tk, MAX_LF, oam.isInUseSame());
                }
                // Otherwise, we have computed the new expected number of
                // buckets, and there should be exactly one allocation for the
                // new bucket array.
                else {
                    ASSERTV(LINE, tk, MAX_LF,
                            EXP_NUM_BUCKETS,   X.numBuckets(),
                            EXP_NUM_BUCKETS == X.numBuckets());

                    ASSERTV(LINE, tk, oam.numBlocksTotalChange(),
                            1 == oam.numBlocksTotalChange());

                    // We release the old array allocation to make use of the
                    // new, so there should be no change in the number of
                    // allocated blocks in use.  However, in the case of
                    // the first allocation on an empty container, we will
                    // actually be creating the first array block.
                    if (0 < LENGTH ) {
                        ASSERTV(LINE, tk, oam.isInUseSame());
                    }
                    else {
                        ASSERTV(LINE, tk, oam.numBlocksInUseChange(),
                                1 == oam.numBlocksInUseChange());
                    }
                }
            }
        }
#if defined BDE_BUILD_TARGET_EXC
        // The following set of tests are expected, at least in some test
        // configurations, to fail by throwing exceptions.
        if (verbose) printf("Testing behavior at extremities.\n");
        {
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mX(HASH, COMPARE, 1, 1.0f, scratchAlloc);
            try {
                mX.rehashForNumBuckets(std::numeric_limits<size_t>::max());
                ASSERT(!"rehash(max size_t) should throw a 'logic_error'");
            }
            catch (const std::logic_error &) {
                // This is the expected code path
            }
            catch (...) {
                ASSERT(!"rehash(max size_t) threw the wrong exception type");
            }
        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1e30f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1e-6f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1.0f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
//        {
//            Obj mZ(HASH, COMPARE, NUM_BUCKETS, 1.0f, scratchAlloc);
//            const Obj& Z = gg(&mZ,  SPEC);
//        }
#endif
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase9()
{
    // ------------------------------------------------------------------------
    // COPY-ASSIGNMENT OPERATOR:
    //   Ensure that we can assign the value of any object of the class to any
    //   object of the class, such that the two objects subsequently have the
    //   same value.
    //
    // Concerns:
    //: 1 The assignment operator can change the value of any modifiable target
    //:   object to that of any source object.
    //:
    //: 2 If allocator propagation is not enabled for copy assignment, the
    //:   allocator address held by the target object is unchanged.
    //:
    //: 3 If allocator propagation is not enabled for copy assignment, any
    //:   memory allocation is from the target object's allocator.
    //:
    //: 4 The signature and return type are standard.
    //:
    //: 5 The reference returned is to the target object (i.e., '*this').
    //:
    //: 6 The value of the source object is not modified.
    //:
    //: 7 The allocator address held by the source object is unchanged.
    //:
    //: 8 QoI: Assigning a source object having the default-constructed value
    //:   allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //:
    //:10 Assigning an object to itself behaves as expected (alias-safety).
    //:
    //:11 Every object releases any allocated memory at destruction.
    //:
    //:12 If allocator propagation is enabled for copy assignment,
    //:   any memory allocation is from the source object's
    //:   allocator.
    //:
    //:13 If allocator propagation is enabled for copy assignment, the
    //:   allocator address held by the target object is changed to that of the
    //:   source.
    //:
    //:14 If allocator propagation is enabled for copy assignment, any memory
    //:   allocation is from the original target allocator will be released
    //:   after copy assignment.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   copy-assignment operator defined in this component.  (C-4)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values.
    //:
    //: 4 For each row 'R1' (representing a distinct object value, 'V') in the
    //:   table described in P-3: (C-1..2, 5..8, 11)
    //:
    //:   1 Use the value constructor and a "scratch" allocator to create two
    //:     'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
    //:
    //:   2 Execute an inner loop that iterates over each row 'R2'
    //:     (representing a distinct object value, 'W') in the table described
    //:     in P-3:
    //:
    //:   3 For each of the iterations (P-4.2): (C-1..2, 5..8, 11)
    //:
    //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable 'Obj',
    //:       'mX', having the value 'W'.
    //:
    //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
    //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
    //:
    //:     4 Verify that the address of the return value is the same as that
    //:       of 'mX'.  (C-5)
    //:
    //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
    //:
    //:       1 The target object, 'mX', now has the same value as that of 'Z'.
    //:         (C-1)
    //:
    //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
    //:
    //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify that
    //:       the respective allocator addresses held by the target and source
    //:       objects are unchanged.  (C-2, 7)
    //:
    //:     7 Use the appropriate test allocators to verify that: (C-8, 11)
    //:
    //:       1 For an object that (a) is initialized with a value that did NOT
    //:         require memory allocation, and (b) is then assigned a value
    //:         that DID require memory allocation, the target object DOES
    //:         allocate memory from its object allocator only (irrespective of
    //:         the specific number of allocations or the total amount of
    //:         memory allocated); also cross check with what is expected for
    //:         'mX' and 'Z'.
    //:
    //:       2 An object that is assigned a value that did NOT require memory
    //:         allocation, does NOT allocate memory from its object allocator;
    //:         also cross check with what is expected for 'Z'.
    //:
    //:       3 No additional memory is allocated by the source object.  (C-8)
    //:
    //:       4 All object memory is released when the object is destroyed.
    //:         (C-11)
    //:
    //: 5 Repeat steps similar to those described in P-4 except that, this
    //:   time, there is no inner loop (as in P-4.2); instead, the source
    //:   object, 'Z', is a reference to the target object, 'mX', and both 'mX'
    //:   and 'ZZ' are initialized to have the value 'V'.  For each row
    //:   (representing a distinct object value, 'V') in the table described in
    //:   P-3: (C-9)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mX'; also use the value constructor and a distinct "scratch"
    //:     allocator to create a 'const' 'Obj' 'ZZ'.
    //:
    //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
    //:
    //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions (using
    //:     the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  (C-9)
    //:
    //:   5 Verify that the address of the return value is the same as that of
    //:     'mX'.
    //:
    //:   6 Use the equality-comparison operator to verify that the target
    //:     object, 'mX', still has the same value as that of 'ZZ'.
    //:
    //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still the
    //:     object allocator.
    //:
    //:   8 Use the appropriate test allocators to verify that:
    //:
    //:     1 Any memory that is allocated is from the object allocator.
    //:
    //:     2 No additional (e.g., temporary) object memory is allocated when
    //:       assigning an object value that did NOT initially require
    //:       allocated memory.
    //:
    //:     3 All object memory is released when the object is destroyed.
    //:
    //: 6 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-3)
    //
    // Testing:
    //   HashTable& operator=(const HashTable& rhs);
    // ------------------------------------------------------------------------

    typedef MakeAllocator<ALLOCATOR> AllocMaker;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nCompare each pair of similar and different"
                        " values (u, ua, v, va) in S X A X S X A"
                        " without perturbation.\n");
    {
        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1   = DATA[ti].d_line;
            const int         INDEX1  = DATA[ti].d_index;
            const char *const SPEC1   = DATA[ti].d_spec;

            const size_t      LENGTH1 = strlen(SPEC1);
            const size_t NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const ALLOCATOR scratchAlloc = AllocMaker::make(&scratch);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, scratchAlloc);
            const Obj& Z  = gg(&mZ,  SPEC1);
            const Obj ZZ(Z, scratchAlloc);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // should perform a self-assignment test here

            // Create second object
            for (int lfj = 0; lfj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfj) {
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const float MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2 = strlen(SPEC2);
                const size_t NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                              MAX_LF2);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);
                const ALLOCATOR objAlloc     = AllocMaker::make(&oa);

                {
                    Obj mX(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, objAlloc);
                    const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                        ASSERTV(LINE1, LINE2,
                                Z.maxLoadFactor(),   X.maxLoadFactor(),
                                Z.maxLoadFactor() == X.maxLoadFactor());

                        // If we can compare the two functor objects, we should
                        // do so, to verify stateful allocators are copied too.
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                    ASSERTV(LINE1, LINE2, scratchAlloc == Z.allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                    // The invariant check in the destructor uses the default
                    // allocator in SAFE_2 builds.
                    // Otherwise, no memory should be allocated by the default
                    // allocator.
                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
#endif
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }
            }
        }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same
    //   allocator.
    //
    // Concerns:
    //: 1 Both functions exchange the state of the (two) supplied objects,
    //:   comprising their values, their functors, and their 'maxLoadFactor'
    //:   attribute.
    //:
    //: 2 Both functions have standard signatures and return types.
    //:
    //: 3 Using either function to swap an object with itself does not
    //:   affect the value of the object (alias-safety).
    //:
    //: 4 If the two objects being swapped use the same allocator, neither
    //:   function allocates memory from any allocator and the allocator
    //:   address held by both objects is unchanged.
    //:
    //: 5 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type', then both function may allocate memory and the
    //:   allocator address held by both object is unchanged.
    //:
    //: 6 If the two objects being swapped uses different allocators and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'true_type', then no memory will be allocated and the allocators will
    //:   also be swapped.
    //:
    //: 7 Both functions provides the strong exception guarantee w.t.r. to
    //:   memory allocation .
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //:
    //: 9 'swap' does not invalidate any references or pointers to elements
    //:   stored in either hash table, unless allocators are unequal and
    //:   'AllocatorTraits::propagate_on_container_swap' is an alias to
    //:   'false_type'.
    //
    // Plan:
    //: 1 Use the addresses of the 'swap' member and free functions defined
    //:   in this component to initialize, respectively, member-function
    //:   and free-function pointers having the appropriate signatures and
    //:   return types.  (C-2)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the
    //:   default allocator (note that a ubiquitous test allocator is
    //:   already installed as the global allocator).
    //:
    //: 3 Using the table-driven technique:
    //:
    //:   1 Specify a set of (unique) valid object values (one per row) in
    //:     terms of their individual attributes, including (a) first, the
    //:     default value, (b) boundary values corresponding to every range
    //:     of values that each individual attribute can independently
    //:     attain, and (c) values that should require allocation from each
    //:     individual attribute that can independently allocate memory.
    //:
    //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
    //:     the expectation of memory allocation for all typical
    //:     implementations of individual attribute types: ('Y') "Yes",
    //:     ('N') "No", or ('?') "implementation-dependent".
    //:
    //: 4 For each row 'R1' in the table of P-3:  (C-1, 3..7)
    //:
    //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   2 Use the value constructor and 'oa' to create a modifiable
    //:     'Obj', 'mW', having the value described by 'R1'; also use the
    //:     copy constructor and a "scratch" allocator to create a 'const'
    //:     'Obj' 'XX' from 'mW'.
    //:
    //:   3 Use the member and free 'swap' functions to swap the value of
    //:     'mW' with itself; verify, after each swap, that:  (C-3..4)
    //:
    //:     1 The value is unchanged.  (C-3)
    //:
    //:     2 The allocator address held by the object is unchanged.  (C-4)
    //:
    //:     3 There was no additional object memory allocation.  (C-4)
    //:
    //:   4 For each row 'R2' in the table of P-3:  (C-1, 4)
    //:
    //:     1 Use the copy constructor and 'oa' to create a modifiable
    //:       'Obj', 'mX', from 'XX' (P-4.2).
    //:
    //:     2 Use the value constructor and 'oa' to create a modifiable
    //:       'Obj', 'mY', and having the value described by 'R2'; also use
    //:       the copy constructor to create, using a "scratch" allocator,
    //:       a 'const' 'Obj', 'YY', from 'Y'.
    //:
    //:     3 Use, in turn, the member and free 'swap' functions to swap
    //:       the values of 'mX' and 'mY'; verify, after each swap, that:
    //:       (C-1..2)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The common object allocator address held by 'mX' and 'mY'
    //:         is unchanged in both objects.  (C-4)
    //:
    //:       3 There was no additional object memory allocation.  (C-4)
    //:
    //:     5 Use the value constructor and 'oaz' to a create a modifiable
    //:       'Obj' 'mZ', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'ZZ', from 'Z'.
    //:
    //:     6 Use the member and free 'swap' functions to swap the values of
    //:       'mX' and 'mZ' respectively (when
    //:       AllocatorTraits::propagate_on_container_swap is an alias to
    //:       false_type) under the presence of exception; verify, after each
    //:       swap, that:  (C-1, 5, 7)
    //:
    //:       1 If exception occurred during the swap, both values are
    //:         unchanged.  (C-7)
    //:
    //:       2 If no exception occurred, the values have been exchanged.
    //:         (C-1)
    //:
    //:       3 The common object allocator address held by 'mX' and 'mZ' is
    //:         unchanged in both objects.  (C-5)
    //:
    //:       4 Temporary memory were allocated from 'oa' if 'mZ' is is not
    //:         empty, and temporary memory were allocated from 'oaz' if 'mX'
    //:         is not empty.  (C-5)
    //:
    //:     7 Create a new object allocator, 'oap'.
    //:
    //:     8 Use the value constructor and 'oap' to create a modifiable 'Obj'
    //:       'mP', having the value described by 'R2'; also use the copy
    //:       constructor to create, using a "scratch" allocator, a const
    //:       'Obj', 'PP', from 'P.
    //:
    //:     9 Manually change 'AllocatorTraits::propagate_on_container_swap' to
    //:       be an alias to 'true_type' (Instead of this manual step, use an
    //:       allocator that enables propagate_on_container_swap when
    //:       AllocatorTraits supports it) and use the the member and free
    //:       'swap functions to swap the values 'mX' and 'mZ' respectively;
    //:       verify, after each swap, that: (C-1, 6)
    //:
    //:       1 The values have been exchanged.  (C-1)
    //:
    //:       2 The allocators addresses have been exchanged.  (C-6)
    //:
    //:       3 There was no additional object memory allocation.  (C-6)
    //:
    //: 5 Verify that the free 'swap' function is discoverable through ADL:
    //:   (C-8)
    //:
    //:   1 Create a set of attribute values, 'A', distinct from the values
    //:     corresponding to the default-constructed object, choosing
    //:     values that allocate memory if possible.
    //:
    //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
    //:
    //:   3 Use the default constructor and 'oa' to create a modifiable
    //:     'Obj' 'mX' (having default attribute values); also use the copy
    //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
    //:     'XX' from 'mX'.
    //:
    //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
    //:     'mY' having the value described by the 'Ai' attributes; also
    //:     use the copy constructor and a "scratch" allocator to create a
    //:     'const' 'Obj' 'YY' from 'mY'.
    //:
    //:   5 Use the 'invokeAdlSwap' helper function template to swap the
    //:     values of 'mX' and 'mY', using the free 'swap' function defined
    //:     in this component, then verify that:  (C-8)
    //:
    //:     1 The values have been exchanged.  (C-1)
    //:
    //:     2 There was no additional object memory allocation.  (C-4)
    //
    // Testing:
    //   void swap(HashTable& other);
    //   void swap(HashTable<K, V, C, A>& a, HashTable<K, V, C, A>& b);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nSWAP MEMBER AND FREE FUNCTIONS"
                        "\n==============================\n");

    typedef MakeAllocator<ALLOCATOR> AllocMaker;

    if (verbose) printf(
                     "\nAssign the address of each function to a variable.\n");
    {
        typedef void (Obj::*funcPtr)(Obj&);
        typedef void (*freeFuncPtr)(Obj&, Obj&);

        // Verify that the signatures and return types are standard.

        funcPtr     memberSwap = &Obj::swap;
        freeFuncPtr freeSwap   = bslstl::swap;

        (void)memberSwap;  // quash potential compiler warnings
        (void)freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const float       MAX_LF1 = DEFAULT_MAX_LOAD_FACTOR[lfi];

        const int         LINE1   = DATA[ti].d_line;
        const char *const SPEC1   = DATA[ti].d_spec;

        const size_t      LENGTH1      = strlen(SPEC1);
        const size_t      NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = AllocMaker::make(&scratch);
        const ALLOCATOR objAlloc     = AllocMaker::make(&oa);

        Obj mW(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, objAlloc);
        const Obj& W = gg(&mW,  SPEC1);
        const Obj XX(W, scratchAlloc);

        if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            mW.swap(mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, objAlloc == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oam(&oa);

            swap(mW, mW);

            ASSERTV(LINE1, XX, W, XX == W);
            ASSERTV(LINE1, objAlloc == W.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
        }

        // There is a symmetry to testing, so test only the "lower triangle"
        // of indices.  Note that we want to support the limiting case where
        // the indices match.
        for (int lfj = 0; lfj <= lfi; ++lfj) {
        for (int  tj = 0;  tj <=  ti; ++ tj) {
            const float       MAX_LF2 = DEFAULT_MAX_LOAD_FACTOR[lfj];

            const int         LINE2   = DATA[tj].d_line;
            const char *const SPEC2   = DATA[tj].d_spec;

            const size_t      LENGTH2      = strlen(SPEC2);
            const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                               MAX_LF2);

            Obj mX(XX, objAlloc);  const Obj& X = mX;

            Obj mY(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, objAlloc);
            const Obj& Y = gg(&mY, SPEC2);
            const Obj YY(Y, scratchAlloc);

            if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // We may consider restoring a test of unequal allocators if the
            // allocator propagate_on_container_swap trait is true.
        }
        }
    }
    }

    if (verbose) printf(
            "\nInvoke free 'swap' function in a context where ADL is used.\n");
    {
        // 'A' values: Should cause memory allocation if possible.

        bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = AllocMaker::make(&scratch);
        const ALLOCATOR objAlloc     = AllocMaker::make(&oa);

        Obj mX(HASH, COMPARE, 0, 1.0f, objAlloc);  const Obj& X = mX;
        const Obj XX(X, scratchAlloc);

        Obj mY(HASH, COMPARE, 40, 0.1f, objAlloc);
        const Obj& Y = gg(&mY, "ABC");
        const Obj YY(Y, scratchAlloc);

        if (veryVerbose) { T_ P_(X) P(Y) }

        bslma::TestAllocatorMonitor oam(&oa);

        invokeAdlSwap(mX, mY);

        ASSERTV(YY, X, YY == X);
        ASSERTV(XX, Y, XX == Y);
        ASSERTV(YY, X, YY.maxLoadFactor() == X.maxLoadFactor());
        ASSERTV(XX, Y, XX.maxLoadFactor() == Y.maxLoadFactor());
        ASSERT(oam.isTotalSame());

        if (veryVerbose) { T_ P_(X) P(Y) }
    }

    if (verbose) printf("Check our implementation of undefined behavior.\n");
    {
        // Ideally we would negatively test for an ASSERT in SAFE mode, and
        // test the strongly exception-safe 'swap' only in other build modes.
        // We will test only one 'swap' of two large containers of different
        // sizes, and having different load factors, as well as different
        // allocators to represent a fair test of our expected implementation
        // of the undefine behavior.

        const float       MAX_LF1 = 0.125f;
        const float       MAX_LF2 = 2.5f;

        const char *const SPEC1   = DATA[NUM_DATA-1].d_spec;
        const char *const SPEC2   = DATA[NUM_DATA-2].d_spec;

        const size_t      LENGTH1      = strlen(SPEC1);
        const size_t      NUM_BUCKETS1 = predictNumBuckets(LENGTH1, MAX_LF1);

        const size_t      LENGTH2      = strlen(SPEC2);
        const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2, MAX_LF2);

        bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
        bslma::TestAllocator oax("x_object", veryVeryVeryVerbose);
        bslma::TestAllocator oaz("z_object", veryVeryVeryVerbose);

        const ALLOCATOR scratchAlloc = AllocMaker::make(&scratch);
        const ALLOCATOR oaxAlloc     = AllocMaker::make(&oax);
        const ALLOCATOR oazAlloc     = AllocMaker::make(&oaz);

        Obj mX(HASH, COMPARE, NUM_BUCKETS1, MAX_LF1, oaxAlloc);
        const Obj& X = gg(&mX, SPEC1);
        const Obj XX(X, scratchAlloc);

        Obj mZ(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, oazAlloc);
        const Obj& Z = gg(&mZ, SPEC2);
        const Obj ZZ(Z, scratchAlloc);

        // member 'swap'
        {
            bslma::TestAllocatorMonitor oaxm(&oax);
            bslma::TestAllocatorMonitor oazm(&oaz);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oax) {
                bslma::ExceptionGuard<Obj> guardX(&X, L_, scratchAlloc);
                bslma::ExceptionGuard<Obj> guardZ(&Z, L_, scratchAlloc);

                mX.swap(mZ);

                guardX.release();
                guardZ.release();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END


            ASSERTV(ZZ, X, ZZ == X);
            ASSERTV(XX, Z, XX == Z);
            ASSERTV(oaxAlloc == X.allocator());
            ASSERTV(oazAlloc == Z.allocator());

            // Concerns about allocated memory?
        }

        // free function 'swap'
        {
            bslma::TestAllocatorMonitor oaxm(&oax);
            bslma::TestAllocatorMonitor oazm(&oaz);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oax) {
                bslma::ExceptionGuard<Obj> guardX(&X, L_, scratchAlloc);
                bslma::ExceptionGuard<Obj> guardZ(&Z, L_, scratchAlloc);

                swap(mX, mZ);

                guardX.release();
                guardZ.release();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(XX, X, XX == X);
            ASSERTV(ZZ, Z, ZZ == Z);
            ASSERTV(oaxAlloc == X.allocator());
            ASSERTV(oazAlloc == Z.allocator());

            // Concerns about allocated memory?
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase7()
{
    // ------------------------------------------------------------------------
    // TESTING COPY CONSTRUCTOR:
    //  One additional constraint to bear in mind, while testing the copy
    //  constructor, is that the copy does not give us any control over the
    //  'numBuckets' allocated for the new copy.  Given our primary manipulator
    //  is a special 'insertElement' function that does not allow us to insert
    //  past the precomputed capacity of a HashTable, some test scenarios may
    //  produce copies that cannot be further modified by our chosen primary
    //  manipulator.  Therefor, we will test for a failed insertion in the case
    //  that the number of elements in the test data exactly matches the limit
    //  that can be allocated to the buckets without forcing a rehash, and in
    //  that case our mutate-event will be to clear the container instead.  We
    //  considered selectively avoiding this corner case with careful selection
    //  of test data, but realized that simply left a predictable, untested
    //  pattern in our test driver.  We will validate the proper behavior when
    //  an 'insert' operation forces a rehash when testing the 'insert' methods
    //  in a subsequent test case.
    //
    // Concern:
    //: 1 The new object's value is the same as that of the original object
    //:   (relying on the equality operator), the same 'maxLoadFactor', the
    //:   same 'hasher' and the same 'comparator'.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The value of the original object is left unaffected.
    //:
    //: 4 Subsequent changes in or destruction of the source object have no
    //:   effect on the copy-constructed object.
    //:
    //: 5 Subsequent changes ('insert's) on the created object have no
    //:   effect on the original.
    //:
    //: 6 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 7 The function is exception neutral w.r.t. memory allocation.
    //:
    //: 8 QoI The new object has a 'loadFactor' <= its 'maxLoadFactor'.
    //
    // Plan:
    //: 1 Specify a set S of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used in the
    //:   following tests.
    //:
    //: 2 For each value in S, initialize objects w and x, copy construct y
    //:   from x and use 'operator==' to verify that both x and y subsequently
    //:   have the same value as w.  Let x go out of scope and again verify
    //:   that w == y.  (C-1..4)
    //:
    //: 3 For each value in S initialize objects w and x, and copy construct y
    //:   from x.  Change the state of y, by using the *primary* *manipulator*
    //:   'insertElement' (or 'removeAll' if 'insertElement' returns a null
    //:   pointer value).  Using the 'operator!=' verify that y differs from x
    //:   and w.  Then apply the same operation to 'w' and verify that y still
    //:   differs from x, and now has the same value as w. (C-5)
    //:
    //: 4 Perform tests performed as P-2:  (C-6)
    //:   1 While passing a testAllocator as a parameter to the new object and
    //:     ascertaining that the new object gets its memory from the provided
    //:     testAllocator.
    //:   2 Verify neither of global and default allocator is used to supply
    //:     memory.  (C-6)
    //:
    //: 5 Perform tests as P-2 in the presence of exceptions during memory
    //:   allocations using a 'bslma::TestAllocator' and varying its
    //:   *allocation* *limit*.  (C-7)
    //
    // Testing:
    //   HashTable(const HashTable& original);
    //   HashTable(const HashTable& original, const A& allocator);
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::KeyType KEY;
    typedef typename KEY_CONFIG::ValueType VALUE;

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    const int TYPE_ALLOC =
         bslalg::HasTrait<KEY, bslalg::TypeTraitUsesBslmaAllocator>::VALUE
         + bslalg::HasTrait<VALUE, bslalg::TypeTraitUsesBslmaAllocator>::VALUE;

    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nTesting parameters: TYPE_ALLOC = %d.\n",
                        TYPE_ALLOC);
    {
        static const char *SPECS[] = {
            "",
            "A",
            "BC",
            "CDE",
            "DEAB",
            "EABCD",
            "ABCDEFG",
            "HFGEDCBA",
            "CFHEBIDGA",
            "BENCKHGMALJDFOI",
            "IDMLNEFHOPKGBCJA",
            "OIQGDNPMLKBACHFEJ"
        };
        const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const float       MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[lfi];
            const char *const SPEC        = SPECS[ti];

            const size_t      LENGTH      = strlen(SPEC);
            const size_t      NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            if (veryVerbose) {
                printf("\nFor an object of length " ZU ":\n", LENGTH);
                P(SPEC);
            }

            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

#if 0
            // It is not yet clear how to handle the "stateless" std-test
            // allocator in this one general function, as we now construct
            // two allocator objects with different test allocators, which
            // violates the contract that we install only the current default
            // allocator when creating a 'stateless' object via the 'make'
            // call.
            //
            // There is no easy way to create this guard for the specific
            // test case of the stateless 'bsltf::StdTestAllocator', nor
            // without second guessing the allocator to use based upon the
            // 'cfg' code.  By the time we return from 'makeAllocator' the
            // test allocator will already have been installed, with no
            // easy way to restore at the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
#endif

            HASHER     hash = HASH;
            COMPARATOR comp = COMPARE;
#if defined(AJM_HAS_IMPLMENTED_GENERIC_STATEFUL_FUNCTOR_CHECKS)
            setHasherState(bsls::Util::addressOf(hash), ti);
            setComparatorState(bsls::Util::addressOf(comp), ti);
#endif

            const ALLOCATOR dfltAlloc = MakeAllocator<ALLOCATOR>::make(&da);
            const ALLOCATOR objAlloc  = MakeAllocator<ALLOCATOR>::make(&oa);

            // Create control object w, with space for an extra element.
            Obj mW(hash, comp, NUM_BUCKETS + 1, MAX_LF, dfltAlloc);
            const Obj& W = gg(&mW, SPEC);

            ASSERTV(ti, LENGTH == W.size()); // same lengths
            if (veryVerbose) { printf("\tControl Obj: "); P(W); }

            Obj mX(hash, comp, NUM_BUCKETS, MAX_LF, objAlloc);
            const Obj& X = gg(&mX, SPEC);

            // Sanity check only, previous test cases established this.
            ASSERTV(MAX_LF, SPEC, W, X,  W == X);

            if (veryVerbose) { printf("\t\tDynamic Obj: "); P(X); }

            {   // Testing concern 1..4 and 8

                if (veryVeryVerbose) { printf("\t\t\tRegular Case :"); }

                Obj *pX =
                         new Obj(HASH, COMPARE, NUM_BUCKETS, MAX_LF, objAlloc);
                gg(pX, SPEC);

                ASSERTV(MAX_LF, SPEC, *pX, X, *pX == X);
                ASSERTV(MAX_LF, SPEC, *pX, W, *pX == W);

                const Obj Y0(*pX);

                ASSERTV(MAX_LF, SPEC, *pX, Y0, *pX == Y0);
                ASSERTV(MAX_LF, SPEC,   X, Y0,   X == Y0);
                ASSERTV(MAX_LF, SPEC,   W, Y0,   W == Y0);
                ASSERTV(MAX_LF, SPEC,   W,  X,   W ==  X);

                const ALLOCATOR expectedAlloc =
                    bsl::allocator_traits<ALLOCATOR>::
                        select_on_container_copy_construction(pX->allocator());
                ASSERTV(MAX_LF, SPEC,
                        BSL_TF_EQ(Y0.allocator(), expectedAlloc));
                ASSERTV(MAX_LF, SPEC, pX->maxLoadFactor(), Y0.maxLoadFactor(),
                         pX->maxLoadFactor() == Y0.maxLoadFactor());
                ASSERTV(MAX_LF, SPEC, Y0.loadFactor(), Y0.maxLoadFactor(),
                        Y0.loadFactor() <= Y0.maxLoadFactor());
                delete pX;
                ASSERTV(MAX_LF, SPEC, W, Y0, W == Y0);
            }
            {   // Testing concern 5.

                if (veryVeryVerbose) printf("\t\t\tInsert into created obj, "
                                            "without test allocator:\n");

                Obj Y1(W);

                if (veryVeryVerbose) {
                    printf("\t\t\t\tBefore Insert: "); P(Y1);
                }

                Link *RESULT = insertElement(&Y1, VALUES['Z' - 'A']);
                if(0 == RESULT) {
                    Y1.removeAll();
                }

                if (veryVeryVerbose) {
                    printf("\t\t\t\tAfter Insert : "); P(Y1);
                }

                if (RESULT || 0 != W.size()) {
                    const size_t EXPECTED_LENGTH = RESULT
                                                 ? LENGTH + 1
                                                 : 0;

                    ASSERTV(MAX_LF, SPEC, Y1.size(), EXPECTED_LENGTH,
                            Y1.size() == EXPECTED_LENGTH);
                    ASSERTV(MAX_LF, SPEC, W, Y1, W != Y1);
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }

                if (RESULT) {
                    RESULT = insertElement(&mW, VALUES['Z' - 'A']);
                    // Remember we reserved enough space at construction.
                    ASSERT(0 != RESULT);
                }
                else {
                    mW.removeAll();
                }

                ASSERTV(MAX_LF, SPEC, W, Y1, W == Y1);
                if (RESULT || 0 < X.size()) {
                    ASSERTV(MAX_LF, SPEC, X, Y1, X != Y1);
                }
            }
            {   // Testing concern 5 with test allocator.

                if (veryVerbose)
                    printf("\t\t\tInsert into created obj, "
                           "with test allocator:\n");

                bslma::TestAllocatorMonitor oam(&oa);

                Obj Y11(X, objAlloc);

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    //const int TYPE_ALLOCS = TYPE_ALLOC * X.size();
                    //ASSERTV(SPEC, BB, AA, BB + 1 + TYPE_ALLOCS == AA);
                    //ASSERTV(SPEC, B, A, B + 1 + TYPE_ALLOCS ==  A);
                    ASSERTV(MAX_LF, SPEC, oam.isTotalUp());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseUp());
                }

                oam.reset(&oa);

                Y11.removeAll();

                if (veryVerbose) {
                    printf("\t\t\t\tAfter Insert : ");
                    P(Y11);
                }

                ASSERTV(SPEC, oam.isTotalSame());

                ASSERTV(SPEC, 0 == Y11.size());
                if (LENGTH != 0) {
                    ASSERTV(MAX_LF, SPEC, W != Y11 || !W.size());
                    ASSERTV(MAX_LF, SPEC, X != Y11);
                }
                ASSERTV(MAX_LF, SPEC, Y11.allocator() == X.allocator());
            }

            if (0 < X.size())
            {   // Exception checking.
                // There is nothing to test if 'X' is empty, and several test
                // conditions would be complicated to allow for 'W == X' in
                // this state.

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    Obj Y2(X, objAlloc);
                    if (veryVerbose) {
                        printf("\t\t\tException Case  :\n");
                        printf("\t\t\t\tObj : "); P(Y2);
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  != Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 ==  X);
                    ASSERTV(MAX_LF, SPEC, Y2.allocator() == X.allocator());

                    Link *RESULT = insertElement(&Y2, VALUES['Z' - 'A']);
                    if(0 == RESULT) {
                        Y2.removeAll();
                    }
                    ASSERTV(MAX_LF, SPEC,  W, Y2, W  == Y2);
                    ASSERTV(MAX_LF, SPEC, Y2,  X, Y2 !=  X);

                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                if (LENGTH == 0) {
                    ASSERTV(MAX_LF, SPEC, oam.isTotalSame());
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
                else {
                    ASSERTV(MAX_LF, SPEC, oam.isInUseSame());
                }
            }
        }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS:
    // Concerns:
    //: 1 Two objects, 'X' and 'Y', compare equal if and only if they contain
    //:   the same values.
    //:
    //: 2 No non-salient attributes participate.  The non-salient attributes of
    //:   a 'HashTable' include the 'allocator', 'loadFactor', 'maxLoadFactor'
    //:   and 'numBuckets'.
    //:
    //: 3 'true  == (X == X)' (i.e., identity)
    //:
    //: 4 'false == (X != X)' (i.e., identity)
    //:
    //: 5 'X == Y' if and only if 'Y == X' (i.e., commutativity)
    //:
    //: 6 'X != Y' if and only if 'Y != X' (i.e., commutativity)
    //:
    //: 7 'X != Y' if and only if '!(X == Y)'
    //:
    //: 8 Comparison is symmetric with respect to user-defined conversion
    //:   (i.e., both comparison operators are free functions).
    //:
    //: 9 Non-modifiable objects can be compared (i.e., objects or references
    //:   providing only non-modifiable access).
    //:
    //:10 'operator==' is defined in terms of
    //:   'operator==(KEY_CONFIG::ValueType)' instead of the supplied
    //:   comparator function (which, along with the hasher, is still used to
    //:   establish key-equivalent groups).
    //:
    //:11 No memory allocation occurs as a result of comparison (e.g., the
    //:   arguments are not passed by value).
    //:
    //:12 The equality operator's signature and return type are standard.
    //:
    //:13 The inequality operator's signature and return type are standard.
    //:
    //:14 Two object with the same elements in different permutation compare
    //:   equal to each other.
    //
    // Plan:
    //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
    //:   initialize function pointers having the appropriate signatures and
    //:   return types for the two homogeneous, free equality- comparison
    //:   operators defined in this component.  (C-8..9, 12..13)
    //:
    //: 2 Create a 'bslma::TestAllocator' object, and install it as the default
    //:   allocator (note that a ubiquitous test allocator is already installed
    //:   as the global allocator).
    //:
    //: 3 Using the table-driven technique, specify a set of distinct
    //:   specifications for the 'gg' function.
    //:
    //: 4 For each row 'R1' in the table of P-3: (C-1..7)
    //:
    //:   1 Create a single object, using a comparator that can be disabled and
    //:     a "scratch" allocator, and use it to verify the reflexive
    //:     (anti-reflexive) property of equality (inequality) in the presence
    //:     of aliasing.  (C-3..4)
    //:
    //:   2 For each row 'R2' in the table of P-3: (C-1..2, 5..7)
    //:
    //:     1 Record, in 'EXP', whether or not distinct objects created from
    //:       'R1' and 'R2', respectively, are expected to have the same value.
    //:
    //:     2 For each of two configurations, 'a' and 'b': (C-1..2, 5..7)
    //:
    //:       1 Create three (object) allocators, 'oax', 'oay' and 'oaz'.
    //:
    //:       2 Create an object 'X', using 'oax', having the value 'R1'.
    //:
    //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
    //:         'oay' in configuration 'b', having the value 'R2', and a
    //:         'maxLoadFactor' of 10.
    //:
    //:       4 Create an object 'Z', using 'oax' in configuration 'a' and
    //:         'oaz' in configuration 'b', having the value 'R2', and a
    //:         'maxLoadFactor' of 0.01.
    //:
    //:       5 Verify the commutativity property and expected return value for
    //:         both '==' and '!=', while monitoring 'oax', 'oay' and 'oaz' to
    //:         ensure that no object memory is ever allocated by either
    //:         operator.  (C-1..2, 5..7)
    //:
    //:       6 Compare the ordering of elements in the lists accessed from
    //:         'Y.elementListRoot()' and 'Z.elementListRoot()', which must
    //:         always be permutations of each other, and set a test-wide flag
    //:         to confirm that the lists have differing orders at least once.
    //:         (C14)
    //:
    //: 5 Use the test allocator from P-2 to verify that no memory is ever
    //:   allocated from the default allocator.  (C-11)
    //
    // Testing:
    //*  bool operator==(const HashTable& lhs, const HashTable& rhs);
    //*  bool operator!=(const HashTable& lhs, const HashTable& rhs);
    // ------------------------------------------------------------------------

    if (verbose) printf("\nEQUALITY-COMPARISON OPERATORS"
                        "\n=============================\n");

    if (verbose)
              printf("\nAssign the address of each operator to a variable.\n");
    {
        typedef bool (*operatorPtr)(const Obj&, const Obj&);

        // Verify that the signatures and return types are standard.

        operatorPtr operatorEq = bslstl::operator==;
        operatorPtr operatorNe = bslstl::operator!=;

        // quash potential compiler warnings about unused variables

        ASSERT(operatorEq != operatorNe);
    }

    static const struct {
        int         d_line;   // source line number
        int         d_index;  // lexical order
        const char *d_spec;   // specification string, for input to 'gg'
    } DATA[] = {
        //line idx  spec
        //---- ---  --------
        { L_,    0, "" },
        { L_,    1, "A" },
        { L_,    2, "AA" },
        { L_,    3, "AB" },
        { L_,    3, "BA" },
        { L_,    4, "ABC" },
        { L_,    4, "BAC" },
        { L_,    4, "BCA" },
        { L_,    5, "ABD" },
        { L_,    6, "ABCDE" },
        { L_,    6, "CEBAD" },
        { L_,    7, "AAAAA" },
        { L_,    8, "ABCDA" },
        { L_,    9, "ABCDF" },
        { L_,    9, "FDCBA" },
        { L_,   10, "AFKPUZ" },
        { L_,   11, "ABFGKLPQUVZ" },
        { L_,   12, "AAABBCDEEFFFGGGHHIJKKLLLMMMNNOPQQRRRSSSTTUVWWXXXYYZ" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    // Create a variable to confirm that our tested data set has an expected
    // (and important) property.
    bool HAVE_TESTED_DISTINCT_PERMUTATIONS = false;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
        const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
        const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

        // Create first object
        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const float       MAX_LF1  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            const int         LINE1    = DATA[ti].d_line;
            const int         INDEX1   = DATA[ti].d_index;
            const char *const SPEC1    = DATA[ti].d_spec;

            const size_t      LENGTH1     = strlen(SPEC1);
            const size_t      NUM_BUCKETS = predictNumBuckets(LENGTH1,
                                                              MAX_LF1);

           if (veryVerbose) { T_ P_(LINE1) P_(INDEX1) P_(LENGTH1) P(SPEC1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(HASH, EQUAL, NUM_BUCKETS, MAX_LF1, &scratch);
                const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int lfj = 0; lfj != lfi; ++lfj) {
            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const float       MAX_LF2  = DEFAULT_MAX_LOAD_FACTOR[lfj];

                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2      = strlen(SPEC2);
                const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                                   MAX_LF2);

                if (veryVerbose) {
                              T_ T_ P_(LINE2) P_(INDEX2) P_(LENGTH2) P(SPEC2) }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(HASH, EQUAL, NUM_BUCKETS,  MAX_LF1, &xa);
                    const Obj& X = gg(&mX, SPEC1);
                    Obj mY(HASH, EQUAL, NUM_BUCKETS2, MAX_LF2, &ya);
                    const Obj& Y = gg(&mY, SPEC2);

                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH1, X.size(),
                            LENGTH1 == X.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF1, X.maxLoadFactor(),
                            MAX_LF1 == X.maxLoadFactor());
                    ASSERTV(LINE1, LINE2, CONFIG, LENGTH2, Y.size(),
                            LENGTH2 == Y.size());
                    ASSERTV(LINE1, LINE2, CONFIG, MAX_LF2, Y.maxLoadFactor(),
                            MAX_LF2 == Y.maxLoadFactor());

                    if (veryVerbose) { T_ T_ P_(X) P(Y); }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&xa);
                    bslma::TestAllocatorMonitor oaym(&ya);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, oaym.isTotalSame());

                    if (!HAVE_TESTED_DISTINCT_PERMUTATIONS && EXP) {
                        // Walk the lists of both 'Y' and 'X' to see if they
                        // they are distinct permutations.
                        Link *yCursor = Y.elementListRoot();
                        Link *xCursor = X.elementListRoot();

                        while (yCursor) {
                            ASSERT(xCursor); // lists should be the same length

                            if (!(ImpUtil::extractValue<KEY_CONFIG>(yCursor) ==
                                  ImpUtil::extractValue<KEY_CONFIG>(xCursor)))
                            {
                                HAVE_TESTED_DISTINCT_PERMUTATIONS = true;
                                break;
                            }
                            yCursor = yCursor->nextLink();
                            xCursor = xCursor->nextLink();
                        }
                    }
                }
            }
            }
        }
        }
    }

    ASSERT(HAVE_TESTED_DISTINCT_PERMUTATIONS);
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //   Ensure each basic accessor:
    //     - elementListRoot
    //     - size
    //     - allocator
    //   properly interprets object state.
    //
    // Concerns:
    //: 1 Each accessor returns the value of the correct property of the
    //:   object.
    //:
    //: 2 Each accessor method is declared 'const'.
    //:
    //: 3 No accessor allocates any memory.
    //:
    //: 4 'elementListRoot' refers to the root of a list with exactly 'size()'
    //:   elements, and a null pointer value if 'size() == 0'.
    //:
    //: 5 'bucketAtIndex' returns a valid bucket for all 0 <= index <
    //:   'numBuckets'.
    //:
    //: 6 QoI: Assert precondition violations for 'bucketAtIndex' when
    //:   'size <= index' are detected in SAFE builds.
    //:
    //: 7 For any value of key, 'bucketIndexForKey' returns a bucket number
    //:   less than 'numBuckets'.
    //
    // Plan:
    //: 1 For each set of 'SPEC' of different length:
    //:
    //:   1 Value construct the object with various configuration:
    //:
    //:     1 Use the 'gg' function to populate the object based on the SPEC.
    //:
    //:     2 Verify the correct allocator is installed with the
    //:       'allocator' method.
    //:
    //:     3 Verify the object all attributes are as expected.
    //:
    //:     4 Use 'verifyListContents' to validate the list rooted at
    //:       'elementListRoot'.
    //:
    //:     5 TBD: Use 'validateBucket to validate the buckets returned by
    //:       'bucketAtIndex'.
    //:
    //:     6 Monitor the memory allocated from both the default and object
    //:       allocators before and after calling the accessor; verify that
    //:       there is no change in total memory allocation.  (C-3)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered for invalid attribute values, but not triggered for
    //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   allocator() const;
    //*  comparator() const;
    //*  hasher() const;
    //*  size() const;
    //*  numBuckets() const;
    //*  maxLoadFactor() const;
    //*  elementListRoot() const;
    //*  bucketAtIndex(SizeType index) const;
    //*  bucketIndexForKey(const KeyType& key) const;
    //*  countElementsInBucket(SizeType index) const;
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    static const struct {
        int         d_line;           // source line number
        const char *d_spec;           // specification string
        float       d_maxLoadFactor;  // max load factor
        size_t      d_numBuckets;     // number of buckets
        const char *d_results;        // expected results
    } DATA[] = {
        //line  spec                 result
        //----  --------             ------
        { L_,   "",       1.0f,   1,  ""       },
        { L_,   "A",      0.9f,   2,  "A"      },
        { L_,   "AB",     0.8f,   3,  "AB"     },
        { L_,   "ABC",    0.7f,   5,  "ABC"    },
        { L_,   "ABCD",   0.6f,   8,  "ABCD"   },
        { L_,   "ABCDE",  0.5f,  13,  "ABCDE"  }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (verbose) {
        printf("\nCreate objects with various allocator configurations.\n");
    }
    {
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE         = DATA[ti].d_line;
            const char *const SPEC         = DATA[ti].d_spec;
            const size_t      LENGTH       = strlen(DATA[ti].d_results);
            const float       MAX_LF       = DATA[ti].d_maxLoadFactor;
            const size_t      NUM_BUCKETS  = DATA[ti].d_numBuckets;

            bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
            const TestValues  EXP(DATA[ti].d_results, &tda);

            // We can now provide a better estimate for number of buckets
//            const size_t NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            HASHER hash = HASH;
            COMPARATOR comp = EQUAL;
#if defined(AJM_HAS_IMPLMENTED_GENERIC_STATEFUL_FUNCTOR_CHECKS)
            setHasherState(bsls::Util::addressOf(hash), ti);
            setComparatorState(bsls::Util::addressOf(comp), ti);
#endif

            if (veryVerbose) { P_(LINE) P_(LENGTH) P(SPEC); }

            for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
                const char CONFIG = *cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // There is no easy way to create this guard for the specific
                // test case of the stateless 'bsltf::StdTestAllocator', nor
                // without second guessing the allocator to use based upon the
                // 'cfg' code.  By the time we return from 'makeAllocator' the
                // test allocator will already have been installed, with no
                // easy way to restore at the end of the test case.

                bsltf::StdTestAllocatorConfigurationGuard bsltfAG('a' == CONFIG
                                                                       ? &sa
                                                                       : &da);

                // ------------------------------------------------------------

                if (veryVerbose) {
                    printf("\n\tTesting bootstrap constructor.\n");
                }

                Obj       *objPtr;
                ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                           CONFIG,
                                                           &fa,
                                                           &sa,
                                                           hash,
                                                           comp,
                                                           NUM_BUCKETS,
                                                           MAX_LF);

                // Verify any attribute allocators are installed properly.

                const bslma::TestAllocator  *oa =
                                                extractTestAllocator(expAlloc);
                const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

                // It is important that these allocators are found, or else the
                // following tests will break severely, dereferencing null
                // pointer.
                BSLS_ASSERT_OPT(oa);
                BSLS_ASSERT_OPT(noa);

                const unsigned INTIIAL_BLOCKS = 0 == NUM_BUCKETS ? 0 : 1;

                // Verify we are starting with a known, expected, number of
                // allocations based on the number of requested buckets.
                ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG, oa->numBlocksTotal(),
                        INTIIAL_BLOCKS == oa->numBlocksTotal());
                ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG, noa->numBlocksTotal(),
                        0 == noa->numBlocksTotal());

                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);

                ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

                // --------------------------------------------------------

                // Verify basic accessors

                bslma::TestAllocatorMonitor oam(oa);

#if defined(AJM_HAS_IMPLMENTED_GENERIC_STATEFUL_FUNCTOR_CHECKS)
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualComparator(comp, X.comparator()));
                ASSERTV(LINE, SPEC, CONFIG,
                        isEqualHasher(hash, X.hasher()));
#endif
                ASSERTV(LINE, SPEC, CONFIG, NUM_BUCKETS, X.numBuckets(),
                        NUM_BUCKETS <= X.numBuckets());
                ASSERTV(LINE, SPEC, CONFIG, MAX_LF, X.maxLoadFactor(),
                        MAX_LF == X.maxLoadFactor());
                ASSERTV(LINE, SPEC, CONFIG, LENGTH, X.size(),
                        LENGTH == X.size());

                ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           EQUAL,
                                                           EXP,
                                                           LENGTH));

                ASSERT(oam.isTotalSame());

                // --------------------------------------------------------

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify no allocation from the non-object allocator.

#if !defined(BDE_BUILD_TARGET_SAFE_2)
                // The invariant check in the destructor uses the default
                // allocator in SAFE_2 builds.
                ASSERTV(LINE, CONFIG, noa->numBlocksTotal(),
                        0 == noa->numBlocksTotal());
#endif

                // Verify all memory is released on object destruction.

                ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());
                ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
    }

    // Create some fresh allocators to use validating final corners of the
    // constructor behavior.  These are special case tests, and will not need
    // to loop many times, or test the contents of the container, so one set of
    // allocators will suffice to the end of the test case.

    bslma::TestAllocator na("negative testing", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific
    // test case of the stateless 'bsltf::StdTestAllocator', nor
    // without second guessing the allocator to use based upon the
    // 'cfg' code.  By the time we return from 'makeAllocator' the test
    // allocator will already have been installed, with no easy way to
    // restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&na);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&na);

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) printf("\t'bucketAtIndex'\n");
        {
            Obj mX(HASH, EQUAL, 1, 1.0f, objAlloc);  const Obj& X = mX;
            size_t numBuckets = X.numBuckets();
            if (0 < numBuckets) { // always true, but needed for warnings
                ASSERT_SAFE_PASS(X.bucketAtIndex(numBuckets - 1));
                ASSERT_SAFE_FAIL(X.bucketAtIndex(numBuckets));
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS gg AND ggg:
    //   Having demonstrated that our primary manipulators work as expected
    //   under normal conditions
    //
    // Concerns:
    //: 1 Valid generator syntax produces expected results
    //:
    //: 2 Invalid syntax is detected and reported.
    //:
    //: 3 'verifyListContents' confirms there is a one-to-one mapping between
    //:   the supplied list and the expected values array, or both are empty.
    //:
    //: 4 'isValidHashTable' returns 'true' if the supplied arguments can
    //:   create a well-formed hash table anchor, and 'false' otherwise.
    //
    // Plan:
    //: 1 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length:
    //:
    //:   1 Use the primitive generator function 'gg' to set the state of a
    //:     newly created object.
    //:
    //:   2 Verify that 'gg' returns a valid reference to the modified argument
    //:     object.
    //:
    //:   3 Use the basic accessors to verify that the value of the object is
    //:     as expected.  (C-1)
    //:
    //: 2 For each of an enumerated sequence of 'spec' values, ordered by
    //:   increasing 'spec' length, use the primitive generator function 'ggg'
    //:   to set the state of a newly created object.
    //:
    //:   1 Verify that 'ggg' returns the expected value corresponding to the
    //:     location of the first invalid value of the 'spec'.  (C-2)
    //
    // Testing:
    //*  int ggg(HashTable *object, const char *spec, int verbose = 1);
    //*  HashTable& gg(HashTable *object, const char *spec);
    //*  verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    //*  bool isValidHashTable(Link *, const HashTableBucket&, int numBuckets);
    // ------------------------------------------------------------------------

    bslma::TestAllocator oa("generator allocator", veryVeryVerbose);

    // There is no easy way to create this guard for the specific
    // test case of the stateless 'bsltf::StdTestAllocator', nor
    // without second guessing the allocator to use based upon the
    // 'cfg' code.  By the time we return from 'makeAllocator' the test
    // allocator will already have been installed, with no easy way to
    // restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&oa);

    const HASHER     HASH  = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR EQUAL = MakeDefaultFunctor<COMPARATOR>::make();

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        static const struct {
            int         d_line;                 // source line number
            const char *d_spec;                 // specification string
            const char *d_results;  // expected element values
        } DATA[] = {
            //line  spec      results
            //----  --------  -------
            { L_,   "",       ""      },
            { L_,   "A",      "A"     },
            { L_,   "B",      "B"     },
            { L_,   "AB",     "AB"    },
            { L_,   "CD",     "CD"    },
            { L_,   "ABC",    "ABC"   },
            { L_,   "ABCD",   "ABCD"  },
            { L_,   "ABCDE",  "ABCDE" },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const size_t      LENGTH = strlen(DATA[ti].d_results);

            bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
            const TestValues  EXP(DATA[ti].d_results, &tda);

            const int         curLen = (int)strlen(SPEC);

            Obj mX(HASH, EQUAL, LENGTH, 1.0f, objAlloc);
            const Obj& X = gg(&mX, SPEC);   // original spec

            if (curLen != oldLen) {
                if (verbose) printf("\tof length %d:\n", curLen);
                ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                oldLen = curLen;
            }

            if (veryVerbose) {
                printf("\t\tSpec = \"%s\"\n", SPEC);
                T_ T_ T_ P(X);
            }

            ASSERTV(LINE, LENGTH == X.size());
            ASSERT(0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                       EQUAL,
                                                       EXP,
                                                       LENGTH));
        }
    }

    if (verbose) printf("\nTesting generator on invalid specs.\n");
    {
        static const struct {
            int         d_line;     // source line number
            const char *d_spec;     // specification string
            int         d_index;    // offending character index
        } DATA[] = {
            //line  spec      index
            //----  --------  -----
            { L_,   "",       -1,     }, // control

            { L_,   "A",      -1,     }, // control
            { L_,   " ",       0,     },
            { L_,   ".",       0,     },
            { L_,   "E",       -1,    }, // control
            { L_,   "a",       0,     },
            { L_,   "z",       0,     },

            { L_,   "AE",     -1,     }, // control
            { L_,   "aE",      0,     },
            { L_,   "Ae",      1,     },
            { L_,   ".~",      0,     },
            { L_,   "~!",      0,     },
            { L_,   "  ",      0,     },

            { L_,   "ABC",    -1,     }, // control
            { L_,   " BC",     0,     },
            { L_,   "A C",     1,     },
            { L_,   "AB ",     2,     },
            { L_,   "?#:",     0,     },
            { L_,   "   ",     0,     },

            { L_,   "ABCDE",  -1,     }, // control
            { L_,   "aBCDE",   0,     },
            { L_,   "ABcDE",   2,     },
            { L_,   "ABCDe",   4,     },
            { L_,   "AbCdE",   1,     }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        size_t oldLen = 99;  // flag value longer than the longest 'SPEC'
        for (int ti = 0; ti != NUM_DATA ; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const char *const SPEC   = DATA[ti].d_spec;
            const int         INDEX  = DATA[ti].d_index;
            const size_t      LENGTH = strlen(SPEC);

            Obj mX(HASH, EQUAL, LENGTH, 1.5f, objAlloc);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n", LENGTH);
                if (99 != oldLen) {  // i.e., not first pass
                    ASSERTV(LINE, oldLen,  LENGTH,
                                  oldLen < LENGTH);  // non-decreasing
                }
                oldLen = LENGTH;
            }

            if (veryVerbose) printf("\t\tSpec = \"%s\"\n", SPEC);

            int RESULT = ggg(&mX, SPEC, veryVerbose);

            ASSERTV(LINE, INDEX == RESULT);
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that a 'HashTable' object can be constructed into
    //   a (valid) default state, then through use of manipulators brought into
    //   any other valid state default constructor, and finally that the object
    //   destroys all its elements and leaks no memory on destruction.  For the
    //   purposes of testing, the default state will be a 'HashTable' having no
    //   elements, having a default-constructed allocator, and having a hasher
    //   and comparator supplied by the 'makeDefaultFunctor' factory function
    //   (which provides a default-constructed functor where available, and is
    //   specialized to provide a standard functor object otherwise), and
    //   initially having no buckets.  The primary manipulators will be a
    //   free function that inserts an element of a specific type (created for
    //   the purpose of testing) and the 'removeAll' method.
    //
    // Concerns:
    //: 1 An object created with the value constructor (with or without a
    //:   supplied allocator) has the supplied hasher, comparator, allocator
    //:   and maxLoadFactor, and at least the supplied initial number of
    //:   buckets.
    //:
    //: 2 The number of buckets is 1 or a prime number.
    //:
    //: 3 If the allocator is a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default allocator in effect at
    //:   the time of construction becomes the object allocator for the
    //:   resulting object.
    //:
    //: 4 If the allocator is not a 'bsl::allocator' and an allocator is NOT
    //:   supplied to the value constructor, the default constructed allocator
    //:   becomes the object allocator for the resulting object.
    //:
    //: 5 If an allocator IS supplied to the default constructor, that
    //:   allocator becomes the object allocator for the resulting object.
    //:
    //: 6 If the allocator is a 'bsl::allocator', supplying a null allocator
    //:   address has the same effect as not supplying an allocator.
    //:
    //: 7 Supplying an allocator to the value constructor has no effect on
    //:   subsequent object values.
    //:
    //: 8 Any memory allocation is from the object allocator.
    //:
    //: 9 There is no temporary allocation from any allocator.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //:
    //:11 QoI: The value constructor allocates no memory if the initial number
    //:   of bucket is 0.
    //:
    //:12 'insertElement' increase the size of the object by 1.
    //:
    //:13 'insertElement' returns the address of the newly added element.
    //:
    //:14 'insertElement' puts the element into the list of element defined by
    //:   'elementListRoot'.
    //:
    //:15 'insertElement' adds an additional element in the bucket returned by
    //:   the 'bucketFromKey' method.
    //:
    //:16 'insertElement' returns a null pointer if adding one more element
    //:   will exceed the 'maxLoadFactor'.
    //:
    //:17 Elements having the same keys (retrieved from the 'extractKey' method
    //:   of the KEY_CONFIG) according to the supplied comparator are inserted
    //:   contiguously of the beginning of the range of existing equivalent
    //:   elements, without changing their relative order.
    //:
    //:18 'removeAll' properly destroys each contained element value.
    //:
    //:19 'removeAll' does not allocate memory.
    //:
    //:20 Any argument can be 'const'.
    //:
    //:21 Any memory allocation is exception neutral.
    //:
    //:22 The constructor fails by throwing a 'std::length_error' if the
    //:   initial length of the bucket array cannot be computed.
    //:
    //:23 The constructor fails by the allocator throwing an exception if the
    //:   initial bucket array is too large to allocate.
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //:   2 Using a loop-based approach, value-construct three distinct empty
    //:     objects, in turn, but configured differently: (a) without passing
    //:     an allocator, (b) passing a null allocator address explicitly,
    //:     and (c) passing the address of a test allocator distinct from the
    //:     default.  For each of these three iterations:  (C-1..14)
    //:
    //:     1 Create three 'bslma::TestAllocator' objects, and install one as
    //:       the current default allocator (note that a ubiquitous test
    //:       allocator is already installed as the global allocator).
    //:
    //:     2 Use the default constructor to dynamically create an object
    //:       'X', with its object allocator configured appropriately (see
    //:       P-2); use a distinct test allocator for the object's footprint.
    //:
    //:     3 Use the (as yet unproven) 'allocator' to ensure that its
    //:       object allocator is properly installed.  (C-2..4)
    //:
    //:     4 Use the appropriate test allocators to verify that no memory is
    //:       allocated by the default constructor.  (C-9)
    //:
    //:     5 Use the individual (as yet unproven) salient attribute accessors
    //:       to verify the default-constructed value.  (C-1)
    //:
    //:     6 Insert 'L - 1' elements in order of increasing value into the
    //:       container.
    //:
    //:     7 Insert the 'L'th value in the presense of exception and use the
    //:       (as yet unproven) basic accessors to verify the container has the
    //:       expected values.  Verify the number of allocation is as expected.
    //:       (C-5..6, 14..15)
    //:
    //:     8 Verify that no temporary memory is allocated from the object
    //:       allocator.  (C-7)
    //:
    //:     9 Invoke 'clear' and verify that the container is empty.  Verify
    //:       that no memory is allocated.  (C-12..13)
    //:
    //:    10 Verify that all object memory is released when the object is
    //:       destroyed.  (C-8)
    //:
    //:    11 Insert 'L' distinct elements and record the iterators returned.
    //:
    //:    12 Insert the same 'L' elements again and verify that incrementing
    //:       the iterators returned gives the iterator to the next smallest
    //:       value.
    //:
    //:    13 Perform P-1.2.12 again.  (C-11)
    //
    //
    // Testing:
    //*  HashTable(HASHER, COMPARATOR, SizeType, float, ALLOC)
    //*  ~HashTable();
    //*  void removeAll();
    //*  insertElement      (test driver function, proxy for basic manipulator)
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);  // Contains 52 distinct increasing values.
    const size_t MAX_LENGTH = 9;    // This should be sufficient to bootstrap.

    {
        // Reassert a global invariant, that should not be necessary to state,
        // but did indeed turn up a real bug.

        bslma::Allocator *installedAlloc =
                     bsltf::StdTestAllocatorConfiguration::delegateAllocator();
        ASSERTV(installedAlloc,   g_bsltfAllocator_p,
                installedAlloc == g_bsltfAllocator_p);
    }

    // Probably want to pick these up as values from some injected policy, so
    // that we can test with stateful variants.  Alternatively, pass some seed
    // to the 'make' function to support stateful functors actually having
    // different states.  Note that we need the 'make' function to supply a
    // default state for functors that are not default constructible.
    const HASHER     HASH    = MakeDefaultFunctor<HASHER>::make();
    const COMPARATOR COMPARE = MakeDefaultFunctor<COMPARATOR>::make();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (verbose) printf("\nTesting with various allocator configurations.\n");

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (size_t ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // There is no easy way to create this guard for the specific
            // test case of the stateless 'bsltf::StdTestAllocator', nor
            // without second guessing the allocator to use based upon the
            // 'cfg' code.  By the time we return from 'makeAllocator' the test
            // allocator will already have been installed, with no easy way to
            // restore at the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG('a' == CONFIG
                                                                   ? &sa
                                                                   : &da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting bootstrap constructor.\n");
            }

            const size_t NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            Obj       *objPtr;
            ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                       CONFIG,
                                                       &fa,
                                                       &sa,
                                                       HASH,
                                                       COMPARE,
                                                       NUM_BUCKETS,
                                                       MAX_LF);
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

            const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
            const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null
            // pointer.
            BSLS_ASSERT_OPT(oa);
            BSLS_ASSERT_OPT(noa);

            // QoI: Verify no allocation from the object/non-object allocators
            // if no buckets are requested (as per the default constructor).
            if (0 == LENGTH) {
                ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                        0 ==  oa->numBlocksTotal());
            }
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Record blocks used by the initial bucket array
            const bsls::Types::Int64 INITIAL_OA_BLOCKS  = oa->numBlocksTotal();

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.
            // None of these accessors are deemed tested until their own test
            // case, but many witnesses give us some confidence in the state.
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            // Verify that remove-all on a default container has no effect.
            // Specifically, no memory allocated, and the root of list and
            // bucket array are unchanged.

            mX.removeAll();

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(MAX_LF, LENGTH, CONFIG, oa->numBlocksTotal(),
                    INITIAL_OA_BLOCKS ==  oa->numBlocksTotal());
            ASSERTV(MAX_LF, LENGTH, CONFIG, noa->numBlocksTotal(),
                    0 == noa->numBlocksTotal());

            // Verify attributes of an empty container.
            // Note that not all of these attributes are salient to value.

            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 < X.numBuckets());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
            ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.countElementsInBucket(0));

            const bslalg::HashTableBucket& bucket2 = X.bucketAtIndex(0);
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.first());
            ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == bucket.last());

            ASSERTV(MAX_LF, LENGTH, CONFIG, &bucket == &bucket2);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting 'insertElement' (bootstrap function).\n");
            }
            if (0 < LENGTH) {
                if (veryVerbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa->numBlocksTotal(),   oa->numBlocksInUse(),
                        oa->numBlocksTotal() == oa->numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPARE,
                                                           VALUES,
                                                           LENGTH));
                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0;j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    SizeType i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf("\n\tTesting 'removeAll'.\n"); }
            {
                const bsls::Types::Int64 BB = oa->numBlocksTotal();

                mX.removeAll();

                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.size());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0  < X.numBuckets());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0 == X.elementListRoot());
                ASSERTV(MAX_LF, LENGTH, CONFIG, MAX_LF == X.maxLoadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG, 0.0f == X.loadFactor());
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        0 == X.countElementsInBucket(0));

                const bsls::Types::Int64 AA = oa->numBlocksTotal();

                ASSERTV(MAX_LF, LENGTH, CONFIG, BB == AA);
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                  "\n\tRepeat testing 'insertElement', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (veryVerbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n", LENGTH);

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(VALUES[LENGTH - 1]),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // check elements with equivalent keys are contiguous
                // check expected elements are present in container, with
                // expected number of duplicates
                {
                    int *foundKeys = new int[X.size()];
                    for (SizeType j = 0; j != X.size(); ++j) {
                        foundKeys[j] = 0;
                    }

                    size_t i = 0;
                    for (Link *it = X.elementListRoot();
                         0 != it;
                         it = it->nextLink(), ++i)
                    {
                        for (SizeType j = 0; j != X.size(); ++j) {
                            if (BSL_TF_EQ(
                                        KEY_CONFIG::extractKey(VALUES[j]),
                                        ImpUtil::extractKey<KEY_CONFIG>(it))) {
                                ASSERTV(MAX_LF, LENGTH, CONFIG, VALUES[j],
                                        !foundKeys[j]);
                                ++foundKeys[j];
                            }
                        }
                    }
                    SizeType missing = 0;
                    for (SizeType j = 0; j != X.size(); ++j) {
                        if (!foundKeys[j]) { ++missing; }
                    }
                    ASSERTV(MAX_LF, LENGTH, CONFIG, missing, 0 == missing);

                    delete[] foundKeys;

                    ASSERTV(MAX_LF, LENGTH, CONFIG, X.size() == i);
                }
            }

            // ----------------------------------------------------------------

            if (veryVerbose) { printf(
                                "\n\tTesting 'insert' duplicated values.\n"); }
            {
                Link *ITER[MAX_LENGTH + 1];

                // The first loop adds a duplicate in front of each already
                // inserted element
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    ITER[tj] = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != ITER[tj]);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(VALUES[tj]),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        2 * LENGTH == X.size());

                // The second loop adds another duplicate in front of each
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(VALUES[tj]),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                         3 * LENGTH == X.size());
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(MAX_LF, LENGTH, CONFIG, da.numBlocksInUse(),
                    0 == da.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, fa.numBlocksInUse(),
                    0 == fa.numBlocksInUse());
            ASSERTV(MAX_LF, LENGTH, CONFIG, sa.numBlocksInUse(),
                    0 == sa.numBlocksInUse());
        }
    }
    }

    // ------------------------------------------------------------------------

    // Create some fresh allocators to use validating final corners of the
    // constructor behavior.  These are special case tests, and will not need
    // to loop many times, or test the contents of the container, so one set of
    // allocators will suffice to the end of the test case.

    bslma::TestAllocator da("corner-testing", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific
    // test case of the stateless 'bsltf::StdTestAllocator', nor
    // without second guessing the allocator to use based upon the
    // 'cfg' code.  By the time we return from 'makeAllocator' the test
    // allocator will already have been installed, with no easy way to
    // restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&da);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&da);

#if defined BDE_BUILD_TARGET_EXC
    if (verbose) printf(
                  "\nTesting correct exceptions are thrown by constructor.\n");
    {
#if 0
        // This test shows up a non-conformance in 'bsl::allocator'
        // which has undefined behavior when asked for this many
        // buckets, rather than simply throwing a 'std::bad_alloc'.

        try {
            Obj mX(HASH,
                   COMPARE,
                   native_std::numeric_limits<int>::max(),
                   1e-30f,
                   objAlloc);
            ASSERT(false);
        }
        catch(const native_std::bad_allocr&) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!!"The wrong exception type was thrown.");
        }
#endif

        try {
            Obj mBad(HASH,
                     COMPARE,
                     native_std::numeric_limits<SizeType>::max(),
                     1e-30f,
                     objAlloc);
            ASSERT(false);
        }
        catch(const native_std::length_error&) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!!"The wrong exception type was thrown.");
        }

        Obj mR(HASH,
               COMPARE,
               3,
               1e-30f,
               objAlloc);
        try {
            mR.insert(VALUES[0]);

            P(mR.numBuckets())
            P(mR.size());

            ASSERT(false);
        }
        catch(const native_std::length_error& e) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!!"The wrong exception type was thrown.");
        }

    }
#endif

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const float FLT_NAN = std::numeric_limits<float>::quiet_NaN();
        const float FLT_INF = std::numeric_limits<float>::infinity();
        const float NEG_INF = -std::numeric_limits<float>::infinity();

        ASSERT_SAFE_PASS_RAW(Obj(HASH, COMPARE, 0, 1.0f,  objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, 0.0f,  objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, -0.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, -1.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, NEG_INF, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, FLT_NAN, objAlloc));
        ASSERT_SAFE_PASS_RAW(Obj(HASH, COMPARE, 0, FLT_INF, objAlloc));
    }
}

//=============================================================================
//                              TEST CASE FUNCTIONS
//-----------------------------------------------------------------------------
// In order to reduce the complexity of the 'switch' statement in 'main' that
// was getting to large for some compilers to handle, we have migrated the
// invokation of each test case into its own function.
//-----------------------------------------------------------------------------
void mainTestCaseUsageExample()
{
        // This case number will rise as remaining tests are implemented.

        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        UsageExamples::main1();
        UsageExamples::main2();
        UsageExamples::main3();
        UsageExamples::main4();

}
#if 0  // Planned test cases, not yet implemented
static
void mainTestCase23()
{
        // --------------------------------------------------------------------
        // TESTING PUBLIC TYPEDEFS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase23,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase22()
{
        // --------------------------------------------------------------------
        // TESTING "max" FUNCTIONS
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase22,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase21()
{
        // --------------------------------------------------------------------
        // TESTING 'countElementsInBucket'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase21,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase20()
{
        // --------------------------------------------------------------------
        // TESTING 'findRange'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase20,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase19()
{
        // --------------------------------------------------------------------
        // TESTING 'find'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase18()
{
        // --------------------------------------------------------------------
        // TESTING 'insertIfMissing(const KeyType& key);
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase18,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase17()
{
        // --------------------------------------------------------------------
        // TESTING 'insertIfMissing(bool *, VALUE)'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase17,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase16()
{
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase16,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase15()
{
        // --------------------------------------------------------------------
        // TESTING 'setMaxLoadFactor'
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase15,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

#endif

static
void mainTestCase14()
{
        // --------------------------------------------------------------------
        // DEFAULT CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting default constructor"
                            "\n===========================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting non-copyable functors"
                            "\n-----------------------------\n");
        // This is the use case we are most concerned about.
        // We probably want to test with a smattering of the following
        // concerns as well, notably with the different allocator patterns.
        RUN_EACH_TYPE(TestDriver_DefaultOnlyFunctors,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // These configurations not available, as functors are not default
        // constructible.

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // The stateless allocator flags issues installing the chosen allocator
        // when it is not the default.

        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");

        // These 3 cases do not have unique keys, which fools the final part of
        // the basic test case.  Will review test logic later, to re-enable
        // these tests.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

#endif

    // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase14();
        TestDriver_AwkwardMaplikeForDefault::testCase14();
}

static
void mainTestCase13()
{
        // --------------------------------------------------------------------
        //  TESTING 'setMaxLoadFactor'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'setMaxLoadFactor'"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase14,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase13,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

#endif

        // Remaining special cases
        TestDriver_AwkwardMaplike::testCase13();
}

static
void mainTestCase12()
{
        // --------------------------------------------------------------------
        // TESTING 'insert'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'insert'"
                            "\n==========================\n");

        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase12,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase11()
{
        // --------------------------------------------------------------------
        // TESTING 'rehashFor...' FUNCTIONS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting 'rehashFor...' functions"
                            "\n================================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");

        // These 3 cases do not have unique keys, which fools the final part of
        // the basic test case.  Will review test logic later, to re-enable
        // these tests.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase11();
}

static
void mainTestCase10()
{
        // --------------------------------------------------------------------
        // STREAMING FUNCTIONALITY
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Streaming Functionality"
                            "\n===============================\n");

        if (verbose) printf("There is no streaming for this component.\n");
}

static
void mainTestCase9()
{
        // --------------------------------------------------------------------
        // ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Assignment Operator"
                            "\n===========================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // degenerate functors are not CopyAssignable, and rely on the
        // copy/swap idiom for the copy-assignment operator to function.

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying
        // to separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // The non-BDE allocators do not propagate the container allocator to
        // their elements, and so will make use of the default allocator when
        // making copies.  Therefore, we use a slightly different list of types
        // when testing with these allocators.

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::EnumeratedTestType::Enum,
                      bsltf::UnionTestType,
                      bsltf::SimpleTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::NonTypicalOverloadsTestType,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::EnumeratedTestType::Enum,
                      bsltf::UnionTestType,
                      bsltf::SimpleTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::NonTypicalOverloadsTestType,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE,
                      bsltf::EnumeratedTestType::Enum,
                      bsltf::UnionTestType,
                      bsltf::SimpleTestType,
                      bsltf::BitwiseMoveableTestType,
                      bsltf::NonTypicalOverloadsTestType,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

#endif  // IBM simplification

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase9();
}

static
void mainTestCase8()
{
        // --------------------------------------------------------------------
        // MANIPULATOR AND FREE FUNCTION 'swap'
        // --------------------------------------------------------------------

        if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                            "\n====================================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying
        // to separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase8,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase8();
}

static
void mainTestCase7()
{
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Copy Constructors"
                            "\n=========================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if 0
        // Revisit these tests once validated the rest.
        // Initial problem are testing the stateless allocator while trying
        // to separately configure a default and an object allocator.
        // Obvious problems with allocator-propagating tests, given the driver
        // currently expects to never propagate.
        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase7,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase7();
}

static
void mainTestCase6()
{
        // --------------------------------------------------------------------
        // EQUALITY OPERATORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Equality Operators"
                            "\n==========================\n");

        // Note that the 'NonEqualComparableTestType' is not appropriate here.
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_PLATFORM_CMP_IBM)
        // We need to limit the test coverage on IBM as the compiler cannot
        // cope with so many template instantiations.

        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        // Grouped tests
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase_GroupedSharedKeys,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                      bsltf::NonAssignableTestType,
                      bsltf::NonDefaultConstructibleTestType);
#endif

        // Remaining special cases
        TestDriverForCase6_AwkwardMaplike::testCase6();
}

static
void mainTestCase5()
{
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Output (<<) Operator"
                            "\n============================\n");

        if (verbose)
                   printf("There is no output operator for this component.\n");
}

static
void mainTestCase4()
{
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Basic Accessors"
                            "\n=======================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);


        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase4();

}

static
void mainTestCase3()
{
        // --------------------------------------------------------------------
        // GENERATOR FUNCTIONS 'gg' and 'ggg'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting generators and test machinery"
                            "\n=====================================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);


        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase3,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase3();

        // Further, need to validate the basic test facilities:
        //   verifyListContents
}

static
void mainTestCase2()
{
        // --------------------------------------------------------------------
        // BOOTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS
        //   This case is implemented as a method of a template test harness,
        //   where the runtime concerns and test plan are documented.  The test
        //   harness will be instantiated and run with a variety of types to
        //   address the template parameter concerns below.  We note that the
        //   bootstrap case has the widest variety of parameterizing concerns
        //   to test, as latest test cases may be able to place additional
        //   requirements on the types that they operate with, but the primary
        //   bootstrap has to validate bringing any valid container into any
        //   valid state for any of the later cases.
        //
        // Concerns:
        //: 1 The class bootstraps with the default template arguments for the
        //:   unordered containers as policy parameters
        //:
        //: 2 The class supports a wide variety of troublesome element types,
        //:   as covered extensively in the template test facility, 'bsltf'.
        //:
        //: 3 STL allocators that are not (BDE) polymorphic, and that never
        //:   propagate on any operations, just like BDE
        //:
        //: 4 STL allocators that are not (BDE) polymorphic, and propagate on
        //:   all possible operations.
        //:
        //: 5 functors that do not const-qualify 'operator()'
        //:
        //: 6 stateful functors
        //:
        //: 7 function pointers as functors
        //:
        //: 8 non-default-constructible functors
        //:
        //: 9 functors overloading 'operator&'
        //:
        //:10 functors overloading 'operator,'
        //:
        //:11 functors that cannot be swapped ('swap' is required to support
        //:   assignment, not bootstrap, default constructor, or most methods)
        //:
        //:12 functors whose argument(s) are convertible-form the key-type
        //:
        //:13 functors with templated function-call operators
        //:
        //:14 support for simple set-like policy
        //:
        //:15 support for basic map-like policy, where key is a sub-state of
        //:   the element's value
        //:
        //:16 support for a minimal key type, with equality-comparable element
        //:   type
        //:
        //:17 support for comparison functors returning an evil boolean-like
        //:    type
        //:
        // Additional concerns (deferred for full allocator_traits support)
        //:18 support for STL allocators returning smart pointers
        //:
        //:19 that the STL allocator functions are called if the STL allocator
        //:   supplies them (rather than always using a default incantation in
        //:   allocator_traits, for example).
        //:
        // Plan:
        //: 1 Run the test harness in a variety of configurations that each, in
        //:   turn, address the concerns and verify that the behavior is as
        //:   expected.
        //
        // Testing:
        //   BOOTSTRAP
        // --------------------------------------------------------------------

        if (verbose) printf("\nTesting Primary Manipulators"
                            "\n============================\n");

        if (verbose) printf("\nTesting basic configurations"
                            "\n----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful functors"
                            "\n-------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors"
                            "\n---------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting degenerate functors without swap"
                            "\n----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting 'bsltf' configuration"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting pointers for functors"
                            "\n-----------------------------\n");
        RUN_EACH_TYPE(TestDriver_FunctionPointers,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting functors taking generic arguments"
                            "\n-----------------------------------------\n");
        RUN_EACH_TYPE(TestDriver_GenericFunctors,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateless STL allocators"
                            "\n--------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        if (verbose) printf("\nTesting stateful STL allocators"
                            "\n-------------------------------\n");
        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation1,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation2,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation3,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        // Be sure to bootstrap the special 'grouped' configurations used in
        // test case 6.
        if (verbose) printf("\nTesting grouped hash with unique key values"
                            "\n-------------------------------------------\n");
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

#if 0
        // These 3 cases do not have unique keys, which fools the final part of
        // the basic test case.  Will review test logic later, to re-enable
        // these tests.
        RUN_EACH_TYPE(TestCase_GroupedUniqueKeys,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfiguration,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

        RUN_EACH_TYPE(TestCase6_DegenerateConfigurationNoSwap,
                      testCase2,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);
#endif

        // Remaining special cases
        if (verbose) printf("\nTesting degenerate map-like"
                            "\n---------------------------\n");
        TestDriver_AwkwardMaplike::testCase2();
}

static
void mainTestCase1()
{
    // ------------------------------------------------------------------------
    // BREATHING TEST
    //   This case exercises (but does not fully test) basic functionality.
    //
    // Concerns:
    //: 1 The class is sufficiently functional to enable comprehensive testing
    //:   in subsequent test cases.
    //
    // Plan:
    //: 1 Execute each method to verify functionality for simple case.
    //
    // Testing:
    //   BREATHING TEST
    // ------------------------------------------------------------------------

    if (verbose) printf("\nBREATHING TEST"
                        "\n==============\n");

    typedef BasicKeyConfig<int>  KEY_CONFIG;

    typedef bslstl::HashTable<BasicKeyConfig<int>,
                              ::bsl::hash<int>,
                              ::bsl::equal_to<int> > Obj;

    int INT_VALUES[] = { INT_MIN, -2, -1, 0, 1, 2, INT_MAX };  // *not* 'const'
    const size_t NUM_INT_VALUES = sizeof(INT_VALUES) / sizeof(*INT_VALUES);

    const ::bsl::hash<int> HASHER = ::bsl::hash<int>();
    const ::bsl::equal_to<int> COMPARATOR = ::bsl::equal_to<int>();

    bslma::TestAllocator defaultAllocator("defaultAllocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);

    bslma::TestAllocator objectAllocator("objectAllocator",
                                         veryVeryVeryVerbose);

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("Construct an empty HashTable.\n");
    {
        bslma::TestAllocator dummyAllocator("dummyAllocator",
                                            veryVeryVeryVerbose);

        // Note that 'HashTable' does not have a default constructor, so we
        // must explicitly supply a default for each attribute.
        Obj mX(HASHER, COMPARATOR, 0, 1.0f, &dummyAllocator);
        const Obj& X = mX;
        ASSERTV(0    == X.size());
        ASSERTV(0    <  X.maxSize());
        ASSERTV(0    == defaultAllocator.numBytesInUse());
        ASSERTV(0    == dummyAllocator.numBytesInUse());

        if (veryVeryVerbose) printf("Call *all* 'const' functions.\n");
        // As a simple compile-check of the template, call every 'const'
        // function member, unless they require a non-empty container.
        ::bsl::hash<int>     h = X.hasher();      (void)h;
        ::bsl::equal_to<int> c = X.comparator();  (void)c;
        bsl::allocator<int>  a = X.allocator();
        ASSERTV(&dummyAllocator == a.mechanism());

        ASSERTV(0 == X.loadFactor());
        ASSERTV(1.0f == X.maxLoadFactor());

        //const int K = 0;
        (void)X.bucketIndexForKey(42);

        bslalg::BidirectionalLink *first, *last;
        X.findRange(&first, &last, 42);
        ASSERTV(0 == first);
        ASSERTV(0 == last);

        first = X.elementListRoot();
        last  = X.find(42);
        ASSERTV(0 == first);
        ASSERTV(0 == last);

        ASSERTV(X.numBuckets() < X.maxNumBuckets());

        bslalg::HashTableBucket bkt = X.bucketAtIndex(0); (void)bkt;
        ASSERTV(0 == X.countElementsInBucket(0));

        ASSERTV(X == X);
        ASSERTV(!(X != X));

        if (veryVeryVerbose) printf("Call *all* modifier functions.\n");
        // As a simple compile-check of the template, call every remaining
        // function member.
        swap(mX, mX);
        ASSERTV(mX == X);
        ASSERTV(!(X != mX));

        bslalg::BidirectionalLink *newLink = mX.insert(INT_VALUES[0]);
        newLink = X.findEndOfRange(newLink);  // last 'const' method to check
        newLink = mX.insert(INT_VALUES[0], X.elementListRoot());
        mX = X;
        mX.swap(mX);
        ASSERTV(mX == X);
        ASSERTV(!(X != mX));

        bool missing;
        newLink = mX.insertIfMissing(&missing, INT_VALUES[0]);
        ASSERTV(!missing);
        ASSERTV(X.elementListRoot() == newLink);

        const bsltf::ConvertibleValueWrapper<int> val(INT_VALUES[0]);
        newLink = mX.insertIfMissing(&missing, val);
        ASSERTV(!missing);
        ASSERTV(X.elementListRoot() == newLink);

       // This makes sense only if 'Value' is a 'pair'.
 //        (void)mX.insertIfMissing(K);

        newLink = mX.remove(newLink);
        mX.removeAll();

        mX.rehashForNumBuckets(0);
        mX.reserveForNumElements(0);

#if defined(BDE_BUILD_TARGET_EXC)
        // The call to 'setMaxLoadFactor' may try to allocate a lot of memory
        // and is known to throw 'bad_alloc' exceptions on AIX test runners.
        try {
            mX.setMaxLoadFactor(9e-9);
        }
        catch(const std::exception& e) {
            if (veryVeryVerbose) {
                printf("exception: '%s'\n", e.what());
            }
        }
#endif
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("Test use of allocators.\n");
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1",
                                              veryVeryVeryVerbose);
        bslma::TestAllocator objectAllocator2("objectAllocator2",
                                              veryVeryVeryVerbose);

        Obj o1(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
        const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            o1.insert(INT_VALUES[i]);
        }
        ASSERTV(NUM_INT_VALUES == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());
    }
    {
        bslma::TestAllocator objectAllocator1("objectAllocator1",
                                              veryVeryVeryVerbose);
        bslma::TestAllocator objectAllocator2("objectAllocator2",
                                              veryVeryVeryVerbose);

        Obj o1(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
        const Obj& O1 = o1;
        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());

        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            bool isInsertedFlag = false;
            o1.insertIfMissing(&isInsertedFlag, INT_VALUES[i]);
            ASSERTV(isInsertedFlag, true == isInsertedFlag);
        }
        ASSERTV(NUM_INT_VALUES == O1.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());
        ASSERTV(0 == objectAllocator2.numBytesInUse());


        // Copied code from below, under evaluation
        if (veryVerbose) printf("Use a different allocator\n");
        {
            bslma::TestAllocatorMonitor monitor(&objectAllocator1);
            Obj o2(O1, &objectAllocator2); const Obj& O2 = o2;
            ASSERTV(&objectAllocator2 == O2.allocator().mechanism());
            ASSERTV(monitor.isInUseSame());
            ASSERTV(monitor.isTotalSame());
            ASSERTV(0 <  objectAllocator1.numBytesInUse());
            ASSERTV(0 <  objectAllocator2.numBytesInUse());
        }
        ASSERTV(0 ==  objectAllocator2.numBytesInUse());


        if (veryVerbose) printf("Copy construct O2(O1)\n");

        Obj o2(O1, &objectAllocator1); const Obj& O2 = o2;

        ASSERTV(&objectAllocator1 == O2.allocator().mechanism());

        ASSERTV(NUM_INT_VALUES == O1.size());
        ASSERTV(NUM_INT_VALUES == O2.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        if (veryVerbose) printf("Default construct O3 and swap with O1\n");
        Obj o3(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator1);
        const Obj& O3 = o3;
        ASSERTV(&objectAllocator1 == O3.allocator().mechanism());

        ASSERTV(NUM_INT_VALUES == O1.size());
        ASSERTV(NUM_INT_VALUES == O2.size());
        ASSERTV(0         == O3.size());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        bslma::TestAllocatorMonitor monitor1(&objectAllocator1);
        o1.swap(o3);
        ASSERTV(0         == O1.size());
        ASSERTV(NUM_INT_VALUES == O2.size());
        ASSERTV(NUM_INT_VALUES == O3.size());
        ASSERTV(monitor1.isInUseSame());
        ASSERTV(monitor1.isTotalSame());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        if (veryVerbose) printf("swap O3 with O2\n");
        o3.swap(o2);
        ASSERTV(0         == O1.size());
        ASSERTV(NUM_INT_VALUES == O2.size());
        ASSERTV(NUM_INT_VALUES == O3.size());
        ASSERTV(!monitor1.isInUseUp());  // Memory usage may go down depending
                                         // on implementation
        ASSERTV(!monitor1.isTotalUp());
        ASSERTV(0 <  objectAllocator1.numBytesInUse());

        ASSERTV(&objectAllocator1 == O1.allocator().mechanism());
        ASSERTV(&objectAllocator1 == O2.allocator().mechanism());
        ASSERTV(&objectAllocator1 == O3.allocator().mechanism());
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) {
        printf("Test primary manipulators/accessors on every permutation.\n");
    }

    // Assume that the array 'INT_VALUES' is initially sorted.  That is true
    // when this test case was initially written, and should be maintained
    // through any future edits.  Otherwise, sort the array at this point.
    do {
        // For each possible permutation of values, insert values, iterate over
        // the resulting container, find values, and then erase values.

        Obj x(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator);
        const Obj& X = x;
        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            Obj y(X, &objectAllocator); const Obj& Y = y;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));

            ASSERTV(i, 0 == X.find(KEY_CONFIG::extractKey(INT_VALUES[i])));

            // Test 'insert'.
            int value = INT_VALUES[i];
            bool isInsertedFlag = false;
            Link *link = x.insertIfMissing(&isInsertedFlag, value);
            ASSERTV(0             != link);
            ASSERTV(true          == isInsertedFlag);
            ASSERTV(KEY_CONFIG::extractKey(INT_VALUES[i] ==
                                       ImpUtil::extractKey<KEY_CONFIG>(link)));
            ASSERTV(INT_VALUES[i] == ImpUtil::extractValue<KEY_CONFIG>(link));

            // Test size, empty.
            ASSERTV(i + 1 == X.size());
            ASSERTV(0 != X.size());

            // Test insert duplicate key
            ASSERTV(link    == x.insertIfMissing(&isInsertedFlag, value));
            ASSERTV(false   == isInsertedFlag);
            ASSERTV(i + 1   == X.size());

            // Test find
            Link *it     = X.find(KEY_CONFIG::extractKey(INT_VALUES[i]));
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(link) ==
                    ImpUtil::extractKey<KEY_CONFIG>(it));

            ASSERTV(X != Y);
            ASSERTV(!(X == Y));

            y = x;
            ASSERTV(X == Y);
            ASSERTV(!(X != Y));
        }

        ASSERTV(0 != objectAllocator.numBytesInUse());
        ASSERTV(0 == defaultAllocator.numBytesInUse());

        // Use remove(link) on all the elements.
        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            Link *it     = x.find(KEY_CONFIG::extractKey(INT_VALUES[i]));
            Link *nextIt = it->nextLink();

            ASSERTV(0       != it);
            ASSERTV(KEY_CONFIG::extractKey(INT_VALUES[i] ==
                                         ImpUtil::extractKey<KEY_CONFIG>(it)));
            ASSERTV(INT_VALUES[i] == ImpUtil::extractValue<KEY_CONFIG>(it));
            Link *resIt = x.remove(it);
            ASSERTV(resIt == nextIt);

            Link *resFind = x.find(KEY_CONFIG::extractKey(INT_VALUES[i]));
            ASSERTV(0 == resFind);

            ASSERTV(NUM_INT_VALUES - i - 1 == X.size());
        }
    } while (native_std::next_permutation(INT_VALUES,
                                          INT_VALUES + NUM_INT_VALUES));

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    if (veryVerbose) printf("Test 'remove(bslalg::BidirectionalLink *)'.\n");
    {
        native_std::random_shuffle(INT_VALUES,  INT_VALUES + NUM_INT_VALUES);

        Obj x(HASHER, COMPARATOR, 0, 1.0f, &objectAllocator);
        const Obj& X = x;
        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            int value = INT_VALUES[i];
            Link *result1 = x.insert(value);
            ASSERTV(0 != result1);
            Link *result2 = x.insert(value);
            ASSERTV(0 != result2);
            ASSERTV(result1 != result2);
            ASSERTV(2 * (i + 1) == X.size());

            Link *start;
            Link *end;
            int key = KEY_CONFIG::extractKey(INT_VALUES[i]);
            x.findRange(&start, &end, key);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start) == key);
            ASSERTV(ImpUtil::extractKey<KEY_CONFIG>(start->nextLink()) == key);
            ASSERTV(start->nextLink()->nextLink() == end);
        }

        for (size_t i = 0; i != NUM_INT_VALUES; ++i) {
            Link *const initialRoot = X.elementListRoot();
            int key = ImpUtil::extractKey<KEY_CONFIG>(initialRoot);
            Link *resIt1 = x.remove(X.elementListRoot());
            ASSERTV(initialRoot != resIt1);
            ASSERTV(initialRoot != X.elementListRoot());
            ASSERTV(X.elementListRoot() == resIt1);
            ASSERTV(X.find(key) == resIt1);

            ASSERTV((2 * (NUM_INT_VALUES - i) - 1),   X.size(),
                    (2 * (NUM_INT_VALUES - i) - 1) == X.size());
            Link *resIt2 = x.remove(x.elementListRoot());
            ASSERTV(x.elementListRoot() == resIt2);
            ASSERTV(resIt2 != resIt1);
            ASSERTV(X.find(key) == 0);
            ASSERTV((2 * (NUM_INT_VALUES - i - 1)),   X.size(),
                    (2 * (NUM_INT_VALUES - i - 1)) == X.size());
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    // Set this test allocator globally, up front, to avoid multiple static
    // allocator references on AIX.  This allocator should always be swapped
    // out by an allocator guard during any specific test case.

    bslma::TestAllocator bsltfAllocator("bsltf-default", veryVeryVeryVerbose);
    g_bsltfAllocator_p = &bsltfAllocator;
    bsltf::StdTestAllocatorConfiguration::setDelegateAllocatorRaw(
                                                           g_bsltfAllocator_p);

    ASSERT(bsltf::StdTestAllocatorConfiguration::delegateAllocator() ==
                                                           g_bsltfAllocator_p);

    switch (test) { case 0:
      case 15: mainTestCaseUsageExample(); break;
//      case 17: mainTestCase17(); break;
//      case 16: mainTestCase16(); break;
//      case 15: mainTestCase15(); break;
      case 14: mainTestCase14(); break;
      case 13: mainTestCase13(); break;
      case 12: mainTestCase12(); break;
      case 11: mainTestCase11(); break;
      case 10: mainTestCase10(); break;
      case  9: mainTestCase9 (); break;
      case  8: mainTestCase8 (); break;
      case  7: mainTestCase7 (); break;
      case  6: mainTestCase6 (); break;
      case  5: mainTestCase5 (); break;
      case  4: mainTestCase4 (); break;
      case  3: mainTestCase3 (); break;
      case  2: mainTestCase2 (); break;
      case  1: mainTestCase1 (); break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // There should be no allocations for the "default STL-test allocator"
    ASSERTV(bsltfAllocator.numBlocksTotal(),
            0 == bsltfAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the global allocator.
    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
