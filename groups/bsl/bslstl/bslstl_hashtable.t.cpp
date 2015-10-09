// bslstl_hashtable.t.cpp                                             -*-C++-*-
#include <bslstl_hashtable.h>

#include <bslstl_equalto.h>
#include <bslstl_hash.h>
#include <bslstl_hashtableiterator.h>  // usage example
#include <bslstl_iterator.h>           // 'distance', in usage example

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_swaputil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_exceptionguard.h>
#include <bslma_rawdeleterguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_conditional.h>
#include <bslmf_isfunction.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_removeconst.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <bsltf_convertiblevaluewrapper.h>
#include <bsltf_degeneratefunctor.h>
#include <bsltf_evilbooleantype.h>
#include <bsltf_stdstatefulallocator.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <stdexcept>  // to verify correct exceptions are thrown

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for 'strcmp'

// The following macros are used for 'printf' format strings to work around
// issues with MSVC non-standard format specifiers.

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU // An alias for a string that can be
                                      // treated as the "%zu" format

#define TD BSLS_BSLTESTUTIL_FORMAT_TD // An alias for a string that can be
                                      // treated as the "%td" format

// We note that certain test cases rely on the reference collapsing rules that
// were adopted shortly after C++03, and so are not a feature of many older
// compilers, or perhaps compilers in strictly conforming modes.  We include
// VC2010 in this set of compilers for this test driver, as there is a bug that
// triggers in type traits dealing with function references that means we
// should disable the same set of tests.

#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700)     \
 || (defined(BSLS_PLATFORM_CMP_GNU)  && BSLS_PLATFORM_CMP_VERSION < 40300)    \
 ||  defined(BSLS_PLATFORM_CMP_SUN)
#  define BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING
#endif

// Note that some compilers struggle with the number of template instantiations
// in this test driver.  We define this macro to simplify the test driver for
// them, until such time as we can provide a more specific review of the type
// based concerns, and narrow the range of tests needed for confirmed coverage.
//
// Currently we are enabling the minimal set of test types on:
// XLC                    (CMP_IBM)
// Sun Studio & Sun gcc   (OS_SOLARIS)
// clang gcc              (OS_DARWIN)
// Linux gcc 4.8+         (OS_LINUX, CMP_GNU, CMP_VERSION >= 40800)

#if !(defined(BSLS_PLATFORM_CMP_IBM)    ||  \
      defined(BSLS_PLATFORM_OS_SOLARIS) ||  \
      defined(BSLS_PLATFORM_OS_DARWIN)  ||  \
      defined(BSLS_PLATFORM_CMP_CLANG)  ||  \
      (defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 40800))
#  define BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS
#endif

#define BSLSTL_HASHTABLE_MINIMALTEST_TYPES      \
        signed char,                            \
        bsltf::TemplateTestFacility::MethodPtr, \
        bsltf::AllocBitwiseMoveableTestType,    \
        TestTypes::MostEvilTestType

// The following macro can be enabled to provide a truly minimal test driver
// (fast enough to be suitable for testing during development):
//
//  #define BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK


#if defined(BSLS_HASHTABLE_SIMPLIFY_TEST_COVERAGE_TO_SPEED_FEEDBACK)
# undef BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS
#
# undef  BSLSTL_HASHTABLE_MINIMALTEST_TYPES
# define BSLSTL_HASHTABLE_MINIMALTEST_TYPES  TestTypes::MostEvilTestType
#endif

#if 0
// Define the macro 'BSLSTL_HASHTABLE_TRIM_TEST_CASE9_COMPLEXITY' to minimize
// the runtime cost of the most time consuming test case.
# define BSLSTL_HASHTABLE_TRIM_TEST_CASE9_COMPLEXITY
#endif

#if (defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1800)
// Earlier Microsoft libraries did not have an attribute to indicate that the
// 'abort' function does not return.  We actively get warings for unreachable
// code with the VC 2013 compiler if the workaround is enabled, so should limit
// the usage based on a version check.  This code has not yet been explicitly
// tested with VC2010 or VC2012, as the actively supported platforms for
// development are VC2008, and VC2013.
# define BSLSTL_HASHTABLE_MSVC_WARNS_ON_RETURN_FROM_ABORT
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
// Similar to the Microsoft workaround above, but we have not yet had reports
// of test drivers warning about unreachable code, so no version check is
// applied.
# define BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT
#endif

using namespace BloombergLP;
using bslstl::CallableVariable;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a value-semantic container class template, whose
// elements are indexed by key, and duplicate key values are permitted.  The
// purpose of the component is to support implementing the four standard
// unordered containers, so it may be customized at compile time with a C++11
// standard conforming allocator, the most important of which is the 'bsl'
// allocator that supports runtime customization of the object allocator.
//
// The basic test plan is to employ the standard 10 test cases for a value-
// semantic type, before incrementally validating the remaining methods.  One
// important issue is the selection of primary manipulators.  The 'insert'
// operations of the container may trigger a 'rehash' operation, which is
// unrelated to value.  As this is an operation we prefer to defer testing
// until after the initial ten value-semantic test cases, we create an
// 'insertElement' function that checks if an insert would trigger a rehash
// before proceeding.  This becomes our primary manipulator, coupled with a
// strategy to always reserve enough space at construction to accommodate all
// the values inserted for that test scenario.  This is then sufficient to
// bring the object to any valid state.
//
// The Primary Manipulators and Basic Accessors are decided to be:
//
// Primary Manipulators:
//: o 'insertElement' free function in this test driver
//: o 'removeAll'
//
// Basic Accessors:
//: o 'allocator'
//: o 'comparator'
//: o 'hasher'
//: o 'size'
//: o 'numBuckets'
//: o 'maxLoadFactor'
//: o 'rehashThreshold'
//: o 'elementListRoot'
//: o 'bucketAtIndex'
//: o 'bucketIndexForKey'
//
// We note that of the basic accessors, only 'size' and 'elementListRoot'
// contribute to value; while none of the others are salient to value, they are
// important for maintaining the efficient indexing of the container, and are
// required to properly inspect the state of the container after any
// manipulation in the succeeding test cases.
//
// One unusual aspect of this class is that the default constructor creates an
// empty container with no space reserved for elements to insert.  As we are
// avoiding rehash operations during the (idiomatic) initial ten value-
// semantic class test cases, this means we must use the value constructor
// through the value-semantic bootstrap tests, as this constructor does allow
// us to specify an initial capacity.  Further, the default constructor has a
// more minimal set of requirements on the template arguments than other
// constructors, so we will test this constructor as the very final test case,
// to confirm that all the other validated functions operate correctly when
// instantiated with the more minimal contracts.
//
// As we are testing a template, we will make heavy use of the 'bsltf' template
// test facility.  Our primary means of implementing each test case will be to
// write a single function template for the case, that can test and satisfy all
// the concerns of each of the eventual instantiations.  For example, while
// testing the range of potential allocator types, and the way memory is
// consumed by the object under test, we will arrange for all allocators to
// wrap a 'bslma::TestAllocator', which can separately be queried for the
// allocator usage, regardless of the interface of the actual allocator types
// themselves.  The concerns for each test case will be split into two
// sections: the type concerns will be documented and addressed in a dispatch
// function for each case, that will address the concerns by instantiated the
// test driver function for a sufficient variety of types to be sure that such
// concerns are addressed.  The test case implementation function template will
// list the runtime behavioral concerns of the test case, regardless of the
// types it is instantiated with, and address those concerns.  This combination
// of testing will ensure thorough testing covering all combinations of concern
// for the component.
//
// TYPES
//*[20] typedef ALLOCATOR                              AllocatorType;
//*[20] typedef ::bsl::allocator_traits<AllocatorType> AllocatorTraits;
//*[20] typedef typename KEY_CONFIG::KeyType           KeyType;
//*[20] typedef typename KEY_CONFIG::ValueType         ValueType;
//*[20] typedef bslalg::BidirectionalNode<ValueType>   NodeType;
//*[20] typedef typename AllocatorTraits::size_type    SizeType;
//
// CREATORS
// [  ] HashTable(const ALLOCATOR& allocator = ALLOCATOR());
// [ 2] HashTable(const HASHER&, const COMPARATOR&, SizeType, const ALLOCATOR&)
// [ 7] HashTable(const HashTable& original);
// [ 7] HashTable(const HashTable& original, const ALLOCATOR& allocator);
// [ 2] ~HashTable();
//
// MANIPULATORS
//*[ 9] operator=(const HashTable& rhs);
//*[13] insert(const SOURCE_TYPE& obj);
//*[13] insert(const ValueType& obj, const bslalg::BidirectionalLink *hint);
//*[16] insertIfMissing(bool *isInsertedFlag, const SOURCE_TYPE& obj);
//*[16] insertIfMissing(bool *isInsertedFlag, const ValueType& obj);
//*[17] insertIfMissing(const KeyType& key);
// [  ] remove(bslalg::BidirectionalLink *node);
// [ 2] removeAll();
//*[11] rehashForNumBuckets(SizeType newNumBuckets);
//*[12] reserveForNumElements(SizeType numElements);
//*[14] setMaxLoadFactor(float loadFactor);
// [ 8] swap(HashTable& other);
//
// ACCESSORS
// [ 4] allocator() const;
// [ 4] comparator() const;
// [ 4] hasher() const;
// [ 4] size() const;
//*[19] maxSize() const;
// [ 4] numBuckets() const;
//*[19] maxNumBuckets() const;
//*[14] loadFactor() const;
// [ 4] maxLoadFactor() const;
// [ 4] rehashThreshold() const;
// [ 4] elementListRoot() const;
//*[18] find(const KeyType& key) const;
//*[18] findRange(BLink **first, BLink **last, const KeyType& k) const;
//*[ 6] findEndOfRange(bslalg::BidirectionalLink *first) const;
// [ 4] bucketAtIndex(SizeType index) const;
// [ 4] bucketIndexForKey(const KeyType& key) const;
// [ 4] countElementsInBucket(SizeType index) const;
//
// [ 6] bool operator==(const HashTable& lhs, const HashTable& rhs);
// [ 6] bool operator!=(const HashTable& lhs, const HashTable& rhs);
//
// ASPECTS:
// [ 8] void swap(HashTable& a, HashTable& b);
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [  ] USAGE EXAMPLE
//
// class HashTable_ImpDetails
// [  ] bslalg::HashTableBucket *defaultBucketAddress();
// [  ] size_t growBucketsForLoadFactor(size_t *, size_t, size_t, double);
// [  ] bslma::Allocator *incidentalAllocator();
// [  ] size_t nextPrime(size_t n);
//
// class HashTable_Util
// [  ] initAnchor<ALLOC>(bslalg::HashTableAnchor *, size_t, const ALLOC&)
// [  ] destroyBucketArray<A>(bslalg::HashTableBucket *, size_t, const A&)
//
// class HashTable_NodeProctor
// [  ] TBD...
//
// class HashTable_ArrayProctor
// [  ] TBD...
//
// class HashTable_ComparatorWrapper
// [  ] TBD...
//
// class HashTable_HashWrapper
// [  ] TBD...
//
// TEST TEST APPARATUS AND GENERATOR FUNCTIONS
// [ 3] int ggg(HashTable *object, const char *spec, int verbose = 1);
// [ 3] HashTable& gg(HashTable *object, const char *spec);
// [ 2] insertElement(HashTable<K, H, E, A> *, const K::ValueType&)
// [ 3] verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
// [  ] bool expectPoolToAllocate(size_t n)
// [  ] size_t predictNumBuckets(size_t length, float maxLoadFactor)
//
// [  ] CONCERN: The type employs the expected size optimizations.
// [  ] CONCERN: The type has the necessary type traits.

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

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

// BDE_VERIFY pragma: push    // Suppress idiomatic issues with usage examples
// BDE_VERIFY pragma: -CC01   // C-style casts are used for readability
// BDE_VERIFY pragma: -FABC01 // Functions ordered for expository purpose
// BDE_VERIFY pragma: -FD01   // Function contract may be documented implicitly

// BDE_VERIFY pragma: set ok_unquoted allocator hash value
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
// hasher, a policy suitable for a set container.  (Later, in {Example 2}, we
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
// 'bslstl::HashTable' (configured using 'UseEntireValueAsKey') as its sole
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
            // default-constructed object of type 'HASH' is used.  Optionally
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
            // Insert the specified 'value' into this set if the 'value' does
            // not already exist in this set; otherwise, this method has no
            // effect.  Return a pair whose 'first' member is an iterator
            // providing non-modifiable access to the (possibly newly inserted)
            // 'KEY' object having 'value' (according to 'EQUAL') and whose
            // 'second' member is 'true' if a new element was inserted, and
            // 'false' if 'value' was already present.

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
// 'bslstl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
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
            // Create an empty 'MyHashedMap' object having a maximum load
            // factor of 1.  Optionally specify at least 'initialNumBuckets' in
            // this container's initial array of buckets.  If
            // 'initialNumBuckets' is not supplied, one empty bucket shall be
            // used and no memory allocated.  Optionally specify 'hash' to
            // generate the hash values associated with the key-value pairs
            // contained in this unordered map.  If 'hash' is not supplied, a
            // default-constructed object of (template parameter) 'HASH' is
            // used.  Optionally specify a key-equality functor 'keyEqual' used
            // to determine whether two keys have the same value.  If
            // 'keyEqual' is not supplied, a default-constructed object of
            // (template parameter) 'EQUAL' is used.  Optionally specify an
            // 'allocator' used to supply memory.  If 'allocator' is not
            // supplied, a default-constructed object of the (template
            // parameter) type 'ALLOCATOR' is used.  If 'ALLOCATOR' is
            // 'bsl::allocator' (the default), then 'allocator' shall be
            // convertible to 'bslma::Allocator *'.  If 'ALLOCATOR' is
            // 'bsl::allocator' and 'allocator' is not supplied, the currently
            // installed default allocator will be used to supply memory.  Note
            // that more than 'initialNumBuckets' buckets may be created in
            // order to preserve the bucket allocation strategy of the
            // hash-table (but never fewer).

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
// a reference offering modifiable access to the 'second' member of the
// (possibly newly added) element.  Note that the 'static_cast' from
// 'HashTableLink *' to 'HashTableNode *' is valid because the nodes derive
// from the link type (see 'bslalg_bidirectionallink' and
// 'bslalg_hashtableimputil').
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
// 'bslstl::HashTable' (configured using 'UseFirstValueOfPairAsKey') as its
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
            // this container's initial array of buckets.  If
            // 'initialNumBuckets' is not supplied, an implementation defined
            // value is used.  Optionally specify a 'hash', a hash-functor used
            // to generate the hash values associated to the key-value pairs
            // contained in this object.  If 'hash' is not supplied, a
            // default-constructed object of (template parameter) 'HASH' type
            // is used.  Optionally specify a key-equality functor 'keyEqual'
            // used to verify that two key values are the same.  If 'keyEqual'
            // is not supplied, a default-constructed object of (template
            // parameter) 'EQUAL' type is used.  Optionally specify an
            // 'allocator' used to supply memory.  If 'allocator' is not
            // supplied, a default-constructed object of the (template
            // parameter) 'ALLOCATOR' type is used.  If 'ALLOCATOR' is
            // 'bsl::allocator' (the default), then 'allocator' shall be
            // convertible to 'bslma::Allocator *'.  If the 'ALLOCATOR' is
            // 'bsl::allocator' and 'allocator' is not supplied, the currently
            // installed default allocator will be used to supply memory.

        //! ~MyHashedMultiMap() = default;
            // Destroy this object.

        // MANIPULATORS
        template <class SOURCE_TYPE>
        iterator insert(const SOURCE_TYPE& value);
            // Insert the specified 'value' into this multi-map, and return an
            // iterator to the newly inserted element.  This method requires
            // that the (class template parameter) types 'KEY' and 'VALUE'
            // types both be "copy-constructible", and that the (function
            // template parameter) 'SOURCE_TYPE' be convertible to the (class
            // template parameter) 'VALUE' type.

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
            // In this policy, the value passed to the hasher is the 'vendorId'
            // attribute, an 'int' type.

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
            // Create an empty 'MySalesRecordContainer' object.  Optionally
            // specify a 'basicAllocator' to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        //! ~MySalesRecordContainer() = default;
            // Destroy this object.

        // MANIPULATORS
        MyPair<ConstItrByOrderNumber, bool> insert(const MySalesRecord& value);
            // Insert the specified 'value' into this set if the 'value' does
            // not already exist in this set; otherwise, this method has no
            // effect.  Return a pair whose 'first' member is an iterator
            // providing non-modifiable access to the (possibly newly inserted)
            // 'MySalesRecord' object having 'value' and whose 'second' member
            // is 'true' if a new element was inserted, and 'false' if 'value'
            // was already present.

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
            bsl::iterator_traits<
                MySalesRecordContainer::ConstItrById>::difference_type count =
                                    bsl::distance(result.first, result.second);

if (verbose) {
            printf("customerId %d, count " TD "\n", customerId, count);
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
            bsl::iterator_traits<
                MySalesRecordContainer::ConstItrById>::difference_type count =
                                    bsl::distance(result.first, result.second);
if (verbose) {
            printf("vendorId %d, count " TD "\n", vendorId, count);
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

// Type trait specializations to suppress warnings on allocator usage.  We omit
// these details from the actual usage examples as the necessary opening and
// closing of multiple namespaces would distract from the narrative.

namespace BloombergLP {
namespace bslma {

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedMap<KEY,
                                                      VALUE,
                                                      HASH,
                                                      EQUAL,
                                                      ALLOCATOR> >
: bsl::is_convertible<Allocator*, ALLOCATOR>::type {};

template <class KEY, class VALUE, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedMultiMap<KEY,
                                                           VALUE,
                                                           HASH,
                                                           EQUAL,
                                                           ALLOCATOR> >
     : bsl::is_convertible<Allocator*, ALLOCATOR>::type {};

template <class KEY, class HASH, class EQUAL, class ALLOCATOR>
struct UsesBslmaAllocator<UsageExamples::MyHashedSet<KEY,
                                                      HASH,
                                                      EQUAL,
                                                      ALLOCATOR> >
     : bsl::is_convertible<Allocator*, ALLOCATOR>::type {};


template <>
struct UsesBslmaAllocator<UsageExamples::MySalesRecordContainer>
     : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace
// BDE_VERIFY pragma: pop  // Suppress idiomatic issues with usage examples

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------
bslma::TestAllocator *g_bsltfAllocator_p = 0;

// Peeking into the "white box" of our standard table of prime numbers in
// 'HashTable_Util', we can see critical sequence lengths around 2, 5 and 13.
// That suggests we will want to pay attention to length 0, 1, 2, 3, 4, 5, 6,
// 12, 13 and 14.  We will pay special attention to all permutations around the
// smaller lengths, and test a representative range of concerns of distinct/
// duplicate values, in and out of order, for the longer sequences.

struct DefaultDataRow {
    int         d_line;     // source line number
    int         d_index;    // lexical order
    const char *d_spec;     // specification string, for input to 'gg' function
    const char *d_results;  // expected element values
};

static
const DefaultDataRow DEFAULT_DATA[] = {
    //line grp              spec            results
    //---- ---  ----------------     --------------
    { L_,    0,               "",                "" },
    { L_,    1,              "A",               "A" },
    { L_,    2,              "B",               "B" },
    { L_,    3,             "AA",              "AA" },
    { L_,    4,             "AB",              "AB" },
    { L_,    4,             "BA",              "AB" },
    { L_,    5,             "AC",              "AC" },
    { L_,    6,             "BB",              "BB" },
    { L_,    7,             "CD",              "CD" },
    { L_,    8,            "AAA",             "AAA" },
    { L_,    9,            "AAB",             "AAB" },
    { L_,    9,            "ABA",             "AAB" },
    { L_,    9,            "BAA",             "AAB" },
    { L_,   10,            "ABC",             "ABC" },
    { L_,   10,            "ACB",             "ABC" },
    { L_,   10,            "BAC",             "ABC" },
    { L_,   10,            "BCA",             "ABC" },
    { L_,   10,            "CAB",             "ABC" },
    { L_,   10,            "CBA",             "ABC" },
    { L_,   11,            "BAD",             "ABD" },
    { L_,   12,           "AAAA",            "AAAA" },
    { L_,   13,           "AABA",            "AAAB" },
    { L_,   14,           "ABAB",            "AABB" },
    { L_,   15,           "AABC",            "AABC" },
    { L_,   15,           "ABCA",            "AABC" },
    { L_,   16,           "ABCB",            "ABBC" },
    { L_,   17,           "ABCC",            "ABCC" },
    { L_,   18,           "ABCD",            "ABCD" },
    { L_,   18,           "ACBD",            "ABCD" },
    { L_,   19,           "BEAD",            "ABDE" },
    { L_,   20,          "ABCDE",           "ABCDE" },
    { L_,   21,          "FEDCB",           "BCDEF" },
    { L_,   22,         "ABCABC",          "AABBCC" },
    { L_,   22,         "AABBCC",          "AABBCC" },
    { L_,   23,         "FEDCBA",          "ABCDEF" },
    { L_,   24,   "ABCDEFGHIJKL",    "ABCDEFGHIJKL" },
    { L_,   25,  "ABCDEFGHIJKLM",   "ABCDEFGHIJKLM" },
    { L_,   26, "ABCDEFGHIJKLMN",  "ABCDEFGHIJKLMN" },
};

static const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;

static
const float DEFAULT_MAX_LOAD_FACTOR[] = {
#if defined(BSLSTL_HASHTABLE_TEST_EXTREME_CASES)
    // Note that using tiny values for 'maxLoadFactor' can consume a huge
    // amount of memory for no real purpose, and greatly slows down the
    // running of the test case.  These test cases are disabled by default
    // as a practical matter, but can be run manually by defining the macro
    // 'BSLSTL_HASHTABLE_TEST_EXTREME_CASES' when building the test driver.
            1e-5,    // This is small enough to test extremely low value
#endif
            0.125f,  // Low value used throughout the test driver
            1.0f,    // Default and most common value
            8.0f,    // High value used throughout the test driver
            native_std::numeric_limits<float>::infinity()  // edge case
};
static const int DEFAULT_MAX_LOAD_FACTOR_SIZE =
              sizeof DEFAULT_MAX_LOAD_FACTOR / sizeof *DEFAULT_MAX_LOAD_FACTOR;

typedef bslalg::HashTableImpUtil     ImpUtil;
typedef bslalg::BidirectionalLink    Link;

//=============================================================================
//                      HELPER CLASSES FOR TESTING
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
        // Return a hash for the specified 'value' such that this functor
        // satisfies the Hash requirements of the C++ Standard,
        // [hash.requirements].
};

}  // close namespace bsl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace TestTypes
{

                       // ===========================
                       // class AwkwardMaplikeElement
                       // ===========================

class AwkwardMaplikeElement {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while the
    // class itself *is* equality-comparable (as required of a value-semantic
    // type) so that a HashTable of these objects should have a well-defined
    // 'operator=='.  Note that this class is a specific example for a specific
    // problem, rather than a template providing the general test type for keys
    // distinct from values, as the template test facility requires an explicit
    // specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  public:
    // CREATORS
    AwkwardMaplikeElement();
        // Create an 'AwkwardMaplikeElement' element having '0' as its 'data'.

    explicit
    AwkwardMaplikeElement(int value);
        // Create an 'AwkwardMaplikeElement' element having the specified
        // 'value' as its 'data'.

    explicit
    AwkwardMaplikeElement(const bsltf::NonEqualComparableTestType& value);
        // Create an 'AwkwardMaplikeElement' element having the specified
        // 'value' as its 'data'.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    const bsltf::NonEqualComparableTestType& key() const;
        // Return a reference offering non-modifiable access to the 'key' of
        // this object.
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
    // Print to the console a textual representation of the specified 'value'.
    // Note that this representation is intended only to support error reports
    // and not as a portable format.

                       // ======================
                       // class MostEvilTestType
                       // ======================

class MostEvilTestType {
    // This class provides an awkward value-semantic type, designed to be used
    // with a KEY_CONFIG policy for a HashTable that supplies a non-equality
    // comparable key-type, using 'data' for the 'extractKey' method, while the
    // class itself *is* equality-comparable (as required of a value-semantic
    // type) so that a HashTable of these objects should have a well-defined
    // 'operator=='.  Note that this class is a specific example for a specific
    // problem, rather than a template providing the general test type for keys
    // distinct from values, as the template test facility requires an explicit
    // specialization of a function template,
    // 'TemplateTestFacility::getIdentifier<T>', which would require a partial
    // template specialization if this class were a template, and that is not
    // supported by the C++ language.

  private:
    bsltf::NonEqualComparableTestType d_data;

  private:
    // NOT IMPLEMENTED
    void operator=(MostEvilTestType&); // = delete

    void operator&();  // = delete;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&); // = delete;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&); // = delete;

    static void* operator new(std::size_t size); // = delete
    static void* operator new(std::size_t size, void *ptr); // = delete
    static void operator delete(void *ptr); // = delete

  public:
    // CREATORS
    explicit MostEvilTestType(int value);
        // Create an 'AwkwardMaplikeElement' object having the specified
        // 'value' as its 'data'.

    // explicit MostEvilTestType(const MostEvilTestType& original) = default;
        // Create a 'MostEvilTestType' object having the same 'data' value as
        // the specified 'original'.

    // ~MostEvilTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

bool operator==(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'MostEvilTestType' objects have the
    // same value if ... (TBD)

bool operator!=(const MostEvilTestType& lhs,
                const MostEvilTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'MostEvilTestType' objects do
    // not have the same value if ... (TBD)

void debugprint(const MostEvilTestType& value);
    // Print to the console a textual representation of the specified 'value'.
    // Note that this representation is intended only to support error reports
    // and not as a portable format.

}  // close namespace TestTypes

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

namespace BloombergLP {
namespace bslstl {
// HashTable-specific print function, which must be provided in the same
// namespace as the class do be printed.

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void debugprint(
        const bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>& t);
    // Print the value of the specified 'HashTable' 't' in a format suitable
    // for reading for debug purposes.
}  // close package namespace

namespace bsltf {

template <>
int TemplateTestFacility::getIdentifier<TestTypes::AwkwardMaplikeElement>(
                               const TestTypes::AwkwardMaplikeElement& object);

template <>
int TemplateTestFacility::getIdentifier<TestTypes::MostEvilTestType>(
                                    const TestTypes::MostEvilTestType& object);

}  // close namespace bsltf
}  // close enterprise namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef BSLS_PLATFORM_CMP_CLANG
namespace TestMachinery
#else
namespace
#endif
{
                       // ====================
                       // class IsBslAllocator
                       // ====================

template <class TYPE>
struct IsBslAllocator : bsl::false_type {};
template <class TYPE>
struct IsBslAllocator<bsl::allocator<TYPE> > : bsl::true_type {};
    // This simple traits class reports 'true' if the (template parameter)
    // 'TYPE' is an instantiation of the 'bsl::allocator' template, and 'false'
    // otherwise.

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
    // NOT IMPLEMENTED
    BoolArray(const BoolArray&); // = delete;
    BoolArray& operator=(const BoolArray&);  // = delete

  public:
    // CREATORS
    explicit BoolArray(size_t n);
        // Create a 'BoolArray' object holding the specified 'n' boolean flags.

    ~BoolArray();
        // Destroy this object, reclaiming any allocated memory.

    // MANIPULATORS
    bool& operator[](size_t index);
        // Return a reference offering modifiable access to the boolean flag at
        // the specified 'index'.  The behavior is undefined unless
        // 'index < size()'.

    // ACCESSORS
    bool operator[](size_t index) const;
        // Return the value of the boolean flag at the specified 'index'.  The
        // behavior is undefined unless 'index < size()'.

    size_t size() const;
        // Return the number of boolean flags held by this object.
};

struct TestException : native_std::exception{};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ================================
                       // class ThrowingEqualityComparator
                       // ================================

template <class TYPE>
class ThrowingEqualityComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  The
    // function-call operator is implemented with integer comparison using
    // integers converted from objects of 'TYPE' by the class method
    // 'TemplateTestFacility::getIdentifier'.  The function-call operator also
    // increments a counter used to keep track the method call count.  Object
    // of this class can be identified by an id passed on construction.

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times 'operator()' is called
    size_t         d_throwInterval; // throw after this many 'operator()' calls

  public:
    // CREATORS
    //! ThrowingEqualityComparator(const ThrowingEqualityComparator& original)
    //                                                               = default;
        // Create a copy of the specified 'original'.

    explicit ThrowingEqualityComparator(size_t id = 0);
        // Create a 'ThrowingEqualityComparator'.  Optionally, specify 'id'
        // that can be used to identify the object.

    // MANIPULATORS
    void setId(size_t value);
        // Set the 'id' of this object to the specified 'value'.

    void setThrowInterval(size_t value);
        // Set to the specified 'value' the number of times 'operator()' may be
        // called after throwing an exception before another such call would
        // throw.  If '0 == value' then disable throwing of exceptions by
        // 'operator()'.
        //
        // TBD: Document behavior if 'value' is less than the number of times
        // 'operator()' has already been called.

    // ACCESSORS
    bool operator() (const TYPE& lhs, const TYPE& rhs) const;
        // Increment a counter that records the number of times this method is
        // called.   Return 'true' if the integer representation of the
        // specified 'lhs' is less than integer representation of the specified
        // 'rhs'.

    size_t count() const;
        // Return the number of times 'operator()' is called.

    size_t id() const;
        // Return the 'id' of this object.

    size_t throwInterval() const;
        // Return the number of times 'operator()' may be called after throwing
        // an exception before another such call would throw, or '0' if no
        // exceptions are ever thrown.
};

template <class TYPE>
bool operator==(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ThrowingEqualityComparator' functors have the
    // same value if they have the same 'id'.

template <class TYPE>
bool operator!=(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'ThrowingEqualityComparator' functors
    // do not have the same value if they do not have the same 'id'.

                       // =========================
                       // class ThrowingHashFunctor
                       // =========================

template <class TYPE>
class ThrowingHashFunctor {
    // This value-semantic class meets the C++11 'Hash' requirements (C++11
    // [hash.requirements], 17.6.3.4) with an overload for 'operator()' that
    // can be configured at runtime to throw exceptions after a user-supplied
    // number of function calls.  It can also be configured to never throw, and
    // merely count the number of invocations of the function call operator.
    // Note that this functor relies on "logical" rather than "physical"
    // 'const' behavior, as it must count the number of times a
    // 'const'-qualified function is invoked.

    // DATA
    size_t         d_id;            // identifier for the functor
    mutable size_t d_count;         // number of times 'operator()' is called
    size_t         d_throwInterval; // throw after this many 'operator()' calls

  public:
    // CREATORS
    //! ThrowingHashFunctor(const ThrowingHashFunctor& original) = default;
        // Create a copy of the specified 'original'.

    explicit ThrowingHashFunctor(size_t id = 0);
        // Create a 'ThrowingHashFunctor'.  Optionally, specify 'id' that can
        // be used to identify the object.

    // MANIPULATORS
    void setId(size_t value);
        // Set the 'id' of this object to the specified value.  Note that the
        // 'id' contributes to the value produced by the 'operator()' method,
        // so the 'id' of a 'ThrowingHashFunctor' should not be changed for
        // functors that are installed in 'HashTable' objects.

    void setThrowInterval(size_t value);
        // Set to the specified 'value' the number of times 'operator()' may be
        // called after throwing an exception before another such call would
        // throw.  If '0 == value' then disable throwing of exceptions by
        // 'operator()'.
        //
        // TBD: Document behavior if 'value' is less than the number of times
        // 'operator()' has already been called.

    // ACCESSORS
    native_std::size_t operator() (const TYPE& obj) const;
        // Increment a counter that records the number of times this method is
        // called.   Return a hash value for the specified 'obj'.  The behavior
        // is undefined unless 'obj' is a value supplied by the BSL template
        // test facility.

    size_t count() const;
        // Return the number of times 'operator()' is called.

    size_t id() const;
        // Return the 'id' of this object.

    size_t throwInterval() const;
        // Return the number of times 'operator()' may be called after throwing
        // an exception before another such call would throw, or '0' if no
        // exceptions are ever thrown.
};

template <class TYPE>
bool operator==(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'ThrowingHashFunctor' functors have the same
    // value if they have the same 'id'.

template <class TYPE>
bool operator!=(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' do not have the same
    // value, and 'false' otherwise.  Two 'ThrowingHashFunctor' functors do not
    // have the same value if they do not have the same 'id'.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
        // Return a hash code for the specified 'k' using the wrapped functor
        // of (template parameter) type 'HASHER' supplied at construction.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
    native_std::size_t operator()(const bsltf::ConvertibleValueWrapper<KEY>& k)
                                                                         const;
        // Return a hash code for the specified 'k' using the wrapped functor
        // of (template parameter) type 'HASHER' supplied at construction.
};

                       // ====================================
                       // class TestConvertibleValueComparator
                       // ====================================

template <class KEY>
class TestConvertibleValueComparator {
    // This test class provides...

  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator()(
                           const bsltf::ConvertibleValueWrapper<KEY>& a,
                           const bsltf::ConvertibleValueWrapper<KEY>& b) const;
        // Return a value convertible to 'true' if the specified 'a' has the
        // same value as the specified 'b', and a value convertible to 'false'
        // otherwise..
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =======================
                       // class GenericComparator
                       // =======================

class GenericComparator {
  public:
    // ACCESSORS
    template <class ARG1_TYPE, class ARG2_TYPE>
    bsltf::EvilBooleanType operator() (ARG1_TYPE& arg1, ARG2_TYPE& arg2);
        // Return a value convertible to 'true' if the specified 'arg1' has the
        // same value as the specified 'arg2', for some unspecified definition
        // that defaults to 'operator==', but may use some other functionality.
};

                       // ===================
                       // class GenericHasher
                       // ===================

class GenericHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.

  public:
    // ACCESSORS
    template <class KEY>
    native_std::size_t operator() (KEY& k);
        // Return a hash code for the specified 'k'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ==========================
                       // class ModifiableComparator
                       // ==========================

template <class KEY>
class ModifiableComparator {
  public:
    // ACCESSORS
    bsltf::EvilBooleanType operator() (KEY& arg1, KEY& arg2);
        // Return a value convertible to 'true' if the specified 'arg1' has the
        // same value as the specified 'arg2', for some unspecified definition
        // that defaults to 'operator==', but may use some other functionality
        // for 'KEY' types that do not support this operator.
};

                       // ======================
                       // class ModifiableHasher
                       // ======================

template <class KEY>
class ModifiableHasher {
    // This test class provides a mechanism that defines a function-call
    // operator that provides a hash code for objects of the parameterized
    // 'KEY'.

  public:
    // ACCESSORS
    native_std::size_t operator() (KEY& k);
        // Return a hash code for the specified 'k'.
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ===========================
                       // class DefaultOnlyComparator
                       // ===========================

template <class TYPE>
class DefaultOnlyComparator {
    // This test class provides a mechanism that defines a function-call
    // operator that compares two objects of the parameterized 'TYPE'.  However
    // it is a perverse type intended for testing templates against their
    // minimum requirements, and as such is neither copyable nor swappable, and
    // is only default constructible.  The function call operator will return a
    // perverse type that is convertible-to-bool.

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
    // NOT IMPLEMENTED
    DefaultOnlyHasher(const DefaultOnlyHasher&); // = delete
    void operator=(const DefaultOnlyHasher&); // = delete;

    void operator&();  // = delete;

    template<class ANY_TYPE>
    void operator,(const ANY_TYPE&); // = delete;

    template<class ANY_TYPE>
    void operator,(ANY_TYPE&); // = delete;

  public:
    // CREATORS
    DefaultOnlyHasher() {}

    // ACCESSORS
    size_t operator() (const TYPE& value)
        // Return the hash of the specified 'value'.
    {
        return bsl::hash<int>().operator()(
                      bsltf::TemplateTestFacility::getIdentifier<TYPE>(value));
    }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // =============================
                       // class FunctionPointerPolicies
                       // =============================

template <class KEY>
struct FunctionPointerPolicies {
    typedef size_t HashFunction(const KEY&);
    typedef bool   ComparisonFunction(const KEY&, const KEY&);

    static bool compare(const KEY& lhs, const KEY& rhs);
        // Return 'true' if the specified 'lhs' has the same value as the
        // specified 'rhs' when compared using the 'bsltf' test facility
        // 'BSLTF_EQ', and 'false' otherwise.

    static size_t hash(const KEY& k);
        // Return the hash value of the specified 'k' according to the 'bsltf'
        // hash functor, 'TestFacilityHasher<KEY>'.
};

                       // ========================
                       // class MakeCallableEntity
                       // ========================

template <class CALLABLE>
struct MakeCallableEntity {
    typedef CALLABLE CallableEntityType;

    static CallableEntityType make();
        // Return a default-constructed object of the (template parameter) type
        // 'CALLABLE'.
};

template <class CALLABLE>
struct MakeCallableEntity<CALLABLE&> {
    typedef CALLABLE& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a default-constructed object of the (template
        // parameter) type 'CALLABLE'.  Note that this is will be a reference
        // to a singleton object; any modifications made to that object will be
        // reflected in references to previous and subsequent calls to this
        // function.
};

template <class FUNCTOR, bool ENABLE_SWAP>
struct MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> > {
    typedef bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> CallableEntityType;

    static CallableEntityType make();
        // Return a functor wrapping a default-constructed object of the
        // (template parameter) type 'FUNCTOR'.  The adapter overloads all
        // operations other than the function-call operator in a most awkward
        // way, and is swappable if and only if (the template parameter)
        // 'ENABLE_SWAP' is 'true'.
};

template <class KEY>
struct MakeCallableEntity<size_t (*)(const KEY&)> {
    typedef size_t FunctionType(const KEY&);
    typedef FunctionType *CallableEntityType;

    static CallableEntityType make();
        // Return the address of a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class KEY>
struct MakeCallableEntity<bool (*)(const KEY&, const KEY&)> {
    typedef bool FunctionType(const KEY&, const KEY&);
    typedef FunctionType *CallableEntityType;

    static CallableEntityType make();
        // Return the address of a function that can compare two objects of the
        // (template parameter) type 'KEY' for equality.
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(&)(ARG)> {
    typedef RESULT FunctionType(ARG);
    typedef FunctionType& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(&)(ARG1, ARG2)> {
    typedef RESULT FunctionType(ARG1, ARG2);
    typedef FunctionType& CallableEntityType;

    static CallableEntityType make();
        // Return a reference to a function that can compare two objects of the
        // (template parameter) type 'KEY' for equality.
};

template <class RESULT, class ARG>
struct MakeCallableEntity<RESULT(ARG)> {
    typedef RESULT CallableEntityType(ARG);

    static CallableEntityType& make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

template <class RESULT, class ARG1, class ARG2>
struct MakeCallableEntity<RESULT(ARG1, ARG2)> {
    typedef RESULT CallableEntityType(ARG1, ARG2);

    static CallableEntityType& make();
        // Return a reference to a function that can compute hash values for
        // objects of the (template parameter) type 'KEY'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
        // Return a default-constructed 'ALLOCATOR' object.
};

template <class TYPE>
struct MakeAllocator<bsl::allocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsl::allocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsl::allocator<TYPE>' object wrapping the specified
        // 'basicAllocator'.
};

template <class TYPE>
struct MakeAllocator<bsltf::StdTestAllocator<TYPE> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdTestAllocator<TYPE> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsltf::StdTestAllocator<TYPE>' object wrapping the
        // specified 'basicAllocator'.
};

template <class TYPE, bool A, bool B, bool C, bool D>
struct MakeAllocator<bsltf::StdStatefulAllocator<TYPE, A, B, C, D> > {
    // TBD This utility class template specialization...

    // PUBLIC TYPES
    typedef bsltf::StdStatefulAllocator<TYPE, A, B, C, D> AllocatorType;

    // CLASS METHODS
    static AllocatorType make(bslma::Allocator *basicAllocator);
        // Return a 'bsltf::StdStatefulAllocator<TYPE, A, B, C, D>' object
        // wrapping the specified 'basicAllocator'.
};

                       // =================
                       // class ObjectMaker
                       // =================

// The 'ObjectMaker' template and its associated specializations customize the
// act of creating a object-under-test, in-place, using an allocator configured
// according to some plan.  Generally, the plans are spelled out 'a' -> 'z',
// with each letter corresponding to a specific way of passing an allocator to
// the test object's constructor.  In practice, we currently define only the
// configurations 'a' -> 'd', and they are called sequentially.  As we become
// more thorough in testing, additional configurations will present themselves,
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

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
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

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "abc"; }
    static const char * specForCopyTests()      { return "abcd"; }
    static const char * specForDefaultTests()   { return "abcd"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
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

    static
    AllocatorType makeObject(Obj                 **objPtr,
                             char                  config,
                             bslma::Allocator     *fa, // "footprint" allocator
                             bslma::TestAllocator *objectAllocator);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // allocator determined by the specified 'config' to the constructor.
        // Return an allocator object that will compare equal to the allocator
        // that is expected to be used to construct the 'HashTable' object.
        // The specified 'objectAllocator' may, or may not, be used to
        // construct the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static
    AllocatorType makeObject(Obj                  **objPtr,
                             char                   config,
                             bslma::Allocator      *fa, //"footprint" allocator
                             bslma::TestAllocator  *objectAllocator,
                             const HASHER&          hash,
                             const COMPARATOR&      compare,
                             SizeType               initialBuckets,
                             float                  initialMaxLoadFactor);
        // Create a 'HashTable' object at the specified 'objPtr' address via an
        // in-place 'new' using the specified 'fa' allocator, and passing the
        // specified 'hash', 'compare', 'initialBuckets',
        // 'initialMaxLoadFactor' and the allocator determined by the specified
        // 'config' to the constructor.  Return an allocator object that will
        // return 'true' when compared with the allocator that is expected to
        // be used to construct the 'HashTable' object using 'operator=='.  The
        // specified 'objectAllocator' may, or may not, be used to construct
        // the 'HashTable' object according to the 'config':
        //..
        //  config  allocator
        //  'a'     use the specified 'objectAllocator'
        //  'b'     use the default supplied by the constructor
        //  'c'     explicitly pass a null pointer of type 'bslma::Allocator *'
        //  'd'     explicitly pass the default allocator
        //..

    static const char * specForBootstrapTests() { return "ab"; }
    static const char * specForCopyTests()      { return "ab"; }
    static const char * specForDefaultTests()   { return "ab"; }
        // Return the set of configurations to pass to 'makeObject' in order to
        // test the behavior of a 'HashTable' instantiated with the matching
        // set of template parameters.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
    // 'bsltf::StdTestAllocator's share the same allocator, which is set by the
    // 'bsltf::StdTestAllocatorConfiguration' utility.  We can determine if
    // this is wrapping a test allocator using 'dynamic_cast'.

template <class TYPE, bool A, bool B, bool C, bool D>
bslma::TestAllocator *
extractTestAllocator(bsltf::StdStatefulAllocator<TYPE, A, B, C, D>& alloc);
    // Return the address of the test allocator wrapped by the specified
    // 'alloc'.

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//       test support functions dealing with hash and comparator functors

template <class COMPARATOR>
bool isEqualComparator(const COMPARATOR&, const COMPARATOR&);
    // Return 'true'.  This function template provides a common signature that
    // may be overloaded for specific hasher types that can support the idea of
    // setting a state value.  It is assumed that any comparator that does not
    // overload this functor is stateless, and so equivalent to any other
    // comparator.

template <class RESULT, class ARG1, class ARG2>
bool isEqualComparator(RESULT (*const &a)(ARG1, ARG2),
                       RESULT (*const &b)(ARG1, ARG2));
    // Return 'true' if the specified addresses 'a' and 'b' are the same, and
    // 'false' otherwise.  Note that we pass pointers by 'const &' in order to
    // avoid ambiguities on the IBM XLC compiler.

template <class KEY>
bool isEqualComparator(const ThrowingEqualityComparator<KEY>& lhs,
                       const ThrowingEqualityComparator<KEY>& rhs);
    // Provide an overloaded function to compare comparators.  Return 'true' if
    // the specified 'lhs' compares equal to the specified 'rhs' under the
    // expression 'lhs == rhs'.  This template will not instantiate if the
    // expression does not compile.

template <class HASHER>
bool isEqualHasher(const HASHER&, const HASHER&);
    // Return 'true'.  This function template provides a common signature that
    // may be overloaded for specific hasher types that can support the idea of
    // setting a state value.

template <class RESULT, class ARGUMENT>
bool isEqualHasher(RESULT (*const &a)(ARGUMENT),
                   RESULT (*const &b)(ARGUMENT));
    // Return 'true' if the specified addresses 'a' and 'b' are the same, and
    // 'false' otherwise.  Note that we pass pointers by 'const &' in order to
    // avoid ambiguities on the IBM XLC compiler.


bool isEqualHasher(const ThrowingHashFunctor<int>& lhs,
                   const ThrowingHashFunctor<int>& rhs);
    // Provide an overloaded function to compare hash functors.  Return 'true'
    // if the specified 'lhs' compares equal to the specified 'rhs' under the
    // expression 'lhs == rhs'.  This template will not instantiate if the
    // expression does not compile.

template <class COMPARATOR>
void setComparatorState(COMPARATOR *comparator, size_t id);
    // This is a null function, that has no effect and does not use the
    // specified 'comparator' nor the specified 'id'.  This function provides a
    // common signature that may be overloaded for specific 'COMPARATOR' types
    // that can support the idea of setting a state value.  Test code can then
    // call a function with this signature and get the right behavior (without
    // a compile error) regardless of the properties of the 'COMPARATOR' type.

template <class KEY>
void setComparatorState(ThrowingEqualityComparator<KEY> *comparator,size_t id);

template <class HASHER>
void setHasherState(HASHER *hasher, size_t id);
    // This is a null function, that has no effect and does not use the
    // specified 'hasher' nor the specified 'id'.  This function provides a
    // common signature that may be overloaded for specific 'HASHER' types
    // that can support the idea of setting a state value.  Test code can then
    // call a function with this signature and get the right behavior (without
    // a compile error) regardless of the properties of the 'HASHER' type.

void setHasherState(ThrowingHashFunctor<int> *hasher, size_t id);

#if !defined(BSLS_PLATFORM_CMP_CLANG)
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

                       // ----------------------
                       // class MostEvilTestType
                       // ----------------------

// CREATORS

inline
MostEvilTestType::MostEvilTestType(int value)
: d_data(value)
{
}

// MANIPULATORS
inline
void MostEvilTestType::setData(int value)
{
    d_data.setData(value);
}

// ACCESSORS
inline
int MostEvilTestType::data() const
{
    return d_data.data();
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

inline
bool TestTypes::operator==(const MostEvilTestType& lhs,
                           const MostEvilTestType& rhs)
{
    return BSL_TF_EQ(lhs.data(), rhs.data());
}

inline
bool TestTypes::operator!=(const MostEvilTestType& lhs,
                           const MostEvilTestType& rhs)
{
    return !(lhs == rhs);
}

inline
void TestTypes::debugprint(const MostEvilTestType& value)
{
    bsls::debugprint(value.data());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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

template <>
inline
int TemplateTestFacility::getIdentifier<TestTypes::MostEvilTestType>(
                                     const TestTypes::MostEvilTestType& object)
{
    return object.data();
}

}  // close namespace bsltf
}  // close enterprise namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if !defined(BSLS_PLATFORM_CMP_CLANG)
namespace TestMachinery
#else
namespace
#endif
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
                       // class ThrowingEqualityComparator
                       // ----------------------------

// CREATORS
template <class TYPE>
inline
ThrowingEqualityComparator<TYPE>::ThrowingEqualityComparator(size_t id)
: d_id(id)
, d_count(0)
, d_throwInterval(0)
{
}

// MANIPULATORS
template <class TYPE>
inline
void ThrowingEqualityComparator<TYPE>::setId(size_t value)
{
    d_id = value;
}

template <class TYPE>
inline
void ThrowingEqualityComparator<TYPE>::setThrowInterval(size_t value)
{
    d_throwInterval = value;
}

// ACCESSORS
template <class TYPE>
inline
bool ThrowingEqualityComparator<TYPE>::operator() (const TYPE& lhs,
                                                   const TYPE& rhs) const
{
    ++d_count;

    if (d_throwInterval && !(d_count % d_throwInterval)) {
        BSLS_THROW(TestException());
    }

    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(lhs)
        == bsltf::TemplateTestFacility::getIdentifier<TYPE>(rhs);
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t ThrowingEqualityComparator<TYPE>::throwInterval() const
{
    return d_throwInterval;
}

template <class TYPE>
inline
bool operator==(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() == rhs.id();
}

template <class TYPE>
inline
bool operator!=(const ThrowingEqualityComparator<TYPE>& lhs,
                const ThrowingEqualityComparator<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

                       // ---------------------
                       // class ThrowingHashFunctor
                       // ---------------------

// CREATORS
template <class TYPE>
inline
ThrowingHashFunctor<TYPE>::ThrowingHashFunctor(size_t id)
: d_id(id)
, d_count(0)
, d_throwInterval()
{
}

// MANIPULATORS
template <class TYPE>
inline
void ThrowingHashFunctor<TYPE>::setId(size_t value)
{
    d_id = value;
}

template <class TYPE>
inline
void ThrowingHashFunctor<TYPE>::setThrowInterval(size_t value)
{
    d_throwInterval = value;
}

// ACCESSORS
template <class TYPE>
inline
native_std::size_t
ThrowingHashFunctor<TYPE>::operator() (const TYPE& obj) const
{
    ++d_count;

    if (d_throwInterval && !(d_count % d_throwInterval)) {
        BSLS_THROW(TestException());
    }

    return bsltf::TemplateTestFacility::getIdentifier<TYPE>(obj) + d_id;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::count() const
{
    return d_count;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::id() const
{
    return d_id;
}

template <class TYPE>
inline
size_t ThrowingHashFunctor<TYPE>::throwInterval() const
{
    return d_throwInterval;
}

template <class TYPE>
inline
bool operator==(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
{
    return lhs.id() != rhs.id();
}

template <class TYPE>
inline
bool operator!=(const ThrowingHashFunctor<TYPE>& lhs,
                const ThrowingHashFunctor<TYPE>& rhs)
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
    int temp =  bsltf::TemplateTestFacility::getIdentifier<KEY>(k);
    return HASHER::operator()(temp);
}

                       // --------------------------------
                       // class TestConvertibleValueHasher
                       // --------------------------------

template <class KEY, class HASHER>
inline
TestConvertibleValueHasher<KEY, HASHER>::TestConvertibleValueHasher(
                                                            const HASHER& hash)
: Base(hash)
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
    return BSL_TF_EQ(static_cast<const KEY&>(a),
                     static_cast<const KEY&>(b));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------
                       // class GenericComparator
                       // -----------------------

// ACCESSORS
template <class ARG1_TYPE, class ARG2_TYPE>
inline
bsltf::EvilBooleanType GenericComparator::operator() (ARG1_TYPE& arg1,
                                                      ARG2_TYPE& arg2)
{
    return BSL_TF_EQ(arg1, arg2);
}

                       // -------------------
                       // class GenericHasher
                       // -------------------

// ACCESSORS
template <class KEY>
native_std::size_t GenericHasher::operator() (KEY& k)
{
    // do not inline initially due to static local data

    typedef typename bsl::remove_const<KEY>::type KEY_TYPE;

    static const TestFacilityHasher<KEY_TYPE> HASHER;
    return HASHER(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // --------------------------
                       // class ModifiableComparator
                       // --------------------------

// ACCESSORS
template <class KEY>
inline
bsltf::EvilBooleanType ModifiableComparator<KEY>::operator() (KEY& arg1,
                                                              KEY& arg2)
{
    return BSL_TF_EQ(arg1, arg2);
}

                       // ----------------------
                       // class ModifiableHasher
                       // ----------------------

// ACCESSORS
template <class KEY>
native_std::size_t ModifiableHasher<KEY>::operator() (KEY& k)
{
    // do not inline initially due to static local data

    typedef typename bsl::remove_const<KEY>::type KEY_TYPE;

    static const TestFacilityHasher<KEY_TYPE> HASHER;
    return HASHER(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // -----------------------------
                       // class FunctionPointerPolicies
                       // -----------------------------

// do not inline initially due to static local data
template <class KEY>
inline
bool FunctionPointerPolicies<KEY>::compare(const KEY& lhs, const KEY& rhs)
{
    return BSL_TF_EQ(lhs, rhs);
}

template <class KEY>
size_t FunctionPointerPolicies<KEY>::hash(const KEY& k)
{
    static const TestFacilityHasher<KEY> s_hasher = TestFacilityHasher<KEY>();
    return s_hasher(k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                       // ------------------------
                       // class MakeCallableEntity
                       // ------------------------

template <class CALLABLE>
inline
typename MakeCallableEntity<CALLABLE>::CallableEntityType
MakeCallableEntity<CALLABLE>::make()
{
    return CALLABLE();
}

template <class CALLABLE>
inline
typename MakeCallableEntity<CALLABLE &>::CallableEntityType
MakeCallableEntity<CALLABLE &>::make()
{
    static CALLABLE sharedCallable = MakeCallableEntity<CALLABLE>::make();
    return sharedCallable;
}

template <class FUNCTOR, bool ENABLE_SWAP>
inline
typename MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> >::
                                                             CallableEntityType
MakeCallableEntity<bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP> >::make()
{
    return bsltf::DegenerateFunctor<FUNCTOR, ENABLE_SWAP>::
                          cloneBaseObject(MakeCallableEntity<FUNCTOR>::make());
}

template <class KEY>
inline
typename MakeCallableEntity<size_t (*)(const KEY&)>::CallableEntityType
MakeCallableEntity<size_t (*)(const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::hash;
}

template <class KEY>
inline
typename
       MakeCallableEntity<bool (*)(const KEY&, const KEY&)>::CallableEntityType
MakeCallableEntity<bool (*)(const KEY&, const KEY&)>::make()
{
    return &FunctionPointerPolicies<KEY>::compare;
}

template <class RESULT, class ARG>
inline
typename MakeCallableEntity<RESULT(&)(ARG)>::CallableEntityType
MakeCallableEntity<RESULT(&)(ARG)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

template <class RESULT, class ARG1, class ARG2>
inline
typename MakeCallableEntity<RESULT(&)(ARG1, ARG2)>::CallableEntityType
MakeCallableEntity<RESULT(&)(ARG1, ARG2)>::make()
{
    return *MakeCallableEntity<FunctionType *>::make();
}

template <class RESULT, class ARG>
inline
typename MakeCallableEntity<RESULT(ARG)>::CallableEntityType &
MakeCallableEntity<RESULT(ARG)>::make()
{
    return *MakeCallableEntity<CallableEntityType *>::make();
}

template <class RESULT, class ARG1, class ARG2>
inline
typename MakeCallableEntity<RESULT(ARG1, ARG2)>::CallableEntityType &
MakeCallableEntity<RESULT(ARG1, ARG2)>::make()
{
    return *MakeCallableEntity<CallableEntityType *>::make();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

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
    // This method is a little bit of overkill (heavy on the assertions) as a
    // left-over from when we were trying hard to nail down a tricky bug that
    // manifests only on the IBM AIX compiler.  It should probably be cleaned
    // up for final release.

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
        return objAlloc;                                              // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj();
        return ALLOCATOR();                                           // RETURN
      } break;
      case 'c': {
        ALLOCATOR result = ALLOCATOR();
        *objPtr = new (*fa) Obj(result);
        return result;                                                // RETURN
      } break;
      case 'd': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
        *objPtr = new (*fa) Obj(objAlloc);
        return objAlloc;                                              // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLSTL_HASHTABLE_MSVC_WARNS_ON_RETURN_FROM_ABORT)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
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
           const HASHER&          hash,
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
        return objAlloc;                                              // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor);
        return ALLOCATOR();                                           // RETURN
      } break;
      case 'c': {
        ALLOCATOR result = ALLOCATOR();
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                result);
        return result;                                                // RETURN
      } break;
      case 'd': {
        ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(
                                           bslma::Default::defaultAllocator());
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                objAlloc);
        return objAlloc;                                              // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLSTL_HASHTABLE_MSVC_WARNS_ON_RETURN_FROM_ABORT)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
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
        return objectAllocator;                                       // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj();
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'c': {
        *objPtr = new (*fa) Obj(static_cast<bslma::Allocator *>(0));
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'd': {
        *objPtr = new (*fa) Obj(bslma::Default::defaultAllocator());
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLSTL_HASHTABLE_MSVC_WARNS_ON_RETURN_FROM_ABORT)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
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
           const HASHER&          hash,
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
        return objectAllocator;                                       // RETURN
      } break;
      case 'b': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor);
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'c': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                static_cast<bslma::Allocator *>(0));
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
      case 'd': {
        *objPtr = new (*fa) Obj(hash,
                                compare,
                                initialBuckets,
                                initialMaxLoadFactor,
                                bslma::Default::defaultAllocator());
        return AllocatorType(bslma::Default::allocator());            // RETURN
      } break;
    }

    ASSERTV(config, !"Bad allocator config.");
    abort();
#if defined (BSLSTL_HASHTABLE_MSVC_WARNS_ON_RETURN_FROM_ABORT)
    // Microsoft 'abort' is not decorated with a no-return annotation, so
    // static analysis still reports that the function has control paths that
    // do not return a value.  This 'exit' should never be called, but will
    // resolve warnings that are often configured to act as hard errors.
    exit(-99);
#elif defined(BSLSTL_HASHTABLE_IBM_WARNS_ON_RETURN_FROM_ABORT)
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
    // 'bsltf::StdStatefulAllocator' objects are not DefaultConstructible.  We
    // know that the default allocator installed for this test driver will be a
    // test allocator, so we can safely expect a 'dynamic_cast' to not return a
    // null pointer.  Likewise, the 'objectAllocator' should not be a null
    // pointer either, so we can simply construct the desired allocator object
    // using the test allocator specified by the 'config' parameter, and use
    // that to explicitly construct the desired 'HashTable' object into
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
           const HASHER&          hash,
           const COMPARATOR&      compare,
           SizeType               initialBuckets,
           float                  initialMaxLoadFactor)
{
    // 'bsltf::StdStatefulAllocator' objects are not DefaultConstructible.  We
    // know that the default allocator installed for this test driver will be a
    // test allocator, so we can safely expect a 'dynamic_cast' to not return a
    // null pointer.  Likewise, the 'objectAllocator' should not be a null
    // pointer either, so we can simply construct the desired allocator object
    // using the test allocator specified by the 'config' parameter, and use
    // that to explicitly construct the desired 'HashTable' object into
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//                         test support functions

template <class ALLOCATOR>
inline
bslma::TestAllocator *
extractTestAllocator(ALLOCATOR&)
    // Return a null pointer value.  Note that in general, there is no way to
    // extract a test allocator from a generic allocator type.
{
    return 0;
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsl::allocator<TYPE>& alloc)
    // Return the address of the test allocator wrapped by the specified
    // 'alloc', and a null pointer value if 'alloc' does not wrap a
    // 'bslma::TestAllocator'.
{
    return dynamic_cast<bslma::TestAllocator *>(alloc.mechanism());
}

template <class TYPE>
inline
bslma::TestAllocator *
extractTestAllocator(bsltf::StdTestAllocator<TYPE>&)
    // Return the address of the test allocator wrapped by the standard test
    // allocator singleton, and a null pointer value if the standard test
    // allocator singleton 'alloc' does not currently wrap a
    // 'bslma::TestAllocator'.
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <class COMPARATOR>
inline
bool isEqualComparator(const COMPARATOR&, const COMPARATOR&)
{
    return true;
}

template <class RESULT, class ARG1, class ARG2>
inline
bool isEqualComparator(RESULT (*const &a)(ARG1, ARG2),
                       RESULT (*const &b)(ARG1, ARG2))
{
    return a == b;
}

template <class KEY>
inline
bool isEqualComparator(const ThrowingEqualityComparator<KEY>& lhs,
                       const ThrowingEqualityComparator<KEY>& rhs)
{
    return lhs == rhs;
}

template <class HASHER>
inline
bool isEqualHasher(const HASHER&, const HASHER&)
{
    return true;
}

template <class RESULT, class ARGUMENT>
inline
bool isEqualHasher(RESULT (*const &a)(ARGUMENT),
                   RESULT (*const &b)(ARGUMENT))
{
    return a == b;
}

inline
bool isEqualHasher(const ThrowingHashFunctor<int>& lhs,
                   const ThrowingHashFunctor<int>& rhs)
{
    return lhs == rhs;
}


template <class COMPARATOR>
inline
void setComparatorState(COMPARATOR *comparator, size_t id)
{
    (void) comparator;
    (void) id;
}

template <class KEY>
inline
void setComparatorState(ThrowingEqualityComparator<KEY> *comparator, size_t id)
{
    comparator->setId(id);
}

template <class HASHER>
inline
void setHasherState(HASHER *hasher, size_t id)
{
    (void) hasher;
    (void) id;
}

inline
void setHasherState(ThrowingHashFunctor<int> *hasher, size_t id)
{
    hasher->setId(id);
}

#if !defined(BSLS_PLATFORM_CMP_CLANG)
}  // close namespace TestMachinery
#else
}  // close unnamed namespace
#endif

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace
{

bool expectPoolToAllocate(size_t n)
    // Return 'true' if the memory pool used by the container under test is
    // expected to allocate memory on the inserting the specified 'n'th
    // element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}

template <class KEY_CONFIG, class HASH, class EQUAL, class ALLOC>
Link* insertElement(
                  bslstl::HashTable<KEY_CONFIG, HASH, EQUAL, ALLOC> *hashTable,
                  const typename KEY_CONFIG::ValueType&              value)
    // Insert an element having the specified 'value' into the specified
    // 'hashTable' and return the address of the new node, unless the insertion
    // would cause the hash table to exceed its 'maxLoadFactor' and rehash, in
    // which case return a null pointer value.  Return a null pointer value if
    // the 'hashTable' address is a null pointer value.
{
    if (!hashTable) {
        return 0;                                                     // RETURN
    }

    if (hashTable->size() == hashTable->rehashThreshold()) {
        return 0;                                                     // RETURN
    }

    return hashTable->insert(value);
}

template <class SIZE_TYPE>
SIZE_TYPE predictNumBuckets(SIZE_TYPE length, float maxLoadFactor)
    // Return the minimum number of buckets necessary to support the specified
    // 'length' array of elements in a 'HashTable' having the specified
    // 'maxLoadFactor' without rehashing.  Note that typically the result will
    // be passed to a 'HashTable' constructor or reserve call, which may in
    // turn choose to create even more buckets to preserve its growth strategy.
    // This function does not attempt to predict that growth strategy, but
    // merely predict the minimum number of buckets that strategy must
    // accommodate.
{
    if (!length) {
        return 0;                                                     // RETURN
    }

    if (1.0 / static_cast<double>(native_std::numeric_limits<SIZE_TYPE>::max())
                                                             > maxLoadFactor) {
        return native_std::numeric_limits<SIZE_TYPE>::max();          // RETURN
    }

    SIZE_TYPE result = static_cast<SIZE_TYPE>(ceil(static_cast<double>(length)
                     / maxLoadFactor));
    return result ? result : 1;
}

template<class KEY_CONFIG, class COMPARATOR, class VALUE>
int verifyListContents(Link                                 *containerList,
                       const COMPARATOR&                     compareKeys,
                       const bsltf::TestValuesArray<VALUE>&  expectedValues,
                       size_t                                expectedSize)
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
    typedef typename KEY_CONFIG::ValueType ValueType;

    enum { SUCCESS          =  0,
           NO_LIST          = -1,
           LIST_TOO_SHORT   = -2,
           LIST_TOO_LONG    = -3,
           TOO_FEW_VALUES   = -4,
           DISCONTIGUOUS    = -5
    };

    // Check to avoid creating an array of length zero.
    if (0 == containerList) {
        return 0 == expectedSize ? SUCCESS : NO_LIST;                 // RETURN
    }

    if (0 == expectedSize) {
        return LIST_TOO_LONG;                                         // RETURN
    }

    if (expectedValues.size() < expectedSize) {
        return TOO_FEW_VALUES;                                        // RETURN
    }

#if !defined(BSLS_PLATFORM_CMP_CLANG)
    TestMachinery::BoolArray foundValues(expectedSize);
#else
    BoolArray foundValues(expectedSize);
#endif

    size_t i = 0;
    for (Link *cursor = containerList;
         cursor;
         cursor = cursor->nextLink(), ++i)
    {
        const ValueType& element = ImpUtil::extractValue<KEY_CONFIG>(cursor);
        const int nextId = bsltf::TemplateTestFacility::getIdentifier(element);

        for (size_t j = 0; j != expectedSize; ++j) {
            if (bsltf::TemplateTestFacility::getIdentifier(expectedValues[j])
                                                                   == nextId) {
                if (!foundValues[j]) {
                    foundValues[j] = true;
                    break;
                }
            }
        }
    }

    if (expectedSize < i) {
        return LIST_TOO_LONG;                                         // RETURN
    }
    if (i < expectedSize) {
        return LIST_TOO_SHORT;                                        // RETURN
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

    // We make a copy of the comparator in case we are supplied a predicate
    // that has a non-const qualified 'operator()'.  Note that we are now
    // requiring that the comparator be copy-constructible, which may be an
    // issue for testing a 'HashTable' default-constructed with functors that
    // are only default-constructible themselves.

    typename bsl::conditional<bsl::is_function<COMPARATOR>::value,
                             COMPARATOR&,
                             COMPARATOR>::type doCompareKeys(compareKeys);

    // All elements are present, check the contiguity requirement.  Note that
    // this test is quadratic in the length of the list, although we will
    // optimize for the case of duplicates actually occurring.
    for (Link *cursor = containerList; cursor; cursor = cursor->nextLink()) {
        Link *next = cursor->nextLink();
        // Walk to end of key-equivalent sequence
        while (next && doCompareKeys(ImpUtil::extractKey<KEY_CONFIG>(cursor),
                                     ImpUtil::extractKey<KEY_CONFIG>(next))) {
            cursor = next;
            next   = next->nextLink();
        }

        // Check there are no more equivalent keys in the list.  Note that this
        // test also serves to check there are no duplicates in the preceding
        // part of the list, as this check would have failed earlier if that
        // were the case.
        for ( ; next; next = next->nextLink()) {
            if (doCompareKeys(ImpUtil::extractKey<KEY_CONFIG>(cursor),
                              ImpUtil::extractKey<KEY_CONFIG>(next))) {
                return DISCONTIGUOUS;                                 // RETURN
            }
        }
    }

    return SUCCESS;
}

}  // close unnamed namespace

// ============================================================================
//                          TEST DRIVER HARNESS
// ----------------------------------------------------------------------------

#if !defined(BSLS_PLATFORM_CMP_CLANG)
using namespace TestMachinery;
#endif

// - - - - - Wrapper to forward test-class adapters to the main harness - - - -
// Note that this is a generic facility that should be exported to a component
// in its own right, in the 'bsltf' package.

template <class CONFIGURED_DRIVER>
struct TestDriver_ForwardTestCasesByConfiguation {
    // This utility class template provides forwarding utilities for invoking
    // test case functions of the (template type parameter) 'CONFIGURED_DRIVER'
    // class.  It is expected that a 'CONFIGURED_DRIVER' class is an
    // instantiation of the 'TestDriver' class template, with the template
    // arguments chosen to provide a specific portion of test coverage of the
    // whole type-space under test.
    //
    // This supports the following set of templates, implemented in terms of
    // this class template, that are parameterized on the single value type for
    // the container under test, which is the form required for support by the
    // 'bsltf' template testing facility.

    // TEST CASES

    //     There is no testCase1.
    static void testCase2() { CONFIGURED_DRIVER::testCase2(); }
    static void testCase3() { CONFIGURED_DRIVER::testCase3(); }
    static void testCase4() { CONFIGURED_DRIVER::testCase4(); }
        // There is no testCase5.
    static void testCase6() { CONFIGURED_DRIVER::testCase6(); }
    static void testCase7() { CONFIGURED_DRIVER::testCase7(); }
    static void testCase8() { CONFIGURED_DRIVER::testCase8(); }
    static void testCase9() { CONFIGURED_DRIVER::testCase9(); }
        // There is no testCase10.
    static void testCase11() { CONFIGURED_DRIVER::testCase11(); }
    static void testCase12() { CONFIGURED_DRIVER::testCase12(); }
    static void testCase13() { CONFIGURED_DRIVER::testCase13(); }
    static void testCase14() { CONFIGURED_DRIVER::testCase14(); }
    static void testCase15() { CONFIGURED_DRIVER::testCase15(); }
    static void testCase16() { CONFIGURED_DRIVER::testCase16(); }
    static void testCase17() { CONFIGURED_DRIVER::testCase17(); }
    static void testCase18() { CONFIGURED_DRIVER::testCase18(); }
    static void testCase19() { CONFIGURED_DRIVER::testCase19(); }
    static void testCase20() { CONFIGURED_DRIVER::testCase20(); }
    static void testCase21() { CONFIGURED_DRIVER::testCase21(); }
    static void testCase22() { CONFIGURED_DRIVER::testCase22(); }
    static void testCase23() { CONFIGURED_DRIVER::testCase23(); }
    static void testCase24() { CONFIGURED_DRIVER::testCase24(); }
    static void testCase25() { CONFIGURED_DRIVER::testCase25(); }
    static void testCase26() { CONFIGURED_DRIVER::testCase26(); }
    static void testCase27() { CONFIGURED_DRIVER::testCase27(); }
        // Run the test case with the matching number from the supplied
        // (template parameter) type 'CONFIGURED_DRIVER'.
};

//- - - - - - - - - - - - Classes to implement test cases - - - - - - - - - - -

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
class TestDriver;
    // Forward declaration of the main test driver.

// Concerns for template parameters
// --------------------------------
// The primary template supports four template parameters, each of which raises
// concerns that the template behaves correctly when instantiated with a
// variety of types that satisfy the requirements of the appropriate type
// parameter.  These concerns may affect all test cases, although a subset of
// concerns may not apply in specific cases where more precise requirements
// apply.  We enumerate the type-related concerns for each template parameter
// here in a common place that may be referenced in the testing concerns for
// each test case below.  There are also common concerns for the function-like
// types 'HASHER' and 'COMPARATOR' that we catalog separately.
//
// KEY_CONFIG
//- - - - - -
//
//Test Plan
//---------
// We will use a variety of test allocators from 'bsltf' to test the range of
// allocator-related concerns.
//
// Test with a minimal allocator relying on 'allocator_traits' for everything.
// Test with a complete allocator that supplies distinct, observable behavior
//   for each property supported by 'allocator_traits'
// Provide allocators supporting each combination of trait that triggers
//   different allocator behavior
//: o type alias 'propagate_on_container_copy_construction'  (true false)
//: o type alias 'propagate_on_container_copy_assignment'    (true false)
//: o type alias 'propagate_on_container_swap'               (true false)
//: o type alias 'pointer' is a native pointer, or a "smart" pointer
//
// Note that for this initial release we can use a greatly simplified plan, as
// our current implementation of 'allocator_traits' does not support any
// deduction of traits.  We will still use the minimal and maximal allocator
// interfaces supported by this implementation, and the range of customizable
// behaviors.
//
// Simplifying assumptions of our limited 'allocator_traits' implementation:
//: o no support for allocators returning smart pointers
//: o no support for any of the fine-grained propagation traits
//: o no deducing typedefs, everything must be supplied in the allocator
//: o copy constructing a container copies the allocator of the original
//    unless it is 'bsl::allocator', in which case the default is used.
//
// - - - - - - - - - - Pre-packaged test harness adapters - - - - - - - - - - -
// The template test facility, 'bsltf', requires class templates taking a
// single argument, that is the element type to vary in the test.  We desire a
// variety of configurations pushing the various policy parameters of the
// 'HashTable' class template, such as the type of functors, the key extraction
// policy, etc. so we write some simple adapters that will generate the
// appropriate instantiation of the test harness, from a template parameterized
// on only the element type (to be tested).  Note that in C++11 we would use
// the alias- template facility to define these templates, rather than using
// public inheritance through a dispatcher-shim.

// - - - - - - Configuration policies to instantiate HashTable with - - - - - -

template <class ELEMENT>
struct BasicKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static const KeyType& extractKey(const ValueType& value)
        // Return the specified 'value'.
    {
        return value;
    }
};


template <class ELEMENT>
struct ModifiableKeyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.  It also allows for functors that expect to take
    // their argument by a reference to non-const, although behavior will be
    // undefined should any such functor actually modify such an argument.

    typedef ELEMENT KeyType;
    typedef ELEMENT ValueType;

    static KeyType& extractKey(ValueType& value)
        // Return the specified 'value'.
    {
        return value;
    }
};


template <class ELEMENT>
struct BsltfConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef int     KeyType;
    typedef ELEMENT ValueType;

    static const int& extractKey(const ValueType& value);
        // Return a reference to the key corresponding to the specified 'value'
        // if it were to be inserted into a 'HashTable'.
};

template <class ELEMENT>
const int& BsltfConfig<ELEMENT>::extractKey(const ValueType& value)
{
    // Note that this function MUST return a reference, but we have no actual
    // storage to return a reference to.  As we know the specific usage
    // patterns of this test driver, we can ensure that no two hash
    // computations happen while the first result is still held as a reference.
    // Unfortunately, we have no such guarantee on simultaneous evaluations in
    // the HashTable facility itself, so we cycle through a cache of 64
    // results, as no reference should be so long lived that we see 64 live
    // references.  Note that an early version of this test driver demonstrated
    // that we could have at least 16 live references, before the limit was
    // raised to 64.

    static int results_cache[64] = {};
    static int index = -1;

    if (64 == ++index) {
        index = 0;
    }

    results_cache[index] = bsltf::TemplateTestFacility::getIdentifier(value);
    return results_cache[index];
}

struct TrickyConfig {
    // This class provides the most primitive possible KEY_CONFIG type that can
    // support a 'HashTable'.  It might be consistent with use as a 'set' or a
    // 'multiset' container.

    typedef bsltf::NonEqualComparableTestType KeyType;
    typedef TestTypes::AwkwardMaplikeElement  ValueType;

    static const KeyType& extractKey(const ValueType& value)
        // Return the result of calling 'key()' on the specified 'value'.
    {
        return value.key();
    }
};

//- - - - - - - - - - - Test Driver Configuration classes - - - - - - - - - - -
//- - - - - - - (providing coverage of test-parameter concerns) - - - - - - - -

// Configurations concerned with the 'KEY_CONFIG' template parameter

template <class ELEMENT>
struct TestDriver_BasicConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Basic configuration that acts like a multi-set of 'ELEMENT' values.
    // This is the most basic configuration that test the overwhelmingly common
    // case usage.
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
    // 'int' as key, where the 'int' is computed for each element.  This is the
    // simplest way to generate a configuration compatible with the 'bsltf'
    // template testing framework used above, without rewriting each test case
    // to additional support for separate test tables of pairs to initialize a
    // more traditional-style map, with different math for computed values and
    // separate logic for duplicate elements and groups.
};

struct TestDriver_AwkwardMaplike
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< TrickyConfig
                     , TestFacilityHasher<TrickyConfig::KeyType>
                     , ::bsl::equal_to<TrickyConfig::KeyType>
                     , ::bsl::allocator<TrickyConfig::ValueType>
                     >
       > {
    // This configuration is especially tricky, as the element type itself is
    // not equality comparable, but the key-type of the element, which is the
    // sole constituent of the element, does support key-comparison (but not
    // equality comparison) through the supplied comparison functor.
};

// Configurations concerned with the 'HASHER' and 'COMPARATOR' parameters
//
// Function-like types
// - - - - - - - - - -
// A function-like type used to instantiate the 'HashTable' template may be:
//: o A function type
//: o A function pointer type
//: o An object type convertible to a function pointer
//: o An object type convertible to a function reference
//: o An object type that overloads 'operator()'
//: o A reference to a function
//: o A reference to a function pointer
//: o A reference to an object type convertible to a function pointer
//: o A reference to an object type convertible to a function reference
//: o A reference to an object type that overloads 'operator()'
//
// For each function-like type, the arguments to the function call me be:
//: o Passed by 'const &'
//: o Passed by 'const &' to something convertible from the desired argument
//: o Passed by const-unqualified '&' (subject to constraints below)
//: o Passed by value
//: o Passed by value convertible from the desired argument
//
// In each case, the callable signature may have a trailing ellipsis.
//
// For function, function-pointer, and reference-to-function types, the
// underlying function type may also have 'extern "C"' linkage.
//
// For an object-type overloading 'operator()', there are additional concerns:
//: o The operator might be 'const' qualified
//: o The operator might not be 'const' qualified
//: o The operator may be a virtual function
//: o The operator may be a function template
//: o The object-type may be a class template
//: o THe object-type may be a union (THIS IS NOT SUPPORTED)
//
// For an object-type overloading 'operator()'
//: o The type may be const-qualified
//: o The object may have internal state
//: o The object may allocate memory for state from its own allocator
// We note that a simple example of a function-like type holding state is a
// function pointer.
//
// Any invocation of a function-like type, within contract, may throw an
// exception of any type.
//
// For user-defined class types, any constructor may throw an exception of any
// type, as may any assignment operator.
//
// User defined types need not support all the implicitly declared operations,
// such as:
//: o default constructor
//: o copy constructor
//: o operator=
//: o operator&
//: o operator,
//: o operator new
//: o operator delete
//
// QoI: Where the function-like entity is an empty class type, it should not
//      add to the overall size of the 'HashTable' object.
//
// We note, with mild relief, that the one thing we are not required to support
// is a pointer-to-member-function.
//
// HASHER
//- - - -
// 'HASHER' is a function-like type that takes a single argument convertible
// from 'typename KEY_CONFIG::KeyType' and returns a value of the type
// 'std::size_t'.
//
// COMPARATOR
//- - - - - -
// 'COMPARATOR' is a function-like type that takes two arguments, each
// convertible from 'typename KEY_CONFIG::KeyType', and returns a value of a
// type that is implicitly convertible to 'bool'.

template <class ELEMENT>
struct TestDriver_StatefulConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , ThrowingHashFunctor<ELEMENT>
                     , ThrowingEqualityComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // Simple configuration to test simple stateful functors for the hasher and
    // comparator policies.  Both functors support an 'id' that provides state
    // that is not actually used when evaluating the function call operator,
    // and track the total number of calls to the function call operator.  An
    // additional member may be used to enable testing of exception safety, by
    // throwing exceptions from the function call operator every 'N'th time it
    // is called.  This extra state is declared as 'mutable' so that, as a
    // particularly unusual case, the function to change this state can be
    // declared 'const' to better facilitate testing.
};

template <class ELEMENT>
struct TestDriver_GroupedUniqueKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values of the 'ELEMENT' type to the same hash code, but maintaining a
    // unique key value for each of those cases.  This should lead to a higher
    // rate of collisions.
};

template <class ELEMENT>
struct TestDriver_GroupedSharedKeys
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GroupedEqualityComparator<ELEMENT, 5>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration "groups" values into buckets by hashing 5 consecutive
    // values of the 'ELEMENT' type to the same hash code, and similarly
    // arranging for those keys to compare equal to each other.  This should
    // lead to behavior similar to a 'multiset'.
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT> >
                     , bsltf::DegenerateFunctor<
                                          ThrowingEqualityComparator<ELEMENT> >
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration utilizes awkward functors, that abuse the implicitly
    // declared operations (such as the address-of and comma operators) and
    // return awkward types from predicates, rather than a simple 'bool'.
    // However, this configuration does support the 'swap' operation in order
    // to support a broad range of test cases.
};

template <class ELEMENT>
struct TestDriver_DegenerateConfiguationWithNoSwap
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , bsltf::DegenerateFunctor<TestFacilityHasher<ELEMENT>,
                                                false>
                     , bsltf::DegenerateFunctor<
                                           ThrowingEqualityComparator<ELEMENT>,
                                           false>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration utilizes awkward functors, that abuse the implicitly
    // declared operations (such as the address-of and comma operators) and
    // return awkward types from predicates, rather than a simple 'bool'.  As
    // this configuration does not support swapping of the functor types, it
    // does not support as broad a range of test cases as some of the other
    // configurations.
};

template <class ELEMENT>
struct TestDriver_ConstFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , const TestFacilityHasher<ELEMENT>
                     , const ::bsl::equal_to<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration employs const-qualified functors, and so cannot be
    // used to test behavior that may require modifying functors, such as the
    // assignment operator, or 'swap'.
};

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING)
template <class ELEMENT>
struct TestDriver_FunctorReferences
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestFacilityHasher<ELEMENT> &
                     , ::bsl::equal_to<ELEMENT> &
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with reference type for
    // the callable template parameters, which reference types that overload
    // the function call operator.  Note that this is more general than C++11
    // requires, as the hasher must be a hash function *object*.  Technically
    // we must support the comparator being a function-type or a function-
    // reference, so we test this implementation-detail component supports the
    // hasher as well.
};
#endif

template <class ELEMENT>
struct TestDriver_FunctionTypes
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(const ELEMENT&)
                     , bool(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-type
    // arguments for the hash type and key-comparison type.  Note that this is
    // more general than C++11 requires, as the hasher must be a hash function
    // *object*.  Technically we must support the comparator being a
    // function-type or a function-reference, so we test this implementation-
    // detail component supports the hasher as well.
};

template <class ELEMENT>
struct TestDriver_FunctionPointers
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(*)(const ELEMENT&)
                     , bool(*)(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-pointer type
    // arguments for the hash type and key-comparison type.  Note that this
    // corresponds to testing a stateful functor, with the awkward case that
    // the default value for the callable types is known to produce undefined
    // behavior (null pointer values for the function pointers).
};

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
template <class ELEMENT>
struct TestDriver_FunctionReferences
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , size_t(&)(const ELEMENT&)
                     , bool(&)(const ELEMENT&, const ELEMENT&)
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration instantiates the HashTable with function-reference
    // arguments for the hash type and key-comparison type.  Note that this is
    // more general than C++11 requires, as the hasher must be a hash function
    // *object*.  Technically we must support the comparator being a
    // function-type or a function-reference, so we test this implementation-
    // detail component supports the hasher as well.
};
#endif

template <class ELEMENT>
struct TestDriver_ConvertibleValueConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , TestConvertibleValueHasher<ELEMENT>
                     , TestConvertibleValueComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration tests functors whose arguments are convertible from
    // the key-type of the configuration.
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
    // This configuration tests functors whose function-call operator is a
    // function template that will deduce type as appropriate from the
    // arguments supplied to the function call.
};

template <class ELEMENT>
struct TestDriver_ModifiableFunctors
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< ModifiableKeyConfig<ELEMENT>
                     , ModifiableHasher<ELEMENT>
                     , ModifiableComparator<ELEMENT>
                     , ::bsl::allocator<ELEMENT>
                     >
       > {
    // This configuration tests functors whose arguments to the function-call
    // operator are passed by non-'const' reference, but honor the spirit of
    // the rule by not actually making any modifications to those arguments.
    // This is something we test only because standard conformance requires it.
};

// Configurations concerned with the 'ALLOCATOR' template parameter

// ALLOCATOR
// - - - - -
// The 'ALLOCATOR' argument must conform to the C++11 allocator requirements,
// and a minimal allocator would have (almost) all of its properties deduced by
// 'std::allocator_traits'.  Therefore, allocators that do or do not supply
// each of the following should be supported, and behave as specified by
// 'std::allocator_traits' (noting that 'allocator_traits' is a separately
// implemented and tested component that we depend on - this is not intended to
// test 'allocator_traits', but merely that this component correctly uses
// 'allocator_traits' rather than the object allocator directly):
//: o type alias 'pointer'
//: o type alias 'const_pointer'
//: o type alias 'void_pointer'
//: o type alias 'const_void_pointer'
//: o type alias 'size_type'
//: o type alias 'difference_type'
//: o type alias 'propagate_on_container_copy_construction'
//: o type alias 'propagate_on_container_copy_assignment'
//: o type alias 'propagate_on_container_swap'
//: o alias template 'rebind<OTHER>'
//: o 'construct(ANY*, ARGS...)'
//: o 'destroy(ANY*)'
//: o 'max_size()'
//
// While an allocator must be CopyConstructible, there is no requirement that
// it support any of the other implicitly declared operations:
//: o default constructor
//: o operator=
//: o operator&
//: o operator,
//: o operator new
//: o operator delete
// Note that some specific functions, such as the 'HashTable' default
// constructor, may require some of these extra functions, but there is no
// general requirement to supply these unless a specific function contract
// explicitly states that this operation is required, and that additional
// requirement applies to only that one function.
//
// When the allocator is 'bsl::allocator<TYPE>' then we have a number of
// additional concerns, that are mostly runtime issues rather than type-
// related issues.  They will be raised and addressed in the usual manner under
// the actual test case.

template <class ELEMENT>
struct TestDriver_StdAllocatorConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GenericHasher
                     , bsl::equal_to<ELEMENT>
                     , bsltf::StdTestAllocator<ELEMENT>
                     >
       > {
    // This configuration uses a stateless std-conforming allocator, rather
    // than using the BDE allocator model.  To enhance test coverage of the
    // functor, we mix the generic hasher with 'bsl::equal_to', the default
    // comparator.  Note that all of these parameter types are empty, so we
    // expect to see the largest benefit from empty-base optimizations.
};

template <class ELEMENT>
struct TestDriver_StatefulAllocatorConfiguation
     : TestDriver_ForwardTestCasesByConfiguation<
           TestDriver< BasicKeyConfig<ELEMENT>
                     , GroupedHasher<ELEMENT, bsl::hash<int>, 5>
                     , GenericComparator
                     , bsltf::StdStatefulAllocator<ELEMENT,
                                                   true,
                                                   false,
                                                   false,
                                                   false>
                     >
       > {
    // This configuration uses a std-conforming allocator with per-object state
    // rather than using the BDE allocator model, and uses a different
    // allocator propagation strategy.  Allocators will propagate on copy
    // construction, but on no other operation.  We pick another mix of hasher
    // and comparator that were previously paired differently when testing
    // callable-parameter concerns.
};

#if defined(BSLSTL_ALLOCATOR_TRAITS_COMPUTES_ALLOCTOR_PROPAGATION)
// bsl::allocator_traits does not yet detect and support the allocator
// propagation predicate traits; these test configurations will be fully
// specced out once support is available

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
#endif

// - - - - - Special configurations for testing default constructor - - - - - -

// These configurations are required only for a special set of tests using the
// default constructor for HashTable, which can be instantiated for hasher and
// comparator types that are default constructible, but awkward in every other
// way.  This will be the only test driver to examine the limits of default
// constructible (only) types.

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

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static int ggg(Obj *object, const char *spec, bool verbose = true);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'insert' and white-box
        // manipulator 'clear'.  Optionally specify 'verbose' as false to
        // suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Returned
        // values lower than '-1' indicate some error other than parsing the
        // 'spec' string.  Note that this function is used to implement 'gg' as
        // well as allow for verification of syntax error detection.

  public:
    // TEST CASES

    //          Test case 1 is handled separately
    static void testCase2();
    static void testCase3();
    static void testCase4();
        //      Test case 5 is not supported
    static void testCase6();
    static void testCase7();
    static void testCase8();
    static void testCase9();
        //      Test case 10 is not supported
    static void testCase11();
    static void testCase12();
    static void testCase13();
    static void testCase14();
    static void testCase15();
    static void testCase16();
        // Run the test case with the corresponding case number for
        // 'HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>'.
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                               // --------------
                               // TEST APPARATUS
                               // --------------

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
bslstl::HashTable<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>&
TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::gg(Obj        *object,
                                                          const char *spec)
{
    int result = ggg(object, spec);
    ASSERTV(result, -1 == result);
    return *object;
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
int TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::ggg(
                                                           Obj        *object,
                                                           const char *spec,
                                                           bool        verbose)
{
    enum { SUCCESS = -1 };

    if (!object) {
        return -2;                                                    // RETURN
    }

    if (!spec) {
        return -3;                                                    // RETURN
    }

    if (object->rehashThreshold() < object->size() + strlen(spec)) {
        return -4;                                                    // RETURN
    }

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int i = 0; spec[i]; ++i) {
        if (spec[i] <'A' || 'Z' < spec[i]) {
            if (verbose) {
                printf("Error, bad character ('%c') "
                       "in spec \"%s\" at position %d.\n", spec[i], spec, i);
            }
            return i;   // Discontinue processing this spec.          // RETURN
        }

        if (!insertElement(object, VALUES[spec[i] - 'A'])) {
            if (verbose) {
                printf("Unexpected error inserting element no. %d from the "
                       "spec string ('%s').\n", i, spec);
            }
            return -100 -i;   // Discontinue processing this spec.    // RETURN
        }
   }
   return SUCCESS;
}

//- - - - - - - - - - - - - TEST CASE IMPLEMENTATIONS - - - - - - - - - - - - -
bool exchange(bool &value, bool newValue)
    // Return the original specified 'value', and set 'value' to the specified
    // 'newValue'.  Note that this function is a specific implementation of a
    // standard C++11 template, intended to eliminate use of the deprecated
    // post-increment operator on 'bool'.
{
    bool result = value;
    value = newValue;
    return result;
}

template<class KEY_CONFIG, class COMPARATOR>
void testVerifyListContents(const COMPARATOR& compare)
    // ------------------------------------------------------------------------
    // TESTING TEST MACHINERY FUNCTION 'verifyListContents'
    //   This function is an implementation detail of 'testCase3' that tests
    //   only the 'verifyListContents' test machinery function using the
    //   specified 'compare' functor to determine that two list elements have
    //   the same value.  It is pulled out into a separate function to avoid
    //   redundant work, as many instantiations of 'testCase3' would
    //   redundantly test the same combination of type parameters.  These kinds
    //   of optimizations are necessary due to the unusual complexity of this
    //   test driver, which can push compiler resource limits.
    //
    // Concerns:
    //: 1 Passing a null pointer value for the link returns an error code,
    //:   unless the expected sequence has a length of zero.
    //:
    //: 2 Passing an expected size that is not the same as the length of the
    //:   list rooted at passed link returns an error code.
    //:
    //: 3 Passing an expected size that is larger than the size of the passed
    //:   value-array returns an error code.
    //:
    //: 4 Passing a value-list longer than the expected size is not an error.
    //:
    //: 5 An error code is returned if there are duplicate elements in the
    //:   passed list, as determined by the passed comparator, and those
    //:   duplicates do not form a contiguous subsequence within the list.
    //:
    //: 6 Success code of '0' is returned if the list rooted at the passed link
    //:   holds elements corresponding to a permutation of the first N values
    //:   in the value-list, where N is the length of the list.
    //:
    //: 7 If the list is not a permutation of the first N values, then return
    //:   the number of elements that do not correspond to values in the
    //:   first N positions of the value-list.
    //:
    //: 8 No memory is allocated by either the global or default allocators.
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    // ------------------------------------------------------------------------
{
    static bool alreadyTested = false;
    if (exchange(alreadyTested, true)) {
        // This function may be invoked many times, but each instantiation need
        // only run the first time.

        return;                                                       // RETURN
    }

    typedef typename KEY_CONFIG::ValueType    ValueType;
    typedef bsltf::TestValuesArray<ValueType> TestValues;

    bslma::TestAllocator *gta = dynamic_cast<bslma::TestAllocator *>(
                                            bslma::Default::globalAllocator());
    if (!gta) {
        printf("BAD CONFIGURATION OF TEST DRIVER: global allocator is not a"
               " TestAllocator.\n");
        abort();
    }

    bslma::TestAllocatorMonitor gam(gta);

    bslma::TestAllocator da("default testing verifyListContents",
                            veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocatorMonitor dam(&da);

    bslma::TestAllocator pa("pool allocator", veryVeryVeryVerbose);
    bslstl::BidirectionalNodePool<ValueType, bsl::allocator<ValueType> >
                                                                    pool(&pa);

    if (verbose) printf("\nTesting verifyListContents initial conditions.\n");
    {
        static const struct {
            int         d_line;      // source line number
            const char *d_listSpec;  // specification string to build list
            const char *d_valSpec;   // specification string for test values
            size_t      d_expSz;     // expected size
            int         d_error;     // expected error code
        } DATA[] = {
            //line  list    vals  expSz  error
            //----  ----    ----  -----  -----
            { L_,     "",     "",     0,     0 }, // control
            { L_,     "",     "",     1,    -1 },
            { L_,     "",    "A",     0,     0 },
            { L_,     "",    "A",     1,    -1 },

            { L_,    "A",     "",     0,    -3 },
            { L_,    "A",     "",     1,    -4 },
            { L_,    "A",    "A",     0,    -3 },
            { L_,    "A",    "A",     1,     0 },
            { L_,    "A",    "A",     2,    -4 },
            { L_,    "A",    "B",     0,    -3 },
            { L_,    "A",    "B",     1,     1 },
            { L_,    "A",    "B",     2,    -4 },

            { L_,   "AA",     "",     0,    -3 },
            { L_,   "AA",     "",     1,    -4 },
            { L_,   "AA",     "",     2,    -4 },
            { L_,   "AA",     "",     3,    -4 },
            { L_,   "AA",    "A",     2,    -4 },
            { L_,   "AA",    "B",     2,    -4 },
            { L_,   "AA",   "AA",     2,     0 },
            { L_,   "AA",   "AB",     2,     1 },
            { L_,   "AA",   "BB",     2,     2 },
            { L_,   "AA",  "AAA",     2,     0 },
            { L_,   "AA",  "AAZ",     2,     0 },
            { L_,   "AA",  "ABC",     2,     1 },
            { L_,   "AA",  "AAZ",     3,    -2 },
            { L_,   "AA",  "BCD",     3,    -2 },

            { L_,   "AB",    "A",     2,    -4 },
            { L_,   "AB",    "B",     2,    -4 },
            { L_,   "AB",   "AA",     2,     1 },
            { L_,   "AB",   "AB",     2,     0 },
            { L_,   "AB",   "AC",     2,     1 },
            { L_,   "AB",   "BA",     2,     0 },
            { L_,   "AB",   "BB",     2,     1 },
            { L_,   "AB",   "BC",     2,     1 },
            { L_,   "AB",   "CA",     2,     1 },
            { L_,   "AB",   "CB",     2,     1 },
            { L_,   "AB",   "CC",     2,     2 },
            { L_,   "AB",  "AAA",     2,     1 },
            { L_,   "AB",  "ABC",     2,     0 },
            { L_,   "AB",  "ACB",     2,     1 },

            { L_,  "AAA",     "",     2,    -4 },
            { L_,  "AAA",  "AAA",     2,    -3 },
            { L_,  "AAA",  "AAA",     3,     0 },
            { L_,  "AAA",  "AAA",     4,    -4 },
            { L_,  "AAA",  "AAB",     3,     1 },
            { L_,  "AAA",  "ABA",     3,     1 },
            { L_,  "AAA",  "BAA",     3,     1 },
            { L_,  "AAA",  "ABB",     3,     2 },
            { L_,  "AAA",  "ABC",     3,     2 },
            { L_,  "AAA",  "BCD",     3,     3 },

            { L_,  "AAX",  "AAX",     3,     0 },
            { L_,  "AAX",  "AXA",     3,     0 },
            { L_,  "AXA",  "AAX",     3,    -5 },
            { L_,  "XAA",  "AAX",     3,     0 },
            { L_,  "BCA",  "CAB",     3,     0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti != NUM_DATA ; ++ti) {
            const int         LINE      = DATA[ti].d_line;
            const char *const LIST_SPEC = DATA[ti].d_listSpec;
            const char *const VAL_SPEC  = DATA[ti].d_valSpec;
            const size_t      EXP_SIZE  = DATA[ti].d_expSz;
            const int         EXP_ERR   = DATA[ti].d_error;

            bslma::TestAllocator tva("test values", veryVeryVeryVerbose);
            const TestValues  EXP_VALS(VAL_SPEC, &tva);

            bslma::TestAllocator lva("list values", veryVeryVeryVerbose);
            const TestValues  LIST_VALS(LIST_SPEC, &lva);

            Link *pRoot = 0;
            for (size_t i = LIST_VALS.size(); i; /**/) {
                Link *newNode = pool.createNode(LIST_VALS[--i]);
                bslalg::BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                                     newNode,
                                                                     pRoot);
                pRoot = newNode;
            }

            int failCode = verifyListContents<KEY_CONFIG>(pRoot,
                                                          compare,
                                                          EXP_VALS,
                                                          EXP_SIZE);
            ASSERTV(LINE, EXP_ERR, failCode, EXP_ERR == failCode);

            // Note that this clean-up loop at the end of scope is not
            // exception safe, but we know our test types should not throw
            // under these test conditions.

            while (pRoot) {
                Link *next = pRoot->nextLink();
                pool.deleteNode(pRoot);
                pRoot = next;
            }
        }
    }

    ASSERT(0 == dam.numBlocksTotalChange());
    ASSERT(0 == gam.numBlocksTotalChange());
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase2()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
    //   The basic concern is that a 'HashTable' object can be constructed into
    //   a (valid) well-defined state, then through use of manipulators brought
    //   into any other valid state, and finally that the object destroys all
    //   its elements and leaks no memory on destruction.  For the purposes of
    //   testing, the well-defined state will be a 'HashTable' having no
    //   elements, having the specified or default-constructed allocator,
    //   having a hasher and comparator supplied by the 'MakeCallableEntity'
    //   factory function (which provides a default-constructed functor where
    //   available, and is specialized to provide a standard functor object
    //   otherwise), and having a number of buckets calculated to be sufficient
    //   to support the largest number of elements required for that test
    //   iteration, at the specified maximum load factor.  The primary
    //   manipulators will be a free function ('insertElement', created for the
    //   purpose of testing) that inserts a single element of a specific type,
    //   but only if there is sufficient capacity in the table to insert
    //   without requiring a rehash, and the 'removeAll' method.
    //
    // Concerns:
    //: 1 An object created with the value constructor (with or without a
    //:   supplied allocator) has the supplied hasher, comparator, allocator
    //:   and maxLoadFactor, and at least the supplied initial number of
    //:   buckets.
    //:
    //: 2 The number of buckets is 1 or a prime number.
    //:
//: 2 If the requested number of buckets cannot be supplied, an exception
//:   is thrown (need to nail down the contract on which exception)
    //:
    //: 2 If more than zero buckets are requested, there shall be at least
    //:   enough buckets to insert one element, or else the constructor will
    //:   fail with an exception.
    //:
    //: 2 The maximum load factor may be arbitrarily large, including
    //:   infinity.
    //:
    //: 2 The maximum load factor may be arbitrarily tiny, as long as it is
    //:   positive.
    //:
    //: 2 QoI: An assertion will be triggered in appropriate build modes if
    //:   the requested max load factor is zero, negative, or a NaN.
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
    //:10 If any constructor fails by throwing an exception, no memory is
    //:   leaked.
    //:
    //:10 Every object releases any allocated memory at destruction.
    //:
    //:11 QoI: The value constructor allocates no memory if the initial number
    //:   of bucket is 0.
    //:
    //:12 'insertElement' increases the size of the object by 1.
    //:
    //:13 'insertElement' returns the address of the newly added element.
    //:
    //:14 'insertElement' puts the element into the list of elements defined by
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
    //:   contiguously at the beginning of the range of existing equivalent
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
    //   HashTable(HASHER, COMPARATOR, SizeType, float, ALLOC)
    //   ~HashTable();
    //   void removeAll();
    //   insertElement      (test driver function, proxy for basic manipulator)
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::KeyType   Key;
    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);  // Contains 52 distinct increasing values.
    const SizeType MAX_LENGTH = 19; // This should be sufficient to bootstrap.

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

    typedef typename CallableVariable<    HASHER>::type     HASHER_TYPE;
    typedef typename CallableVariable<COMPARATOR>::type COMPARATOR_TYPE;

    const HASHER_TYPE     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR_TYPE COMPARE = MakeCallableEntity<COMPARATOR>::make();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (verbose) printf("\nTesting fail-modes of 'insertElement'.\n");
    {
        bslma::TestAllocator dummy("dummy allocator", veryVeryVeryVerbose);
        bsltf::StdTestAllocatorConfigurationGuard dummyGuard(&dummy);
        ALLOCATOR dummyAlloc = MakeAllocator<ALLOCATOR>::make(&dummy);

        Link *inserted = insertElement(static_cast<Obj *>(0), VALUES[0]);
        ASSERT(0 == inserted);

        Obj mX(HASH, COMPARE, 0, 1.0f, dummyAlloc);
        ASSERTV(mX.rehashThreshold(), 0 == mX.rehashThreshold());

        inserted = insertElement(&mX, VALUES[0]);
        ASSERT(0 == inserted);

        // An infinite max load factor appears to imply "never rehash" but a
        // default-constructed HashTable has no storage.

        Obj mY(HASH,
               COMPARE,
               0,
               native_std::numeric_limits<float>::infinity(),
               dummyAlloc);
        ASSERTV(mY.rehashThreshold(), 0 == mY.rehashThreshold());

        inserted = insertElement(&mY, VALUES[0]);
        ASSERT(0 == inserted);
    }

    if (verbose) printf("\nTesting with various allocator configurations.\n");

    for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
    for (SizeType ti = 0; ti < MAX_LENGTH; ++ti) {
        const float    MAX_LF = DEFAULT_MAX_LOAD_FACTOR[lfi];
        const SizeType LENGTH = ti;

        for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
            const char CONFIG = *cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            {
            bslma::DefaultAllocatorGuard dag(&da);

            // There is no easy way to create this guard for the specific test
            // case of the stateless 'bsltf::StdTestAllocator', nor without
            // second guessing the allocator to use based upon the 'cfg' code.
            // By the time we return from 'makeAllocator' the test allocator
            // will already have been installed, with no easy way to restore at
            // the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG('a' == CONFIG
                                                                   ? &sa
                                                                   : &da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting bootstrap constructor.\n");
            }

            const SizeType NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            Obj       *objPtr;
            ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                        CONFIG,
                                                       &fa,
                                                       &sa,
                                                        HASH,
                                                        COMPARE,
                                                        NUM_BUCKETS,
                                                        MAX_LF);
            bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                       &fa);
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

            const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
            const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null pointer.
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
                       "\t\tOn an object of initial length " ZU ".\n",
                       static_cast<size_t>(LENGTH));

                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        oa->numBlocksTotal(),   oa->numBlocksInUse(),
                        oa->numBlocksTotal() == oa->numBlocksInUse());

                for (size_t tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    Key key = ImpUtil::extractKey<KEY_CONFIG>(RESULT);
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(elem),
                                     key));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                Element lastElem = VALUES[LENGTH - 1];
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(lastElem),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        LENGTH == X.size());

                // check elements with equivalent keys are contiguous

                // check expected elements are present in container, with
                // expected number of duplicates

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPARE,
                                                           VALUES,
                                                           LENGTH));
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

            // If we repeat the 'insertElement' test, then memory usage is much
            // more predictable, as the node-pool should have sufficient free
            // nodes from the previous loop.

            if (veryVerbose) { printf(
                  "\n\tRepeat testing 'insertElement', with memory checks.\n");
            }
            if (0 < LENGTH) {
                if (veryVerbose) printf(
                       "\t\tOn an object of initial length " ZU ".\n",
                       static_cast<size_t>(LENGTH));

                for (SizeType tj = 0; tj < LENGTH - 1; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                     KEY_CONFIG::extractKey(elem),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   VALUES[tj],
                                   ImpUtil::extractValue<KEY_CONFIG>(RESULT)));
                }

                Link *RESULT = insertElement(&mX, VALUES[LENGTH - 1]);
                ASSERT(0 != RESULT);
                Element lastElem = VALUES[LENGTH - 1];
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(KEY_CONFIG::extractKey(lastElem),
                                  ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                ASSERTV(MAX_LF, LENGTH, CONFIG,
                        BSL_TF_EQ(VALUES[LENGTH - 1],
                                  ImpUtil::extractValue<KEY_CONFIG>(RESULT)));

                ASSERTV(MAX_LF, LENGTH, CONFIG, LENGTH == X.size());

                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.

                ASSERTV(MAX_LF, LENGTH, CONFIG, X,
                       0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                           COMPARE,
                                                           VALUES,
                                                           LENGTH));
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
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                   KEY_CONFIG::extractKey(elem),
                                   ImpUtil::extractKey<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                }
                ITER[LENGTH] = 0;

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                        2 * LENGTH == X.size());

#if 0
                // The second loop adds another duplicate in front of each
                // the items from the previous loop, and not in the middle of
                // any subranges.
                for (SizeType tj = 0; tj < LENGTH; ++tj) {
                    Link *RESULT = insertElement(&mX, VALUES[tj]);
                    ASSERT(0 != RESULT);
                    Element elem = VALUES[tj];
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            X.comparator()(
                                     KEY_CONFIG::extractKey(elem),
                                     ImpUtil::extractKey<KEY_CONFIG>(RESULT)));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            BSL_TF_EQ(
                                 VALUES[tj],
                                 ImpUtil::extractValue<KEY_CONFIG>(ITER[tj])));
                    ASSERTV(MAX_LF, LENGTH, tj, CONFIG,
                            ITER[tj] == RESULT->nextLink());
                }

                ASSERTV(MAX_LF, LENGTH, CONFIG, X.size(),
                         3 * LENGTH == X.size());
#endif
            }

            // ----------------------------------------------------------------

            // Reclaim dynamically allocated object under test.

            //fa.deleteObject(objPtr);
            }

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

    // There is no easy way to create this guard for the specific test case of
    // the stateless 'bsltf::StdTestAllocator', nor without second guessing the
    // allocator to use based upon the 'cfg' code.  By the time we return from
    // 'makeAllocator' the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&da);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&da);

#if defined BDE_BUILD_TARGET_EXC
    if (verbose) printf(
                  "\nTesting correct exceptions are thrown by constructor.\n");
    {
        if (veryVerbose) printf(
                        "\tmax SizeType buckets at max load factor of 1.0.\n");
        try {
            Obj mX(HASH,
                   COMPARE,
                   native_std::numeric_limits<SizeType>::max(),
                   1.0f,
                   objAlloc);
            ASSERT(false);
        }
        catch(const native_std::length_error&) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!"The wrong exception type was thrown.");
        }

        if (veryVerbose) printf(
                 "\tmax SizeType buckets with an infinite max load factor.\n");
        try {
            // Even with an infinite load factor, we cannot allocate more
            // buckets than fit into memory.

            Obj mX(HASH,
                   COMPARE,
                   native_std::numeric_limits<SizeType>::max(),
                   std::numeric_limits<float>::infinity(),
                   objAlloc);
            ASSERT(false);
        }
        catch(const native_std::length_error&) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!"The wrong exception type was thrown.");
        }

#if !defined(BSLS_PLATFORM_CPU_64_BIT)
        // this will time out on 64-bit platforms, is it still probing anything
        // useful?  Yes - the 'max' above will be larger than the max value in
        // the prime number table, and so generate a length_error from a
        // different check.
        if (veryVerbose) printf(
                           "\tmax 'int' buckets at max load factor of 1.0.\n");
        try {
            const Obj X(HASH,
                        COMPARE,
                        native_std::numeric_limits<int>::max(),
                        1.0f,
                        objAlloc);
            // It is unlikely this will fit into a 64-bit allocation, but we
            // will allow for the possibility.
            ASSERT(
                 static_cast<SizeType>(native_std::numeric_limits<int>::max())
                                                             < X.numBuckets());
        }
        catch(const native_std::bad_alloc&) {
            // This is the expected code path
        }
        catch(...) {
            ASSERT(!"The wrong exception type was thrown.");
        }
#endif  // BSLS_PLATFORM_CPU_64_BIT
       if (veryVerbose) printf(
                      "\tmax SizeType buckets with a tiny max load factor.\n");
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
            ASSERT(!"The wrong exception type was thrown.");
        }
    }
#endif

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        const float FLT_TINY = std::numeric_limits<float>::denorm_min();
        const float FLT_NAN  = std::numeric_limits<float>::quiet_NaN();
        const float FLT_INF  = std::numeric_limits<float>::infinity();
        // silence the "unused variable" warning(s) in release builds:
        (void) FLT_TINY;
        (void) FLT_NAN;
        (void) FLT_INF;

        ASSERT_PASS_RAW(Obj(HASH, COMPARE, 0,      1.0f, objAlloc));
        ASSERT_PASS_RAW(Obj(HASH, COMPARE, 0,  FLT_TINY, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0,      0.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0,     -0.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0, -FLT_TINY, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0,     -1.0f, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0,  -FLT_INF, objAlloc));
        ASSERT_SAFE_FAIL_RAW(Obj(HASH, COMPARE, 0,   FLT_NAN, objAlloc));
        ASSERT_PASS_RAW(Obj(HASH, COMPARE, 0,   FLT_INF, objAlloc));
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase3()
{
    // ------------------------------------------------------------------------
    // TESTING PRIMITIVE GENERATOR FUNCTIONS 'gg' AND 'ggg':
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
    //   int ggg(HashTable *object, const char *spec, bool verbose);
    //   HashTable& gg(HashTable *object, const char *spec);
    //   verifyListContents(Link *, const COMPARATOR&, const VALUES&, size_t);
    // ------------------------------------------------------------------------

    typedef typename Obj::SizeType         SizeType;

    const size_t NUM_DATA                  = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef typename CallableVariable<    HASHER>::type     HASHER_TYPE;
    typedef typename CallableVariable<COMPARATOR>::type COMPARATOR_TYPE;

    const HASHER_TYPE     HASH  = MakeCallableEntity<HASHER>::make();
    const COMPARATOR_TYPE EQUAL = MakeCallableEntity<COMPARATOR>::make();

    bslma::TestAllocator oa("generator allocator", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific test case of
    // the stateless 'bsltf::StdTestAllocator', nor without second guessing the
    // allocator to use based upon the 'cfg' code.  By the time we return from
    // 'makeAllocator' the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&oa);

    if (verbose) printf("\nTesting 'testVerifyListContents'.\n");
    {
        testVerifyListContents<KEY_CONFIG>(EQUAL);
    }

    if (verbose) printf("\nTesting generator initial conditions.\n");
    {
        bslma::TestAllocator tda("bad gerenators", veryVeryVeryVerbose);
        Obj obj(HASH, EQUAL, 0, 1.0f, objAlloc);

        int failCode = ggg(&obj, "", verbose);  // success!
        ASSERTV(&obj, failCode, -1 == failCode);

        failCode = ggg(0, "", verbose);         // null OBJ address
        ASSERTV(failCode, -2 == failCode);

        failCode = ggg(&obj, 0, verbose);       // null SPEC address
        ASSERTV(&obj, failCode, -3 == failCode);

        failCode = ggg(&obj, "A", verbose);     // not enough buckets
        ASSERTV(&obj, failCode, -4 == failCode);
    }

    if (verbose) printf("\nTesting generator on valid specs.\n");
    {
        if (veryVerbose) {
            printf("\tTesting with Max Load Factor\n");
        }

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
            const float MAX_LF  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            if (veryVerbose) {
                T_ P(MAX_LF);
            }

            SizeType oldLen = 0;
            for (size_t ti = 0; ti != NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;
                const char *const EXP_S  = DATA[ti].d_results;

                const SizeType   LENGTH = static_cast<SizeType>(strlen(EXP_S));

                bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
                const TestValues  EXP(EXP_S, &tda);

                const SizeType curLen = static_cast<SizeType>(strlen(SPEC));

                if (curLen != oldLen || !oldLen) {
                    if (verbose) printf("\tof length " ZU ":\n",
                                        static_cast<size_t>(curLen));
                    ASSERTV(LINE, oldLen <= curLen);  // non-decreasing

                    // Let us check for a fail code reported from 'ggg' for a
                    // SPEC string longer than can be supported without a
                    // rehash.

                    if (0 < oldLen) {
                        // First we compute a number of buckets to request that
                        // should be too few to support the required number of
                        // elements.

                        SizeType badLen = oldLen / 2;
                        if (0 < badLen) {
                            --badLen;
                        }

                        Obj mBadObj(HASH,
                                    EQUAL,
                                    static_cast<SizeType>(badLen),
                                    1.0f,
                                    objAlloc);
                        int failCode = ggg(&mBadObj, SPEC, verbose);
                        ASSERTV(LINE, SPEC, LENGTH, badLen, failCode,
                                -4 == failCode);
                    }
                    oldLen = curLen;
                }

                const SizeType NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

                Obj mX(HASH, EQUAL, NUM_BUCKETS, MAX_LF, objAlloc);
                const Obj& X = gg(&mX, SPEC);   // original spec

                if (veryVerbose) {
                    printf("\t\tSpec = \"%s\"\n", SPEC);
                    T_ T_ T_ P(X);
                }

                ASSERTV(LINE, LENGTH == X.size());
                ASSERTV(LINE, MAX_LF, SPEC, EXP_S,
                    0 == verifyListContents<KEY_CONFIG>(X.elementListRoot(),
                                                        EQUAL,
                                                        EXP,
                                                        LENGTH));
            }
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
            const SizeType    LENGTH = static_cast<SizeType>(strlen(SPEC));

            Obj mX(HASH, EQUAL, static_cast<SizeType>(LENGTH), 1.5f, objAlloc);

            if (LENGTH != oldLen) {
                if (verbose) printf("\tof length " ZU ":\n",
                                    static_cast<size_t>(LENGTH));
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
    //   comparator() const;
    //   hasher() const;
    //   size() const;
    //   numBuckets() const;
    //   maxLoadFactor() const;
    //   rehashThreshold() const;
    //   elementListRoot() const;
    //   bucketAtIndex(SizeType index) const;
    //   bucketIndexForKey(const KeyType& key) const;
    //   countElementsInBucket(SizeType index) const;
    // ------------------------------------------------------------------------

    typedef typename KEY_CONFIG::ValueType Element;
    typedef bslalg::HashTableImpUtil       ImpUtil;
    typedef typename Obj::SizeType         SizeType;

    const bool VALUE_TYPE_USES_ALLOCATOR =
                                     bslma::UsesBslmaAllocator<Element>::value;

    // If we use this predicate in more than one test case, add a trait to this
    // test driver that is has a partial specialization for 'bsl::allocator<T>'
    // to return 'true', otherwise return 'false'.
    const bool ALLOCATOR_IS_BSL_ALLOCATOR =
        bsl::is_same<typename bsl::allocator_traits<ALLOCATOR>::
                                   template rebind_traits<int>::allocator_type,
                     bsl::allocator<int> >::value;

    if (verbose) { P(VALUE_TYPE_USES_ALLOCATOR); }
    if (verbose) { P(ALLOCATOR_IS_BSL_ALLOCATOR); }

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    typedef typename CallableVariable<    HASHER>::type     HASHER_TYPE;
    typedef typename CallableVariable<COMPARATOR>::type COMPARATOR_TYPE;

    const HASHER_TYPE     HASH  = MakeCallableEntity<HASHER>::make();
    const COMPARATOR_TYPE EQUAL = MakeCallableEntity<COMPARATOR>::make();

    const char *ALLOC_SPEC = ObjMaker::specForBootstrapTests();

    if (verbose) {
        printf("\nCreate objects with various allocator configurations.\n");
    }
    {
        if (veryVerbose) {
            printf("\tTesting with Max Load Factor\n");
        }

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
            const float MAX_LF  = DEFAULT_MAX_LOAD_FACTOR[lfi];

            if (veryVerbose) {
                T_ P(MAX_LF);
            }

            size_t oldLen = 0;
            for (int ti = 0; ti != NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec;
                const char *const EXP_S  = DATA[ti].d_results;

                const SizeType   LENGTH = static_cast<SizeType>(strlen(EXP_S));

                bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
                const TestValues  EXP(EXP_S, &tda);

                const size_t curLen = strlen(SPEC);

                if (curLen != oldLen || !oldLen) {
                    if (verbose) printf("\tof length " ZU ":\n", curLen);
                    oldLen = curLen;
                }

                const SizeType NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

                HASHER_TYPE     hash = HASH;
                COMPARATOR_TYPE comp = EQUAL;
                setHasherState(bsls::Util::addressOf(hash), ti);
                setComparatorState(bsls::Util::addressOf(comp), ti);

                if (veryVerbose) { P_(LINE) P_(LENGTH) P(SPEC); }

                for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
                    const char CONFIG = *cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    // There is no easy way to create this guard for the
                    // specific test case of the stateless
                    // 'bsltf::StdTestAllocator', nor without second guessing
                    // the allocator to use based upon the 'cfg' code.  By the
                    // time we return from 'makeAllocator' the test allocator
                    // will already have been installed, with no easy way to
                    // restore at the end of the test case.

                    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(
                                                    'a' == CONFIG ? &sa : &da);

                    // --------------------------------------------------------

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

                    const bslma::TestAllocator  *oa = extractTestAllocator(
                                                                     expAlloc);
                    const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

                    // It is important that these allocators are found, or else
                    // the following tests will break severely, dereferencing
                    // null pointers.

                    BSLS_ASSERT_OPT(oa);
                    BSLS_ASSERT_OPT(noa);

                    const unsigned INTIIAL_BLOCKS = 0 == NUM_BUCKETS ? 0 : 1;

                    // Verify we are starting with a known, expected, number of
                    // allocations based on the number of requested buckets.
                    ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG, oa->numBlocksTotal(),
                            INTIIAL_BLOCKS == oa->numBlocksTotal());
                    ASSERTV(MAX_LF, SPEC, LENGTH, CONFIG,
                            noa->numBlocksTotal(),
                            0 == noa->numBlocksTotal());

                    Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC);

                    if (veryVerbose) {
                        printf("\t\tSpec = \"%s\"\n", SPEC);
                        T_ T_ T_ P(X);
                    }

                    ASSERTV(MAX_LF, LENGTH, CONFIG, expAlloc == X.allocator());

                    // --------------------------------------------------------

                    // Verify basic accessors

                    bslma::TestAllocatorMonitor oam(oa);

                    ASSERTV(LINE, SPEC, CONFIG,
                            isEqualComparator(comp, X.comparator()));
                    ASSERTV(LINE, SPEC, CONFIG,
                            isEqualHasher(hash, X.hasher()));

                    ASSERTV(LINE, SPEC, CONFIG, MAX_LF, X.maxLoadFactor(),
                            MAX_LF == X.maxLoadFactor());
                    ASSERTV(LINE, SPEC, CONFIG, LENGTH, X.size(),
                            LENGTH == X.size());

                    int VERIFY_ERROR_CODE = verifyListContents<KEY_CONFIG>(
                                                           X.elementListRoot(),
                                                           EQUAL,
                                                           EXP,
                                                           LENGTH);
                    ASSERTV(VERIFY_ERROR_CODE, 0 == VERIFY_ERROR_CODE);

                    // Verify bucket accessors

                    const SizeType bucketCount = X.numBuckets();
                    ASSERTV(LINE, SPEC, CONFIG, NUM_BUCKETS, bucketCount,
                            NUM_BUCKETS <= bucketCount);

                    SizeType elementCount = 0;
                    for (SizeType iBkt = 0; iBkt != bucketCount; ++iBkt) {
                        const bslalg::HashTableBucket& bucket =
                                                         X.bucketAtIndex(iBkt);
                        bslalg::BidirectionalLink *bucketRoot = bucket.first();
                        SizeType bucketSize = X.countElementsInBucket(iBkt);
                        if (!bucketSize) {
                            ASSERTV(bucketSize, bucketRoot, 0 == bucketRoot);
                        }
                        else {
                            elementCount += bucketSize;
                            for (SizeType idx = 0; idx != bucketSize; ++idx) {
                                ASSERTV(idx, bucketRoot, 0 != bucketRoot);
                                SizeType index = X.bucketIndexForKey(
                                  ImpUtil::extractKey<KEY_CONFIG>(bucketRoot));
                                ASSERTV(iBkt, index, iBkt == index);
                                bucketRoot = bucketRoot->nextLink();
                            }
                        }
                    }
                    ASSERTV(X.size(), elementCount, X.size() == elementCount);

                    ASSERT(oam.isTotalSame());

                    // Ideally we would test this by inserting elements up to
                    // the threshold, and check that no rehash occurred, and
                    // then insert one more element and notice that the bucket
                    // count has changed.  This could get extremely expensive
                    // when the maxLoadFactor is high, or even infinite, and it
                    // messes up the existing memory-consumption calculations
                    // for little gain.  That part of the test will be deferred
                    // to test case XX for the 'insert' operation, after
                    // 'rehash' has already been tested.  To provide meaning,
                    // we compute the implications of adding 'rehashThreshold'
                    // additional items to the current container, and
                    // demonstrate that adding one more should exceed the
                    // 'maxLoadFactor', unless it is infinite.

                    SizeType threshold = X.rehashThreshold();
                    double thresholdLoadFactor = static_cast<double>(threshold)
                                            / static_cast<double>(bucketCount);
                    ASSERTV(threshold, bucketCount,
                            thresholdLoadFactor,   X.maxLoadFactor(),
                            thresholdLoadFactor <= X.maxLoadFactor());

                    if (!threshold) {
                        // The HashTable is empty and no buckets have been
                        // allocated.

                        ASSERT(!X.size());
                        ASSERT(!X.elementListRoot());
                        ASSERTV(bucketCount, 1 == bucketCount);
                    }
                    else if (X.maxLoadFactor() <
                               native_std::numeric_limits<float>::infinity()) {
                        ++threshold;
                        double exceedLoadFactor =
                                              static_cast<double>(threshold)
                                            / static_cast<double>(bucketCount);
                        ASSERTV(threshold, bucketCount,
                                X.maxLoadFactor(),  exceedLoadFactor,
                                X.maxLoadFactor() < exceedLoadFactor);
                    }
                    else {
                        ASSERTV(threshold,
                                threshold ==
                                  native_std::numeric_limits<SizeType>::max());
                    }

                    // --------------------------------------------------------

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify no allocation from the non-object allocator.

                    // In build configuration 'a', the 'noa' allocator is the
                    // default allocator, otherwise 'oa' is the default
                    // allocator.  If the allocator type is not
                    // 'bsl::allocator' and the elements being inserted
                    // allocate memory, the element stored in the container
                    // will be created using the default allocator but there
                    // should be only one such allocation per element.
                    // Otherwise, there should be no residual evidence of using
                    // the 'noa' allocator.

                    long long EXP_NUM_BLOCKS =  VALUE_TYPE_USES_ALLOCATOR  &&
                                               !ALLOCATOR_IS_BSL_ALLOCATOR &&
                                                'a' == CONFIG
                                             ? strlen(SPEC)
                                             : 0;

                    ASSERTV(LINE, CONFIG, EXP_NUM_BLOCKS,
                            noa->numBlocksTotal(),
                            EXP_NUM_BLOCKS == noa->numBlocksTotal());

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
    }

    // Create some fresh allocators to use validating final corners of the
    // constructor behavior.  These are special case tests, and will not need
    // to loop many times, or test the contents of the container, so one set of
    // allocators will suffice to the end of the test case.

    bslma::TestAllocator na("negative testing", veryVeryVeryVerbose);

    // There is no easy way to create this guard for the specific test case of
    // the stateless 'bsltf::StdTestAllocator', nor without second guessing the
    // allocator to use based upon the 'cfg' code.  By the time we return from
    // 'makeAllocator' the test allocator will already have been installed,
    // with no easy way to restore at the end of the test case.

    bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&na);
    ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&na);

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        if (veryVerbose) printf("\t'bucketAtIndex'\n");
        {
            Obj mX(HASH, EQUAL, 1, 1.0f, objAlloc);  const Obj& X = mX;
            SizeType numBuckets = X.numBuckets();
            if (0 < numBuckets) { // always true, but needed for warnings
                ASSERT_SAFE_PASS(X.bucketAtIndex(numBuckets - 1));
                ASSERT_SAFE_FAIL(X.bucketAtIndex(numBuckets));

                ASSERT_SAFE_PASS(X.countElementsInBucket(numBuckets - 1));
                ASSERT_SAFE_FAIL(X.countElementsInBucket(numBuckets));
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase6()
{
    // ---------------------------------------------------------------------
    // TESTING EQUALITY OPERATORS
    //
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
    //   bool operator==(const HashTable& lhs, const HashTable& rhs);
    //   bool operator!=(const HashTable& lhs, const HashTable& rhs);
    // ------------------------------------------------------------------------

    if (verbose)  printf("\nTESTING EQUALITY OPERATORS"
                         "\n==========================\n");

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
        { L_,   12, "AAABBCDEEFFFGGGHHIJKKLLLMMMNNOPQQRRRSSSTTUVWWXXXYYZ" },
        { L_,   12, "ABCDEFGHIJKLMNOPQRSTUVWXYZABEFGHKLMNQRSTWXYXRSLMGFA" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    typedef typename CallableVariable<    HASHER>::type     HASHER_TYPE;
    typedef typename CallableVariable<COMPARATOR>::type COMPARATOR_TYPE;

    const HASHER_TYPE     HASH  = MakeCallableEntity<HASHER>::make();
    const COMPARATOR_TYPE EQUAL = MakeCallableEntity<COMPARATOR>::make();

    // Create a variable to confirm that our tested data set has an expected
    // (and important) property.
    bool HAVE_TESTED_DISTINCT_PERMUTATIONS = false;

    // It would be interesting to make this variable static, so that different
    // test cases start off at different indexes, increasing the variety of the
    // test space.  Unfortunately, each instantiation of this test function
    // would obtain its own static variable, side-stepping the intent.  The
    // alternative would be to make this a global variable, which might then be
    // recycled for this purpose among multiple test cases.  Something to
    // consider for the future.

    size_t innerLoadFactorIndex = 0;

    if (verbose) printf("\nCompare every value with every value.\n");
    {
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

           if (veryVerbose) {
               T_ P_(LINE1) P_(MAX_LF1) P_(INDEX1) P_(LENGTH1) P(SPEC1);
           }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                ALLOCATOR objAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

                Obj mX(HASH, EQUAL, NUM_BUCKETS, MAX_LF1, objAlloc);
                const Obj& X = gg(&mX, SPEC1);

                ASSERTV(LINE1, MAX_LF1, SPEC1, X,   X == X);
                ASSERTV(LINE1, MAX_LF1, SPEC1, X, !(X != X));
            }

            for (int tj = 0; tj <= ti; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const int         INDEX2  = DATA[tj].d_index;
                const char *const SPEC2   = DATA[tj].d_spec;

                const size_t      LENGTH2      = strlen(SPEC2);
                const float       MAX_LF2  = DEFAULT_MAX_LOAD_FACTOR[
                        ++innerLoadFactorIndex % DEFAULT_MAX_LOAD_FACTOR_SIZE];

                const size_t      NUM_BUCKETS2 = predictNumBuckets(LENGTH2,
                                                                   MAX_LF2);

                if (veryVerbose) {
                   T_ T_ P_(LINE2) P_(MAX_LF2) P_(INDEX2) P_(LENGTH2) P(SPEC2);
                }

                const bool EXP = INDEX1 == INDEX2;  // expected result

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    ALLOCATOR allocX = MakeAllocator<ALLOCATOR>::make(&xa);
                    ALLOCATOR allocY = MakeAllocator<ALLOCATOR>::make(&ya);

                    Obj mX(HASH, EQUAL, NUM_BUCKETS,  MAX_LF1, allocX);
                    const Obj& X = gg(&mX, SPEC1);
                    Obj mY(HASH, EQUAL, NUM_BUCKETS2, MAX_LF2, allocY);
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

    ASSERT(HAVE_TESTED_DISTINCT_PERMUTATIONS);
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

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<KEY>::value
                         + bslma::UsesBslmaAllocator<VALUE>::value;

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    if (verbose) printf("\nTesting parameters: TYPE_ALLOC = %d.\n",
                        TYPE_ALLOC);
    {
        static const char *SPECS[] = { "",
                                      "A",
                                     "AA",
                                     "AB",
                                     "BC",
                                    "AAA",
                                    "ABB",
                                    "CDE",
                                   "AAAA",
                                   "ABBA",
                                   "DEAB",
                                  "AAAAA",
                                  "AAABC",
                                  "ABBAC",
                                  "EABCD",
                                "ABCDEFG",
                               "HFGEDCBA",
                              "CFHEBIDGA",
                        "BENCKHGMALJDFOI",
                       "IDMLNEFHOPKGBCJA",
                      "OIQGDNPMLKBACHFEJ"
        };
        const size_t NUM_SPECS = sizeof SPECS / sizeof *SPECS;

        for (int lfi = 0; lfi < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++lfi) {
        for (size_t ti = 0; ti < NUM_SPECS; ++ti) {
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
            // allocator in this one general function, as we now construct two
            // allocator objects with different test allocators, which violates
            // the contract that we install only the current default allocator
            // when creating a 'stateless' object via the 'make' call.
            //
            // There is no easy way to create this guard for the specific test
            // case of the stateless 'bsltf::StdTestAllocator', nor without
            // second guessing the allocator to use based upon the 'cfg' code.
            // By the time we return from 'makeAllocator' the test allocator
            // will already have been installed, with no easy way to restore at
            // the end of the test case.

            bsltf::StdTestAllocatorConfigurationGuard bsltfAG(&oa);
#endif

            HASHER     hash = HASH;
            COMPARATOR comp = COMPARE;
            setHasherState(bsls::Util::addressOf(hash), ti);
            setComparatorState(bsls::Util::addressOf(comp), ti);

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
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase8()
{
    // ------------------------------------------------------------------------
    // SWAP MEMBER AND FREE FUNCTIONS
    //   Ensure that, when member and free 'swap' are implemented, we can
    //   exchange the values of any two objects that use the same allocator.
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

        (void) memberSwap;  // quash potential compiler warnings
        (void) freeSwap;
    }

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

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

        // There is a symmetry to testing, so test only the "lower triangle" of
        // indices.  Note that we want to support the limiting case where the
        // indices match.
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

            // free function 'swap' is always well-defined
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mX, mY);

                ASSERTV(LINE1, LINE2, YY, X, YY == X);
                ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // Member 'swap' may not be defined if allocators are not the same,
            // although guaranteed in this case.
            {
                ASSERT(X.allocator() == Y.allocator()); // sanity check

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, XX, X, XX == X);
                ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                ASSERTV(LINE1, LINE2, objAlloc == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
            }

            // We may consider restoring a test of unequal allocators if the
            // allocator propagate_on_container_swap trait is true, but
            // 'bsl::allocator_traits' does not properly detect this trait yet.
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

        // We know that the types of 'mX' and 'mY' do not overload the unary
        // address-of 'operator&'.

        bslalg::SwapUtil::swap(&mX, &mY);

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
        // of the undefined behavior.

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

        // Sanity check

        ASSERTV(XX, X, XX == X);
        ASSERTV(ZZ, Z, ZZ == Z);
        ASSERTV( X, Z,  X != Z);

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

            ASSERTV(XX, Z, XX == Z);
            ASSERTV(ZZ, X, ZZ == X);
            ASSERTV(oaxAlloc == X.allocator());
            ASSERTV(oazAlloc == Z.allocator());

            // Concerns about allocated memory?
        }

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            // member 'swap'
            ASSERT_PASS(mX.swap(mX));
            ASSERT_FAIL(mZ.swap(mX));
            ASSERT_FAIL(mX.swap(mZ));
            ASSERT_PASS(mZ.swap(mZ));
        }

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

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    // Repeatedly running the strong exception safety test can be expensive, so
    // we toggle a boolean variable to alternate tests in the cycle.  This
    // retains a fairly exhaustive coverage, while significantly improving test
    // timings.

    bool testForStrongExceptionSafety = false;

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
                const ALLOCATOR objAlloc  = AllocMaker::make(&oa);

                {
                    Obj mX(HASH, COMPARE, NUM_BUCKETS2, MAX_LF2, objAlloc);
                    const Obj& X  = gg(&mX,  SPEC2);

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X,
                            (Z == X) == (INDEX1 == INDEX2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    testForStrongExceptionSafety =
                                                 !testForStrongExceptionSafety;
                    if (testForStrongExceptionSafety) {
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
                    }
                    else {
                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);
                        ASSERTV(LINE1, LINE2,
                                Z.maxLoadFactor(),   X.maxLoadFactor(),
                                Z.maxLoadFactor() == X.maxLoadFactor());
                    }

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    // Should test that allocator propagates if, and only if,
                    // the allocator has the propagate_on_container_copy_assign
                    // trait.  Currently we assume (and test) that allocators
                    // never propagate, as our implementation of allocator
                    // traits for C++03 containers does not deduce or support
                    // any of the propagation traits.

                    ASSERTV(LINE1, LINE2, objAlloc == X.allocator());
                    ASSERTV(LINE1, LINE2, scratchAlloc == Z.allocator());

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
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
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase11()
{
    // ------------------------------------------------------------------------
    // TESTING 'rehashForNumBuckets'
    //
    // Concerns:
    //: 1 'rehashForNumBuckets' allocates at least the specified number of
    //:   buckets.
    //:
    //: 2 'rehashForNumBuckets' does not affect the value of the object.
    //:
    //: 3 'rehashForNumBuckets' does not affect the order of the inserted
    //:   elements with the same value.
    //:
    //: 4 'rehashForNumBuckets' is a no-op if the requested number of buckets
    //:   is less than the current 'numBuckets' in the object.
    //:
    //: 5 Any memory allocation is from the object allocator.
    //:
    //: 6 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 7 'rehashForNumBuckets' provides the strong exception guarantee if the
    //:   hasher does not throw.
    //:
    //: 8 'rehashForNumBuckets' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   rehashForNumBuckets(SizeType newNumBuckets);
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

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        if (veryVerbose) printf("Testing rehashForNumBuckets on spec: %s,"
                                " index: %i\n", SPEC,  ti);

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
                    // allocated blocks in use.  However, in the case of the
                    // first allocation on an empty container, we will actually
                    // be creating the first array block.
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
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase12()
{
    // ------------------------------------------------------------------------
    // TESTING 'reserveForNumElements'
    //
    // Concerns:
    //: 1 'reserveForNumElements' allocates sufficient buckets so that, after
    //:   the rehash, 'numBuckets() / maxLoadFactor()' >= the specified number
    //:   of elements.
    //:
    //: 2 'reserveForNumElements' does not affect the value of the object.
    //:
    //: 3 'reserveForNumElements' does not affect the order of the inserted
    //:   elements with the same value.
    //:
    //: 4 'reserveForNumElements' is a no-op if the requested number of
    //:   elements can already be accommodated without exceeding the
    //:   'maxLoadFactor' of the object.
    //:
    //: 5 Any memory allocation is from the object allocator.
    //:
    //: 6 The only memory allocation is a single allocation for the new bucket
    //:   array; no new nodes are allocated, and the old array is reclaimed by
    //:   the object allocator.
    //:
    //: 7 'reserveForNumElements' provides the strong exception guarantee if
    //:   the hasher does not throw.
    //:
    //: 8 'reserveForNumElements' will reset the object to an empty container,
    //:   without leaking memory or objects, if a hasher throws.
    //
    // Plan:
    //: 1 TBD
    //
    // Testing:
    //   reserveForNumElements(SizeType numElements);
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    if (verbose) printf(
       "\nUse a table of distinct object values and expected memory usage.\n");

    const size_t RESERVE_SIZE[] = {
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
    const int NUM_RESERVE_SIZE = sizeof RESERVE_SIZE / sizeof *RESERVE_SIZE;

    const int NUM_DATA                     = DEFAULT_NUM_DATA;
    const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

    bslma::TestAllocator tda("test-array values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const char *const SPEC   = DATA[ti].d_spec;
        const size_t      LENGTH = strlen(SPEC);

        if (veryVerbose) printf("Testing reserveForNumElements on spec: %s,"
                                " index: %i\n", SPEC,  ti);

        for (int tj = 0; tj < DEFAULT_MAX_LOAD_FACTOR_SIZE; ++tj) {
            const float  MAX_LF      = DEFAULT_MAX_LOAD_FACTOR[tj];
            const size_t NUM_BUCKETS = predictNumBuckets(LENGTH, MAX_LF);

            if (veryVeryVerbose) printf("\twith max load factor of: %f\n",
                                        MAX_LF);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
            ALLOCATOR scratchAlloc = MakeAllocator<ALLOCATOR>::make(&scratch);

            Obj mZ(HASH, COMPARE, NUM_BUCKETS, MAX_LF, scratchAlloc);
            const Obj& Z = gg(&mZ,  SPEC);

            for (int tk = 0; tk < NUM_RESERVE_SIZE; ++tk) {
                bslma::TestAllocator reserve("reserveAlloc",
                                             veryVeryVeryVerbose);
                bslma::TestAllocator noReserve("noReserveAlloc",
                                               veryVeryVeryVerbose);

                ALLOCATOR reserveAlloc = MakeAllocator<ALLOCATOR>::make(
                                                                     &reserve);
                ALLOCATOR noReserveAlloc = MakeAllocator<ALLOCATOR>::make(
                                                                   &noReserve);

                Obj mResX(Z, reserveAlloc); const Obj& resX = mResX;
                Obj mNoResX(Z, noReserveAlloc); const Obj& noResX = mNoResX;

                if (veryVerbose) { T_ P_(LINE) P_(Z) P_(resX) P(noResX) }

                const size_t ADDITIONAL_ELEMENTS = RESERVE_SIZE[tk];
                const size_t INITIAL_SIZE = resX.size();
                const size_t RESERVE_ELEMENTS = ADDITIONAL_ELEMENTS +
                                                                  INITIAL_SIZE;

                if (veryVeryVerbose) printf(
                        "\t\treserve for " ZU " elements\n", RESERVE_ELEMENTS);

                mResX.reserveForNumElements(RESERVE_ELEMENTS);

                const bsls::Types::Int64 RESERVED_MEM =
                                                       reserve.numBytesTotal();

                mResX.reserveForNumElements(RESERVE_ELEMENTS);

                // Reserve doesn't allocate memory when some is already
                // available from previous reserve
                ASSERTV(LINE, tk, RESERVE_ELEMENTS, reserve.numBytesTotal() ==
                                                                 RESERVED_MEM);

                for (size_t tl = 0; tl < ADDITIONAL_ELEMENTS; ++tl) {
                    mResX.insert( VALUES[tl % 10] );
                    mNoResX.insert( VALUES[tl % 10] );
                }

                // Reserving X elements allocates same or less amount of memory
                // as inserting 'X' elements one at a time
                ASSERTV(LINE, tk, RESERVE_ELEMENTS, reserve.numBytesTotal() <=
                                                    noReserve.numBytesTotal());
            }
        }
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase13()
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
    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

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

            {
            bslma::DefaultAllocatorGuard dag(&da);

            // ----------------------------------------------------------------

            if (veryVerbose) {
                printf("\n\tTesting bootstrap constructor.\n");
            }

            Obj                  *objPtr;

            //const size_t NUM_BUCKETS = predictNumBuckets(3*LENGTH, MAX_LF);

            ALLOCATOR expAllocator = ObjMaker::makeObject( &objPtr
                                                         , CONFIG
                                                         , &fa
                                                         , &sa
                                                         , HASH
                                                         , COMPARE
                                                         , 0
                                                         , MAX_LF);
            bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                       &fa);
            Obj&                   mX = *objPtr;  const Obj& X = mX;

            // Verify any attribute allocators are installed properly.

            ASSERTV(MAX_LF, LENGTH, CONFIG, expAllocator == X.allocator());


            bslma::TestAllocator  *oa = extractTestAllocator(expAllocator);
            bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

            // It is important that these allocators are found, or else the
            // following tests will break severely, dereferencing null pointer.
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

                bslma::TestAllocator scratch1("scratch1", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch1);

                    // This will fail on the initial insert as we must also
                    // create the bucket array, so there is an extra pass.  Not
                    // sure why that means the block counts get out of sync
                    // though, is this catching a real bug?
#if defined(HAVE_WORKED_OUT_CORRECT_MEMORY_USE_COMPUTATIONS)
                    ASSERTV(CONFIG, LENGTH,
                            oa->numBlocksTotal(),   oa->numBlocksInUse(),
                            oa->numBlocksTotal() == oa->numBlocksInUse());
#endif

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // These tests assume that the object allocator is used
                    // only is stored elements also allocate memory.  This does
                    // not allow for rehashes as the container grows.
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
                    //
                    // !!! BROKEN TEST CONDITION !!!
                    //
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
                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.
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

                bslma::TestAllocator scratch2("scratch2", veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(*oa) {
                    bslma::ExceptionGuard<Obj> guard(&X, L_, &scratch2);

                    bslma::TestAllocatorMonitor tam(oa);
                    Link *RESULT = mX.insert(VALUES[LENGTH - 1]);
                    ASSERT(0 != RESULT);

                    // The number of buckets should not have changed, so no
                    // reason to allocate a fresh bucket array
//                    ASSERTV(MAX_LF, LENGTH, CONFIG,
//                            bucketCount,   X.numBuckets(),
//                            bucketCount == X.numBuckets());

                    // These tests assume that the object allocator is used
                    // only if stored elements also allocate memory.  This does
                    // not allow for rehashes as the container grows.  Hence we
                    // run the same test sequence a second time after clearing
                    // the container, so we can validate knowing that no
                    // rehashes should be necessary, and will in fact show up
                    // as a memory use error.  'LENGTH' was the high-water mark
                    // of the initial run on the container before removing all
                    // elements.
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

                // Check that elements with equivalent keys are contiguous.
                // Check that the expected elements are present in the
                // container, with expected number of duplicates.
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

                // The second loop adds another duplicate in front of each of
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

            // Reclaim dynamically allocated object under test.

            //fa.deleteObject(objPtr);
            }

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

    // ----------------------------------------------------------------

#if defined BDE_BUILD_TARGET_EXC
    {
        Obj mR(HASH,
               COMPARE,
               0,
               native_std::numeric_limits<float>::denorm_min());
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
            ASSERT(!"The wrong exception type was thrown.");
        }
    }
#endif  // BDE_BUILD_TARGET_EXC
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase14()
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

    const HASHER     HASH    = MakeCallableEntity<HASHER>::make();
    const COMPARATOR COMPARE = MakeCallableEntity<COMPARATOR>::make();

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

        static const float FLT_TINY = std::numeric_limits<float>::denorm_min();
        static const float FLT_NAN  = std::numeric_limits<float>::quiet_NaN();
        static const float FLT_INF  = std::numeric_limits<float>::infinity();

        (void) FLT_TINY;
        (void) FLT_NAN;

        Obj mX(HASH, COMPARE, 0, 1.0f, scratchAlloc);

//        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(  FLT_TINY));
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(      0.5f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(      0.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(     -0.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor( -FLT_TINY));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(     -1.0f));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(  -FLT_INF));
        ASSERT_SAFE_FAIL(mX.setMaxLoadFactor(   FLT_NAN));
        ASSERT_SAFE_PASS(mX.setMaxLoadFactor(   FLT_INF));
    }

}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase15()
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
    //   last method tested, so that it can check every other method being
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

    bslma::TestAllocator tda("test values", veryVeryVeryVerbose);
    const TestValues VALUES(&tda);  // Contains 52 distinct increasing values.

    const char *ALLOC_SPEC = ObjMaker::specForDefaultTests();

    for (const char *cfg = ALLOC_SPEC; *cfg; ++cfg) {
        const char CONFIG = *cfg;  // how we specify the allocator

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        // ----------------------------------------------------------------

        if (veryVerbose) {
            printf("\n\tTesting default constructor.\n");
        }

        {
        Obj       *objPtr;
        ALLOCATOR  expAlloc = ObjMaker::makeObject(&objPtr,
                                                   CONFIG,
                                                   &fa,
                                                   &sa);
        bslma::RawDeleterGuard<Obj, bslma::TestAllocator> guardObj(objPtr,
                                                                   &fa);
        Obj& mX = *objPtr;  const Obj& X = mX;

        // Verify any attribute allocators are installed properly.

        ASSERTV(CONFIG, expAlloc == X.allocator());

        const bslma::TestAllocator  *oa = extractTestAllocator(expAlloc);
        const bslma::TestAllocator *noa = &sa == oa ? &da : &sa;

        // It is important that these allocators are found, or else the
        // following tests will break severely, dereferencing null pointer.

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

        // -----------------------------------------------------------------

        // check all other methods of the class continue to work as expected
        // for a default constructed object.

        ASSERTV(CONFIG,   X == X);
        ASSERTV(CONFIG, !(X != X));

        // an infinite load factor suggests we should never rehash.  However,
        // in setting the load factor we will allocate the initial bucket
        // array.

        mX.setMaxLoadFactor(std::numeric_limits<float>::infinity());
        ASSERTV(CONFIG, X.maxLoadFactor(),
                X.maxLoadFactor() == std::numeric_limits<float>::infinity());

        size_t INITIAL_BUCKETS = X.numBuckets();
        ASSERTV(CONFIG, 0 == X.size());
        ASSERTV(CONFIG, 1  < INITIAL_BUCKETS);
        ASSERTV(CONFIG, 0 == X.elementListRoot());
        ASSERTV(CONFIG, 0.0f == X.loadFactor());

        mX.insert(VALUES[0]);

        ASSERTV(CONFIG, 1 == X.size());
        ASSERTV(CONFIG, INITIAL_BUCKETS == X.numBuckets());
        ASSERTV(CONFIG, 0 != X.elementListRoot());
        ASSERTV(CONFIG, 0.0f < X.loadFactor());

        // -----------------------------------------------------------------

        // Reclaim dynamically allocated object under test.

        //fa.deleteObject(objPtr);
        }

        // Verify all memory is released on object destruction.

        ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(CONFIG, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
    }
}

template <class KEY_CONFIG, class HASHER, class COMPARATOR, class ALLOCATOR>
void TestDriver<KEY_CONFIG, HASHER, COMPARATOR, ALLOCATOR>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING traits and other compile-time properties
    //
    // Concerns:
    //: 1 HashTable supports the expected traits
    //:
    //: 2 HashTable has the expected size
    //
    // Plan:
    //: 1 For each value of increasing length, 'L':
    //:
    //
    // Testing:
    //*  traits
    //*  sizeof(HashTable)
    // ------------------------------------------------------------------------

    if (verbose) {
        printf("\nTesting bsl traits.\n");
    }

    typedef typename KEY_CONFIG::KeyType   KeyType;
    typedef typename KEY_CONFIG::ValueType ValueType;


    ASSERT((bsl::is_same<typename Obj::KeyType,   KeyType>::value));
    ASSERT((bsl::is_same<typename Obj::ValueType, ValueType>::value));

    // These traits should never be true

    ASSERT(!bsl::is_trivially_copyable<             Obj>::value);
    ASSERT(!bsl::is_trivially_default_constructible<Obj>::value);
    ASSERT(!bslmf::IsBitwiseEqualityComparable<     Obj>::value);

    // The uses-allocator trait should depend only on the allocator type.

    bool usesBslAllocator        = IsBslAllocator<ALLOCATOR>::value;
    bool objUsesBslmaAllocator   = bslma::UsesBslmaAllocator<Obj>::value;

    ASSERTV(usesBslAllocator, objUsesBslmaAllocator,
            usesBslAllocator == objUsesBslmaAllocator);

    // The stored type has no bearing on whether a node-based container is
    // bitwise-moveable.  However, the allocator and two custom function-like
    // types must all be bitwise-moveable for the whole container to be
    // bitwise-moveable.

    bool allocIsBitwiseMoveable = bslmf::IsBitwiseMoveable<ALLOCATOR>::value;
    bool hashIsBitwiseMoveable  = bslmf::IsBitwiseMoveable<HASHER>::value;
    bool compIsBitwiseMoveable  = bslmf::IsBitwiseMoveable<COMPARATOR>::value;
    bool objIsBitwiseMoveable   = bslmf::IsBitwiseMoveable<Obj>::value;

    ASSERTV(allocIsBitwiseMoveable,
            hashIsBitwiseMoveable,
            compIsBitwiseMoveable,
            objIsBitwiseMoveable,
     (allocIsBitwiseMoveable && hashIsBitwiseMoveable && compIsBitwiseMoveable)
                                                      == objIsBitwiseMoveable);
}

//=============================================================================
//                      TEST CASE DISPATCH FUNCTIONS
//-----------------------------------------------------------------------------
// In order to reduce the complexity of the 'switch' statement in 'main' that
// was getting to large for some compilers to handle, we have migrated the
// invocation of each test case into its own function.
//-----------------------------------------------------------------------------

static
void mainTestCase1()
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
{
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
        ::bsl::hash<int>     h = X.hasher();      (void) h;
        ::bsl::equal_to<int> c = X.comparator();  (void) c;
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

        bslalg::HashTableBucket bkt = X.bucketAtIndex(0); (void) bkt;
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
            mX.setMaxLoadFactor(9e-9f);
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

static
void mainTestCase2()
    // --------------------------------------------------------------------
    // BOOTSTRAP CONSTRUCTOR AND PRIMARY MANIPULATORS
    //   This case is implemented as a method of a template test harness, where
    //   the runtime concerns and test plan are documented.  The test harness
    //   will be instantiated and run with a variety of types to address the
    //   template parameter concerns below.  We note that the bootstrap case
    //   has the widest variety of parameterized concerns to test, as later
    //   test cases may be able to place additional requirements on the types
    //   that they operate with, but the primary bootstrap has to validate
    //   bringing any valid container into any valid state for any of the later
    //   cases.
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
    //:13 functors with function-call operators that are function templates
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
    //*  HashTable(HASHER, COMPARATOR, SizeType, float, ALLOC)
    //   ~HashTable();
    //   void removeAll();
    //   insertElement  (test driver function, proxy for basic manipulator)
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE2_TYPES \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

    if (verbose) printf("\nTesting Primary Manipulators"
                        "\n============================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase2,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE2_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE2_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM) // name mangling bug
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);
#endif

    if (verbose) printf("\nTesting function pointers"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting function references"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase2,
                  BSLSTL_HASHTABLE_TESTCASE2_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE2_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase2();
}

static
void mainTestCase3()
    // --------------------------------------------------------------------
    // GENERATOR FUNCTIONS 'gg' and 'ggg' and other test machinery
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE3_TYPES \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

    if (verbose) printf("\nTesting generators and test machinery"
                        "\n=====================================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE3_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE3_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM) // name mangling bug
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);
#endif

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting function referecnes"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

#if 0    // Types with BDE allocators make temporaries with the default
         // allocator, that are not yet accounted for in the arithmetic of
         // this test case.
# define  BSLSTL_HASHTABLE_TESTCASE3_NO_ALLOCATING_TYPES   \
          BSLSTL_HASHTABLE_TESTCASE3_TYPES
#else
# define  BSLSTL_HASHTABLE_TESTCASE3_NO_ALLOCATING_TYPES   \
          BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE, \
          BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
#endif
    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_NO_ALLOCATING_TYPES);
#undef BSLSTL_HASHTABLE_TESTCASE3_NO_ALLOCATING_TYPES

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase3,
                  BSLSTL_HASHTABLE_TESTCASE3_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE3_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase3();
}

static
void mainTestCase4()
    // --------------------------------------------------------------------
    // BASIC ACCESSORS
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE4_TYPES \
        BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_ALL

    if (verbose) printf("\nTesting Basic Accessors"
                        "\n=======================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE4_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE4_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM) // 'Obj::comparator()' does not resolve
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);
#endif

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting function referecnes"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

#if 0    // Types with BDE allocators make temporaries with the default
         // allocator, that are not yet accounted for in the arithmetic of
         // this test case.
# define  BSLSTL_HASHTABLE_TESTCASE4_NO_ALLOCATING_TYPES   \
          BSLSTL_HASHTABLE_TESTCASE4_TYPES
#else
# define  BSLSTL_HASHTABLE_TESTCASE4_NO_ALLOCATING_TYPES   \
          BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE, \
          BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_AWKWARD
#endif
    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_NO_ALLOCATING_TYPES);
#undef BSLSTL_HASHTABLE_TESTCASE4_NO_ALLOCATING_TYPES

#if 0
    // This test mostly seems to work, but the public interface for the
    // accessor 'bucketForKey' is not compatible with these functors, and it
    // will take some effort to factor out a test that does not compile that
    // signature for just this configuration.  Alternatively, we might want to
    // redesign the public API, but there is no real desire to support these
    // functions/functors with awkward signatures any more than is strictly
    // necessary for standard conformance.

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);
#endif

   if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase4,
                  BSLSTL_HASHTABLE_TESTCASE4_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE4_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase4();
}

static
void mainTestCase5()
    // --------------------------------------------------------------------
    // TESTING OUTPUT (<<) OPERATOR
    // --------------------------------------------------------------------
{
    if (verbose) printf("\nTesting Output (<<) Operator"
                        "\n============================\n");

    if (verbose)
               printf("There is no output operator for this component.\n");
}

static
void mainTestCase6()
    // --------------------------------------------------------------------
    // EQUALITY-COMPARISON OPERATORS
    // --------------------------------------------------------------------
{
    if (verbose) printf("\nTesting Equality-comparison Operators"
                        "\n=====================================\n");

    // Note that the 'NonEqualComparableTestType' is not appropriate here.

#define BSLSTL_HASHTABLE_TESTCASE6_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE6_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE6_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM) // name mangling bug
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);
#endif

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting function referecnes"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

#if 0  // This always causes trouble
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase6,
                  BSLSTL_HASHTABLE_TESTCASE6_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE6_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase6();
}

static
void mainTestCase7()
    // --------------------------------------------------------------------
    // COPY CONSTRUCTOR
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE7_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting Copy Constructors"
                        "\n=========================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase7,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR,
                  bsltf::NonAssignableTestType,
                  bsltf::NonDefaultConstructibleTestType);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE7_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE7_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

#if 0
    // Revisit these tests once validated the rest.
    // Initial problem are testing the stateless allocator while trying to
    // separately configure a default and an object allocator.
    // Obvious problems with allocator-propagating tests, given the driver
    // currently expects to never propagate.
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase7,
                  BSLSTL_HASHTABLE_TESTCASE7_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE7_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase7();
}

static
void mainTestCase8()
    // --------------------------------------------------------------------
    // MANIPULATOR AND FREE FUNCTION 'swap'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE8_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nMANIPULATOR AND FREE FUNCTION 'swap'"
                        "\n====================================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE8_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE8_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

#if 0
    // Revisit these tests once validated the rest.
    // Initial problem are testing the stateless allocator while trying to
    // separately configure a default and an object allocator.
    // Obvious problems with allocator-propagating tests, given the driver
    // currently expects to never propagate.
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase8,
                  BSLSTL_HASHTABLE_TESTCASE8_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE8_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase8();
}

static
void mainTestCase9()
    // --------------------------------------------------------------------
    // ASSIGNMENT OPERATOR
    // --------------------------------------------------------------------
{
#if defined(BSLS_PLATFORM_CMP_CLANG)
    // The Clang compiler is known to be particularly slow executing this test
    // case, so we really cut back on the variations.  This was last evaluated
    // with Clang 3.1, and should be re-evaluated when Clang 3.2 is installed.

    if (verbose) printf("\nTesting basic configurations"
                        "\n---------------------------\n");
    TestDriver_StatefulConfiguation<TestTypes::MostEvilTestType>::testCase9();

    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase9();
    return;                                                           // RETURN
#endif

#define BSLSTL_HASHTABLE_TESTCASE9_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting Assignment Operator"
                        "\n===========================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

#if defined(BSLSTL_HASHTABLE_TRIM_TEST_CASE9_COMPLEXITY)
#  undef  BSLSTL_HASHTABLE_TESTCASE9_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE9_TYPES TestTypes::MostEvilTestType
#elif !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE9_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE9_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    // We need to limit the test coverage on IBM as the compiler cannot cope
    // with so many template instantiations.

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

#if 0
    // Degenerate functors are not CopyAssignable, and rely on the copy/swap
    // idiom for the copy-assignment operator to function.

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES,
                  bsltf::AllocTestType,
                  bsltf::AllocBitwiseMoveableTestType);
#endif

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

#if 0    // Types with BDE allocators make temporaries with the default
         // allocator, that are not yet accounted for in the arithmetic of this
         // test case.
# define  BSLSTL_HASHTABLE_TESTCASE9_NO_ALLOCATING_TYPES   \
          BSLSTL_HASHTABLE_TESTCASE9_TYPES
#else
# if !defined(BSLSTL_HASHTABLE_TRIM_TEST_CASE9_COMPLEXITY)
#   define  BSLSTL_HASHTABLE_TESTCASE9_NO_ALLOCATING_TYPES   \
            BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE, \
            bsltf::NonAssignableTestType,                    \
            bsltf::NonDefaultConstructibleTestType
# endif
#endif
    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_NO_ALLOCATING_TYPES);
#undef BSLSTL_HASHTABLE_TESTCASE9_NO_ALLOCATING_TYPES

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);


    // The non-BDE allocators do not propagate the container allocator to their
    // elements, and so will make use of the default allocator when making
    // copies.  Therefore, we use a slightly different list of types when
    // testing with these allocators.  This leaves unaddressed the issue of
    // testing 'HashTable' with these allocator types, and elements that in
    // turn allocate their own memory, using their own allocator.

#if !defined(BSLSTL_HASHTABLE_TRIM_TEST_CASE9_COMPLEXITY)
# undef  BSLSTL_HASHTABLE_TESTCASE9_TYPES
# define BSLSTL_HASHTABLE_TESTCASE9_TYPES                 \
         BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE, \
         bsltf::EnumeratedTestType::Enum,                 \
         bsltf::UnionTestType,                            \
         bsltf::SimpleTestType,                           \
         bsltf::BitwiseMoveableTestType,                  \
         bsltf::NonTypicalOverloadsTestType,              \
         bsltf::NonAssignableTestType,                    \
         bsltf::NonDefaultConstructibleTestType
#endif

    // Next we have a sub-set of tests that do not yet want to support testing
    // the allocatable types, as non-BDE allocators do not scope the object
    // allocator to the elements, and so use the default allocator instead,
    // which confuses the math of the test case.

#if 0
    // Revisit these tests once validated the rest.
    // Initial problem are testing the stateless allocator while trying to
    // separately configure a default and an object allocator.
    // Obvious problems with allocator-propagating tests, given the driver
    // currently expects to never propagate.
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase9,
                  BSLSTL_HASHTABLE_TESTCASE9_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE9_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase9();
}

static
void mainTestCase10()
    // --------------------------------------------------------------------
    // STREAMING FUNCTIONALITY
    // --------------------------------------------------------------------
{
    if (verbose) printf("\nTesting Streaming Functionality"
                        "\n===============================\n");

    if (verbose) printf("There is no streaming for this component.\n");
}

static
void mainTestCase11()
    // --------------------------------------------------------------------
    // TESTING 'rehashForNumBuckets'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE11_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTESTING 'rehashForNumBuckets'"
                        "\n=============================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE11_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE11_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#if 0
    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase11,
                  BSLSTL_HASHTABLE_TESTCASE11_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE11_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase11();
}

static
void mainTestCase12()
    // --------------------------------------------------------------------
    // TESTING 'reserveForNumElements'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE12_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTESTING 'reserveForNumElements'"
                        "\n===============================n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");

    // Because 'reserveForNumElements' uses already tested functionality, we
    // need to run only a basic battery of test cases to verify the logic.
    // Issues such as different allocators and types with strange overloads are
    // already covered by other tests, such as case 11 for
    // 'rehashIntoExactlyNumBuckets'.

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase12,
                  BSLSTL_HASHTABLE_TESTCASE12_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE12_TYPES
}

static
void mainTestCase13()
    // --------------------------------------------------------------------
    // TESTING 'insert'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE13_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting 'insert'"
                        "\n================\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE13_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE13_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase13,
                  BSLSTL_HASHTABLE_TESTCASE13_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE13_TYPES
}

static
void mainTestCase14()
    // --------------------------------------------------------------------
    //  TESTING 'setMaxLoadFactor'
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE14_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting 'setMaxLoadFactor'"
                        "\n==========================\n");

    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE14_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE14_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    // We need to limit the test coverage on IBM as the compiler cannot cope
    // with so many template instantiations.

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase14,
                  BSLSTL_HASHTABLE_TESTCASE14_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE14_TYPES

    // Remaining special cases
    TestDriver_AwkwardMaplike::testCase14();
}
static
void mainTestCase15()
    // --------------------------------------------------------------------
    // DEFAULT CONSTRUCTOR
    // --------------------------------------------------------------------
{
#define BSLSTL_HASHTABLE_TESTCASE15_TYPES \
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR, \
                  bsltf::NonAssignableTestType,                  \
                  bsltf::NonDefaultConstructibleTestType
    // This test case will use 'operator==' on the container, so cannot support
    // elements that do not, in turn, directly overload the operator.

    if (verbose) printf("\nTesting default constructor"
                        "\n===========================\n");

    if (verbose) printf("\nTesting basic configurations"
                        "\n----------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting non-copyable functors"
                        "\n-----------------------------\n");
    // This is the use case we are most concerned about.

    // We probably want to test with a smattering of the following concerns as
    // well, notably with the different allocator patterns.
    RUN_EACH_TYPE(TestDriver_DefaultOnlyFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if !defined(BSLS_HASHTABLE_TEST_ALL_TYPE_CONCERNS)
#  undef  BSLSTL_HASHTABLE_TESTCASE15_TYPES
#  define BSLSTL_HASHTABLE_TESTCASE15_TYPES BSLSTL_HASHTABLE_MINIMALTEST_TYPES
#endif

    if (verbose) printf("\nTesting map-like configuration"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_BsltfConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if 0
    // These configurations not available, as functors are not default
    // constructible.

    if (verbose) printf("\nTesting degenerate functors"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

#if 0
    // Function pointers do not give usable defaults for functors.

    if (verbose) printf("\nTesting pointers for functors"
                        "\n-----------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

    if (verbose) printf("\nTesting functors taking generic arguments"
                        "\n-----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GenericFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting functors taking convertible arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ConvertibleValueConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

    if (verbose) printf("\nTesting functors taking modifiable arguments"
                        "\n---------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_ModifiableFunctors,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#if 0
    // The stateless allocator flags issues installing the chosen allocator
    // when it is not the default.

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);
#endif

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase15,
                  BSLSTL_HASHTABLE_TESTCASE15_TYPES);

#undef BSLSTL_HASHTABLE_TESTCASE15_TYPES

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase15();
    TestDriver_AwkwardMaplikeForDefault::testCase15();
}

static
void mainTestCase16()
    // --------------------------------------------------------------------
    // TESTING traits and other compile-time properties
    // --------------------------------------------------------------------
    // For this test, the standard "minimal" set of test types provides
    // adequate coverage for the different properties dependent on the value
    // type.  We must simply test sufficient pre-packaged configurations to
    // cover the possibilities of the hash functor, comparator and allocator
    // too.  As of this writing, there is no package that separately isolates
    // bitwise copyable hasher and comparator, to test as separate dimensions.
{
    if (verbose) printf("\nTesting basic configuration"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting stateful functors"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting grouped hash with unique key values"
                        "\n-------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedUniqueKeys,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting grouped hash with grouped key values"
                        "\n--------------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_GroupedSharedKeys,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting degenerate functors without swap"
                        "\n----------------------------------------\n");
    RUN_EACH_TYPE(TestDriver_DegenerateConfiguationWithNoSwap,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting const functors"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_ConstFunctors,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING)
    if (verbose) printf("\nTesting functor referencess"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctorReferences,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);
#endif

    if (verbose) printf("\nTesting function pointers"
                        "\n-------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionPointers,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

#if !defined(BSLSTL_HASHTABLE_NO_REFERENCE_COLLAPSING) \
 && !defined(BSLS_PLATFORM_CMP_IBM)
    if (verbose) printf("\nTesting function types"
                        "\n----------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionTypes,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting function references"
                        "\n---------------------------\n");
    RUN_EACH_TYPE(TestDriver_FunctionReferences,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);
#endif

    if (verbose) printf("\nTesting stateless STL allocators"
                        "\n--------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StdAllocatorConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    if (verbose) printf("\nTesting stateful STL allocators"
                        "\n-------------------------------\n");
    RUN_EACH_TYPE(TestDriver_StatefulAllocatorConfiguation,
                  testCase16,
                  BSLSTL_HASHTABLE_MINIMALTEST_TYPES);

    // Remaining special cases
    if (verbose) printf("\nTesting degenerate map-like"
                        "\n---------------------------\n");
    TestDriver_AwkwardMaplike::testCase16();
}

#if 0  // Planned test cases, not yet implemented
static
void mainTestCase16()
    // --------------------------------------------------------------------
    // TESTING 'setMaxLoadFactor'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase16,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase16,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase17()
    // --------------------------------------------------------------------
    // TESTING 'insert'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase17,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase17,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

}

static
void mainTestCase18()
    // --------------------------------------------------------------------
    // TESTING 'insertIfMissing(bool *, VALUE)'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase18,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase18,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase19()
    // --------------------------------------------------------------------
    // TESTING 'insertIfMissing(const KeyType& key);
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase19,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase19,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase20()
    // --------------------------------------------------------------------
    // TESTING 'find'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase20,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase20,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase21()
    // --------------------------------------------------------------------
    // TESTING 'findRange'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase21,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase21,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase22()
    // --------------------------------------------------------------------
    // TESTING 'countElementsInBucket'
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase22,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase22,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase23()
    // --------------------------------------------------------------------
    // TESTING "max" FUNCTIONS
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase23,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase23,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

static
void mainTestCase24()
    // --------------------------------------------------------------------
    // TESTING PUBLIC TYPEDEFS
    // --------------------------------------------------------------------
{
    RUN_EACH_TYPE(TestDriver_BasicConfiguation,
                  testCase24,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);

    RUN_EACH_TYPE(TestDriver_StatefulConfiguation,
                  testCase24,
                  BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
}

#endif



void mainTestCaseUsageExample()
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
{
        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

        UsageExamples::main1();
        UsageExamples::main2();
        UsageExamples::main3();
        UsageExamples::main4();

}

//=============================================================================
//                            STATIC ASSERTIONS
//-----------------------------------------------------------------------------
namespace static_assertions {

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                         ::bsl::hash<int>,
                                         ::bsl::equal_to<int>,
                                         ::bsl::allocator<int> > TestAsTrue;

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                        ::bsl::hash<int>,
                                        ::bsl::equal_to<int>,
                                        bsltf::StdTestAllocator<int> >
                                                                   TaseAsFalse;

typedef bslstl::HashTable_ImplParameters<BasicKeyConfig<int>,
                                         ::bsl::hash<int>,
                                         ::bsl::equal_to<int>,
                                         bsltf::StdStatefulAllocator<int,
                                                                     true,
                                                                     false,
                                                                     false,
                                                                     false> >
                                                                  TestAsFalse2;

BSLMF_ASSERT( bslma::UsesBslmaAllocator<TestAsTrue>::value);
BSLMF_ASSERT(!bslma::UsesBslmaAllocator<TaseAsFalse>::value);
BSLMF_ASSERT(!bslma::UsesBslmaAllocator<TestAsFalse2>::value);

}  // close namespace static_assertions
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

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -TP05 // Test doc is in delegated functions
// BDE_VERIFY pragma: -TP17 // No test-banners in a delegating switch statement
    switch (test) { case 0:
      case 17: { mainTestCaseUsageExample(); } break;
//      case 18: { mainTestCase18(); } break;
//      case 17: { mainTestCase17(); } break;
      case 16: { mainTestCase16(); } break;
      case 15: { mainTestCase15(); } break;
      case 14: { mainTestCase14(); } break;
      case 13: { mainTestCase13(); } break;
      case 12: { mainTestCase12(); } break;
      case 11: { mainTestCase11(); } break;
      case 10: { mainTestCase10(); } break;
      case  9: { mainTestCase9 (); } break;
      case  8: { mainTestCase8 (); } break;
      case  7: { mainTestCase7 (); } break;
      case  6: { mainTestCase6 (); } break;
      case  5: { mainTestCase5 (); } break;
      case  4: { mainTestCase4 (); } break;
      case  3: { mainTestCase3 (); } break;
      case  2: { mainTestCase2 (); } break;
      case  1: { mainTestCase1 (); } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }
// BDE_VERIFY pragma: pop

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

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
